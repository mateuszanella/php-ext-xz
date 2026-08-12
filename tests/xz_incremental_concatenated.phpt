--TEST--
Incremental xz decode: concatenated streams with XZ_CONCATENATED flag.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$stream1 = xzencode('Part One: ');
$stream2 = xzencode('Part Two.');
$concatenated = $stream1 . $stream2;

// Without CONCATENATED flag it stops after first stream
$ctx_no_concat = xz_decode_init();
$result1 = xz_decode_add($ctx_no_concat, $concatenated);
$result1 .= xz_decode_finish($ctx_no_concat);
var_dump($result1 === 'Part One: ');

// With CONCATENATED flag it processes both streams
$ctx_concat = xz_decode_init(XZ_FORMAT_XZ, ['flags' => XZ_CONCATENATED]);
$result2 = xz_decode_add($ctx_concat, $concatenated);
$result2 .= xz_decode_finish($ctx_concat);
var_dump($result2 === 'Part One: Part Two.');
var_dump(xz_decode_get_status($ctx_concat) === 1);
var_dump(xz_decode_get_read_len($ctx_concat) === strlen($concatenated));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
