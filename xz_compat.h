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

#ifndef XZ_COMPAT_H
#define XZ_COMPAT_H

#if PHP_VERSION_ID < 80600
# define zend_ini_long_literal(name) zend_ini_long((name), sizeof("" name) - 1, 0)
#endif

#if PHP_VERSION_ID < 80000
#define RETURN_THROWS() return
#endif

#if !defined(ZEND_PARSE_PARAMETERS_NONE) && PHP_VERSION_ID < 80000
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

#ifndef ZEND_ACC_NO_DYNAMIC_PROPERTIES
#define ZEND_ACC_NO_DYNAMIC_PROPERTIES 0
#endif

#ifndef ZEND_ACC_NOT_SERIALIZABLE
#define ZEND_ACC_NOT_SERIALIZABLE 0
#endif

#endif /* XZ_COMPAT_H */
