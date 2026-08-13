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

#include "xz_decode_context.h"

#include "php.h"
#include "xz_compat.h"
#include "xz_options.h"

zend_class_entry *xz_decode_context_ce;

static zend_object_handlers php_xz_decode_context_object_handlers;

zend_object *php_xz_decode_context_create_obj(zend_class_entry *class_type)
{
	php_xz_decode_context_obj *obj = ecalloc(1, sizeof(php_xz_decode_context_obj));
	zend_object_std_init(&obj->std, class_type);
	object_properties_init(&obj->std, class_type);
	obj->std.handlers = &php_xz_decode_context_object_handlers;
	return &obj->std;
}

static void php_xz_decode_context_free_obj(zend_object *object)
{
	php_xz_decode_context_obj *obj = php_xz_decode_context_from_obj(object);
	if (obj->strm.internal) {
		lzma_end(&obj->strm);
	}
	zend_object_std_dtor(object);
}

void php_xz_decode_context_register_handlers(void)
{
	memcpy(&php_xz_decode_context_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_xz_decode_context_object_handlers.free_obj = php_xz_decode_context_free_obj;
	php_xz_decode_context_object_handlers.clone_obj = NULL;
	php_xz_decode_context_object_handlers.offset = offsetof(php_xz_decode_context_obj, std);
}

static zend_string *php_xz_decode_context_process(php_xz_decode_context_obj *obj, const char *data, size_t data_len, lzma_action action)
{
	lzma_stream *strm = &obj->strm;

	strm->next_in = (const uint8_t *)data;
	strm->avail_in = data_len;

	size_t out_used = 0;
	zend_string *out = zend_string_alloc(XZ_BUFFER_SIZE, 0);
	strm->next_out = (uint8_t *)ZSTR_VAL(out);
	strm->avail_out = XZ_BUFFER_SIZE;

	zend_bool finished = 0;

	do {
		lzma_ret ret = lzma_code(strm, action);
		out_used = ZSTR_LEN(out) - strm->avail_out;
		obj->status = ret;

		switch (ret) {
			case LZMA_OK:
				if (strm->avail_out == 0) {
					out = zend_string_extend(out, ZSTR_LEN(out) + XZ_BUFFER_SIZE, 0);
					strm->next_out = (uint8_t *)ZSTR_VAL(out) + out_used;
					strm->avail_out = XZ_BUFFER_SIZE;
				} else {
					finished = 1;
				}
				break;
			case LZMA_STREAM_END:
				finished = 1;
				break;
			case LZMA_BUF_ERROR:
				if (action == LZMA_FINISH && strm->avail_out == 0) {
					out = zend_string_extend(out, ZSTR_LEN(out) + XZ_BUFFER_SIZE, 0);
					strm->next_out = (uint8_t *)ZSTR_VAL(out) + out_used;
					strm->avail_out = XZ_BUFFER_SIZE;
				} else {
					finished = 1;
				}
				break;
			default:
				zend_string_release(out);
				return NULL;
		}
	} while (!finished);

	out = zend_string_truncate(out, out_used, 0);
	ZSTR_VAL(out)[out_used] = '\0';
	return out;
}

static int xz_decode_validate_lzma_params(zend_long lc, zend_long lp, zend_long pb, zend_long dict_size)
{
	if (lc < -1 || lc > 4 || lp < -1 || lp > 4 || pb < -1 || pb > 4) {
		return 0;
	}
	if (lc >= 0 && lp >= 0 && lc + lp > 4) {
		return 0;
	}
	if (dict_size < 0) {
		return 0;
	}
	return 1;
}

/* {{{ proto XZDecodeContext xz_decode_init(int format, array options)
   Creates a new incremental xz or raw LZMA decompression context. */
PHP_FUNCTION(xz_decode_init)
{
	zend_long format = XZ_FORMAT_XZ;
	HashTable *options = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(format)
		Z_PARAM_ARRAY_HT(options)
	ZEND_PARSE_PARAMETERS_END();

	if (format != XZ_FORMAT_XZ && format != XZ_FORMAT_RAW) {
		XZ_VALUE_ERROR(1, "must be XZ_FORMAT_XZ or XZ_FORMAT_RAW", "format must be XZ_FORMAT_XZ or XZ_FORMAT_RAW");
	}

	zend_long flags = 0;
	zend_long memory_limit = (zend_long)zend_ini_long_literal("xz.max_memory");
	zend_long filter_id = LZMA_FILTER_LZMA2;
	zend_long dict_size = 0;
	zend_long lc = -1;
	zend_long lp = -1;
	zend_long pb = -1;

	if (options) {
		php_xz_opt_get_long(options, "flags", &flags);
		php_xz_opt_get_long(options, "memory_limit", &memory_limit);
		php_xz_opt_get_long(options, "filter", &filter_id);
		php_xz_opt_get_long(options, "dict_size", &dict_size);
		php_xz_opt_get_long(options, "lc", &lc);
		php_xz_opt_get_long(options, "lp", &lp);
		php_xz_opt_get_long(options, "pb", &pb);
	}

	if (filter_id != LZMA_FILTER_LZMA1 && filter_id != LZMA_FILTER_LZMA2) {
		XZ_VALUE_ERROR(2, "options['filter'] must be XZ_FILTER_LZMA1 or XZ_FILTER_LZMA2", "filter must be XZ_FILTER_LZMA1 or XZ_FILTER_LZMA2");
	}

	if (!xz_decode_validate_lzma_params(lc, lp, pb, dict_size)) {
		XZ_VALUE_ERROR(2, "options['lc'], options['lp'], options['pb'] must be between 0 and 4 with lc + lp <= 4, and options['dict_size'] must be non-negative", "invalid LZMA parameters");
	}

	object_init_ex(return_value, xz_decode_context_ce);
	php_xz_decode_context_obj *obj = php_xz_decode_context_from_obj(Z_OBJ_P(return_value));

	lzma_ret ret;
	if (format == XZ_FORMAT_RAW) {
		lzma_options_lzma opt;
		if (lzma_lzma_preset(&opt, 6)) {
			zval_ptr_dtor(return_value);
			RETURN_FALSE;
		}

		if (dict_size) {
			opt.dict_size = (uint32_t)dict_size;
		}
		if (lc >= 0) {
			opt.lc = (uint32_t)lc;
		}
		if (lp >= 0) {
			opt.lp = (uint32_t)lp;
		}
		if (pb >= 0) {
			opt.pb = (uint32_t)pb;
		}

		lzma_filter filters[] = {
			{ .id = (lzma_vli)filter_id, .options = &opt },
			{ .id = LZMA_VLI_UNKNOWN,  .options = NULL },
		};

		ret = lzma_raw_decoder(&obj->strm, filters);
	} else {
		ret = lzma_auto_decoder(&obj->strm,
			memory_limit ? (uint64_t)memory_limit : UINT64_MAX, (uint32_t)flags);
	}

	if (ret != LZMA_OK) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}

	obj->format = (uint32_t)format;
	obj->status = LZMA_OK;
}
/* }}} */

/* {{{ proto string xz_decode_add(XZDecodeContext context, string data)
   Incrementally decodes data. Uses LZMA_RUN action. */
PHP_FUNCTION(xz_decode_add)
{
	zval *context_zv;
	char *data;
	size_t data_len;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_OBJECT(context_zv)
		Z_PARAM_STRING(data, data_len)
	ZEND_PARSE_PARAMETERS_END();

	XZ_EXPECTED_OBJECT(context_zv, xz_decode_context_ce, "XZDecodeContext");

	php_xz_decode_context_obj *obj = php_xz_decode_context_from_obj(Z_OBJ_P(context_zv));

	if (obj->status == LZMA_STREAM_END) {
		php_error_docref(NULL, E_WARNING, "decoding context has already finished");
		RETURN_FALSE;
	}

	zend_string *out = php_xz_decode_context_process(obj, data, data_len, LZMA_RUN);
	if (!out) {
		RETURN_FALSE;
	}

	RETURN_STR(out);
}
/* }}} */

/* {{{ proto string xz_decode_finish(XZDecodeContext context)
   Finalizes the decoding stream. Returns any remaining output. */
PHP_FUNCTION(xz_decode_finish)
{
	zval *context_zv;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT(context_zv)
	ZEND_PARSE_PARAMETERS_END();

	XZ_EXPECTED_OBJECT(context_zv, xz_decode_context_ce, "XZDecodeContext");

	php_xz_decode_context_obj *obj = php_xz_decode_context_from_obj(Z_OBJ_P(context_zv));

	if (obj->status == LZMA_STREAM_END) {
		RETURN_EMPTY_STRING();
	}

	zend_string *out = php_xz_decode_context_process(obj, "", 0, LZMA_FINISH);
	if (!out) {
		RETURN_FALSE;
	}

	RETURN_STR(out);
}
/* }}} */

/* {{{ proto int xz_decode_get_status(XZDecodeContext context)
   Returns the last status code from the decoding context. */
PHP_FUNCTION(xz_decode_get_status)
{
	zval *context_zv;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT(context_zv)
	ZEND_PARSE_PARAMETERS_END();

	XZ_EXPECTED_OBJECT(context_zv, xz_decode_context_ce, "XZDecodeContext");

	php_xz_decode_context_obj *obj = php_xz_decode_context_from_obj(Z_OBJ_P(context_zv));
	RETURN_LONG(obj->status);
}
/* }}} */

/* {{{ proto int xz_decode_get_read_len(XZDecodeContext context)
   Returns the total number of compressed bytes consumed by the decoder. */
PHP_FUNCTION(xz_decode_get_read_len)
{
	zval *context_zv;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT(context_zv)
	ZEND_PARSE_PARAMETERS_END();

	XZ_EXPECTED_OBJECT(context_zv, xz_decode_context_ce, "XZDecodeContext");

	php_xz_decode_context_obj *obj = php_xz_decode_context_from_obj(Z_OBJ_P(context_zv));
	RETURN_LONG((zend_long)obj->strm.total_in);
}
/* }}} */
