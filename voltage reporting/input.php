<?php
$volt = $_GET["v"];
$time = strval(time());
$values = array($time,$volt);
$file = fopen("data.csv","a");
header("Content-Type: text/plain");
print(fputcsv($file,$values));
fclose($file);
?>