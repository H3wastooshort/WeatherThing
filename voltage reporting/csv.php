<?php
$filename = "data.csv";
$file = fopen($filename,"r");
header("Content-Type: text/plain");
print(fread($file,filesize($filename)));
fclose($file);
?>