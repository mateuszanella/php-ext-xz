#include <lzma.h>

#include "php.h"
#include "php_streams.h"
#include "fopen_wrappers.h"
#include "php_xz.h"
#include "xz_encode.h"
#include "xz_decode.h"
#include "xz_compat.h"
#include "xz_fopen_wrapper.h"

struct php_xz_stream_ctx {
	lzma_stream strm;
	uint8_t    *in_buf;
	uint8_t    *out_buf;
	uint8_t    *out_buf_idx;
	php_stream *stream;
	bool        is_write;
};

static int php_xz_decompress(struct php_xz_stream_ctx *self)
{
	lzma_stream *strm = &self->strm;

	if (strm->avail_in == 0 && !php_stream_eof(self->stream)) {
		ssize_t read = php_stream_read(self->stream, (char *)self->in_buf, XZ_BUFFER_SIZE);
		if (read < 0) {
			return -1;
		}
		strm->avail_in = read;
		strm->next_in = self->in_buf;
	}

	lzma_ret ret = lzma_code(strm, LZMA_RUN);
	if (ret != LZMA_OK && ret != LZMA_STREAM_END) {
		return -1;
	}

	return 0;
}

static int php_xz_compress(struct php_xz_stream_ctx *self)
{
	lzma_stream *strm = &self->strm;
	int to_write = (int)strm->avail_in;

	while (strm->avail_in > 0) {
		lzma_ret ret = lzma_code(strm, LZMA_RUN);
		size_t len = XZ_BUFFER_SIZE - strm->avail_out;
		if (ret != LZMA_OK) {
			to_write = -1;
			break;
		}
		if (len && php_stream_write(self->stream, (char *)self->out_buf, len) != (ssize_t)len) {
			to_write = -1;
			break;
		}
		strm->next_out = self->out_buf;
		strm->avail_out = XZ_BUFFER_SIZE;
	}

	strm->next_in = self->in_buf;

	return to_write;
}

static int php_xz_init_decoder(struct php_xz_stream_ctx *self, uint64_t memory_limit)
{
	if (!php_xz_decoder_init_stream(&self->strm, memory_limit)) {
		return 0;
	}

	self->in_buf = emalloc(XZ_BUFFER_SIZE);
	self->strm.avail_in = 0;
	self->strm.next_in = self->in_buf;

	self->out_buf = emalloc(XZ_BUFFER_SIZE);
	self->out_buf_idx = self->out_buf;
	self->strm.avail_out = XZ_BUFFER_SIZE;
	self->strm.next_out = self->out_buf;

	return 1;
}

static int php_xz_init_encoder(struct php_xz_stream_ctx *self, uint32_t level)
{
	if (!php_xz_encoder_init_stream(&self->strm, level)) {
		return 0;
	}

	self->in_buf = emalloc(XZ_BUFFER_SIZE);
	self->strm.avail_in = 0;
	self->strm.next_in = self->in_buf;

	self->out_buf = emalloc(XZ_BUFFER_SIZE);
	self->strm.avail_out = XZ_BUFFER_SIZE;
	self->strm.next_out = self->out_buf;

	return 1;
}

static ssize_t php_xziop_read(php_stream *stream, char *buf, size_t count)
{
	struct php_xz_stream_ctx *self = (struct php_xz_stream_ctx *) stream->abstract;
	lzma_stream *strm = &self->strm;

	size_t to_read = count, have_read = 0;

	while (to_read > 0) {
		if (to_read < (size_t)(strm->next_out - self->out_buf_idx)) {
			memcpy(buf + have_read, self->out_buf_idx, to_read);
			self->out_buf_idx += to_read;
			have_read += to_read;
			break;
		} else if (strm->next_out - self->out_buf_idx > 0) {
			memcpy(buf + have_read, self->out_buf_idx, strm->next_out - self->out_buf_idx);
			have_read += strm->next_out - self->out_buf_idx;
			to_read -= strm->next_out - self->out_buf_idx;

			if (strm->avail_out) {
				self->out_buf_idx = strm->next_out;
			} else {
				self->out_buf_idx = strm->next_out = self->out_buf;
				strm->avail_out = XZ_BUFFER_SIZE;
			}
		}

		if (self->out_buf_idx == strm->next_out && php_stream_eof(self->stream) && strm->avail_in == 0) {
			stream->eof = 1;
			return have_read;
		}

		if (php_xz_decompress(self) < 0) {
			if (!have_read) {
				return -1;
			}
			break;
		}
	}

	return have_read;
}

static ssize_t php_xziop_write(php_stream *stream, const char *buf, size_t count)
{
	struct php_xz_stream_ctx *self = (struct php_xz_stream_ctx *) stream->abstract;
	size_t wrote = 0;
	int bytes_consumed = 0;

	lzma_stream *strm = &self->strm;

	while (count - wrote > XZ_BUFFER_SIZE - strm->avail_in) {
		memcpy((char *)self->in_buf + strm->avail_in, buf + wrote, XZ_BUFFER_SIZE - strm->avail_in);
		wrote += XZ_BUFFER_SIZE - strm->avail_in;
		strm->avail_in = XZ_BUFFER_SIZE;
		bytes_consumed = php_xz_compress(self);
		if (bytes_consumed < 0) {
			break;
		}
	}

	if (count - wrote > 0) {
		memcpy((char *) self->in_buf + strm->avail_in, buf + wrote, count - wrote);
		strm->avail_in += count - wrote;
	}

	return (bytes_consumed < 0 ? -1 : (ssize_t)count);
}

static int php_xziop_close(php_stream *stream, int close_handle)
{
	struct php_xz_stream_ctx *self = (struct php_xz_stream_ctx *) stream->abstract;
	int ret = EOF;

	lzma_stream *strm = &self->strm;

	if (self->is_write) {
		lzma_ret lz_ret;

		do {
			strm->next_out = self->out_buf;
			strm->avail_out = XZ_BUFFER_SIZE;
			lz_ret = lzma_code(strm, LZMA_FINISH);

			if (strm->avail_out < XZ_BUFFER_SIZE) {
				size_t write_size = XZ_BUFFER_SIZE - strm->avail_out;
				php_stream_write(self->stream, (char *)self->out_buf, write_size);
				strm->next_out = self->out_buf;
				strm->avail_out = XZ_BUFFER_SIZE;
			}

		} while (lz_ret == LZMA_OK);
	}

	lzma_end(&self->strm);

	if (self->stream) {
		php_stream_free(self->stream, PHP_STREAM_FREE_CLOSE | (close_handle == 0 ? PHP_STREAM_FREE_PRESERVE_HANDLE : 0));
	}

	efree(self->in_buf);
	efree(self->out_buf);
	efree(self);

	return ret;
}

static int php_xziop_flush(php_stream *stream)
{
	struct php_xz_stream_ctx *self = (struct php_xz_stream_ctx *) stream->abstract;
	if (self->is_write) {
		php_xz_compress(self);
	}
	php_stream_flush(self->stream);
	return 0;
}

php_stream_ops php_stream_xzio_ops = {
	php_xziop_write,
	php_xziop_read,
	php_xziop_close,
	php_xziop_flush,
	"XZ",
	NULL,
	NULL,
	NULL,
	NULL
};

php_stream *php_stream_xzopen(php_stream_wrapper *wrapper, const char *path, const char *mode_pass, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC)
{
	char mode[64];
	zend_long level = (zend_long)zend_ini_long_literal("xz.compression_level");
	zend_long mem = (zend_long)zend_ini_long_literal("xz.max_memory");

	php_stream *stream = NULL, *innerstream = NULL;

	strncpy(mode, mode_pass, sizeof(mode));
	mode[sizeof(mode) - 1] = '\0';

	char *colonp = strchr(mode, ':');
	if (colonp) {
		level = strtoul(colonp + 1, NULL, 10);
		*colonp = '\0';
	}

	if ((strchr(mode, '+')) || ((strchr(mode, 'r')) && (strchr(mode, 'w')))) {
		php_error_docref(NULL, E_ERROR, "cannot open xz stream for reading and writing at the same time.");
		return NULL;
	}

	if ((level < 0) || (level > 9)) {
		php_error_docref(NULL, E_ERROR, "Invalid compression level");
		return NULL;
	}

	if (strncasecmp("compress.lzma://", path, 16) == 0) {
		path += 16;
	}

	if (context) {
		zval *tmpzval;
		if (NULL != (tmpzval = php_stream_context_get_option(context, "xz", "compression_level"))) {
			level = zval_get_long(tmpzval);
		}
		if (NULL != (tmpzval = php_stream_context_get_option(context, "xz", "max_memory"))) {
			mem = zval_get_long(tmpzval);
		}
	}

	innerstream = php_stream_open_wrapper_ex(path, mode, STREAM_MUST_SEEK | options | STREAM_WILL_CAST, opened_path, context);

	if (innerstream) {
		int fd;
		if (php_stream_cast(innerstream, PHP_STREAM_AS_FD, (void **) &fd, REPORT_ERRORS) == SUCCESS) {
			struct php_xz_stream_ctx *self = ecalloc(1, sizeof(struct php_xz_stream_ctx));
			self->stream = innerstream;
			self->is_write = (mode[0] == 'w');
			stream = php_stream_alloc_rel(&php_stream_xzio_ops, self, 0, mode);

			if (stream) {
				stream->flags |= PHP_STREAM_FLAG_NO_BUFFER;
				if ((strcmp(mode, "w") == 0) || (strcmp(mode, "wb") == 0)) {
					if (!php_xz_init_encoder(self, (uint32_t)level)) {
						php_error_docref(NULL, E_WARNING, "Could not initialize xz encoder.");
						efree(self);
						php_stream_close(stream);
						return NULL;
					}
				} else if ((strcmp(mode, "r") == 0) || (strcmp(mode, "rb") == 0)) {
					if (!php_xz_init_decoder(self, (uint64_t)mem)) {
						php_error_docref(NULL, E_WARNING, "Could not initialize xz decoder");
						efree(self);
						php_stream_close(stream);
						return NULL;
					}
				} else {
					php_error_docref(NULL, E_WARNING, "Can only open in read (r) or write (w) mode.");
					efree(self);
					php_stream_close(stream);
					return NULL;
				}
				return stream;
			}
			efree(self);
			php_error_docref(NULL, E_WARNING, "failed opening xz stream");
		}
		php_stream_close(innerstream);
	}

	return NULL;
}

static php_stream_wrapper_ops xz_stream_wops = {
	php_stream_xzopen,
	NULL,
	NULL,
	NULL,
	NULL,
	"XZ",
	NULL,
	NULL,
	NULL,
	NULL
};

php_stream_wrapper php_stream_xz_wrapper = {
	&xz_stream_wops,
	NULL,
	0
};
