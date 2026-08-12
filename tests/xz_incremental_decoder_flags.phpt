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

// IGNORE_CHECK: does not break valid stream decoding
$ctx = xz_decode_init(XZ_IGNORE_CHECK);
var_dump($ctx !== false);
if ($ctx !== false) {
	$decoded = xz_decode_add($ctx, $valid);
	$decoded .= xz_decode_finish($ctx);
	var_dump(xz_decode_get_status($ctx) === 1);
	var_dump($decoded !== false);
}

// IGNORE_CHECK | CONCATENATED: combined flags
$ctx2 = xz_decode_init(XZ_IGNORE_CHECK | XZ_CONCATENATED);
var_dump($ctx2 !== false);
if ($ctx2 !== false) {
	$decoded2 = xz_decode_add($ctx2, $valid);
	$decoded2 .= xz_decode_finish($ctx2);
	var_dump(xz_decode_get_status($ctx2) === 1);
}

// FAIL_FAST: may be unsupported by older liblzma, accept false from init
$corrupt = $valid;
$corrupt[10] = chr(ord($corrupt[10]) ^ 0xFF);
$ctx3 = xz_decode_init(XZ_FAIL_FAST);
if ($ctx3 === false) {
	echo "FAIL_FAST unsupported by this liblzma build\n";
} else {
	$result = @xz_decode_add($ctx3, $corrupt);
	var_dump($result === false);
	var_dump(xz_decode_get_status($ctx3) !== 1);
}

// FAIL_FAST | CONCATENATED: ditto
$ctx4 = xz_decode_init(XZ_FAIL_FAST | XZ_CONCATENATED);
if ($ctx4 === false) {
	echo "FAIL_FAST|CONCATENATED unsupported by this liblzma build\n";
} else {
	$result2 = @xz_decode_add($ctx4, $corrupt);
	var_dump($result2 === false);
}
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
%a
