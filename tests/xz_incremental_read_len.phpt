--TEST--
Incremental xz decode: get_read_len accuracy (single-pass and chunked).
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$input = str_repeat('read_len accuracy test. ', 50);
$compressed = xzencode($input);
$compressed_len = strlen($compressed);

// Single-pass: read_len should equal total compressed length
$ctx = xz_decode_init();
xz_decode_add($ctx, $compressed);
var_dump(xz_decode_get_read_len($ctx) === $compressed_len);

// Chunked: read_len should increase incrementally
$chunk_size = 40;
$ctx2 = xz_decode_init();
$offset = 0;
$reads = [];
while ($offset < $compressed_len) {
	$chunk = substr($compressed, $offset, $chunk_size);
	$offset += $chunk_size;
	xz_decode_add($ctx2, $chunk);
	$reads[] = xz_decode_get_read_len($ctx2);
}
var_dump($reads[count($reads) - 1] === $compressed_len);

// read_len should be 0 before any data is fed
$ctx3 = xz_decode_init();
var_dump(xz_decode_get_read_len($ctx3) === 0);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
