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

#ifndef PHP_XZ_ENCODE_CONTEXT_H
#define PHP_XZ_ENCODE_CONTEXT_H

#include <lzma.h>
#include "php_xz.h"

typedef struct _php_xz_encode_context_obj {
	lzma_stream strm;
	lzma_ret status;
	zend_object std;
} php_xz_encode_context_obj;

static inline php_xz_encode_context_obj *php_xz_encode_context_from_obj(zend_object *obj)
{
	return (php_xz_encode_context_obj *)((char *)obj - XtOffsetOf(php_xz_encode_context_obj, std));
}

extern zend_class_entry *xz_encode_context_ce;

void php_xz_encode_context_register_handlers(void);
zend_object *php_xz_encode_context_create_obj(zend_class_entry *class_type);

PHP_FUNCTION(xz_encode_init);
PHP_FUNCTION(xz_encode_add);
PHP_FUNCTION(xz_encode_finish);

#endif /* PHP_XZ_ENCODE_CONTEXT_H */
