--TEST--
Incremental xz encode/decode: empty input.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$ctx = xz_encode_init();
$encoded = xz_encode_finish($ctx);
$dctx = xz_decode_init();
$decoded = xz_decode_add($dctx, $encoded);
$decoded .= xz_decode_finish($dctx);

var_dump($encoded !== false);
var_dump($decoded !== false);
var_dump($decoded);
?>
--EXPECT--
bool(true)
bool(true)
string(0) ""
