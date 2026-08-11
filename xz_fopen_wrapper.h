#ifndef XZ_FOPEN_WRAPPER_H
#define XZ_FOPEN_WRAPPER_H

php_stream *php_stream_xzopen(php_stream_wrapper *wrapper,
    const char *path, const char *mode_pass, int options,
    zend_string **opened_path, php_stream_context *context STREAMS_DC);

#endif /* XZ_FOPEN_WRAPPER_H */
