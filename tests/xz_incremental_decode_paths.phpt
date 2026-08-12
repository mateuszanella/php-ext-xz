--TEST--
Incremental xz decode: all three decode paths produce identical output.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$input = str_repeat("Different decode paths should agree. ", 300);
$compressed = xzencode($input);

$one_shot = xzdecode($compressed);

$dctx = xz_decode_init();
$single_pass = xz_decode_add($dctx, $compressed);
$single_pass .= xz_decode_finish($dctx);

$chunk_size = 73;
$dctx2 = xz_decode_init();
$chunked = "";
$offset = 0;
$target = strlen($compressed);
while ($offset < $target) {
	$chunk = substr($compressed, $offset, $chunk_size);
	$offset += $chunk_size;
	$chunked .= xz_decode_add($dctx2, $chunk);
}
$chunked .= xz_decode_finish($dctx2);

var_dump($one_shot === $single_pass);
var_dump($single_pass === $chunked);
var_dump($chunked === $input);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
