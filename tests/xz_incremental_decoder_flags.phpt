--TEST--
Incremental xz decode: FAIL_FAST and IGNORE_CHECK flags.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$valid = xzencode(str_repeat('decoder flags test. ', 100));

// FAIL_FAST: corrupt early byte should fail immediately
$corrupt = $valid;
$corrupt[10] = chr(ord($corrupt[10]) ^ 0xFF);
$ctx = xz_decode_init(XZ_FAIL_FAST);
$result = @xz_decode_add($ctx, $corrupt);
var_dump($result === false);
var_dump(xz_decode_get_status($ctx) !== 1);

// FAIL_FAST | CONCATENATED: flags can be OR'd
$ctx2 = xz_decode_init(XZ_FAIL_FAST | XZ_CONCATENATED);
$result2 = @xz_decode_add($ctx2, $corrupt);
var_dump($result2 === false);

// IGNORE_CHECK: does not break valid stream decoding
$ctx3 = xz_decode_init(XZ_IGNORE_CHECK);
$decoded = xz_decode_add($ctx3, $valid);
$decoded .= xz_decode_finish($ctx3);
var_dump(xz_decode_get_status($ctx3) === 1);
var_dump($decoded !== false);

// IGNORE_CHECK | CONCATENATED: combined flags
$ctx4 = xz_decode_init(XZ_IGNORE_CHECK | XZ_CONCATENATED);
$decoded4 = xz_decode_add($ctx4, $valid);
$decoded4 .= xz_decode_finish($ctx4);
var_dump(xz_decode_get_status($ctx4) === 1);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
