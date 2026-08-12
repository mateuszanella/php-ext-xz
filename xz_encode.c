/*
	+----------------------------------------------------------------------+
	| Copyright (c) 2019 The PHP Group                                     |
	+----------------------------------------------------------------------+
	| This source file is subject to version 3.01 of the PHP license,      |
	| that is bundled with this package in the file LICENSE, and is        |
	| available through the world-wide-web at the following url:           |
	| http://www.php.net/license/3_01.txt                                  |
	| If you did not receive a copy of the PHP license and are unable to   |
	| obtain it through the world-wide-web, please send a note to          |
	| license@php.net so we can mail you a copy immediately.               |
	+----------------------------------------------------------------------+
	| Authors: Payden Sutherland <payden@paydensutherland.com>             |
	|          Dan Ungureanu <udan1107@gmail.com>                          |
	|          authors of the `zlib` extension (for guidance)              |
	|          krakjoe (updated for PHP 7)                                 |
	+----------------------------------------------------------------------+
*/

#include "xz_encode.h"

#include <lzma.h>
#include "php.h"

zend_string *php_xz_encode_string(const uint8_t *data, size_t data_len, uint32_t level, uint32_t format)
{
	lzma_options_lzma opt;
	if (lzma_lzma_preset(&opt, level)) {
		return NULL;
	}

	lzma_filter filters[] = {
		{ .id = LZMA_FILTER_LZMA2, .options = &opt },
		{ .id = LZMA_VLI_UNKNOWN,  .options = NULL },
	};

	lzma_stream strm = LZMA_STREAM_INIT;
	lzma_ret init_ret = (format == XZ_FORMAT_RAW)
		? lzma_raw_encoder(&strm, filters)
		: lzma_stream_encoder(&strm, filters, LZMA_CHECK_CRC64);
	if (init_ret != LZMA_OK) {
		return NULL;
	}

	strm.next_in = data;
	strm.avail_in = data_len;

	zend_string *out = zend_string_alloc(XZ_BUFFER_SIZE, 0);
	strm.next_out = (uint8_t *)ZSTR_VAL(out);
	strm.avail_out = XZ_BUFFER_SIZE;

	size_t out_used = 0;

	while (strm.avail_in > 0) {
		lzma_ret ret = lzma_code(&strm, LZMA_RUN);
		if (ret != LZMA_OK) {
			zend_string_release(out);
			lzma_end(&strm);
			return NULL;
		}
		if (strm.avail_out == 0) {
			out_used = ZSTR_LEN(out) - strm.avail_out;
			out = zend_string_extend(out, ZSTR_LEN(out) + XZ_BUFFER_SIZE, 0);
			strm.next_out = (uint8_t *)ZSTR_VAL(out) + out_used;
			strm.avail_out = XZ_BUFFER_SIZE;
		}
	}

	lzma_ret ret;
	do {
		ret = lzma_code(&strm, LZMA_FINISH);
		if (ret != LZMA_OK && ret != LZMA_STREAM_END) {
			zend_string_release(out);
			lzma_end(&strm);
			return NULL;
		}
		if (strm.avail_out == 0 || ret == LZMA_STREAM_END) {
			out_used = ZSTR_LEN(out) - strm.avail_out;
			if (ret != LZMA_STREAM_END) {
				out = zend_string_extend(out, ZSTR_LEN(out) + XZ_BUFFER_SIZE, 0);
				strm.next_out = (uint8_t *)ZSTR_VAL(out) + out_used;
				strm.avail_out = XZ_BUFFER_SIZE;
			}
		}
	} while (ret != LZMA_STREAM_END);

	out_used = ZSTR_LEN(out) - strm.avail_out;
	out = zend_string_truncate(out, out_used, 0);
	ZSTR_VAL(out)[out_used] = '\0';

	lzma_end(&strm);
	return out;
}

int php_xz_encoder_init_stream(lzma_stream *strm, uint32_t level)
{
	lzma_options_lzma opt;
	if (lzma_lzma_preset(&opt, level)) {
		return 0;
	}

	lzma_filter filters[] = {
		{ .id = LZMA_FILTER_LZMA2, .options = &opt },
		{ .id = LZMA_VLI_UNKNOWN,  .options = NULL },
	};

	return lzma_stream_encoder(strm, filters, LZMA_CHECK_CRC64) == LZMA_OK;
}
