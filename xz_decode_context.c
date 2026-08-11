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
	php_xz_decode_context_object_handlers.offset = XtOffsetOf(php_xz_decode_context_obj, std);
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

/* {{{ proto XZDecodeContext xz_decode_init(int flags, int memory_limit)
   Creates a new incremental xz decompression context. */
PHP_FUNCTION(xz_decode_init)
{
	zend_long memory_limit = (zend_long)zend_ini_long_literal("xz.max_memory");
	zend_long flags = 0;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
		Z_PARAM_LONG(memory_limit)
	ZEND_PARSE_PARAMETERS_END();

	object_init_ex(return_value, xz_decode_context_ce);
	php_xz_decode_context_obj *obj = php_xz_decode_context_from_obj(Z_OBJ_P(return_value));

	lzma_ret ret = lzma_auto_decoder(&obj->strm,
		memory_limit ? (uint64_t)memory_limit : UINT64_MAX, (uint32_t)flags);
	if (ret != LZMA_OK) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}
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
