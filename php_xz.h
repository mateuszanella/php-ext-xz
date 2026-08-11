#ifndef PHP_XZ_H
# define PHP_XZ_H

#include "php.h"
#include <lzma.h>

#define PHP_XZ_VERSION "1.2.0"

extern zend_module_entry xz_module_entry;
extern php_stream_wrapper php_stream_xz_wrapper;

# define phpext_xz_ptr &xz_module_entry

#define XZ_BUFFER_SIZE 4096

#ifdef PHP_WIN32
#	define PHP_XZ_API __declspec(dllexport)
#elif defined(__GNUC__) && (__GNUC__ >= 4)
#	define PHP_XZ_API __attribute__ ((visibility("default")))
#else
#	define PHP_XZ_API
#endif

#ifdef ZTS
#	include "TSRM.h"
#endif

#if PHP_VERSION_ID < 80600
# define zend_ini_long_literal(name) zend_ini_long((name), sizeof("" name) - 1, 0)
#endif

#if !defined(ZEND_PARSE_PARAMETERS_NONE) && PHP_VERSION_ID < 80000
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

php_stream *php_stream_xzopen(php_stream_wrapper *wrapper, const char *path, const char *mode_pass, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC);

#endif	/* PHP_XZ_H */
