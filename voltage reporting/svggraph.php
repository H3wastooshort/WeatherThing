<!DOCTYPE html>
<head>
 <meta charset="utf-8">
 <meta http-equiv="X-UA-Compatible" content="IE=edge">
 <meta name="viewport" content="width=device-width, initial-scale=1">

 <meta http-equiv="refresh" content="60">

 <title>Voltage Graph</title>
 <meta name="description" content="A Voltage Curve not using JavaScript.">

<style>body {
  background-color: black;
}

div#graph_div {
  top: 0;
  bottom: 0;
  position: absolute;
  width: max-content; 
}

svg#graph {
  height: 100vh;
  width: max-content;
}</style>
</head>
<body>
<div id="graph_div">
<?php
//arduino style map()
function mapfloat(float $x, float $in_min, float $in_max, float $out_min, float $out_max) {
  return ($x - $in_min) * ($out_max - $out_min) / ($in_max - $in_min) + $out_min;
}

$file = fopen("data.csv", "r");
$data = array();
while (($col = fgetcsv($file, 100, ",")) !== FALSE) {
	array_push($data, $col);
}
fclose($file);
$datapoints = count($data);
$scale = 0.25;
if (isset($_GET['scale'])) {$scale = $_GET['scale'];}

$svgwidth = ($datapoints-1) * $scale;

print("<svg height=\"100%\" viewBox=\"0 0 $svgwidth 100\">\n");

for ($v=10.5; $v<15; $v+=0.5) {
    round($y = max(0, min(100, mapfloat($v, 10.4, 14.6, 100, 0))),3);
    $y2 = $y - 0.3;
	print("<line x1=\"0\" y1=\"$y\" x2=\"$svgwidth\" y2=\"$y\" class=\"y_line\" /><text class=\"y_label\" x=\"0\" y=\"$y2\">$v</text>\n");
}

print("\n");

$lastDay = 0;
$lastTS = $data[1][0];
$y_shift = false;
for ($p = 1; $p < $datapoints; $p++) {
	$currentDate = getdate($data[$p][0]);
	
	if ($lastDay != $currentDate['mday']) {
		$datestring = date(DATE_RFC822, $data[$p][0]);
		$x = ($p * $scale);
        $y = 90;
        if ($y_shift) { $y -= 3; }
        $y_shift = !$y_shift;
		print("<line x1=\"$x\" y1=\"0\" x2=\"$x\" y2=\"100\" class=\"x_line\" /><text class=\"x_label\" x=\"$x\" y=\"$y\">$datestring</text>\n");
		$lastDay = $currentDate['mday'];
	}

    if ($data[$p][0] > $lastTS) {
        $x = ($p * $scale);
        print("<line x1=\"$x\" y1=\"0\" x2=\"$x\" y2=\"100\" class=\"gap_line\" /><text class=\"gap_label\" x=\"$x\" y=\"10\">GAP IN DATA!</text>\n");
    }

    $lastTS = $data[$p][0] + 3600;
}


$maxOnDay = 0;
$minOnDay = 9999;
$lastDayX = 0;

for ($p = 1; $p < $datapoints; $p++) {
	$y =  round(max(0, min(100, mapfloat(floatval($data[$p][1]), 10.4, 14.6, 100, 0))),3);
    $x = $p * $scale;
	$currentDate = getdate($data[$p][0]);
	
    $maxOnDay = max($maxOnDay, $data[$p][1]);
    $minOnDay = min($minOnDay, $data[$p][1]);

	if ($lastDay != $currentDate['mday'] or $p == $datapoints-1) {
        //print minmax
		
        print("<text class=\"min_max_label\" x=\"$lastDayX\" y=\"5\">MAX $maxOnDay V\nMIN $minOnDay V</text>\n");

        $lastDay = $currentDate['mday'];
		$lastDayX = $x+1;
        $maxOnDay = 0;
        $minOnDay = 9999;
	}
}


print("\n<polyline points=\"");

for ($p = 1; $p < $datapoints; $p++) {
	$y =  round(max(0, min(100, mapfloat(floatval($data[$p][1]), 10.4, 14.6, 100, 0))),3);
    $x = $p * $scale;
    $ts = $data[$p][0];
    $v = $data[$p][1];
	print("$x,$y ");
}

print('" fill="none" stroke="lime" style="stroke-width: 0.1;"/>');

if ($data[$datapoints-1][0] < time() - 3600) {
        $x = (($datapoints - 1) * $scale);
        $h = round((time() - $data[$datapoints-1][0]) / 3600, 1);
        print("<line x1=\"$x\" y1=\"0\" x2=\"$x\" y2=\"100\" class=\"stale_line\" /><text class=\"stale_label\" x=\"$x\" y=\"80\">DATA STALE BY $h HOURS!</text>\n");
}
?>

<style>
.y_label {font: 3px monospace; fill: magenta; position: fixed; left: 0;}
.y_line {stroke-width: 0.1; stroke: magenta;}
.x_label {font: 2.25px monospace; fill: cyan;}
.x_line {stroke-width: 0.1; stroke: cyan;}
.gap_label {font: 2.25px monospace; fill: orange;}
.gap_line {stroke-width: 0.25; stroke: orange;}
.stale_label {font: 4px monospace; fill: red; text-anchor: end;}
.stale_line {stroke-width: 0.25; stroke: red;}
.min_max_line {stroke-width: 0.1; stroke: gray;}
.min_max_label {font: 2.25px monospace; fill: white;}
</style>
</svg>
</div>

<script>
var graph = document.getElementById("graph");
window.scrollTo(graph.width, 0);


function b(ts,v) { //Datapoint pop-up box
    let date = new Date();
    date.setTime(ts * 1000);
    let text = "Unix Timestamp: ";
    text += ts;
    text += "\nDate: ";
    text += date.toString();
    text += "\nVoltage: ";
    text += v;
    alert(text);   
});
</script>

</body>
</html>