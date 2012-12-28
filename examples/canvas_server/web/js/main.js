require([],function()
{

var begin = 100;
var end = begin + 10;
var ctx = document.getElementById('main_canvas').getContext('2d');

function drawShape(){
    ctx.beginPath();
    ctx.moveTo(begin,125);
    ctx.lineTo(end,125);
    ctx.stroke();
    begin+=10;
    end+=10;
}

var f = new FileReader();
f.onload = function(e) {
  var data = new Float32Array(e.target.result);
  console.log(data);
}

var w = new WebSocket("ws://localhost:1100");
w.onmessage = function(e) {
  drawShape();
  f.readAsArrayBuffer(e.data);
}

});
