#ifndef PHP_XZ_DECODE_H
#define PHP_XZ_DECODE_H

#include <lzma.h>
#include "php_xz.h"

zend_string *php_xz_decode_string(const uint8_t *data, size_t data_len, uint64_t memory_limit);
int php_xz_decoder_init_stream(lzma_stream *strm, uint64_t memory_limit);

#endif /* PHP_XZ_DECODE_H */
