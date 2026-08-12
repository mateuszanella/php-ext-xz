--TEST--
Test `xzencode`: error conditions.
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
echo "*** Testing xzencode() : error conditions ***\n";

$data = 'string_val';
$level = 2;
$encoding_mode = FORCE_DEFLATE;

echo "\n-- Testing with larger than 9 compression level --\n";

var_dump(xzencode($data, 99));

echo "\n-- Testing with lower than -1 compression level --\n";

var_dump(xzencode($data, -99));

echo "\n-- Testing with invalid format --\n";

var_dump(xzencode($data, -1, 999));

?>
--EXPECTF--
*** Testing xzencode() : error conditions ***

-- Testing with larger than 9 compression level --

Warning: xzencode(): compression level must be between -1 and 9 in %s on line %d
bool(false)

-- Testing with lower than -1 compression level --

Warning: xzencode(): compression level must be between -1 and 9 in %s on line %d
bool(false)

-- Testing with invalid format --

Warning: xzencode(): format must be XZ_FORMAT_XZ or XZ_FORMAT_RAW in %s on line %d
bool(false)