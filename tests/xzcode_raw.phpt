--TEST--
Test `xzencode`/`xzdecode`: raw LZMA2 format and memory_limit parameter.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$data = 'Raw one-shot LZMA2 round-trip';

// Level 1 preset uses a 1 MiB dictionary.
$raw = xzencode($data, 1, XZ_FORMAT_RAW);
var_dump(is_string($raw));

$ctx = xz_decode_init(XZ_FORMAT_RAW, ['filter' => XZ_FILTER_LZMA2, 'dict_size' => 1 << 20]);
$decoded  = xz_decode_add($ctx, $raw);
$decoded .= xz_decode_finish($ctx);
var_dump($decoded === $data);

// xz container still round-trips through the one-shot functions.
$xz = xzencode($data, 1, XZ_FORMAT_XZ);
var_dump(xzdecode($xz) === $data);

// memory_limit parameter: -1 (INI default) and 0 (unlimited).
var_dump(xzdecode($xz, -1) === $data);
var_dump(xzdecode($xz, 0) === $data);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
