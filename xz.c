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
	REGISTER_INI_ENTRIES();
	php_register_url_stream_wrapper("compress.lzma", &php_stream_xz_wrapper);
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

/* {{{ proto string xzencode(string str)
   Returns the encoded string. */
PHP_FUNCTION(xzencode)
{
	char *in;
	size_t in_len;
	zend_long compression_level = (zend_long)zend_ini_long_literal("xz.compression_level");

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(in, in_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(compression_level)
	ZEND_PARSE_PARAMETERS_END();

	if (compression_level < 0 || compression_level > 9) {
#if PHP_VERSION_ID >= 80000
		zend_argument_value_error(2, "must be between 0 and 9");
		RETURN_THROWS();
#else
		php_error_docref(NULL, E_WARNING, "compression level must be between 0 and 9");
		RETURN_FALSE;
#endif
	}

	zend_string *out = php_xz_encode_string((const uint8_t *)in, in_len, (uint32_t)compression_level);

	if (!out) {
		RETURN_FALSE;
	}

	RETURN_STR(out);
}
/* }}} */

/* {{{ proto string xzdecode(string str)
   Returns the decoded string. */
PHP_FUNCTION(xzdecode)
{
	char *in;
	size_t in_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(in, in_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!in_len) {
		RETURN_FALSE;
	}

	uint64_t memory_limit = (uint64_t)zend_ini_long_literal("xz.max_memory");

	zend_string *out = php_xz_decode_string((const uint8_t *)in, in_len, memory_limit);

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
