# php-ext-xz

PHP Extension providing XZ (LZMA2) compression/decompression functions.

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

## Credits

This repository is a fork from [php-ext-xz by codemasher](https://github.com/codemasher/php-ext-xz), originally forked from the [RFC](https://news-web.php.net/php.internals/106654).

You can see the full list of contributors [here](https://github.com/mateuszanella/php-ext-xz/graphs/contributors).
