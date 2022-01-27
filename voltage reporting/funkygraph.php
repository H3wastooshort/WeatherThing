<!DOCTYPE html>
<head>
 <meta charset="utf-8">
 <meta http-equiv="X-UA-Compatible" content="IE=edge">
 <meta name="viewport" content="width=device-width, initial-scale=1">

 <meta http-equiv="refresh" content="60">

 <title>Voltage Graph</title>
 <meta name="description" content="A very horrible Voltage Curve but at leat it's not using JavaScript.">

<style>body {
  background: black;
  color: lime;
}

div#graph {
  height: 100vh;
  width: max-content;
}

#graph div {
  height: 100%;
  width: min-content;
  display: inline-block;
  width: 0.5px;
}

#graph div span {
  position: absolute;
}</style>
</head>
<body>
<div id="graph">
<?php
$file = fopen("data.csv", "r");
$data = array();
while (($col = fgetcsv($file, 100, ",")) !== FALSE) {
	array_push($data, $col);
}
fclose($file);

print("\n<!--\n");
var_dump($data);
print("\n-->\n");


//c++ style map()
function mapfloat(float $x, float $in_min, float $in_max, float $out_min, float $out_max) {
  return ($x - $in_min) * ($out_max - $out_min) / ($in_max - $in_min) + $out_min;
}

foreach ($data as $point) {
	$pos =  max(0, min(100, mapfloat(floatval($point[1]), 10.4, 14.6, 100, 0)));
	print("<div><span style=\"top: $pos%\">•</span></div>");
}
?>
</div>
</body>
