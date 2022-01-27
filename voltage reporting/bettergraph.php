<!DOCTYPE html>
<head>
 <meta charset="utf-8">
 <meta http-equiv="X-UA-Compatible" content="IE=edge">
 <meta name="viewport" content="width=device-width, initial-scale=1">

 <meta http-equiv="refresh" content="60">

 <title>Voltage Graph</title>
 <meta name="description" content="A very horrible Voltage Curve.">

<style>body {
  background: black;
  color: lime;
  scrollbar-width: thin;
}

#graph {
  height: 100%;
  width: max-content;
  position: absolute;
  top: 0;
  bottom: 0;
  left: 0;
  right: 0;
}</style>
</head>
<body>
<canvas id="graph"></canvas>

<script>
const data = <?php
$file = fopen("data.csv", "r");
$data = array();
while (($col = fgetcsv($file, 100, ",")) !== FALSE) {
	array_push($data, $col);
}
fclose($file);

print(json_encode($data));
?>;

//c++ style map()
function mapfloat(x, in_min, in_max, out_min, out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

const canvas = document.getElementById('graph');
const ctx = canvas.getContext('2d');

canvas.width = data.length;
canvas.height = document.documentElement.clientHeight;
	
ctx.fillStyle = 'black';
ctx.fillRect(0,0,canvas.width,canvas.height);

ctx.lineWidth = 1;

var oldDay = 0;
var lastTime = 9999999999999999;
ctx.fillStyle = 'lime';
ctx.font = '16px monospace';

ctx.fillStyle = 'magenta'; //Draw voltage scale lines
for (var volt = 11; volt <= 14; volt++) {
	let y = mapfloat(volt, 10.4, 14.6, canvas.height, 0);
    ctx.fillRect(0,y, canvas.width,1);
    ctx.fillText(volt + "V", 3, y-10);
}

for (var x = 1; x < data.length; x+=1) {
    let date = new Date(); //Draw date lines
    date.setTime(data[x][0] * 1000);
    if (date.getDay() != oldDay) {
        ctx.fillStyle = 'cyan';
        ctx.fillRect(x,0,1,canvas.height);
        ctx.fillText(date.toDateString(), x+5, canvas.height-50);
        ctx.fillStyle = 'lime';
		oldDay = date.getDay();
    }
	
	if (data[x][0] - lastTime > 3600) {//Draw line for data gap
		ctx.fillStyle = 'orange';
		ctx.fillRect(x,0,1,canvas.height);
		ctx.fillText("GAP IN DATA", x+5, 100);
		ctx.fillStyle = 'lime';
	}
	lastTime = data[x][0];
	
	let y = mapfloat(data[x][1], 10.4, 14.6, canvas.height, 0); //Draw data point
	ctx.fillRect(x-1,y-1,3,3);
}

canvas.addEventListener("mousedown", function(e) { //Datapoint pop-up box
    //let x = e.clientX - canvas.getBoundingClientRect().left + 1;
	let x = e.offsetX;
    let date = new Date();
    date.setTime(data[x][0] * 1000);
    let text = "Unix Timestamp: ";
    text += data[x][0];
    text += "\nDate: ";
    text += date.toString();
    text += "\nVoltage: ";
    text += data[x][1];
    alert(text);   
});
</script>
</body>
