# Usage

This guide covers all features of the xz extension, from basic one-shot compression to
incremental streaming and file operations.

## One-shot compression

For simple cases where all data is in memory, use `xzencode()` and `xzdecode()`. These
are single-call functions that compress or decompress an entire string at once.

### Encoding

```php
$compressed = xzencode('Hello, World!');
// $compressed is a binary xz-compressed string

// With explicit compression level (0 = fastest, 9 = best compression):
$compressed = xzencode('Hello, World!', 6);
```

| Parameter | Type | Description |
|---|---|---|
| `$str` | `string` | The uncompressed input data. |
| `$compression_level` | `?int` | Level 0–9. When `null` or omitted, uses the `xz.compression_level` INI setting (default 5). |

Returns `string` on success, `false` on failure.

### Decoding

```php
$original = xzdecode($compressed);
// $original === 'Hello, World!'
```

| Parameter | Type | Description |
|---|---|---|
| `$str` | `string` | The xz-compressed input data. Must not be empty. |

Returns `string` on success, `false` on failure (e.g. invalid or corrupt input).

### Example: roundtrip

```php
$data = 'Some data to compress';
$compressed = xzencode($data);
$restored = xzdecode($compressed);

echo $data === $restored ? 'OK' : 'FAIL'; // OK
```

---

## Incremental compression

When data is streamed or arrives in chunks — network sockets, large files, generator
pipelines — use the incremental API. It mirrors PHP's `deflate_init()` / `deflate_add()`
pattern from ext/zlib.

### Encoding

```php
$ctx = xz_encode_init();                       // default: CRC64 check, INI compression level
$out  = xz_encode_add($ctx, 'Hello, ');
$out .= xz_encode_add($ctx, 'World!');
$out .= xz_encode_finish($ctx);                // flush remaining output, finalize stream

// $out is a complete xz-compressed string

// Don't reuse $ctx — it's consumed after finish().
```

**Lifecycle:** `init` → `add`* (0 or more) → `finish` → done

#### `xz_encode_init()`

```php
xz_encode_init(
    int $check = XZ_CHECK_CRC64,
    array $options = []
): XZEncodeContext|false
```

| Parameter | Type | Description |
|---|---|---|
| `$check` | `int` | Integrity check type. One of `XZ_CHECK_NONE`, `XZ_CHECK_CRC32`, `XZ_CHECK_CRC64` (default), `XZ_CHECK_SHA256`. |
| `$options` | `array` | Associative array. Supported key: `"level"` (int, 0–9). Defaults to `xz.compression_level` INI setting. |

```php
// Examples
$ctx = xz_encode_init();                                        // defaults: CRC64, level from INI
$ctx = xz_encode_init(XZ_CHECK_SHA256);                         // SHA-256 integrity, INI level
$ctx = xz_encode_init(XZ_CHECK_CRC64, ['level' => 3]);          // CRC64, fast compression
$ctx = xz_encode_init(XZ_CHECK_CRC64, ['level' => 9]);          // CRC64, maximum compression
```

Returns an `XZEncodeContext` object on success, `false` on failure (PHP 8+: throws
`\ValueError` for invalid arguments).

#### `xz_encode_add()`

```php
xz_encode_add(XZEncodeContext $context, string $data): string|false
```

Feeds uncompressed data into the encoder. Returns a compressed output chunk. After
`finish()` has been called, further calls return `false` with a warning.

#### `xz_encode_finish()`

```php
xz_encode_finish(XZEncodeContext $context): string|false
```

Flushes any buffered data and writes the stream footer. Returns the final compressed
bytes. The context must not be used after this call.

### Decoding

```php
$ctx = xz_decode_init();
$decoded  = xz_decode_add($ctx, $compressedChunk1);
$decoded .= xz_decode_add($ctx, $compressedChunk2);
// The decoder auto-finishes at XZ_STREAM_END — no finish() call needed for single streams.
$decoded .= xz_decode_finish($ctx);  // silent no-op here, but safe to call

$status = xz_decode_get_status($ctx);     // XZ_STREAM_END (1)
$consumed = xz_decode_get_read_len($ctx); // bytes consumed from compressed input
```

**Lifecycle:** `init` → `add`* (0 or more, auto-ends at stream end) → optional `finish` → done

#### `xz_decode_init()`

```php
xz_decode_init(
    int $flags = 0,
    int $memory_limit = 0
): XZDecodeContext|false
```

| Parameter | Type | Description |
|---|---|---|
| `$flags` | `int` | Bitmask of decoder flags (see [Constants](#decoder-flags)). Commonly `0` for single-stream or `XZ_CONCATENATED` for multi-stream input. |
| `$memory_limit` | `int` | Maximum memory in bytes the decoder may allocate, or `0` for unlimited. Defaults to `xz.max_memory` INI setting if omitted. |

```php
// Examples
$ctx = xz_decode_init();                               // single stream, unlimited memory
$ctx = xz_decode_init(XZ_CONCATENATED);                // support multiple concatenated .xz streams
$ctx = xz_decode_init(0, 64 * 1024 * 1024);            // single stream, 64 MB memory limit
$ctx = xz_decode_init(XZ_CONCATENATED | XZ_TELL_ANY_CHECK, 0);
```

Returns an `XZDecodeContext` object on success, `false` on failure.

#### `xz_decode_add()`

```php
xz_decode_add(XZDecodeContext $context, string $data): string|false
```

Feeds compressed data into the decoder. Returns a decompressed output chunk. The
decoder may buffer data internally until a complete block is available. After
`XZ_STREAM_END`, further calls return `false` with a warning.

#### `xz_decode_finish()`

```php
xz_decode_finish(XZDecodeContext $context): string|false
```

Signals that no more compressed data follows. For single-stream input, this is a
silent no-op (the decoder has already auto-finished). For concatenated streams
(`XZ_CONCATENATED` flag), this ensures the last stream is properly terminated.

#### `xz_decode_get_status()`

```php
xz_decode_get_status(XZDecodeContext $context): int|false
```

Returns the last lzma status code. Common values:
- `XZ_OK` (0) — ready for more data
- `XZ_STREAM_END` (1) — stream finished successfully

Other codes (e.g. `XZ_DATA_ERROR`) indicate problems. See [Constants](#status-codes).

#### `xz_decode_get_read_len()`

```php
xz_decode_get_read_len(XZDecodeContext $context): int|false
```

Returns the total number of compressed bytes consumed by the decoder so far. Useful
for detecting where the compressed stream ends when the input contains trailing data.

### Real-world pattern: wrapper class

```php
class XzCompressor
{
    private XZEncodeContext $ctx;

    public function __construct(int $level = 6)
    {
        $this->ctx = xz_encode_init(XZ_CHECK_CRC64, ['level' => $level]);
        if ($this->ctx === false) {
            throw new \RuntimeException('Failed to initialize xz compression context');
        }
    }

    public function compress(string $data): string
    {
        $result = xz_encode_add($this->ctx, $data);
        if ($result === false) {
            throw new \RuntimeException('Failed to compress data');
        }
        return $result;
    }

    public function finish(): string
    {
        $result = xz_encode_finish($this->ctx);
        if ($result === false) {
            throw new \RuntimeException('Failed to finish compression');
        }
        return $result;
    }
}
```

```php
class XzDecompressor
{
    private XZDecodeContext $ctx;

    public function __construct(int $flags = 0, int $memoryLimit = 0)
    {
        $this->ctx = xz_decode_init($flags, $memoryLimit);
        if ($this->ctx === false) {
            throw new \RuntimeException('Failed to initialize xz decompression context');
        }
    }

    public function decompress(string $data): string
    {
        $result = xz_decode_add($this->ctx, $data);
        if ($result === false) {
            throw new \RuntimeException('Failed to decompress data');
        }
        return $result;
    }

    public function finish(): string
    {
        $result = xz_decode_finish($this->ctx);
        if ($result === false) {
            throw new \RuntimeException('Failed to finish decompression');
        }
        return $result;
    }

    public function isDone(): bool
    {
        return xz_decode_get_status($this->ctx) === XZ_STREAM_END;
    }
}
```

### Example: streaming large files

```php
// Compress a large file in chunks
function compressFile(string $inputPath, string $outputPath, int $level = 5): void
{
    $in = fopen($inputPath, 'rb');
    $out = fopen($outputPath, 'wb');
    $ctx = xz_encode_init(XZ_CHECK_CRC64, ['level' => $level]);

    while (!feof($in)) {
        $chunk = fread($in, 8192);
        if ($chunk === false) {
            break;
        }
        fwrite($out, xz_encode_add($ctx, $chunk));
    }
    fwrite($out, xz_encode_finish($ctx));

    fclose($in);
    fclose($out);
}

// Decompress a large file in chunks
function decompressFile(string $inputPath, string $outputPath): void
{
    $in = fopen($inputPath, 'rb');
    $out = fopen($outputPath, 'wb');
    $ctx = xz_decode_init();

    while (!feof($in)) {
        $chunk = fread($in, 8192);
        if ($chunk === false) {
            break;
        }
        fwrite($out, xz_decode_add($ctx, $chunk));
    }
    fwrite($out, xz_decode_finish($ctx));

    fclose($in);
    fclose($out);
}
```

---

## Stream wrapper

The extension registers the `compress.lzma` stream wrapper for transparent xz
compression on file operations. The `xzopen()` / `xzwrite()` / `xzread()` /
`xzclose()` / `xzpassthru()` functions provide a low-level interface.

### Writing

```php
$fp = xzopen('/path/to/archive.xz', 'w');   // 'w' = write, 'wb' = binary write
xzwrite($fp, 'Data to store in the archive');
xzwrite($fp, 'More data');
xzclose($fp);
```

### Reading

```php
$fp = xzopen('/path/to/archive.xz', 'r');
while (!feof($fp)) {
    echo xzread($fp, 4096);
}
xzclose($fp);
```

```php
$fp = xzopen('/path/to/archive.xz', 'r');
xzpassthru($fp);   // reads entire file and sends it to output buffer
xzclose($fp);
```

### Mode strings

| Mode | Description |
|---|---|
| `"r"` | Open for reading. |
| `"w"` | Open for writing (compression). |
| `"rb"` | Open for reading (binary mode). |
| `"wb"` | Open for writing (binary mode). |

### Writing with custom compression level

```php
// 4th argument sets compression level
$fp = xzopen('/path/to/archive.xz', 'w', 9);
xzwrite($fp, 'Highly compressed data');
xzclose($fp);
```

---

## Constants reference

### Actions

Used with `xz_encode_add()` and `xz_decode_add()` when the optional third parameter
is provided (for advanced use). The default action when omitted is `XZ_RUN`.

| Constant | Value | Description |
|---|---|---|
| `XZ_RUN` | `0` | Continue processing. Default action for `add()`. |
| `XZ_SYNC_FLUSH` | `1` | Flush all pending output (degrades compression ratio). |
| `XZ_FULL_FLUSH` | `2` | Full flush with byte alignment (degrades ratio significantly). |
| `XZ_FULL_BARRIER` | `4` | Full flush with barrier marker. |
| `XZ_FINISH` | `3` | Finish the stream. Used internally by `finish()`. |

### Status codes

Returned by `xz_decode_get_status()`, or the last `status` property on a context.

| Constant | Value | Description |
|---|---|---|
| `XZ_OK` | `0` | Operation succeeded. Ready for more data. |
| `XZ_STREAM_END` | `1` | End of stream reached. |
| `XZ_NO_CHECK` | `2` | Input stream has no integrity check. |
| `XZ_UNSUPPORTED_CHECK` | `3` | Input stream uses an unsupported check type. |
| `XZ_GET_CHECK` | `4` | Integrity check type is available. |
| `XZ_MEM_ERROR` | `5` | Cannot allocate memory. |
| `XZ_MEMLIMIT_ERROR` | `6` | Memory limit was exceeded. |
| `XZ_FORMAT_ERROR` | `7` | File format not recognized. |
| `XZ_OPTIONS_ERROR` | `8` | Invalid or unsupported options. |
| `XZ_DATA_ERROR` | `9` | Data is corrupt. |
| `XZ_BUF_ERROR` | `10` | No progress is possible (e.g. needs more input space). |

### Check types

Pass to `xz_encode_init()` to select the integrity check embedded in the xz stream.

| Constant | Value | Description |
|---|---|---|
| `XZ_CHECK_NONE` | `0` | No integrity check. |
| `XZ_CHECK_CRC32` | `1` | CRC-32 (fast, 4 bytes). |
| `XZ_CHECK_CRC64` | `4` | CRC-64 (default, 8 bytes). |
| `XZ_CHECK_SHA256` | `10` | SHA-256 (slowest, 32 bytes). |

### Decoder flags

Pass to `xz_decode_init()` as a bitmask.

| Constant | Value | Description |
|---|---|---|
| `XZ_CONCATENATED` | `8` | Accept multiple concatenated xz streams. |
| `XZ_TELL_NO_CHECK` | `1` | Report if the stream has no integrity check. |
| `XZ_TELL_UNSUPPORTED_CHECK` | `2` | Report if the integrity check type is unsupported. |
| `XZ_TELL_ANY_CHECK` | `4` | Report even if the check type is supported. |
| `XZ_IGNORE_CHECK` | `16` | Don't verify the integrity check on decompression. |

### Preset constants

| Constant | Value | Description |
|---|---|---|
| `XZ_PRESET_DEFAULT` | `6` | The default compression preset. |
| `XZ_PRESET_EXTREME` | `2147483648` | Bit flag for extreme compression mode. |

---

## INI settings

| Setting | Default | Description |
|---|---|---|
| `xz.compression_level` | `5` | Default compression level for `xzencode()` and `xz_encode_init()` when the level is not explicitly provided. Range: 0–9. |
| `xz.max_memory` | `0` (unlimited) | Maximum memory (in bytes) the decoder may allocate. Used by `xz_decode_init()` when `$memory_limit` is omitted. |

```ini
; php.ini
xz.compression_level = 6
xz.max_memory = 134217728  ; 128 MB
```

---

## Error handling

### PHP 8.0+

Invalid arguments throw exceptions:

| Condition | Exception |
|---|---|
| Invalid check type in `xz_encode_init()` | `\ValueError` |
| Invalid level in `xz_encode_init()` | `\ValueError` |
| Wrong object type passed to `add()` | `\TypeError` |
| I/O or lzma operation failure | Returns `false` |

### PHP 7.x

Same conditions trigger `E_WARNING` and return `false`.

### Context lifecycle

| Context state | `add()` behavior | `finish()` behavior |
|---|---|---|
| Active (`XZ_OK`) | Processes data, returns output | Finalizes, returns trailing output |
| Finished (`XZ_STREAM_END`) | Encode: returns `false` + warning<br>Decode: returns `false` + warning | Encode: returns `false` + warning<br>Decode: returns `""` (silent no-op) |

The decode `finish()` is intentionally silent because single-stream decoders
auto-finish — calling `finish()` defensively shouldn't raise warnings.
