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

## Basic usage

### String-based operations

You can easily compress and decompress strings.

```php
$originalString = 'This is a test string that will be compressed and then decompressed.';

// Compress a string
$compressed = xzencode($originalString);

// Decompress a string
$decompressed = xzdecode($compressed);
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

// Clean up
unlink($file);
```

## Credits

This repository is a fork from [php-ext-xz by codemasher](https://github.com/codemasher/php-ext-xz), originally forked from the [RFC](https://news-web.php.net/php.internals/106654).

You can see the full list of contributors [here](https://github.com/mateuszanella/php-ext-xz/graphs/contributors).
