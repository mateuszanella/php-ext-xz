--TEST--
Test `xzopen`: valid and invalid modes.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$filename = __DIR__ . '/test_xzopen_modes.tmp.xz';

// Write mode should work
$w = xzopen($filename, 'w');
var_dump(is_resource($w));
xzwrite($w, 'test data');
xzclose($w);
var_dump(file_exists($filename));

// Read mode should work
$r = xzopen($filename, 'r');
var_dump(is_resource($r));
$data = xzread($r, 1024);
var_dump($data);
xzclose($r);

// Cleanup
unlink($filename);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(9) "test data"
