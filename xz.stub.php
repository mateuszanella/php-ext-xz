<?php

/**
 * @generate-legacy-arginfo
 * @generate-class-entries
 * @undocumentable
 */

/**
 * Reads from an xz-compressed file stream.
 *
 * @param resource $fp     A valid file pointer returned by {@see xzopen()}.
 * @param int|null $length Maximum number of bytes to read. If null or not
 *                         specified, reads until end of file.
 * @return string|false The read data, or false on failure.
 *
 * @alias fread
 */
function xzread(resource $fp, ?int $length = UNKNOWN): string|false {}

/**
 * Writes to an xz-compressed file stream.
 *
 * @param resource   $fp     A valid file pointer returned by {@see xzopen()}.
 * @param string     $str    The string to write.
 * @param int|null   $length If specified, writing stops after length bytes
 *                           have been written or the end of string is reached.
 * @return int|false The number of bytes written, or false on failure.
 *
 * @alias fwrite
 */
function xzwrite(resource $fp, string $str, ?int $length = UNKNOWN): int|false {}

/**
 * Closes an xz-compressed file stream.
 *
 * @param resource $fp A valid file pointer returned by {@see xzopen()}.
 * @return bool True on success, false on failure.
 *
 * @alias fclose
 */
function xzclose(resource $fp): bool {}

/**
 * Reads from an xz-compressed file stream until EOF and writes the
 * result to the output buffer.
 *
 * @param resource $fp A valid file pointer returned by {@see xzopen()}.
 * @return int|false The number of characters read, or false on failure.
 *
 * @alias fpassthru
 */
function xzpassthru(resource $fp): int|false {}

/**
 * Encodes a string with xz (LZMA2) compression.
 *
 * @param string   $str               The uncompressed input data.
 * @param int|null $compression_level Compression level (0 &ndash; 9). If null,
 *                                    uses the `xz.compression_level` INI setting
 *                                    (default is 5). Higher levels produce
 *                                    smaller output but take more time and memory.
 * @return string|false The xz-compressed data, or false on failure.
 */
function xzencode(string $str, ?int $compression_level = null): string|false {}

/**
 * Decodes an xz (LZMA2) compressed string.
 *
 * @param string $str The xz-compressed input data. Must not be empty.
 * @return string|false The decompressed data, or false on failure (e.g.
 *                      invalid or corrupt input).
 */
function xzdecode(string $str): string|false {}

/**
 * Opens an xz-compressed file for reading or writing.
 *
 * @param string $filename         The file path.
 * @param string $mode             The fopen-compatible mode string
 *                                 (e.g. `"r"`, `"w"`, `"wb"`).
 * @param int    $compression_level Compression level (0 &ndash; 9) for write
 *                                 modes. Uses the `xz.compression_level` INI
 *                                 setting if omitted.
 * @return resource|false A file pointer resource, or false on failure.
 */
function xzopen(string $filename, string $mode, int $compression_level = UNKNOWN): resource|false {}

/**
 * Initializes an incremental compression context.
 *
 * Use {@see xz_encode_add()} to feed uncompressed data in chunks, and
 * {@see xz_encode_finish()} to finalize the stream.
 *
 * @param int   $format  The container format. One of {@see XZ_FORMAT_XZ}
 *                       (default) or {@see XZ_FORMAT_RAW}. The xz format
 *                       produces a self-contained xz stream; the raw format
 *                       produces a bare LZMA1/LZMA2 stream with no container,
 *                       header, or integrity check.
 * @param array $options An associative array of encoder options:
 *                       - `"check"` (int, xz only): the integrity check type
 *                         embedded in the xz stream. One of
 *                         {@see XZ_CHECK_NONE}, {@see XZ_CHECK_CRC32},
 *                         {@see XZ_CHECK_CRC64} (default), or
 *                         {@see XZ_CHECK_SHA256}.
 *                       - `"level"` (int, 0 &ndash; 9): compression level.
 *                         Defaults to the `xz.compression_level` INI setting.
 *                       - `"filter"` (int, raw only): the codec. One of
 *                         {@see XZ_FILTER_LZMA1} or
 *                         {@see XZ_FILTER_LZMA2} (default).
 *                       - `"dict_size"` (int): the dictionary size in bytes.
 *                       - `"lc"` (int, 0 &ndash; 4): number of literal context bits.
 *                       - `"lp"` (int, 0 &ndash; 4): number of literal position bits.
 *                       - `"pb"` (int, 0 &ndash; 4): number of position bits.
 * @return XZEncodeContext|false An encode context object, or false if
 *                               initialization fails.
 */
function xz_encode_init(int $format = XZ_FORMAT_XZ, array $options = []): XZEncodeContext|false {}

/**
 * Feeds uncompressed data into an incremental xz compression context.
 *
 * Each call consumes the given data chunk and returns any compressed output
 * available so far. When all data has been fed, call
 * {@see xz_encode_finish()} to flush any remaining output and finalize
 * the stream.
 *
 * @param XZEncodeContext $context A compression context from {@see xz_encode_init()}.
 * @param string          $data    The uncompressed data chunk to compress.
 * @return string|false The compressed output, or false on error (e.g.
 *                      the context has already been finished).
 */
function xz_encode_add(XZEncodeContext $context, string $data): string|false {}

/**
 * Finalizes an incremental xz compression stream.
 *
 * Flushes any buffered data and writes the stream footer. After this call,
 * the context is consumed and must not be used further.
 *
 * @param XZEncodeContext $context A compression context from {@see xz_encode_init()}.
 * @return string|false The remaining compressed output, or false on error
 *                      (e.g. the context was already finished).
 */
function xz_encode_finish(XZEncodeContext $context): string|false {}

/**
 * Returns the LZMA properties for a raw compression context.
 *
 * The returned bytes describe the codec configuration (dictionary size and,
 * for LZMA1, the literal/position bit counts) and can be stored alongside a
 * raw stream so that it can be decoded later. For LZMA2 the value is a single
 * byte; for LZMA1 it is five bytes.
 *
 * @param XZEncodeContext $context A compression context created with
 *                                 {@see XZ_FORMAT_RAW}.
 * @return string|false The property bytes, or false if the context was not
 *                      created with {@see XZ_FORMAT_RAW}.
 */
function xz_encode_get_properties(XZEncodeContext $context): string|false {}

/**
 * Initializes an incremental decompression context.
 *
 * Use {@see xz_decode_add()} to feed compressed data in chunks. For xz
 * streams the decoder automatically finishes when it reaches the end of a
 * complete stream; call {@see xz_decode_finish()} only when using the
 * {@see XZ_CONCATENATED} flag to process multiple concatenated streams.
 *
 * @param int   $format  The container format. One of {@see XZ_FORMAT_XZ}
 *                       (default) or {@see XZ_FORMAT_RAW}. The raw format
 *                       decodes a bare LZMA1/LZMA2 stream with no container.
 * @param array $options An associative array of decoder options:
 *                       - `"flags"` (int, xz only): a bitmask of decoder
 *                         flags. {@see XZ_FAIL_FAST} reports errors immediately
 *                         on corrupt data; {@see XZ_IGNORE_CHECK} skips
 *                         integrity verification; {@see XZ_CONCATENATED}
 *                         enables processing of multiple concatenated xz
 *                         streams. Use `0` for single-stream decoding.
 *                       - `"memory_limit"` (int, xz only): maximum memory (in
 *                         bytes) the decoder is allowed to allocate, or 0 for
 *                         unlimited. Defaults to the `xz.max_memory` INI
 *                         setting.
 *                       - `"filter"` (int, raw only): the codec. One of
 *                         {@see XZ_FILTER_LZMA1} or
 *                         {@see XZ_FILTER_LZMA2} (default).
 *                       - `"dict_size"` (int, raw only): the dictionary size
 *                         in bytes.
 *                       - `"lc"` (int, raw only, LZMA1): literal context bits.
 *                       - `"lp"` (int, raw only, LZMA1): literal position bits.
 *                       - `"pb"` (int, raw only, LZMA1): position bits.
 * @return XZDecodeContext|false A decode context object, or false if
 *                               initialization fails.
 */
function xz_decode_init(int $format = XZ_FORMAT_XZ, array $options = []): XZDecodeContext|false {}

/**
 * Feeds compressed xz data into an incremental decompression context.
 *
 * Each call consumes the given data chunk and returns any decompressed
 * output produced so far. The decoder may buffer data internally until
 * a complete block is available. When the end of the xz stream is
 * reached, the context status becomes `1`.
 *
 * @param XZDecodeContext $context A decompression context from {@see xz_decode_init()}.
 * @param string          $data    The compressed xz data chunk to decompress.
 * @return string|false The decompressed output, or false on error
 *                      (e.g. the context has already finished, or the
 *                      compressed data is corrupt).
 */
function xz_decode_add(XZDecodeContext $context, string $data): string|false {}

/**
 * Finalizes an incremental xz decompression stream.
 *
 * For single-stream xz data, this call is a no-op: the decoder
 * automatically finishes when the end of the stream is reached. For
 * concatenated streams (using the {@see XZ_CONCATENATED} flag), call this
 * after feeding all data to signal that no more compressed input follows.
 *
 * @param XZDecodeContext $context A decompression context from {@see xz_decode_init()}.
 * @return string|false The remaining decompressed output, or false on error.
 */
function xz_decode_finish(XZDecodeContext $context): string|false {}

/**
 * Returns the current status of an incremental decompression context.
 *
 * @param XZDecodeContext $context A decompression context from {@see xz_decode_init()}.
 * @return int|false The last lzma status code. `0` when ready for more
 *                   data, `1` when the stream has finished, `9` when
 *                   the data is corrupt. Returns false if the context
 *                   is invalid.
 */
function xz_decode_get_status(XZDecodeContext $context): int|false {}

/**
 * Returns the total number of compressed bytes consumed by the decoder.
 *
 * Useful for detecting where the compressed xz stream ends when the
 * input contains trailing data after the stream.
 *
 * @param XZDecodeContext $context A decompression context from {@see xz_decode_init()}.
 * @return int|false The number of bytes consumed, or false if the
 *                   context is invalid.
 */
function xz_decode_get_read_len(XZDecodeContext $context): int|false {}
