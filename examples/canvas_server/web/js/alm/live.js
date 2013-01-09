importScripts('../../../js/lib/require.js');
require({baseUrl : '../'}, [], function()
{
/*
var f = new FileReader();
f.onload = function(e) {
  data = new Uint32Array(e.target.result);
  webkitPostMessage(data);
}

var w = new WebSocket("ws://localhost:1100");
w.onmessage = function(e) {
  f.readAsArrayBuffer(e.data);
}
*/
});
