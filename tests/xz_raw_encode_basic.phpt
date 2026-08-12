--TEST--
Raw LZMA2 encode/decode round-trip with explicit dictionary size.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$input = 'Raw LZMA2 stream round-trip test.';

$ctx = xz_encode_init(XZ_FORMAT_RAW, ['filter' => XZ_FILTER_LZMA2, 'dict_size' => 1 << 20]);
var_dump($ctx !== false);

$out  = xz_encode_add($ctx, $input);
$out .= xz_encode_finish($ctx);

$props = xz_encode_get_properties($ctx);
var_dump(is_string($props));
var_dump(strlen($props));

$dctx = xz_decode_init(XZ_FORMAT_RAW, ['filter' => XZ_FILTER_LZMA2, 'dict_size' => 1 << 20]);
var_dump($dctx !== false);

$decoded  = xz_decode_add($dctx, $out);
$decoded .= xz_decode_finish($dctx);
var_dump($decoded === $input);
?>
--EXPECT--
bool(true)
bool(true)
int(1)
bool(true)
bool(true)
