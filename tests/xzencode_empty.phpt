--TEST--
Test `xzencode` and `xzdecode`: empty string input.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$encoded = xzencode('');
var_dump($encoded);
$decoded = xzdecode($encoded);
var_dump($decoded);
?>
--EXPECTF--
string(%d) "%s"
string(0) ""
