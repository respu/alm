define([], function()
{
  var point = function(ctx, data)
  {
    this._ctx  = ctx;
    this.data = data;
  };

  point.prototype.render = function()
  {
    this._ctx.beginPath();
    this._ctx.moveTo(this.data[0],this.data[1]);
    this._ctx.arc(this.data[0],this.data[1],2, 0, 2 * Math.PI, false);
    this._ctx.fill();
  };

  var serie = function(ctx)
  {
    this._ctx      = ctx;
    this.previous = null;
    this.point    = null;
  };

  serie.prototype.add = function(data)
  {
    this.point = new point(this._ctx, data);
  };

  serie.prototype.render = function()
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

  serie.prototype._drawLine = function()
  {
    this._ctx.beginPath();
    this._ctx.moveTo(this.previous.data[0],this.previous.data[1]);
    this._ctx.lineTo(this.point.data[0],this.point.data[1]);
    this._ctx.stroke();
  }

  return{
    point : point,

    serie : serie
  };
  
});
