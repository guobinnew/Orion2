// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.Button = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	/////////////////////////////////////
	var pluginProto = cr.plugins_.Button.prototype;
		
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
			cr.logexport("[Orion 2] Button plugin not supported on this platform - the object will not be created");
			return;
		}
		
		this.isCheckbox = (this.properties[0] === 1);
		
		this.inputElem = document.createElement("input");
		
		if (this.isCheckbox)
			this.elem = document.createElement("label");
		else
			this.elem = this.inputElem;
			
		this.labelText = null;
		
		this.inputElem.type = (this.isCheckbox ? "checkbox" : "button");
		this.inputElem.id = this.properties[6];
		jQuery(this.elem).appendTo(this.runtime.canvasdiv ? this.runtime.canvasdiv : "body");
		
		if (this.isCheckbox)
		{
			jQuery(this.inputElem).appendTo(this.elem);
			this.labelText = document.createTextNode(this.properties[1]);
			jQuery(this.elem).append(this.labelText);
			
			this.inputElem.checked = (this.properties[7] !== 0);
			
			// Avoid yucky serif font for checkbox labels
			jQuery(this.elem).css("font-family", "sans-serif");
			
			// Allow setting width and height on label
			jQuery(this.elem).css("display", "inline-block");
			jQuery(this.elem).css("color", "black");
		}
		else
			this.inputElem.value = this.properties[1];
		
		this.elem.title = this.properties[2];
		this.inputElem.disabled = (this.properties[4] === 0);
		
		this.autoFontSize = (this.properties[5] !== 0);
		this.element_hidden = false;
		
		if (this.properties[3] === 0)
		{
			jQuery(this.elem).hide();
			this.visible = false;
			this.element_hidden = true;
		}
		
		this.inputElem.onclick = (function (self) {
			return function(e) {
				e.stopPropagation();
				
				self.runtime.isInUserInputEvent = true;
				self.runtime.trigger(cr.plugins_.Button.prototype.cnds.OnClicked, self);
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
			"tooltip": this.elem.title,
			"disabled": !!this.inputElem.disabled
		};
			
		if (this.isCheckbox)
		{
			o["checked"] = !!this.inputElem.checked;
			o["text"] = this.labelText.nodeValue;
		}
		else
		{
			o["text"] = this.elem.value;
		}
		
		return o;
	};
	
	instanceProto.loadFromJSON = function (o)
	{
		this.elem.title = o["tooltip"];
		this.inputElem.disabled = o["disabled"];
		
		if (this.isCheckbox)
		{
			this.inputElem.checked = o["checked"];
			this.labelText.nodeValue = o["text"];
		}
		else
		{
			this.elem.value = o["text"];
		}
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
		
		if (this.autoFontSize)
			jQuery(this.elem).css("font-size", ((this.layer.getScale(true) / this.runtime.devicePixelRatio) - 0.2) + "em");
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
			"title": "Button",
			"properties": [
				{"name": "Text", "value": this.isCheckbox ? this.labelText.nodeValue : this.elem.value},
				{"name": "Checked", "value": this.isCheckbox ? this.inputElem.checked : false, "readonly": !this.isCheckbox}
			]
		});
	};
	
	instanceProto.onDebugValueEdited = function (header, name, value)
	{
		if (name === "Text")
		{
			if (this.isCheckbox)
				this.labelText.nodeValue = value;
			else
				this.elem.value = value;
		}
		else if (name === "Checked" && this.isCheckbox)
		{
			this.inputElem.checked = value;
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
	
	Cnds.prototype.IsChecked = function ()
	{
		return this.isCheckbox && this.inputElem.checked;
	};
	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.SetText = function (text)
	{
		if (this.runtime.isDomFree)
			return;
		
		if (this.isCheckbox)
			this.labelText.nodeValue = text;
		else
			this.elem.value = text;
	};
	
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
	
	Acts.prototype.SetEnabled = function (en)
	{
		if (this.runtime.isDomFree)
			return;
		
		this.inputElem.disabled = (en === 0);
	};
	
	Acts.prototype.SetFocus = function ()
	{
		if (this.runtime.isDomFree)
			return;
		
		this.inputElem.focus();
	};
	
	Acts.prototype.SetBlur = function ()
	{
		if (this.runtime.isDomFree)
			return;
		
		this.inputElem.blur();
	};
	
	Acts.prototype.SetCSSStyle = function (p, v)
	{
		if (this.runtime.isDomFree)
			return;
			
		jQuery(this.elem).css(p, v);
	};
	
	Acts.prototype.SetChecked = function (c)
	{
		if (this.runtime.isDomFree || !this.isCheckbox)
			return;
			
		this.inputElem.checked = (c === 1);
	};
	
	Acts.prototype.ToggleChecked = function ()
	{
		if (this.runtime.isDomFree || !this.isCheckbox)
			return;
			
		this.inputElem.checked = !this.inputElem.checked;
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	pluginProto.exps = new Exps();

}());