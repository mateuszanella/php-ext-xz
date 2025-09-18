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

#ifndef PHP_XZ_H
# define PHP_XZ_H

#define PHP_XZ_VERSION "1.1.4"

extern zend_module_entry xz_module_entry;
extern php_stream_wrapper php_stream_xz_wrapper;

# define phpext_xz_ptr &xz_module_entry

/* The default size of the buffer used for compression and decompression. */
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

php_stream *php_stream_xzopen(php_stream_wrapper *wrapper, const char *path, const char *mode_pass, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC);

#endif	/* PHP_XZ_H */
