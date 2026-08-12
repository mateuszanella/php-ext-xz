/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 2e775e5da24736ce0bbbfef40f14ac3e7f299ae1 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xzread, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, fp, resource, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xzwrite, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, fp, resource, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xzclose, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, fp, resource, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xzpassthru, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, fp, resource, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xzencode, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, compression_level, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xzdecode, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_xzopen, 0, 2, resource, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mode, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, compression_level, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_xz_encode_init, 0, 0, XZEncodeContext, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, format, IS_LONG, 0, "XZ_FORMAT_XZ")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xz_encode_add, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, context, XZEncodeContext, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xz_encode_finish, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, context, XZEncodeContext, 0)
ZEND_END_ARG_INFO()

#define arginfo_xz_encode_get_properties arginfo_xz_encode_finish

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_xz_decode_init, 0, 0, XZDecodeContext, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, format, IS_LONG, 0, "XZ_FORMAT_XZ")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xz_decode_add, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, context, XZDecodeContext, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xz_decode_finish, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, context, XZDecodeContext, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_xz_decode_get_status, 0, 1, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, context, XZDecodeContext, 0)
ZEND_END_ARG_INFO()

#define arginfo_xz_decode_get_read_len arginfo_xz_decode_get_status

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
