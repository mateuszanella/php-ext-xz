--TEST--
Incremental xz encode: options array.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$input = "Hello World options test";

$ctx = xz_encode_init(XZ_FORMAT_XZ, ['check' => XZ_CHECK_CRC32, 'level' => 1]);
$out  = xz_encode_add($ctx, $input);
$out .= xz_encode_finish($ctx);

$dctx = xz_decode_init();
$decoded = xz_decode_add($dctx, $out);
$decoded .= xz_decode_finish($dctx);

var_dump($decoded === $input);

$ctx2 = xz_encode_init(XZ_FORMAT_XZ, ['check' => XZ_CHECK_SHA256]);
$out2  = xz_encode_add($ctx2, $input);
$out2 .= xz_encode_finish($ctx2);
$dctx2 = xz_decode_init();
$decoded2 = xz_decode_add($dctx2, $out2);
$decoded2 .= xz_decode_finish($dctx2);
var_dump($decoded2 === $input);
?>
--EXPECT--
bool(true)
bool(true)
