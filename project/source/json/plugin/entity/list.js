// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.List = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	/////////////////////////////////////
	var pluginProto = cr.plugins_.List.prototype;
		
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
		// Not supported in DC
		if (this.runtime.isDomFree)
		{
			cr.logexport("[Construct 2] List plugin not supported on this platform - the object will not be created");
			return;
		}

		this.elem = document.createElement("select");
		this.elem.id = this.properties[7];
		jQuery(this.elem).appendTo(this.runtime.canvasdiv ? this.runtime.canvasdiv : "body");
		this.elem.title = this.properties[1];
		this.elem.disabled = (this.properties[3] === 0);
		
		// If type is list box, set size to 2 to force list box mode
		if (this.properties[4] === 0)
			this.elem.size = 2;
		
		this.elem["multiple"] = (this.properties[5] !== 0);
		this.autoFontSize = (this.properties[6] !== 0);
		
		if (this.properties[2] === 0)
		{
			jQuery(this.elem).hide();
			this.visible = false;
		}
		
		// add initial items if any
		if (this.properties[0])
		{
			var itemsArr = this.properties[0].split(";");
			var i, len, o;
			for (i = 0, len = itemsArr.length; i < len; i++)
			{
				o = document.createElement("option");
				o.text = itemsArr[i];
				this.elem.add(o);
			}
		}
		
		var self = this;
		
		this.elem.onchange = function() {
				self.runtime.trigger(cr.plugins_.List.prototype.cnds.OnSelectionChanged, self);
			};
		
		this.elem.onclick = function(e) {
				e.stopPropagation();
				self.runtime.isInUserInputEvent = true;
				self.runtime.trigger(cr.plugins_.List.prototype.cnds.OnClicked, self);
				self.runtime.isInUserInputEvent = false;
			};
		
		this.elem.ondblclick = function(e) {
				e.stopPropagation();
				self.runtime.isInUserInputEvent = true;
				self.runtime.trigger(cr.plugins_.List.prototype.cnds.OnDoubleClicked, self);
				self.runtime.isInUserInputEvent = false;
			};
		
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
		this.isVisible = true;
		
		this.updatePosition(true);
		
		this.runtime.tickMe(this);
	};
	
	instanceProto.saveToJSON = function ()
	{
		var o = {
			"tooltip": this.elem.title,
			"disabled": !!this.elem.disabled,
			"items": [],
			"sel": []
		};
		
		var i, len;
		var itemsarr = o["items"];
		
		for (i = 0, len = this.elem.length; i < len; i++)
		{
			itemsarr.push(this.elem.options[i].text);
		}
		
		var selarr = o["sel"];
		
		if (this.elem["multiple"])
		{
			for (i = 0, len = this.elem.length; i < len; i++)
			{
				if (this.elem.options[i].selected)
					selarr.push(i);
			}
		}
		else
		{
			selarr.push(this.elem["selectedIndex"]);
		}
		
		return o;
	};
	
	instanceProto.loadFromJSON = function (o)
	{
		this.elem.title = o["tooltip"];
		this.elem.disabled = o["disabled"];
		
		var itemsarr = o["items"];
		
		// Clear the list
		while (this.elem.length)
			this.elem.remove(this.elem.length - 1);
			
		var i, len, opt;
		for (i = 0, len = itemsarr.length; i < len; i++)
		{
			opt = document.createElement("option");
			opt.text = itemsarr[i];
			this.elem.add(opt);
		}
		
		var selarr = o["sel"];
		
		if (this.elem["multiple"])
		{
			for (i = 0, len = selarr.length; i < len; i++)
			{
				if (selarr[i] < this.elem.length)
					this.elem.options[selarr[i]].selected = true;
			}
		}
		else if (selarr.length >= 1)
		{
			this.elem["selectedIndex"] = selarr[0];
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
			if (this.isVisible)
				jQuery(this.elem).hide();
			
			this.isVisible = false;
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
			if (!this.isVisible)
			{
				jQuery(this.elem).show();
				this.isVisible = true;
			}
			
			return;
		}
			
		this.lastLeft = left;
		this.lastTop = top;
		this.lastRight = right;
		this.lastBottom = bottom;
		this.lastWinWidth = curWinWidth;
		this.lastWinHeight = curWinHeight;
		
		if (!this.isVisible)
		{
			jQuery(this.elem).show();
			this.isVisible = true;
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
			"title": "List",
			"properties": [
				{"name": "Item count", "value": this.elem.length, "readonly": true},
				{"name": "Enabled", "value": !this.elem.disabled},
				{"name": "Tooltip", "value": this.elem.title},
				{"name": "Selected index", "value": this.elem.selectedIndex}
			]
		});
		
		var props = [], i, len;
		for (i = 0, len = this.elem.length; i < len; ++i)
		{
			props.push({"name": i.toString(), "value": this.elem.options[i].text});
		}
		
		propsections.push({
			"title": "Items",
			"properties": props
		});
	};
	
	instanceProto.onDebugValueEdited = function (header, name, value)
	{
		if (header === "List")
		{
			switch (name) {
			case "Enabled":
				this.elem.disabled = !value;
				break;
			case "Tooltip":
				this.elem.title = value;
				break;
			case "Selected index":
				this.elem.selectedIndex = value;
				break;
			}
		}
		else if (header === "Items")
		{
			this.elem.options[parseInt(name, 10)].text = value;
		}
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};
	
	Cnds.prototype.CompareSelection = function (cmp_, x_)
	{
		return cr.do_cmp(this.elem["selectedIndex"], cmp_, x_);
	};
	
	Cnds.prototype.OnSelectionChanged = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnClicked = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnDoubleClicked = function ()
	{
		return true;
	};
	
	Cnds.prototype.CompareSelectedText = function (x_, case_)
	{
		var selected_text = "";
		var i = this.elem["selectedIndex"];
		
		if (i < 0 || i >= this.elem.length)
			return false;
		
		selected_text = this.elem.options[i].text;
		
		if (case_)
			return selected_text == x_;
		else
			return cr.equals_nocase(selected_text, x_);
	};
	
	Cnds.prototype.CompareTextAt = function (i_, x_, case_)
	{
		var text = "";
		var i = Math.floor(i_);
		
		if (i < 0 || i >= this.elem.length)
			return false;
		
		text = this.elem.options[i].text;
		
		if (case_)
			return text == x_;
		else
			return cr.equals_nocase(text,x_);
	};
	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};
	
	Acts.prototype.Select = function (i)
	{
		if (this.runtime.isDomFree)
			return;
		
		this.elem["selectedIndex"] = i;
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
		
		this.elem.disabled = (en === 0);
	};
	
	Acts.prototype.SetFocus = function ()
	{
		if (this.runtime.isDomFree)
			return;
		
		this.elem.focus();
	};
	
	Acts.prototype.SetBlur = function ()
	{
		if (this.runtime.isDomFree)
			return;
		
		this.elem.blur();
	};
	
	Acts.prototype.SetCSSStyle = function (p, v)
	{
		if (this.runtime.isDomFree)
			return;
			
		jQuery(this.elem).css(p, v);
	};
	
	Acts.prototype.AddItem = function (text_)
	{
		if (this.runtime.isDomFree)
			return;
		
		var o = document.createElement("option");
		o.text = text_;
		this.elem.add(o);
	};
	
	Acts.prototype.AddItemAt = function (index_, text_)
	{
		if (this.runtime.isDomFree)
			return;
			
		index_ = Math.floor(index_);
		
		if (index_ < 0)
			index_ = 0;
			
		var o = document.createElement("option");
		o.text = text_;
		
		if (index_ >= this.elem.length)
			this.elem.add(o);
		else
		{
			this.elem.add(o, this.elem.options[index_]);
		}
	};
	
	Acts.prototype.Remove = function (index_)
	{
		if (this.runtime.isDomFree)
			return;
		
		index_ = Math.floor(index_);
		this.elem.remove(index_);
	};
	
	Acts.prototype.SetItemText = function (index_, text_)
	{
		if (this.runtime.isDomFree)
			return;
			
		index_ = Math.floor(index_);
		if (index_ < 0 || index_ >= this.elem.length)
			return;
			
		this.elem.options[index_].text = text_;
	};
	
	Acts.prototype.Clear = function ()
	{
		if (this.runtime.isDomFree)
			return;
		
		while (this.elem.length)
			this.elem.remove(0);
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.ItemCount = function (ret)
	{
		if (this.runtime.isDomFree)
		{
			ret.set_int(0);
			return;
		}
		
		ret.set_int(this.elem.length);
	};
	
	Exps.prototype.ItemTextAt = function (ret, i)
	{
		if (this.runtime.isDomFree)
		{
			ret.set_string("");
			return;
		}
		
		i = Math.floor(i);		
		
		if (i < 0 || i >= this.elem.length)
		{
			ret.set_string("");
			return;
		}
		
		ret.set_string(this.elem.options[i].text);
	};
	
	Exps.prototype.SelectedIndex = function (ret)
	{
		if (this.runtime.isDomFree)
		{
			ret.set_int(0);
			return;
		}
		
		ret.set_int(this.elem["selectedIndex"]);
	};
	
	Exps.prototype.SelectedText = function (ret)
	{
		if (this.runtime.isDomFree)
		{
			ret.set_string("");
			return;
		}
		
		var i = this.elem["selectedIndex"];
		
		if (i < 0 || i >= this.elem.length)
		{
			ret.set_string("");
			return;
		}
		
		ret.set_string(this.elem.options[i].text);
	};
	
	Exps.prototype.SelectedCount = function (ret)
	{
		if (this.runtime.isDomFree)
		{
			ret.set_int(0);
			return;
		}
		
		var i, len, count = 0;
		for (i = 0, len = this.elem.length; i < len; i++)
		{
			if (this.elem.options[i].selected)
				count++;
		}
		
		ret.set_int(count);
	};
	
	Exps.prototype.SelectedIndexAt = function (ret, index_)
	{
		if (this.runtime.isDomFree)
		{
			ret.set_int(0);
			return;
		}
		
		index_ = Math.floor(index_);
		
		var i, len, count = 0, result = 0;
		for (i = 0, len = this.elem.length; i < len; i++)
		{
			if (this.elem.options[i].selected)
			{
				if (count === index_)
				{
					result = i;
					break;
				}
				
				count++;
			}
		}
		
		ret.set_int(result);
	};
	
	Exps.prototype.SelectedTextAt = function (ret, index_)
	{
		if (this.runtime.isDomFree)
		{
			ret.set_string("");
			return;
		}
		
		index_ = Math.floor(index_);
		
		var i, len, count = 0, result = "";
		for (i = 0, len = this.elem.length; i < len; i++)
		{
			if (this.elem.options[i].selected)
			{
				if (count === index_)
				{
					result = this.elem.options[i].text;
					break;
				}
				
				count++;
			}
		}
		
		ret.set_string(result);
	};
	
	pluginProto.exps = new Exps();

}());