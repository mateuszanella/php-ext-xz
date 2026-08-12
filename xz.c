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

#include <lzma.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "ext/standard/file.h"
#include "ext/standard/info.h"
#include "php.h"
#include "php_ini.h"
#include "php_xz.h"
#include "xz_encode.h"
#include "xz_decode.h"
#include "xz_compat.h"
#include "xz_fopen_wrapper.h"
#include "xz_encode_context.h"
#include "xz_decode_context.h"

#if PHP_VERSION_ID >= 80000
# include "xz_arginfo.h"
#else
# include "xz_legacy_arginfo.h"
#endif

/* {{{ INI entries. */
PHP_INI_BEGIN()
	PHP_INI_ENTRY("xz.compression_level", "5", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("xz.max_memory", "0", PHP_INI_SYSTEM, NULL)
PHP_INI_END()
/* }}} */

/* {{{ MINIT */
PHP_MINIT_FUNCTION(xz)
{
	zend_class_entry tmp_ce;

	REGISTER_INI_ENTRIES();

	php_xz_encode_context_register_handlers();
	INIT_CLASS_ENTRY(tmp_ce, "XZEncodeContext", NULL);
	xz_encode_context_ce = zend_register_internal_class(&tmp_ce);
	xz_encode_context_ce->create_object = php_xz_encode_context_create_obj;
	xz_encode_context_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES | ZEND_ACC_NOT_SERIALIZABLE;

	php_xz_decode_context_register_handlers();
	INIT_CLASS_ENTRY(tmp_ce, "XZDecodeContext", NULL);
	xz_decode_context_ce = zend_register_internal_class(&tmp_ce);
	xz_decode_context_ce->create_object = php_xz_decode_context_create_obj;
	xz_decode_context_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES | ZEND_ACC_NOT_SERIALIZABLE;

	php_register_url_stream_wrapper("compress.lzma", &php_stream_xz_wrapper);

	// Encode integrity check types. See xz_encode_init
	REGISTER_LONG_CONSTANT("XZ_CHECK_NONE", LZMA_CHECK_NONE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XZ_CHECK_CRC32", LZMA_CHECK_CRC32, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XZ_CHECK_CRC64", LZMA_CHECK_CRC64, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XZ_CHECK_SHA256", LZMA_CHECK_SHA256, CONST_CS | CONST_PERSISTENT);

	// Compression container formats. See xz_encode_init / xz_decode_init
	REGISTER_LONG_CONSTANT("XZ_FORMAT_XZ", XZ_FORMAT_XZ, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XZ_FORMAT_RAW", XZ_FORMAT_RAW, CONST_CS | CONST_PERSISTENT);

	// Raw filter ids. See xz_encode_init / xz_decode_init
	REGISTER_LONG_CONSTANT("XZ_FILTER_LZMA1", LZMA_FILTER_LZMA1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XZ_FILTER_LZMA2", LZMA_FILTER_LZMA2, CONST_CS | CONST_PERSISTENT);

	// Decode context flags. See xz_decode_init
	REGISTER_LONG_CONSTANT("XZ_CONCATENATED", LZMA_CONCATENATED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XZ_FAIL_FAST", LZMA_FAIL_FAST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("XZ_IGNORE_CHECK", LZMA_IGNORE_CHECK, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ MSHUTDOWN */
PHP_MSHUTDOWN_FUNCTION(xz)
{
	php_unregister_url_stream_wrapper("compress.lzma");
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ MINFO */
PHP_MINFO_FUNCTION(xz)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "xz support", "enabled");
	php_info_print_table_row(2, "xz extension version ", PHP_XZ_VERSION);
	if (strcmp(LZMA_VERSION_STRING, lzma_version_string())) {
		php_info_print_table_row(2, "liblzma headers version", LZMA_VERSION_STRING);
		php_info_print_table_row(2, "liblzma library version", lzma_version_string());
	} else {
		php_info_print_table_row(2, "liblzma version", lzma_version_string());
	}
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ proto resource xzopen(string filename, string mode)
   Opens a file stream. */
PHP_FUNCTION(xzopen)
{
	char *filename, *mode;
	size_t filename_len, mode_len;
	zend_long compression_level = (zend_long)zend_ini_long_literal("xz.compression_level");

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(filename, filename_len)
		Z_PARAM_STRING(mode, mode_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(compression_level)
	ZEND_PARSE_PARAMETERS_END();

	char *mode_to_pass = emalloc(mode_len + 32);
	snprintf(mode_to_pass, mode_len + 32, "%s:" ZEND_LONG_FMT, mode, compression_level);

	php_stream *stream = php_stream_xzopen(NULL, filename, mode_to_pass, 0, NULL, NULL STREAMS_CC);

	if (!stream) {
		efree(mode_to_pass);
		RETURN_FALSE;
	}

	efree(mode_to_pass);
	php_stream_to_zval(stream, return_value);
}
/* }}} */

/* {{{ proto string xzencode(string data, int level, int format)
   Returns the encoded string. */
PHP_FUNCTION(xzencode)
{
	char *in;
	size_t in_len;
	zend_long level = -1;
	zend_long format = XZ_FORMAT_XZ;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STRING(in, in_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(level)
		Z_PARAM_LONG(format)
	ZEND_PARSE_PARAMETERS_END();

	if (level == -1) {
		level = (zend_long)zend_ini_long_literal("xz.compression_level");
	}

	if (level < 0 || level > 9) {
		XZ_VALUE_ERROR(2, "must be between -1 and 9", "compression level must be between -1 and 9");
	}

	if (format != XZ_FORMAT_XZ && format != XZ_FORMAT_RAW) {
		XZ_VALUE_ERROR(3, "must be XZ_FORMAT_XZ or XZ_FORMAT_RAW", "format must be XZ_FORMAT_XZ or XZ_FORMAT_RAW");
	}

	zend_string *out = php_xz_encode_string((const uint8_t *)in, in_len, (uint32_t)level, (uint32_t)format);

	if (!out) {
		RETURN_FALSE;
	}

	RETURN_STR(out);
}
/* }}} */

/* {{{ proto string xzdecode(string data, int memory_limit)
   Returns the decoded string. */
PHP_FUNCTION(xzdecode)
{
	char *in;
	size_t in_len;
	zend_long memory_limit = -1;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(in, in_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(memory_limit)
	ZEND_PARSE_PARAMETERS_END();

	if (!in_len) {
		RETURN_FALSE;
	}

	if (memory_limit < -1) {
		XZ_VALUE_ERROR(2, "must be greater than or equal to -1", "memory limit must be greater than or equal to -1");
	}

	if (memory_limit == -1) {
		memory_limit = (zend_long)zend_ini_long_literal("xz.max_memory");
	}

	zend_string *out = php_xz_decode_string((const uint8_t *)in, in_len, (uint64_t)memory_limit);

	if (!out) {
		RETURN_FALSE;
	}

	RETURN_STR(out);
}
/* }}} */

/* {{{ xz_module_entry */
zend_module_entry xz_module_entry = {
    STANDARD_MODULE_HEADER,
    "xz",
    ext_functions,
    PHP_MINIT(xz),
    PHP_MSHUTDOWN(xz),
    NULL,
    NULL,
    PHP_MINFO(xz),
    PHP_XZ_VERSION,
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XZ
#ifdef ZTS
    ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(xz)
#endif
