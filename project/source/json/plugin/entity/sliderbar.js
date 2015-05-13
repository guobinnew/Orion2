// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.sliderbar = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	/////////////////////////////////////
	var pluginProto = cr.plugins_.sliderbar.prototype;
		
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
			cr.logexport("[Construct 2] Slider Bar plugin not supported on this platform - the object will not be created");
			return;
		}
		
		this.elem = document.createElement("input");
		this.elem.type = "range";
		
		this.elem["max"] = this.properties[2];
		this.elem["min"] = this.properties[1];
		this.elem["step"] = this.properties[3];
		this.elem["value"] = this.properties[0];
		
		this.elem.disabled = (this.properties[6] === 0);
		this.elem.id = this.properties[7];
		this.elem.title = this.properties[4];
		jQuery(this.elem).appendTo(this.runtime.canvasdiv ? this.runtime.canvasdiv : "body");
		
		this.element_hidden = false;
		
		if (this.properties[5] === 0)
		{
			jQuery(this.elem).hide();
			this.visible = false;
			this.element_hidden = true;
		}
		
		this.elem.onclick = (function (self) {
			return function(e) {
				e.stopPropagation();
				
				self.runtime.isInUserInputEvent = true;
				self.runtime.trigger(cr.plugins_.sliderbar.prototype.cnds.OnClicked, self);
				self.runtime.isInUserInputEvent = false;
			};
		})(this);
		
		this.elem.onchange = (function (self) {
			return function(e) {
				self.runtime.isInUserInputEvent = true;
				self.runtime.trigger(cr.plugins_.sliderbar.prototype.cnds.OnChanged, self);
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
		
		// Prevent key presses being blocked by the Keyboard object
		jQuery(this.elem).keydown(function (e) {
			e.stopPropagation();
		});
		
		jQuery(this.elem).keyup(function (e) {
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
			"mi": this.elem["min"],
			"ma": this.elem["max"],
			"s": this.elem["step"]
		};
		
		return o;
	};
	
	instanceProto.loadFromJSON = function (o)
	{
		this.elem["min"] = o["mi"];
		this.elem["max"] = o["ma"];
		this.elem["step"] = o["s"];
		this.elem["value"] = o["v"];
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
			"title": "Slider bar",
			"properties": [
				{"name": "Value", "value": this.elem["value"]},
				{"name": "Minimum", "value": this.elem["min"]},
				{"name": "Maximum", "value": this.elem["max"]},
				{"name": "Step", "value": this.elem["step"]}
			]
		});
	};
	
	instanceProto.onDebugValueEdited = function (header, name, value)
	{
		if (name === "Value")
		{
			this.elem["value"] = value;
		}
		else if (name === "Minimum")
		{
			this.elem["min"] = value;
		}
		else if (name === "Maximum")
		{
			this.elem["max"] = value;
		}
		else if (name === "Step")
		{
			this.elem["step"] = value;
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
	
	Cnds.prototype.OnChanged = function ()
	{
		return true;
	};
	
	Cnds.prototype.CompareValue = function (cmp, x)
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
	
	Acts.prototype.SetValue = function (x)
	{
		if (this.runtime.isDomFree)
			return;
		
		this.elem["value"] = x;
	};
	
	Acts.prototype.SetMaximum = function (x)
	{
		if (this.runtime.isDomFree)
			return;
		
		this.elem["max"] = x;
	};
	
	Acts.prototype.SetMinimum = function (x)
	{
		if (this.runtime.isDomFree)
			return;
		
		this.elem["min"] = x;
	};
	
	Acts.prototype.SetStep = function (x)
	{
		if (this.runtime.isDomFree)
			return;
		
		this.elem["step"] = x;
	};
	
	Acts.prototype.SetEnabled = function (en)
	{
		if (this.runtime.isDomFree)
			return;
		
		this.elem.disabled = (en === 0);
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.Value = function (ret)
	{
		ret.set_float(this.runtime.isDomFree ? 0 : (parseFloat(this.elem["value"]) || 0));
	};
	
	Exps.prototype.Maximum = function (ret)
	{
		ret.set_float(this.runtime.isDomFree ? 0 : (parseFloat(this.elem["max"]) || 0));
	};
	
	Exps.prototype.Minimum = function (ret)
	{
		ret.set_float(this.runtime.isDomFree ? 0 : (parseFloat(this.elem["min"]) || 0));
	};
	
	Exps.prototype.Step = function (ret)
	{
		ret.set_float(this.runtime.isDomFree ? 0 : (parseFloat(this.elem["step"]) || 0));
	};
	
	pluginProto.exps = new Exps();

}());