/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 4eb5aa02ece4df1bedcaeda79c2ad951bee474a1 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_xzread, 0, 0, 1)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xzwrite, 0, 0, 2)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xzclose, 0, 0, 1)
	ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

#define arginfo_xzpassthru arginfo_xzclose

ZEND_BEGIN_ARG_INFO_EX(arginfo_xzencode, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, level)
	ZEND_ARG_INFO(0, format)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xzdecode, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, memory_limit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xzopen, 0, 0, 2)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, compression_level)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xz_encode_init, 0, 0, 0)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xz_encode_add, 0, 0, 2)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xz_encode_finish, 0, 0, 1)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

#define arginfo_xz_encode_get_properties arginfo_xz_encode_finish

#define arginfo_xz_decode_init arginfo_xz_encode_init

#define arginfo_xz_decode_add arginfo_xz_encode_add

#define arginfo_xz_decode_finish arginfo_xz_encode_finish

#define arginfo_xz_decode_get_status arginfo_xz_encode_finish

#define arginfo_xz_decode_get_read_len arginfo_xz_encode_finish

ZEND_FUNCTION(fread);
ZEND_FUNCTION(fwrite);
ZEND_FUNCTION(fclose);
ZEND_FUNCTION(fpassthru);
ZEND_FUNCTION(xzencode);
ZEND_FUNCTION(xzdecode);
ZEND_FUNCTION(xzopen);
ZEND_FUNCTION(xz_encode_init);
ZEND_FUNCTION(xz_encode_add);
ZEND_FUNCTION(xz_encode_finish);
ZEND_FUNCTION(xz_encode_get_properties);
ZEND_FUNCTION(xz_decode_init);
ZEND_FUNCTION(xz_decode_add);
ZEND_FUNCTION(xz_decode_finish);
ZEND_FUNCTION(xz_decode_get_status);
ZEND_FUNCTION(xz_decode_get_read_len);

static const zend_function_entry ext_functions[] = {
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("xzread", zif_fread, arginfo_xzread, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY("xzread", zif_fread, arginfo_xzread, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("xzwrite", zif_fwrite, arginfo_xzwrite, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY("xzwrite", zif_fwrite, arginfo_xzwrite, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("xzclose", zif_fclose, arginfo_xzclose, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY("xzclose", zif_fclose, arginfo_xzclose, 0)
#endif
#if (PHP_VERSION_ID >= 80400)
	ZEND_RAW_FENTRY("xzpassthru", zif_fpassthru, arginfo_xzpassthru, 0, NULL, NULL)
#else
	ZEND_RAW_FENTRY("xzpassthru", zif_fpassthru, arginfo_xzpassthru, 0)
#endif
	ZEND_FE(xzencode, arginfo_xzencode)
	ZEND_FE(xzdecode, arginfo_xzdecode)
	ZEND_FE(xzopen, arginfo_xzopen)
	ZEND_FE(xz_encode_init, arginfo_xz_encode_init)
	ZEND_FE(xz_encode_add, arginfo_xz_encode_add)
	ZEND_FE(xz_encode_finish, arginfo_xz_encode_finish)
	ZEND_FE(xz_encode_get_properties, arginfo_xz_encode_get_properties)
	ZEND_FE(xz_decode_init, arginfo_xz_decode_init)
	ZEND_FE(xz_decode_add, arginfo_xz_decode_add)
	ZEND_FE(xz_decode_finish, arginfo_xz_decode_finish)
	ZEND_FE(xz_decode_get_status, arginfo_xz_decode_get_status)
	ZEND_FE(xz_decode_get_read_len, arginfo_xz_decode_get_read_len)
	ZEND_FE_END
};
