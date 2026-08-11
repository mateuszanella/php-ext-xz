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

#if PHP_VERSION_ID >= 70400
# define XZ_STREAM_RET     ssize_t
# define XZ_STREAM_ERR_VAL -1
#else
# define XZ_STREAM_RET     size_t
# define XZ_STREAM_ERR_VAL 0
#endif

#if PHP_VERSION_ID >= 70400
# define XZ_STREAM_READ_INTO(strm, stream, buf) \
	do { \
		ssize_t _xr = php_stream_read((stream), (char *)(buf), XZ_BUFFER_SIZE); \
		if (_xr < 0) { return -1; } \
		(strm)->avail_in = _xr; \
		(strm)->next_in = (buf); \
	} while (0)
#else
# define XZ_STREAM_READ_INTO(strm, stream, buf) \
	do { \
		(strm)->avail_in = php_stream_read((stream), (char *)(buf), XZ_BUFFER_SIZE); \
		(strm)->next_in = (buf); \
	} while (0)
#endif

#define XZ_EXPECTED_OBJECT_EX(obj_zv, ce, class_name, arg_num) \
	do { \
		if (Z_OBJCE_P(obj_zv) != (ce)) { \
			XZ_TYPE_ERROR(arg_num, class_name, obj_zv); \
		} \
	} while (0)

#define XZ_EXPECTED_OBJECT(obj_zv, ce, class_name) \
	XZ_EXPECTED_OBJECT_EX(obj_zv, ce, class_name, 1)

#if PHP_VERSION_ID >= 80000

#define XZ_TYPE_ERROR(arg_num, class_name, obj_zv) \
	do { \
		zend_argument_type_error(arg_num, "must be of type " class_name ", %s given", zend_zval_type_name(obj_zv)); \
		RETURN_THROWS(); \
	} while (0)

#define XZ_VALUE_ERROR(arg_num, php8_msg, php7_msg) \
	do { \
		zend_argument_value_error(arg_num, php8_msg); \
		RETURN_THROWS(); \
	} while (0)

#else

#define XZ_TYPE_ERROR(arg_num, class_name, obj_zv) \
	do { \
		php_error_docref(NULL, E_WARNING, "expected " class_name); \
		RETURN_FALSE; \
	} while (0)

#define XZ_VALUE_ERROR(arg_num, php8_msg, php7_msg) \
	do { \
		php_error_docref(NULL, E_WARNING, php7_msg); \
		RETURN_FALSE; \
	} while (0)

#endif

#endif /* XZ_COMPAT_H */
