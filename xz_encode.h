#ifndef PHP_XZ_ENCODE_H
#define PHP_XZ_ENCODE_H

#include <lzma.h>
#include "php_xz.h"

zend_string *php_xz_encode_string(const uint8_t *data, size_t data_len, uint32_t level);
int php_xz_encoder_init_stream(lzma_stream *strm, uint32_t level);

#endif /* PHP_XZ_ENCODE_H */
