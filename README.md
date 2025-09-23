# php-ext-xz

PHP Extension providing XZ (LZMA2) compression/decompression functions.

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
xz.decode_memory_limit=65536
```

## Build from Source

For detailed build and installation instructions from source, please see [docs/BUILD.md](docs/BUILD.md).

## Functions

### `string xzencode(string $data, int $preset = 6)`

Compresses a string using XZ compression.

- `$data`: The string to be compressed.
- `$preset`: Compression preset level. Can range from 0 to 9. If not provided, the value from `xz.compression_level` in `php.ini` will be used.

### `string xzdecode(string $data, int $memory_limit = 100000000)`

Decompresses a string that was compressed using XZ compression.

- `$data`: The string to be decompressed.
- `$memory_limit`: The maximum amount of memory to be used for decompression. If not provided, the value from `xz.decode_memory_limit` in `php.ini` will be used.

### `resource|false xzopen(string $path, string $mode)`

Opens an XZ-compressed file.

- `$path`: The path to the file.
- `$mode`: The mode in which to open the file (`'r'` for reading, `'w'` for writing).

Returns a file handle resource on success, or `false` on failure.

### `int xzwrite(resource $stream, string $data)`

Writes to an open XZ file handle.

- `$stream`: The file handle resource from `xzopen()`.
- `$data`: The string to be written.

Returns the number of bytes written.

### `void xzclose(resource $stream)`

Closes an open XZ file handle.

- `$stream`: The file handle resource to close.

### `void xzpassthru(resource $stream)`

Outputs all remaining data from an XZ file handle.

- `$stream`: The file handle resource.

## Basic usage

### String-based operations

You can easily compress and decompress strings.

```php
$originalString = 'This is a test string that will be compressed and then decompressed.';

// Compress the string with default compression level (6)
$compressed = xzencode($originalString);

echo "Original size: " . strlen($originalString) . " bytes\n";
echo "Compressed size: ". strlen($compressed) . " bytes\n";

// Decompress the string
$decompressed = xzdecode($compressed);

assert($originalString === $decompressed);

// Using a higher compression level (e.g., 9) for better compression ratio
$compressed_high = xzencode($originalString, 9);
echo "Compressed size (level 9): ". strlen($compressed_high) . " bytes\n";
```

### File-based operations

The extension also supports stream-based operations for working with `.xz` files.

```php
$file = '/tmp/test.xz';
$dataToWrite = 'Data you would like compressed and written to a file. This can be a very long string.';

// Writing to an .xz file
$wh = xzopen($file, 'w');
if ($wh) {
    xzwrite($wh, $dataToWrite);
    xzclose($wh);
    echo "Successfully wrote to $file\n";
} else {
    die("Failed to open $file for writing.");
}

// Reading from an .xz file
$rh = xzopen($file, 'r');
if ($rh) {
    echo "Reading from $file:\n";
    // xzpassthru outputs the data directly to the output buffer
    xzpassthru($rh);
    xzclose($rh);
} else {
    die("Failed to open $file for reading.");
}

// Clean up the file
unlink($file);
```

## Credits

This repository is a fork from [php-ext-xz by codemasher](https://github.com/codemasher/php-ext-xz), originally forked from the [RFC](https://news-web.php.net/php.internals/106654).

You can see the full list of contributors [here](https://github.com/mateuszanella/php-ext-xz/graphs/contributors).
