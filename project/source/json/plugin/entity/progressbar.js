// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.progressbar = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	/////////////////////////////////////
	var pluginProto = cr.plugins_.progressbar.prototype;
		
	/////////////////////////////////////
	// Object type class
	pluginProto.Type = function(plugin)
	{
		this.plugin = plugin;
		this.runtime = plugin.runtime;
	};

	var typeProto = pluginProto.Type.prototype;

	// called on startup for each object type
	typeProto.onCreate = function()
	{
	};

	/////////////////////////////////////
	// Instance class
	pluginProto.Instance = function(type)
	{
		this.type = type;
		this.runtime = type.runtime;
	};
	
	var instanceProto = pluginProto.Instance.prototype;

	// called whenever an instance is created
	instanceProto.onCreate = function()
	{
		// Not supported in directCanvas
		if (this.runtime.isDomFree)
		{
			cr.logexport("[Construct 2] Progress Bar plugin not supported on this platform - the object will not be created");
			return;
		}
		
		// elem must be the label wrapper if a checkbox, otherwise is same as input elem
		this.elem = document.createElement("progress");
		
		this.value = this.properties[0];
		this.max = this.properties[1];
		
		// Determinate
		if (this.max > 0 && this.value >= 0)
		{
			this.elem["max"] = this.max;
			this.elem["value"] = this.value;
		}
		
		this.elem.id = this.properties[4];
		this.elem.title = this.properties[2];
		jQuery(this.elem).appendTo(this.runtime.canvasdiv ? this.runtime.canvasdiv : "body");
		
		this.element_hidden = false;
		
		if (this.properties[3] === 0)
		{
			jQuery(this.elem).hide();
			this.visible = false;
			this.element_hidden = true;
		}
		
		this.elem.onclick = (function (self) {
			return function(e) {
				e.stopPropagation();
				
				self.runtime.isInUserInputEvent = true;
				self.runtime.trigger(cr.plugins_.progressbar.prototype.cnds.OnClicked, self);
				self.runtime.isInUserInputEvent = false;
			};
		})(this);
		
		// Prevent touches reaching the canvas
		this.elem.addEventListener("touchstart", function (e) {
			e.stopPropagation();
		}, false);
		
		this.elem.addEventListener("touchmove", function (e) {
			e.stopPropagation();
		}, false);
		
		this.elem.addEventListener("touchend", function (e) {
			e.stopPropagation();
		}, false);
		
		// Prevent clicks being blocked
		jQuery(this.elem).mousedown(function (e) {
			e.stopPropagation();
		});
		
		jQuery(this.elem).mouseup(function (e) {
			e.stopPropagation();
		});
		
		this.lastLeft = 0;
		this.lastTop = 0;
		this.lastRight = 0;
		this.lastBottom = 0;
		this.lastWinWidth = 0;
		this.lastWinHeight = 0;
			
		this.updatePosition(true);
		
		this.runtime.tickMe(this);
	};
	
	instanceProto.saveToJSON = function ()
	{
		var o = {
			"v": this.elem["value"],
			"m": this.elem["max"]
		};
		
		return o;
	};
	
	instanceProto.loadFromJSON = function (o)
	{
		this.elem["value"] = o["v"];
		this.elem["max"] = o["m"];
	};
	
	instanceProto.onDestroy = function ()
	{
		if (this.runtime.isDomFree)
			return;
		
		jQuery(this.elem).remove();
		this.elem = null;
	};
	
	instanceProto.tick = function ()
	{
		this.updatePosition();
	};
	
	var last_canvas_offset = null;
	var last_checked_tick = -1;
	
	instanceProto.updatePosition = function (first)
	{
		if (this.runtime.isDomFree)
			return;
		
		var left = this.layer.layerToCanvas(this.x, this.y, true);
		var top = this.layer.layerToCanvas(this.x, this.y, false);
		var right = this.layer.layerToCanvas(this.x + this.width, this.y + this.height, true);
		var bottom = this.layer.layerToCanvas(this.x + this.width, this.y + this.height, false);
		
		var rightEdge = this.runtime.width / this.runtime.devicePixelRatio;
		var bottomEdge = this.runtime.height / this.runtime.devicePixelRatio;
		
		// Is entirely offscreen or invisible: hide
		if (!this.visible || !this.layer.visible || right <= 0 || bottom <= 0 || left >= rightEdge || top >= bottomEdge)
		{
			if (!this.element_hidden)
				jQuery(this.elem).hide();
			
			this.element_hidden = true;
			return;
		}
		
		// Truncate to canvas size
		if (left < 1)
			left = 1;
		if (top < 1)
			top = 1;
		if (right >= rightEdge)
			right = rightEdge - 1;
		if (bottom >= bottomEdge)
			bottom = bottomEdge - 1;
		
		var curWinWidth = window.innerWidth;
		var curWinHeight = window.innerHeight;
			
		// Avoid redundant updates
		if (!first && this.lastLeft === left && this.lastTop === top && this.lastRight === right && this.lastBottom === bottom && this.lastWinWidth === curWinWidth && this.lastWinHeight === curWinHeight)
		{
			if (this.element_hidden)
			{
				jQuery(this.elem).show();
				this.element_hidden = false;
			}
			
			return;
		}
			
		this.lastLeft = left;
		this.lastTop = top;
		this.lastRight = right;
		this.lastBottom = bottom;
		this.lastWinWidth = curWinWidth;
		this.lastWinHeight = curWinHeight;
		
		if (this.element_hidden)
		{
			jQuery(this.elem).show();
			this.element_hidden = false;
		}
		
		var offx = Math.round(left) + jQuery(this.runtime.canvas).offset().left;
		var offy = Math.round(top) + jQuery(this.runtime.canvas).offset().top;
		jQuery(this.elem).css("position", "absolute");
		jQuery(this.elem).offset({left: offx, top: offy});
		jQuery(this.elem).width(Math.round(right - left));
		jQuery(this.elem).height(Math.round(bottom - top));
	};
	
	// only called if a layout object
	instanceProto.draw = function(ctx)
	{
	};
	
	instanceProto.drawGL = function(glw)
	{
	};
	
	/**BEGIN-PREVIEWONLY**/
	instanceProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": "Progress bar",
			"properties": [
				{"name": "Value", "value": this.elem["value"]},
				{"name": "Maximum", "value": this.elem["max"]}
			]
		});
	};
	
	instanceProto.onDebugValueEdited = function (header, name, value)
	{
		if (name === "Value")
		{
			this.elem["value"] = value;
		}
		else if (name === "Maximum")
		{
			this.elem["max"] = value;
		}
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};
	
	Cnds.prototype.OnClicked = function ()
	{
		return true;
	};
	
	Cnds.prototype.CompareProgress = function (cmp, x)
	{
		if (this.isDomFree)
			return false;
		
		return cr.do_cmp(this.elem["value"], cmp, x);
	};
	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.SetTooltip = function (text)
	{
		if (this.runtime.isDomFree)
			return;
		
		this.elem.title = text;
	};
	
	Acts.prototype.SetVisible = function (vis)
	{
		if (this.runtime.isDomFree)
			return;
		
		this.visible = (vis !== 0);
	};
	
	Acts.prototype.SetCSSStyle = function (p, v)
	{
		if (this.runtime.isDomFree)
			return;
			
		jQuery(this.elem).css(p, v);
	};
	
	Acts.prototype.SetProgress = function (x)
	{
		if (this.runtime.isDomFree)
			return;
		
		this.value = x;
		this.elem["max"] = this.max;
		this.elem["value"] = this.value;
	};
	
	Acts.prototype.SetMaximum = function (x)
	{
		if (this.runtime.isDomFree)
			return;
		
		this.max = x;
		this.elem["max"] = this.max;
		this.elem["value"] = this.value;
	};
	
	Acts.prototype.SetIndeterminate = function ()
	{
		if (this.runtime.isDomFree)
			return;
		
		this.elem.removeAttribute("value");
		this.elem.removeAttribute("max");
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.Progress = function (ret)
	{
		ret.set_float(this.runtime.isDomFree ? 0 : this.elem["value"]);
	};
	
	Exps.prototype.Maximum = function (ret)
	{
		ret.set_float(this.runtime.isDomFree ? 0 : this.elem["max"]);
	};
	
	pluginProto.exps = new Exps();

}());