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

#ifndef PHP_XZ_OPTIONS_H
# define PHP_XZ_OPTIONS_H

#include "php.h"

/* Reads a long-valued option from an options HashTable. Returns 0 when the
 * option is absent, 1 when it was found (and stored in *value). */
static inline int php_xz_opt_get_long(const HashTable *options, const char *key, zend_long *value)
{
	zval *zv = zend_hash_str_find(options, key, strlen(key));
	if (zv == NULL) {
		return 0;
	}
	*value = zval_get_long(zv);
	return 1;
}

#endif /* PHP_XZ_OPTIONS_H */
