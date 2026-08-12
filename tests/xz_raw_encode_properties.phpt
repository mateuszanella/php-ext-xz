--TEST--
Raw encode: property bytes for known parameters and non-raw contexts.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
// LZMA2: 1-byte property = (log2(dict_size) - 12) * 2
$ctx = xz_encode_init(XZ_FORMAT_RAW, ['filter' => XZ_FILTER_LZMA2, 'dict_size' => 1 << 20]);
var_dump(bin2hex(xz_encode_get_properties($ctx)));

$ctx = xz_encode_init(XZ_FORMAT_RAW, ['filter' => XZ_FILTER_LZMA2, 'dict_size' => 1 << 23]);
var_dump(bin2hex(xz_encode_get_properties($ctx)));

// LZMA1: 5-byte property = (pb*5+lp)*9+lc, then dict_size LE
$ctx = xz_encode_init(XZ_FORMAT_RAW, ['filter' => XZ_FILTER_LZMA1, 'dict_size' => 1 << 20, 'lc' => 3, 'lp' => 0, 'pb' => 2]);
var_dump(bin2hex(xz_encode_get_properties($ctx)));

// xz container contexts have no properties
$ctx = xz_encode_init(XZ_FORMAT_XZ);
var_dump(@xz_encode_get_properties($ctx));
?>
--EXPECT--
string(2) "10"
string(2) "16"
string(10) "5d00001000"
bool(false)
