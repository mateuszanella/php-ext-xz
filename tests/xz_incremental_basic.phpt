--TEST--
Incremental xz encode/decode: basic roundtrip.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$input = "Hello, World! This is a test of incremental xz compression.";

$ctx = xz_encode_init();
$encoded  = xz_encode_add($ctx, "Hello, ");
$encoded .= xz_encode_add($ctx, "World! ");
$encoded .= xz_encode_add($ctx, "This is a test of incremental xz compression.");
$encoded .= xz_encode_finish($ctx);

$dctx = xz_decode_init();
$decoded = xz_decode_add($dctx, $encoded);
$decoded .= xz_decode_finish($dctx);

var_dump($input === $decoded);
?>
--EXPECT--
bool(true)
