#include "xz_decode.h"

#include <lzma.h>
#include "php.h"

zend_string *php_xz_decode_string(const uint8_t *data, size_t data_len, uint64_t memory_limit)
{
	lzma_stream strm = LZMA_STREAM_INIT;
	if (lzma_auto_decoder(&strm, memory_limit ? memory_limit : UINT64_MAX, LZMA_CONCATENATED) != LZMA_OK) {
		return NULL;
	}

	strm.next_in = data;
	strm.avail_in = data_len;

	zend_string *out = zend_string_alloc(XZ_BUFFER_SIZE, 0);
	strm.next_out = (uint8_t *) ZSTR_VAL(out);
	strm.avail_out = XZ_BUFFER_SIZE;

	size_t out_used = 0;

	while (strm.avail_in > 0) {
		lzma_ret ret = lzma_code(&strm, LZMA_RUN);
		if (ret != LZMA_OK && ret != LZMA_STREAM_END) {
			zend_string_release(out);
			lzma_end(&strm);
			return NULL;
		}
		if (strm.avail_out == 0) {
			out_used = ZSTR_LEN(out) - strm.avail_out;
			out = zend_string_extend(out, ZSTR_LEN(out) + XZ_BUFFER_SIZE, 0);
			strm.next_out = (uint8_t *) ZSTR_VAL(out) + out_used;
			strm.avail_out = XZ_BUFFER_SIZE;
		}
	}

	out_used = ZSTR_LEN(out) - strm.avail_out;
	out = zend_string_truncate(out, out_used, 0);
	ZSTR_VAL(out)[out_used] = '\0';

	lzma_end(&strm);
	return out;
}

int php_xz_decoder_init_stream(lzma_stream *strm, uint64_t memory_limit)
{
	return lzma_auto_decoder(strm, memory_limit ? memory_limit : UINT64_MAX, LZMA_CONCATENATED) == LZMA_OK ? 1 : 0;
}
