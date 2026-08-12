--TEST--
Incremental xz encode/decode: decode status and read_len.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$ctx = xz_encode_init();
$out = xz_encode_add($ctx, "hello");
$out .= xz_encode_add($ctx, " world");
$out .= xz_encode_finish($ctx);

$dctx = xz_decode_init();
var_dump(xz_decode_get_status($dctx)); // XZ_OK = 0

$decoded = xz_decode_add($dctx, $out);
var_dump(xz_decode_get_status($dctx)); // XZ_STREAM_END = 1
var_dump(xz_decode_get_read_len($dctx) > 0);
var_dump($decoded === "hello world");
?>
--EXPECT--
int(0)
int(1)
bool(true)
bool(true)
