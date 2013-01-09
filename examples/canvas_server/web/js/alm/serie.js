define(['alm/point'], function(point)
{
  var Serie = function(ctx)
  {
    this._ctx      = ctx;
    this.previous = null;
    this.point    = null;
  };

  Serie.prototype.add = function(data)
  {
    this.point = new point.Point(this._ctx, data);
  };

  Serie.prototype.render = function()
  {
    if(this.previous != null)
    {
      this._drawLine();
    }
    if(this.point != null)
    {
      this.point.render();
    }
    this.previous = this.point;
  };

  Serie.prototype._drawLine = function()
  {
    this._ctx.beginPath();
    this._ctx.moveTo(this.previous.data[0],this.previous.data[1]);
    this._ctx.lineTo(this.point.data[0],this.point.data[1]);
    this._ctx.stroke();
  }

  return{
    Serie : Serie
  };
  
});
