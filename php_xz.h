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

#include "php.h"

#include "xz_compat.h"

#include <lzma.h>

#define PHP_XZ_VERSION "2.1.0"

/* Compression container formats for the incremental encode/decode contexts. */
#define XZ_FORMAT_XZ  0
#define XZ_FORMAT_RAW 1

/* Default buffer size for incremental encode/decode operations. */
#define XZ_BUFFER_SIZE 4096

extern zend_module_entry xz_module_entry;
extern php_stream_wrapper php_stream_xz_wrapper;

extern zend_class_entry *xz_encode_context_ce;
extern zend_class_entry *xz_decode_context_ce;

# define phpext_xz_ptr &xz_module_entry

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

#endif	/* PHP_XZ_H */
