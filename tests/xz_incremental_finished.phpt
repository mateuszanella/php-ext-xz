--TEST--
Incremental xz encode: context already finished.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$ctx = xz_encode_init();
xz_encode_add($ctx, "hello");
xz_encode_finish($ctx);

$result = @xz_encode_add($ctx, "more");
var_dump($result);

$result2 = @xz_encode_finish($ctx);
var_dump($result2);

$ctx2 = xz_encode_init();
$out = xz_encode_add($ctx2, "data");
$out .= xz_encode_finish($ctx2);

$dctx = xz_decode_init();
xz_decode_add($dctx, $out);

$result3 = @xz_decode_add($dctx, "more");
var_dump($result3);

$result4 = xz_decode_finish($dctx);
var_dump($result4);
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
string(0) ""
