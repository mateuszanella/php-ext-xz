# Build from Source

This document details how to build and install the `xz` extension from source.

## Building on Linux/macOS

### Requirements

This extension requires the `liblzma` library.

On PHP 8.x, the library is already included in the PHP source tree. You may need to manually install it for older PHP versions.

### Build Steps
```bash
git clone https://github.com/mateuszanella/php-ext-xz.git
cd php-ext-xz
phpize
./configure
make
sudo make install
```
After installation, add `extension=xz.so` to your `php.ini` file.

## Building on Windows

The canonical build process is defined in the `.github/workflows/build.yml` file, which is used for CI.
