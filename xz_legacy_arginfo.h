#ifndef PHP_XZ_LEGACY_ARGINFO_H
#define PHP_XZ_LEGACY_ARGINFO_H

ZEND_BEGIN_ARG_INFO(arginfo_xzread, 0)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xzwrite, 0, 0, 2)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xzclose, 0)
	ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xzpassthru, 0)
	ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xzencode, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_xzdecode, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xzopen, 0, 0, 2)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, compression_level)
ZEND_END_ARG_INFO()

static const zend_function_entry ext_functions[] = {
	PHP_FE(xzdecode, arginfo_xzdecode)
	PHP_FE(xzopen, arginfo_xzopen)
	PHP_FE(xzencode, arginfo_xzencode)
	PHP_FALIAS(xzread, fread, arginfo_xzread)
	PHP_FALIAS(xzwrite, fwrite, arginfo_xzwrite)
	PHP_FALIAS(xzclose, fclose, arginfo_xzclose)
	PHP_FALIAS(xzpassthru, fpassthru, arginfo_xzpassthru)
	PHP_FE_END
};

#endif /* PHP_XZ_LEGACY_ARGINFO_H */