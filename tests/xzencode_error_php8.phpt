--TEST--
Test `xzencode`: error conditions.
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
echo "*** Testing xzencode() : error conditions ***\n";

$data = 'string_val';

echo "\n-- Testing with larger than 9 compression level --\n";
try {
    var_dump(xzencode($data, 99));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\n-- Testing with lower than -1 compression level --\n";
try {
    var_dump(xzencode($data, -99));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\n-- Testing with invalid format --\n";
try {
    var_dump(xzencode($data, -1, 999));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing xzencode() : error conditions ***

-- Testing with larger than 9 compression level --
xzencode(): Argument #2 ($level) must be between -1 and 9

-- Testing with lower than -1 compression level --
xzencode(): Argument #2 ($level) must be between -1 and 9

-- Testing with invalid format --
xzencode(): Argument #3 ($format) must be XZ_FORMAT_XZ or XZ_FORMAT_RAW
