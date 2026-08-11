--TEST--
Test `xzdecode`: empty string input.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
var_dump(xzdecode(''));
?>
--EXPECT--
bool(false)
