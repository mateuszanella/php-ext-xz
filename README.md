# php-ext-xz

PHP Extension providing XZ (LZMA2) compression and decompression, with support for
one-shot encoding, incremental streaming, and file operations via stream wrapper.

## Installation

The recommended way to install the extension is using [pie](https://www.php.net/manual/en/install.pie.intro.php):

```bash
pie install mateuszanella/php-ext-xz
```

This will download the source and compile the extension for your current PHP version. After installing the module, you may need to enable it in your `php.ini` file.

## Configuration

### php.ini

Add the following line to your `php.ini` configuration file:

```ini
extension=xz.so
```

You can also configure the default compression level and memory limit:

```ini

; Default compression level. Affects `xzencode` and `xzopen`,
; but only when the level was not specified. Default is 5.
xz.compression_level=5

; The maximum amount of memory that can be used when decompressing. Default is 0 (no limit).
xz.max_memory=65536
```

## Build from Source

For detailed build and installation instructions from source, please see [docs/BUILD.md](docs/BUILD.md).

## Basic usage

### String-based operations

You can easily compress and decompress strings.

```php
$originalString = 'This is a test string that will be compressed and then decompressed.';

// Compress a string
$compressed = xzencode($originalString);

// Decompress a string
$decompressed = xzdecode($compressed);

// With explicit level and format (raw LZMA2 stream, no container)
$raw = xzencode($originalString, 6, XZ_FORMAT_RAW);

// With an explicit decoder memory limit
$decompressed = xzdecode($compressed, 64 * 1024 * 1024);
```

### File-based operations

The extension also supports stream-based operations for working with `.xz` files.

```php
$file = '/tmp/test.xz';

// Writing to an .xz file
$wh = xzopen($file, 'w');
xzwrite($wh, 'Data to write');
xzclose($wh);

// Reading from an .xz file and outputting its contents
$rh = xzopen($file, 'r');
xzpassthru($rh);
xzclose($rh);
```

### Incremental streaming

For large data, network streams, or chunked processing, use the incremental API:

```php
$ctx = xz_encode_init();
$out  = xz_encode_add($ctx, 'Hello, ');
$out .= xz_encode_add($ctx, 'World!');
$out .= xz_encode_finish($ctx);

$ctx = xz_decode_init();
$original = xz_decode_add($ctx, $out);
```

### Raw LZMA streams

Besides the xz container, the incremental API can produce and consume bare
LZMA1/LZMA2 streams (`XZ_FORMAT_RAW`). This is useful when the stream needs a
custom container, for example embedding an archive. The `"filter"` option
selects the codec, and `xz_encode_get_properties()` returns the codec property
bytes:

```php
$ctx = xz_encode_init(XZ_FORMAT_RAW, ['filter' => XZ_FILTER_LZMA2, 'dict_size' => 1 << 20]);
$packed  = xz_encode_add($ctx, $data);
$packed .= xz_encode_finish($ctx);
$props   = xz_encode_get_properties($ctx); // 1 byte for LZMA2, 5 for LZMA1
```

See [docs/USAGE.md](docs/USAGE.md) for the full API reference, constant tables,
real-world wrapper patterns, and error handling details.

## Credits

This repository is a fork from [php-ext-xz by codemasher](https://github.com/codemasher/php-ext-xz), originally forked from the [RFC](https://news-web.php.net/php.internals/106654).

You can see the full list of contributors [here](https://github.com/mateuszanella/php-ext-xz/graphs/contributors).
