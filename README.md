# php-ext-xz

## IMPORTANT NOTICE

THIS REPOSITORY WAS ORIGINALLY FORKED FROM: https://github.com/codemasher/php-ext-xz

The intent of this fork is to maintain and update the extension to better reflect the current PHP ecosystem.

I believe i will try to make it as similar to what the zlib extension provides, doing some alterations:
- make the xzopen and xzencode functions receive the compression level as a parameter, removing the global setting
- adding a incremental compression/decompression API (similar to deflate_init/inflate_init)

After theese are done, this library will be submitted to PECL and PIE.

PHP Extension providing XZ (LZMA2) compression/decompression functions.<br/>
(see [Implement lzma (xz?) compression](https://news-web.php.net/php.internals/106654))

## Build & Installation

### Linux

This module requires [`liblzma-dev`](https://packages.ubuntu.com/search?lang=de&keywords=liblzma-dev&searchon=names) (https://tukaani.org/xz/) as well as php7-dev or php8-dev.
If you are using Ubuntu, you can easily install all of them by typing the following command in your terminal:
```bash
sudo apt-get install git php7.4-dev liblzma-dev
```
To build and install as module, perform the following steps:
```bash
git clone https://github.com/codemasher/php-ext-xz.git
cd php-ext-xz
phpize
./configure
make
sudo make install
```

Do not forget to add `extension=xz.so` to your `php.ini`.

## Basic usage

```php
$fh = xzopen('/tmp/test.xz', 'w');
xzwrite($fh, 'Data you would like compressed and written.');
xzclose($fh);

$fh = xzopen('/tmp/test.xz', 'r');
xzpassthru($fh);
xzclose($fh);
```

```php
$str = 'Data you would like compressed.';

$encoded = xzencode($str);
$decoded = xzdecode($encoded);
```

## Disclaimer
May or may not contain bugs. Use at your own risk.
