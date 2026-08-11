--TEST--
Test `xzdecode`: invalid (non-xz) input.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
var_dump(xzdecode('this is not valid xz data at all'));
?>
--EXPECT--
bool(false)
