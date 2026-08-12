--TEST--
Incremental xz encode: all check types produce valid streams.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$input = 'Testing all xz integrity check types.';

foreach ([XZ_CHECK_NONE, XZ_CHECK_CRC32, XZ_CHECK_CRC64, XZ_CHECK_SHA256] as $check) {
	$ctx = xz_encode_init($check);
	$out = xz_encode_add($ctx, $input);
	$out .= xz_encode_finish($ctx);

	$dctx = xz_decode_init();
	$decoded = xz_decode_add($dctx, $out);
	$decoded .= xz_decode_finish($dctx);

	var_dump($decoded === $input);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
