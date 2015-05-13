// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.filechooser = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	/////////////////////////////////////
	var pluginProto = cr.plugins_.filechooser.prototype;
		
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
	
	var c2URL = window["URL"] || window["webkitURL"] || window["mozURL"] || window["msURL"];

	// called whenever an instance is created
	instanceProto.onCreate = function()
	{
		// Not supported in directCanvas
		if (this.runtime.isDomFree)
		{
			cr.logexport("[Construct 2] File Chooser plugin not supported on this platform - the object will not be created");
			return;
		}
		
		// elem must be the label wrapper if a checkbox, otherwise is same as input elem
		this.elem = document.createElement("input");
		this.elem.type = "file";
		
		this.elem.setAttribute("accept", this.properties[0]);
		
		if (this.properties[1] !== 0)		// multiple selection
			this.elem.setAttribute("multiple", "");
		
		this.elem.id = this.properties[3];
		jQuery(this.elem).appendTo(this.runtime.canvasdiv ? this.runtime.canvasdiv : "body");
		
		this.element_hidden = false;
		
		if (this.properties[2] === 0)
		{
			jQuery(this.elem).hide();
			this.visible = false;
			this.element_hidden = true;
		}
		
		var self = this;
		
		this.elem.onchange = function ()
		{
			self.runtime.trigger(cr.plugins_.filechooser.prototype.cnds.OnChanged, self);
		};
		
		this.lastLeft = 0;
		this.lastTop = 0;
		this.lastRight = 0;
		this.lastBottom = 0;
		this.lastWinWidth = 0;
		this.lastWinHeight = 0;
			
		this.updatePosition(true);
		
		this.runtime.tickMe(this);
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
	};
	
	instanceProto.onDebugValueEdited = function (header, name, value)
	{
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};
	
	Cnds.prototype.OnChanged = function ()
	{
		return true;
	};
	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};

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
	
	Acts.prototype.ReleaseFile = function (f)
	{
		if (this.runtime.isDomFree)
			return;
		
		if (c2URL && c2URL["revokeObjectURL"])
			c2URL["revokeObjectURL"](f);
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.FileCount = function (ret)
	{
		ret.set_int(this.runtime.isDomFree ? 0 : (this.elem["files"].length || 0));
	};
	
	function getFileAt(files, index)
	{
		if (!files)
			return null;
		
		index = Math.floor(index);
		
		if (index < 0 || index >= files.length)
			return null;
			
		return files[index];
	};
	
	Exps.prototype.FileNameAt = function (ret, i)
	{
		var file = this.runtime.isDomFree ? null : getFileAt(this.elem["files"], i);
		
		ret.set_string(file ? (file["name"] || "") : "");
	};
	
	Exps.prototype.FileSizeAt = function (ret, i)
	{
		var file = this.runtime.isDomFree ? null : getFileAt(this.elem["files"], i);
		
		ret.set_int(file ? (file["size"] || 0) : 0);
	};
	
	Exps.prototype.FileTypeAt = function (ret, i)
	{
		var file = this.runtime.isDomFree ? null : getFileAt(this.elem["files"], i);
		
		ret.set_string(file ? (file["type"] || "") : "");
	};
	
	Exps.prototype.FileURLAt = function (ret, i)
	{
		var file = this.runtime.isDomFree ? null : getFileAt(this.elem["files"], i);
		
		if (!file)
		{
			ret.set_string("");
		}
		else if (file["c2url"])		// already created object URL
		{
			ret.set_string(file["c2url"]);
		}
		else if (c2URL && c2URL["createObjectURL"])
		{
			file["c2url"] = c2URL["createObjectURL"](file);
			ret.set_string(file["c2url"]);
		}
		else
		{
			ret.set_string("");
		}
	};
	
	pluginProto.exps = new Exps();

}());