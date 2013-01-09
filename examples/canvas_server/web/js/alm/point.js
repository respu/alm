define([], function()
{
  var Point = function(ctx, data)
  {
    this._ctx  = ctx;
    this.data = data;
  };

  Point.prototype.render = function()
  {
    this._ctx.beginPath();
    this._ctx.moveTo(this.data[0],this.data[1]);
    this._ctx.arc(this.data[0],this.data[1],2, 0, 2 * Math.PI, false);
    this._ctx.fill();
  };

  return{
    Point : Point,
  };
  
});
