<?php
header("Content-Type: text/plain");
$filename = "data.csv";
$content = "unixtime,voltage\n";
$backupname = "data-backup-" . time() . ".csv";
copy($filename, $backupname);
$file = fopen($filename,"w");
fwrite($file, $content);
fclose($file);
print("Data Reset.");
?>
