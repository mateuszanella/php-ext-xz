--TEST--
Incremental xz encode: error on invalid check and level. (PHP 7)
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
if (PHP_VERSION_ID >= 80000) {
	die('skip for PHP 7 only');
}
?>
--FILE--
<?php
echo "*** Testing xz_encode_init() : error conditions ***\n";

echo "\n-- Testing with invalid check --\n";
var_dump(xz_encode_init(999));

echo "\n-- Testing with invalid level --\n";
var_dump(xz_encode_init(XZ_CHECK_CRC64, ['level' => 100]));

echo "\n-- Testing with wrong context type --\n";
@xz_encode_add(new stdClass, "test");
echo "E_WARNING expected\n";
?>
--EXPECTF--
*** Testing xz_encode_init() : error conditions ***

-- Testing with invalid check --

Warning: xz_encode_init(): check must be a valid XZ_CHECK_* constant in %s
bool(false)

-- Testing with invalid level --

Warning: xz_encode_init(): compression level must be between 0 and 9 in %s
bool(false)

-- Testing with wrong context type --
E_WARNING expected
