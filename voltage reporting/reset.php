<?php
if (isset($_POST["reset"])) {
	header("Content-Type: text/plain");
	$filename = "data.csv";
	$content = "unixtime,voltage\n";
	$backupname = "data-backup-" . time() . ".csv";
	copy($filename, $backupname);
	$file = fopen($filename,"w");
	fwrite($file, $content);
	fclose($file);
	die("Data Reset.");
}
?>
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8"/>
<title>Solar Voltage History Reset</title>
</head>
<body>
<h1>Solar Voltage History Reset</h1>
<form method="POST" formaction="reset.php">
<input type="submit" name="reset" value="Reset History">
</form>
</body>
</html>
