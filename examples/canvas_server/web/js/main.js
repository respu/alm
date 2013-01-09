require(['alm/serie'], function(serie)
{

/*
var previous = new Uint32Array(2);
previous[0] = 1;
previous[1] = 100;

var data = new Uint32Array(2);
data[0] = 500;
data[1] = 100;

ctx.translate(-100,0);

ctx.beginPath();
ctx.moveTo(previous[0],previous[1]);
ctx.arc(previous[0],previous[1],2, 0, 2 * Math.PI, false);
ctx.fill();

ctx.beginPath();
ctx.moveTo(previous[0],previous[1]);
ctx.lineTo(data[0],data[1]);
ctx.stroke();

ctx.beginPath();
ctx.moveTo(data[0],data[1]);
ctx.arc(data[0],data[1],2, 0, 2 * Math.PI, false);
ctx.fill();
*/

var ctx = document.getElementById('main_canvas').getContext('2d');

var x_max = 500;
var y_max = 500;

var now = new Date();
ctx.fillText(now.toTimeString(), 0, y_max);
ctx.fillText(now.toTimeString(), 400, y_max);

var s = new serie.Serie(ctx);

var f = new FileReader();
f.onload = function(e) {
  data = new Float64Array(e.target.result);

  var date = new Date(data[0]);

//  s.add(data);
//  s.render();
}

var w = new WebSocket("ws://localhost:1100");
w.onmessage = function(e) {
  f.readAsArrayBuffer(e.data);
}


/************************************************************
*                       WORKER
*************************************************************

var done=false;
var worker = new Worker('js/alm/live.js');
worker.addEventListener('message', function(e) {
  if(e.data[0] > 500 && !done)
  {
    ctx.putImageData(ctx.getImageData(300,0,700,500),0,0);
    done=true;
  }
  s.add(e.data);
  s.render();
}, false);
************************************************************/

});
