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
