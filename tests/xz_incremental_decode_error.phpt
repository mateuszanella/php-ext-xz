--TEST--
Incremental xz decode: error on corrupted data.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$valid = xzencode('hello world');

// Corrupt the middle of the stream
$corrupt_mid = substr($valid, 0, 10) . str_repeat('X', 20) . substr($valid, 10);
$ctx = xz_decode_init();
$result = @xz_decode_add($ctx, $corrupt_mid);
var_dump($result === false);
var_dump(xz_decode_get_status($ctx)); // XZ_DATA_ERROR = 9

// Appended junk after valid stream: decoder ignores it (single stream mode)
$with_junk = $valid . 'garbage_after_stream';
$ctx2 = xz_decode_init();
$result2 = xz_decode_add($ctx2, $with_junk);
var_dump($result2 === 'hello world');
var_dump(xz_decode_get_status($ctx2)); // XZ_STREAM_END = 1

// With CONCATENATED flag, junk after valid stream should fail
$ctx3 = xz_decode_init(XZ_FORMAT_XZ, ['flags' => XZ_CONCATENATED]);
$result3 = @xz_decode_add($ctx3, $with_junk);
var_dump($result3 === false);
var_dump(xz_decode_get_status($ctx3)); // XZ_DATA_ERROR or similar
?>
--EXPECT--
bool(true)
int(9)
bool(true)
int(1)
bool(true)
int(9)
