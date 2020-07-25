<?php
$filename = "data.csv";
$file = fopen($filename,"r");
$rows = file($filename);
$last_row = array_pop($rows);
$data = str_getcsv($last_row);
header("Content-Type: text/plain");
print($data[1]);
fclose($file);
?>