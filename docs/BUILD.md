# Build from Source

This document details how to build and install the `xz` extension from source. For most users, installing via `pecl` is recommended (see the main `README.md`).

## Building on Linux/macOS

### Requirements
- `git`
- `php-dev` (e.g., `php8.1-dev`)
- `liblzma-dev` (or `xz-devel` on CentOS/RHEL)

On a Debian-based distribution (like Ubuntu), you can install dependencies with:
```bash
sudo apt-get install git php-dev liblzma-dev
```

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

Building on Windows requires a specific development environment. The canonical build process is defined in the `.github/workflows/build.yml` file, which is used for CI.

### Requirements
- Visual Studio with C++ development tools
- The [PHP SDK for Windows](https://github.com/php/php-sdk-binary-tools)

### Build Steps
The general process involves:
1.  Setting up the PHP build environment using the PHP SDK.
2.  Ensuring `liblzma` is available. The workflow script in `.github/workflows/build.yml` downloads and compiles it.
3.  Running `phpize`, `configure --with-xz`, and `nmake` to build the extension.

The resulting `php_xz.dll` should be placed in your PHP extension directory (e.g., `ext`), and you should add `extension=xz` to your `php.ini`.
