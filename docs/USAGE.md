# Usage

This extension provides xz (LZMA2) compression and decompression through
one-shot functions, an incremental streaming API, and a stream wrapper for
file operations.

## One-shot Functions

### `xzencode`

Encodes a string with xz (LZMA2) compression.

```php
xzencode(string $str, ?int $compression_level = null): string|false
```

**Parameters**

| Parameter            | Description                                                                                                                                                                                      |
| -------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `$str`               | The uncompressed input data.                                                                                                                                                                     |
| `$compression_level` | Compression level (0–9). `null` or omitted uses the [`xz.compression_level`](#runtime-configuration) INI setting (default 5). Higher levels produce smaller output but use more time and memory. |

**Return Values**

Returns the xz-compressed string on success, or `false` on failure.

**Examples**

```php
$compressed = xzencode('Hello, World!');

// With explicit compression level
$compressed = xzencode($data, 9);
```

---

### `xzdecode`

Decodes an xz (LZMA2) compressed string.

```php
xzdecode(string $str): string|false
```

**Parameters**

| Parameter | Description                                      |
| --------- | ------------------------------------------------ |
| `$str`    | The xz-compressed input data. Must not be empty. |

**Return Values**

Returns the decompressed string on success, or `false` on failure (e.g. invalid or corrupt input).

**Examples**

```php
$original = xzdecode($compressed);
```

---

## Incremental Compression

When data is streamed or arrives in chunks, use the incremental API. It mirrors
the `deflate_init()` / `deflate_add()` pattern from ext/zlib. Each context is
single-use — it cannot be reused after finishing.

### `XZEncodeContext` class

An opaque object representing an incremental xz compression context.
Created by `xz_encode_init()`, consumed by `xz_encode_finish()` and `xz_encode_add()`.

### `xz_encode_init`

Initializes an incremental xz compression context.

```php
xz_encode_init(
    int $check = XZ_CHECK_CRC64,
    array $options = []
): XZEncodeContext|false
```

**Parameters**

| Parameter  | Description                                                                                                                                                                                                                                     |
| ---------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `$check`   | Integrity check type embedded in the xz stream. One of [`XZ_CHECK_NONE`](#predefined-constants), [`XZ_CHECK_CRC32`](#predefined-constants), [`XZ_CHECK_CRC64`](#predefined-constants) (default), or [`XZ_CHECK_SHA256`](#predefined-constants). |
| `$options` | Associative array of encoder options. Supported key: `"level"` (int, 0–9). Defaults to the [`xz.compression_level`](#runtime-configuration) INI setting.                                                                                        |

**Return Values**

Returns an `XZEncodeContext` object on success, or `false` on failure.
PHP 8.0+ throws `\ValueError` for invalid arguments.

**Examples**

```php
$ctx = xz_encode_init();                                 // CRC64, level from INI
$ctx = xz_encode_init(XZ_CHECK_SHA256);                  // SHA-256, level from INI
$ctx = xz_encode_init(XZ_CHECK_CRC64, ['level' => 9]);   // CRC64, maximum compression
```

---

### `xz_encode_add`

Feeds uncompressed data into an incremental compression context.

```php
xz_encode_add(XZEncodeContext $context, string $data): string|false
```

**Parameters**

| Parameter  | Description                                    |
| ---------- | ---------------------------------------------- |
| `$context` | A compression context from `xz_encode_init()`. |
| `$data`    | The uncompressed data chunk to compress.       |

**Return Values**

Returns the compressed output chunk on success. After `xz_encode_finish()` has
been called, returns `false` with a warning.

---

### `xz_encode_finish`

Finalizes an incremental compression stream.

```php
xz_encode_finish(XZEncodeContext $context): string|false
```

**Parameters**

| Parameter  | Description                                    |
| ---------- | ---------------------------------------------- |
| `$context` | A compression context from `xz_encode_init()`. |

**Return Values**

Returns the remaining compressed bytes on success. Returns `false` with a
warning if the context was already finished.

---

### Encode lifecycle

```
init → add* (zero or more) → finish → done
```

```php
$ctx = xz_encode_init();
$out  = xz_encode_add($ctx, 'Hello, ');
$out .= xz_encode_add($ctx, 'World!');
$out .= xz_encode_finish($ctx);
```

---

## Incremental Decompression

### `XZDecodeContext` class

An opaque object representing an incremental xz decompression context.
Created by `xz_decode_init()`. Single-stream decoders auto-finish —
`xz_decode_finish()` is only required with the `XZ_CONCATENATED` flag.

### `xz_decode_init`

Initializes an incremental xz decompression context.

```php
xz_decode_init(
    int $flags = 0,
    int $memory_limit = 0
): XZDecodeContext|false
```

**Parameters**

| Parameter       | Description                                                                                                                                                                                                                                          |
| --------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `$flags`        | Bitmask of [decoder flags](#predefined-constants). `0` for single-stream decoding. Use `XZ_CONCATENATED` for multiple concatenated streams, `XZ_FAIL_FAST` to stop immediately on corrupt data, or `XZ_IGNORE_CHECK` to skip integrity verification. |
| `$memory_limit` | Maximum memory (in bytes) the decoder may allocate. `0` means unlimited. Defaults to the [`xz.max_memory`](#runtime-configuration) INI setting.                                                                                                      |

**Return Values**

Returns an `XZDecodeContext` object on success, or `false` on failure.

**Examples**

```php
$ctx = xz_decode_init();                                  // single stream, unlimited memory
$ctx = xz_decode_init(XZ_CONCATENATED);                   // concatenated streams
$ctx = xz_decode_init(XZ_FAIL_FAST, 64 * 1024 * 1024);    // fail fast, 64 MB limit
$ctx = xz_decode_init(XZ_IGNORE_CHECK | XZ_CONCATENATED); // combined flags
```

---

### `xz_decode_add`

Feeds compressed data into an incremental decompression context.

```php
xz_decode_add(XZDecodeContext $context, string $data): string|false
```

**Parameters**

| Parameter  | Description                                      |
| ---------- | ------------------------------------------------ |
| `$context` | A decompression context from `xz_decode_init()`. |
| `$data`    | The compressed xz data chunk to decompress.      |

**Return Values**

Returns the decompressed output on success. Returns `false` with a warning
if the context has already finished or the data is corrupt.

---

### `xz_decode_finish`

Finalizes an incremental decompression stream.

```php
xz_decode_finish(XZDecodeContext $context): string|false
```

**Parameters**

| Parameter  | Description                                      |
| ---------- | ------------------------------------------------ |
| `$context` | A decompression context from `xz_decode_init()`. |

**Return Values**

For single-stream input, returns an empty string (the decoder auto-finishes).
For concatenated streams (`XZ_CONCATENATED`), returns any remaining output.
Returns `false` on error.

---

### `xz_decode_get_status`

Returns the current status of a decompression context.

```php
xz_decode_get_status(XZDecodeContext $context): int|false
```

**Parameters**

| Parameter  | Description                                      |
| ---------- | ------------------------------------------------ |
| `$context` | A decompression context from `xz_decode_init()`. |

**Return Values**

Returns the last lzma status code as an integer:

- `0` — ready for more data
- `1` — stream finished successfully
- `9` — data is corrupt

> Values match `LZMA_STATUS_*` constants. See LZMA documentation for details.

> TODO: Theese values could be documented and set as PHP constants.

Returns `false` if the context is invalid.

---

### `xz_decode_get_read_len`

Returns the number of compressed bytes consumed by the decoder.

```php
xz_decode_get_read_len(XZDecodeContext $context): int|false
```

**Parameters**

| Parameter  | Description                                      |
| ---------- | ------------------------------------------------ |
| `$context` | A decompression context from `xz_decode_init()`. |

**Return Values**

Returns the total bytes consumed from the compressed input. Useful for
detecting where the xz stream ends when the input contains trailing data.
Returns `false` if the context is invalid.

---

### Decode lifecycle

```
init → add* (zero or more, auto-ends at stream end) → optional finish → done
```

```php
$ctx = xz_decode_init();
$decoded  = xz_decode_add($ctx, $chunk1);
$decoded .= xz_decode_add($ctx, $chunk2);

$status = xz_decode_get_status($ctx);      // 1 when finished
$bytes  = xz_decode_get_read_len($ctx);    // compressed bytes consumed
```

---

## Stream Wrapper Functions

The extension registers the `compress.lzma` stream wrapper for transparent
xz compression on file operations. The following functions mirror the standard
`fopen()` / `fwrite()` / `fread()` / `fclose()` / `fpassthru()` API.

### `xzopen`

Opens an xz-compressed file for reading or writing.

```php
xzopen(string $filename, string $mode, int $compression_level = ?): resource|false
```

| Parameter            | Description                                                                                            |
| -------------------- | ------------------------------------------------------------------------------------------------------ |
| `$filename`          | The file path.                                                                                         |
| `$mode`              | `"r"`, `"w"`, `"rb"`, or `"wb"`.                                                                       |
| `$compression_level` | Compression level (0–9) for write modes. Defaults to [`xz.compression_level`](#runtime-configuration). |

**Return Values**

Returns a file pointer resource on success, or `false` on failure.

### `xzread`

Reads from an xz-compressed file stream. Alias of `fread()`.

```php
xzread(resource $fp, ?int $length = null): string|false
```

### `xzwrite`

Writes to an xz-compressed file stream. Alias of `fwrite()`.

```php
xzwrite(resource $fp, string $str, ?int $length = null): int|false
```

### `xzclose`

Closes an xz-compressed file stream. Alias of `fclose()`.

```php
xzclose(resource $fp): bool
```

### `xzpassthru`

Reads to EOF and writes to the output buffer. Alias of `fpassthru()`.

```php
xzpassthru(resource $fp): int|false
```

**Examples**

```php
// Writing
$fp = xzopen('/tmp/archive.xz', 'w');
xzwrite($fp, 'Data to compress');
xzclose($fp);

// Reading
$fp = xzopen('/tmp/archive.xz', 'r');
echo xzread($fp, 4096);
xzclose($fp);
```

---

## Predefined Constants

### Check types

Pass to `xz_encode_init()`.

| Constant          | Description                  |
| ----------------- | ---------------------------- |
| `XZ_CHECK_NONE`   | No integrity check.          |
| `XZ_CHECK_CRC32`  | CRC-32 (fast, 4 bytes).      |
| `XZ_CHECK_CRC64`  | CRC-64 (default, 8 bytes).   |
| `XZ_CHECK_SHA256` | SHA-256 (slowest, 32 bytes). |

### Decoder flags

Pass to `xz_decode_init()` as a bitmask.

| Constant          | Description                                |
| ----------------- | ------------------------------------------ |
| `XZ_FAIL_FAST`    | Report errors immediately on corrupt data. |
| `XZ_IGNORE_CHECK` | Skip integrity check verification.         |
| `XZ_CONCATENATED` | Accept multiple concatenated xz streams.   |

### Status codes

Returned by `xz_decode_get_status()`.

| Value | Description            |
| ----- | ---------------------- |
| `0`   | Ready for more data.   |
| `1`   | End of stream reached. |
| `9`   | Data is corrupt.       |

---

## Runtime Configuration

| INI setting            | Default         | Description                                                                                                        |
| ---------------------- | --------------- | ------------------------------------------------------------------------------------------------------------------ |
| `xz.compression_level` | `5`             | Default compression level (0–9) for `xzencode()` and `xz_encode_init()` when the level is not explicitly provided. |
| `xz.max_memory`        | `0` (unlimited) | Maximum memory (in bytes) the decoder may allocate. Used by `xz_decode_init()` when `$memory_limit` is omitted.    |

```ini
; php.ini
xz.compression_level = 6
xz.max_memory = 134217728  ; 128 MB
```

---

## Error Handling

| PHP Version | Invalid argument                     | I/O or lzma failure |
| ----------- | ------------------------------------ | ------------------- |
| 8.0+        | Throws `\ValueError` or `\TypeError` | Returns `false`     |
| 7.x         | `E_WARNING` + returns `false`        | Returns `false`     |

Contexts are single-use. After calling `xz_encode_finish()`, further calls to
`xz_encode_add()` or `xz_encode_finish()` return `false` with a warning.
After a decode context reaches end-of-stream (status `1`), further calls to
`xz_decode_add()` return `false` with a warning, while `xz_decode_finish()`
returns an empty string (silent no-op).
