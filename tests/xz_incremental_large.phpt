--TEST--
Incremental xz encode/decode: large input, chunked both directions.
--SKIPIF--
<?php
if (!extension_loaded("xz")) {
	die("skip XZ extension is not loaded!");
}
?>
--FILE--
<?php
$input = str_repeat("The quick brown fox jumps over the lazy dog. ", 500);
$chunk_size = 100;

$ctx = xz_encode_init();
$offset = 0;
$target = strlen($input);
$encoded = "";

while ($offset < $target) {
	$chunk = substr($input, $offset, $chunk_size);
	$offset += $chunk_size;
	$encoded .= xz_encode_add($ctx, $chunk);
}

$encoded .= xz_encode_finish($ctx);

$dctx = xz_decode_init();
$decoded = "";
$offset = 0;
$target = strlen($encoded);
while ($offset < $target) {
	$chunk = substr($encoded, $offset, $chunk_size);
	$offset += $chunk_size;
	$decoded .= xz_decode_add($dctx, $chunk);
}
$decoded .= xz_decode_finish($dctx);

var_dump(strlen($decoded) === strlen($input));
var_dump($decoded === $input);
?>
--EXPECT--
bool(true)
bool(true)
