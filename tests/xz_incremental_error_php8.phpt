--TEST--
Incremental xz encode: error on invalid format, check and level. (PHP 8+)
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
if (PHP_VERSION_ID < 80000) {
	die('skip for PHP 8+ only');
}
?>
--FILE--
<?php
try {
	xz_encode_init(999);
} catch (\ValueError $e) {
	echo $e->getMessage() . \PHP_EOL;
}

try {
	xz_encode_init(XZ_FORMAT_XZ, ['level' => 100]);
} catch (\ValueError $e) {
	echo $e->getMessage() . \PHP_EOL;
}

try {
	xz_encode_init(XZ_FORMAT_XZ, ['check' => 999]);
} catch (\ValueError $e) {
	echo $e->getMessage() . \PHP_EOL;
}

try {
	xz_encode_init(XZ_FORMAT_RAW, ['filter' => 12345]);
} catch (\ValueError $e) {
	echo $e->getMessage() . \PHP_EOL;
}

try {
	xz_encode_add(new stdClass, "test");
} catch (\TypeError $e) {
	echo strpos($e->getMessage(), "XZEncodeContext") !== false ? "TypeError for wrong class\n" : "unexpected\n";
}
?>
--EXPECT--
xz_encode_init(): Argument #1 ($format) must be XZ_FORMAT_XZ or XZ_FORMAT_RAW
xz_encode_init(): Argument #2 ($options) options['level'] must be between 0 and 9
xz_encode_init(): Argument #2 ($options) options['check'] must be a valid XZ_CHECK_* constant
xz_encode_init(): Argument #2 ($options) options['filter'] must be XZ_FILTER_LZMA1 or XZ_FILTER_LZMA2
TypeError for wrong class
