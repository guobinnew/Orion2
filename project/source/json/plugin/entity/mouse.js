// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.Mouse = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var pluginProto = cr.plugins_.Mouse.prototype;
		
	/////////////////////////////////////
	// Object type class
	pluginProto.Type = function(plugin)
	{
		this.plugin = plugin;
		this.runtime = plugin.runtime;
	};

	var typeProto = pluginProto.Type.prototype;

	typeProto.onCreate = function()
	{
	};

	/////////////////////////////////////
	// Instance class
	pluginProto.Instance = function(type)
	{
		this.type = type;
		this.runtime = type.runtime;
		
		this.buttonMap = new Array(4);		// mouse down states
		this.mouseXcanvas = 0;				// mouse position relative to canvas
		this.mouseYcanvas = 0;
		
		this.triggerButton = 0;
		this.triggerType = 0;
		this.triggerDir = 0;
		this.handled = false;
	};

	var instanceProto = pluginProto.Instance.prototype;

	instanceProto.onCreate = function()
	{
		var self = this;
		
		// Bind mouse events via jQuery.  Not supported in DC
		if (!this.runtime.isDomFree)
		{
			jQuery(document).mousemove(
				function(info) {
					self.onMouseMove(info);
				}
			);
			
			jQuery(document).mousedown(
				function(info) {
					self.onMouseDown(info);
				}
			);
			
			jQuery(document).mouseup(
				function(info) {
					self.onMouseUp(info);
				}
			);
			
			jQuery(document).dblclick(
				function(info) {
					self.onDoubleClick(info);
				}
			);
			
			var wheelevent = function(info) {
								self.onWheel(info);
							};
							
			document.addEventListener("mousewheel", wheelevent, false);
			document.addEventListener("DOMMouseScroll", wheelevent, false);
		}
	};
	
	var dummyoffset = {left: 0, top: 0};

	instanceProto.onMouseMove = function(info)
	{
		var offset = this.runtime.isDomFree ? dummyoffset : jQuery(this.runtime.canvas).offset();
		this.mouseXcanvas = info.pageX - offset.left;
		this.mouseYcanvas = info.pageY - offset.top;
	};
	
	instanceProto.mouseInGame = function ()
	{
		if (this.runtime.fullscreen_mode > 0)
			return true;
			
		return this.mouseXcanvas >= 0 && this.mouseYcanvas >= 0
		    && this.mouseXcanvas < this.runtime.width && this.mouseYcanvas < this.runtime.height;
	};

	instanceProto.onMouseDown = function(info)
	{
		// Ignore mousedowns outside the canvas
		if (!this.mouseInGame())
			return;
		
		if (this.runtime.had_a_click && !this.runtime.isMobile)
			info.preventDefault();
		
		// Update button state
		this.buttonMap[info.which] = true;
		
		this.runtime.isInUserInputEvent = true;
		
		// Trigger OnAnyClick
		this.runtime.trigger(cr.plugins_.Mouse.prototype.cnds.OnAnyClick, this);
		
		// Trigger OnClick & OnObjectClicked
		this.triggerButton = info.which - 1;	// 1-based
		this.triggerType = 0;					// single click
		this.runtime.trigger(cr.plugins_.Mouse.prototype.cnds.OnClick, this);
		this.runtime.trigger(cr.plugins_.Mouse.prototype.cnds.OnObjectClicked, this);
		
		this.runtime.isInUserInputEvent = false;
	};

	instanceProto.onMouseUp = function(info)
	{
		// Ignore mouseup if didn't see a corresponding mousedown
		if (!this.buttonMap[info.which])
			return;
		
		if (this.runtime.had_a_click && !this.runtime.isMobile)
			info.preventDefault();
			
		this.runtime.had_a_click = true;
		
		// Update button state
		this.buttonMap[info.which] = false;
		
		this.runtime.isInUserInputEvent = true;
		
		// Trigger OnRelease
		this.triggerButton = info.which - 1;	// 1-based
		this.runtime.trigger(cr.plugins_.Mouse.prototype.cnds.OnRelease, this);
		
		this.runtime.isInUserInputEvent = false;
	};

	instanceProto.onDoubleClick = function(info)
	{
		// Ignore doubleclicks outside the canvas
		if (!this.mouseInGame())
			return;
			
		info.preventDefault();
		
		this.runtime.isInUserInputEvent = true;
		
		// Trigger OnClick & OnObjectClicked
		this.triggerButton = info.which - 1;	// 1-based
		this.triggerType = 1;					// double click
		this.runtime.trigger(cr.plugins_.Mouse.prototype.cnds.OnClick, this);
		this.runtime.trigger(cr.plugins_.Mouse.prototype.cnds.OnObjectClicked, this);
		
		this.runtime.isInUserInputEvent = false;
	};
	
	instanceProto.onWheel = function (info)
	{
		var delta = info.wheelDelta ? info.wheelDelta : info.detail ? -info.detail : 0;
		
		this.triggerDir = (delta < 0 ? 0 : 1);
		this.handled = false;
		
		this.runtime.isInUserInputEvent = true;
		
		this.runtime.trigger(cr.plugins_.Mouse.prototype.cnds.OnWheel, this);
		
		this.runtime.isInUserInputEvent = false;
		
		if (this.handled && cr.isCanvasInputEvent(info))
			info.preventDefault();
	};
	
	/**BEGIN-PREVIEWONLY**/
	instanceProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": "Mouse",
			"properties": [
				{"name": "Absolute position", "value": "(" + this.mouseXcanvas + ", " + this.mouseYcanvas + ")", "readonly": true},
				{"name": "Left button down", "value": !!this.buttonMap[1], "readonly": true},
				{"name": "Middle button down", "value": !!this.buttonMap[2], "readonly": true},
				{"name": "Right button down", "value": !!this.buttonMap[3], "readonly": true},
			]
		});
		
		var props = [], i, len, layer;
		for (i = 0, len = this.runtime.running_layout.layers.length; i < len; ++i)
		{
			layer = this.runtime.running_layout.layers[i];
			
			props.unshift({"name": layer.name, "value": "(" + layer.canvasToLayer(this.mouseXcanvas, this.mouseYcanvas, true) + ", " + layer.canvasToLayer(this.mouseXcanvas, this.mouseYcanvas, false) + ")", "readonly": true});
		}
		
		propsections.push({
			"title": "Mouse position on each layer",
			"properties": props
		});
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	Cnds.prototype.OnClick = function (button, type)
	{
		return button === this.triggerButton && type === this.triggerType;
	};
	
	Cnds.prototype.OnAnyClick = function ()
	{
		return true;
	};
	
	Cnds.prototype.IsButtonDown = function (button)
	{
		return this.buttonMap[button + 1];	// jQuery uses 1-based buttons for some reason
	};
	
	Cnds.prototype.OnRelease = function (button)
	{
		return button === this.triggerButton;
	};
	
	Cnds.prototype.IsOverObject = function (obj)
	{
		// We need to handle invert manually.  If inverted, turn invert off on the condition,
		// and instead pass it to testAndSelectCanvasPointOverlap() which does SOL picking
		// based on the invert status.
		var cnd = this.runtime.getCurrentCondition();

		var mx = this.mouseXcanvas;
		var my = this.mouseYcanvas;
		
		return cr.xor(this.runtime.testAndSelectCanvasPointOverlap(obj, mx, my, cnd.inverted), cnd.inverted);
	};
	
	Cnds.prototype.OnObjectClicked = function (button, type, obj)
	{
		if (button !== this.triggerButton || type !== this.triggerType)
			return false;	// wrong click type
		
		return this.runtime.testAndSelectCanvasPointOverlap(obj, this.mouseXcanvas, this.mouseYcanvas, false);
	};
	
	Cnds.prototype.OnWheel = function (dir)
	{
		this.handled = true;
		return dir === this.triggerDir;
	};
	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};
	
	Acts.prototype.SetCursor = function (c)
	{
		var cursor_style = ["auto", "pointer", "text", "crosshair", "move", "help", "wait", "none"][c];
		
		if (this.runtime.canvas && this.runtime.canvas.style)
			this.runtime.canvas.style.cursor = cursor_style;
	};
	
	Acts.prototype.SetCursorSprite = function (obj)
	{
		if (this.runtime.isDomFree || this.runtime.isMobile || !obj)
			return;
			
		var inst = obj.getFirstPicked();
		
		if (!inst || !inst.curFrame)
			return;
			
		var frame = inst.curFrame;
		var datauri = frame.getDataUri();
		
		var cursor_style = "url(" + datauri + ") " + Math.round(frame.hotspotX * frame.width) + " " + Math.round(frame.hotspotY * frame.height) + ", auto";
		
		jQuery(this.runtime.canvas).css("cursor", cursor_style);
	};
	
	pluginProto.acts = new Acts();

	//////////////////////////////////////
	// Expressions
	function Exps() {};

	Exps.prototype.X = function (ret, layerparam)
	{
		var layer, oldScale, oldZoomRate, oldParallaxX, oldAngle;
		
		if (cr.is_undefined(layerparam))
		{
			// calculate X position on bottom layer as if its scale were 1.0
			layer = this.runtime.getLayerByNumber(0);
			oldScale = layer.scale;
			oldZoomRate = layer.zoomRate;
			oldParallaxX = layer.parallaxX;
			oldAngle = layer.angle;
			layer.scale = 1;
			layer.zoomRate = 1.0;
			layer.parallaxX = 1.0;
			layer.angle = 0;
			ret.set_float(layer.canvasToLayer(this.mouseXcanvas, this.mouseYcanvas, true));
			layer.scale = oldScale;
			layer.zoomRate = oldZoomRate;
			layer.parallaxX = oldParallaxX;
			layer.angle = oldAngle;
		}
		else
		{
			// use given layer param
			if (cr.is_number(layerparam))
				layer = this.runtime.getLayerByNumber(layerparam);
			else
				layer = this.runtime.getLayerByName(layerparam);
				
			if (layer)
				ret.set_float(layer.canvasToLayer(this.mouseXcanvas, this.mouseYcanvas, true));
			else
				ret.set_float(0);
		}
	};
	
	Exps.prototype.Y = function (ret, layerparam)
	{
		var layer, oldScale, oldZoomRate, oldParallaxY, oldAngle;
		
		if (cr.is_undefined(layerparam))
		{
			// calculate X position on bottom layer as if its scale were 1.0
			layer = this.runtime.getLayerByNumber(0);
			oldScale = layer.scale;
			oldZoomRate = layer.zoomRate;
			oldParallaxY = layer.parallaxY;
			oldAngle = layer.angle;
			layer.scale = 1;
			layer.zoomRate = 1.0;
			layer.parallaxY = 1.0;
			layer.angle = 0;
			ret.set_float(layer.canvasToLayer(this.mouseXcanvas, this.mouseYcanvas, false));
			layer.scale = oldScale;
			layer.zoomRate = oldZoomRate;
			layer.parallaxY = oldParallaxY;
			layer.angle = oldAngle;
		}
		else
		{
			// use given layer param
			if (cr.is_number(layerparam))
				layer = this.runtime.getLayerByNumber(layerparam);
			else
				layer = this.runtime.getLayerByName(layerparam);
				
			if (layer)
				ret.set_float(layer.canvasToLayer(this.mouseXcanvas, this.mouseYcanvas, false));
			else
				ret.set_float(0);
		}
	};
	
	Exps.prototype.AbsoluteX = function (ret)
	{
		ret.set_float(this.mouseXcanvas);
	};
	
	Exps.prototype.AbsoluteY = function (ret)
	{
		ret.set_float(this.mouseYcanvas);
	};
	
	pluginProto.exps = new Exps();
	
}());
