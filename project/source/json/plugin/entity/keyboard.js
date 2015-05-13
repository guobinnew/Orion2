// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.Keyboard = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var pluginProto = cr.plugins_.Keyboard.prototype;
		
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
		this.keyMap = new Array(256);	// stores key up/down state
		this.usedKeys = new Array(256);
		this.triggerKey = 0;
	};
	
	var instanceProto = pluginProto.Instance.prototype;

	instanceProto.onCreate = function()
	{
		var self = this;
		
		// Bind keyboard events via jQuery.  Not supported in directCanvas
		if (!this.runtime.isDomFree)
		{
			jQuery(document).keydown(
				function(info) {
					self.onKeyDown(info);
				}
			);
			
			jQuery(document).keyup(
				function(info) {
					self.onKeyUp(info);
				}
			);
		}
	};
	
	// On iframe embedded games like the Scirra Arcade, these keys can end up
	// scrolling the parent page unless we specifically block them every time.
	var keysToBlockWhenFramed = [32, 33, 34, 35, 36, 37, 38, 39, 40, 44];

	instanceProto.onKeyDown = function (info)
	{
		var alreadyPreventedDefault = false;
		
		// Always block certain key presses in frames which can result in page scrolling.
		if (window != window.top && keysToBlockWhenFramed.indexOf(info.which) > -1)
		{
			info.preventDefault();
			alreadyPreventedDefault = true;
			info.stopPropagation();
		}
		
		// Key already down: ignore, must be a repeat
		if (this.keyMap[info.which])
		{
			if (this.usedKeys[info.which] && !alreadyPreventedDefault)
				info.preventDefault();
			
			return;
		}
		
		// Set the key in the key map
		this.keyMap[info.which] = true;
		this.triggerKey = info.which;
		
		this.runtime.isInUserInputEvent = true;
		
		// Trigger 'On Any Key'
		this.runtime.trigger(cr.plugins_.Keyboard.prototype.cnds.OnAnyKey, this);
		
		// Trigger 'On Key'
		var eventRan = this.runtime.trigger(cr.plugins_.Keyboard.prototype.cnds.OnKey, this);
		
		// Trigger 'On Key Code'
		var eventRan2 = this.runtime.trigger(cr.plugins_.Keyboard.prototype.cnds.OnKeyCode, this);
		
		this.runtime.isInUserInputEvent = false;
		
		// If any event ran, prevent the default behavior.  This does not include 'on any key' running though.
		if (eventRan || eventRan2)
		{
			this.usedKeys[info.which] = true;
			
			if (!alreadyPreventedDefault)
				info.preventDefault();
		}
	};

	instanceProto.onKeyUp = function (info)
	{
		// Set the key in the key map
		this.keyMap[info.which] = false;
		this.triggerKey = info.which;
		
		this.runtime.isInUserInputEvent = true;
		
		// Trigger 'On Any Key Released'
		this.runtime.trigger(cr.plugins_.Keyboard.prototype.cnds.OnAnyKeyReleased, this);
		
		// Trigger 'On Key Released'
		var eventRan = this.runtime.trigger(cr.plugins_.Keyboard.prototype.cnds.OnKeyReleased, this);
		
		// Trigger 'On Key Code Released'
		var eventRan2 = this.runtime.trigger(cr.plugins_.Keyboard.prototype.cnds.OnKeyCodeReleased, this);
		
		this.runtime.isInUserInputEvent = false;
		
		// If any event ran, prevent the default behavior
		if (eventRan || eventRan2 || this.usedKeys[info.which])
		{
			this.usedKeys[info.which] = true;
			info.preventDefault();
		}
	};
	
	instanceProto.saveToJSON = function ()
	{
		return { "triggerKey": this.triggerKey };
	};
	
	instanceProto.loadFromJSON = function (o)
	{
		this.triggerKey = o["triggerKey"];
	};
	
	/**BEGIN-PREVIEWONLY**/
	instanceProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": "Keyboard",
			"properties": [
				{"name": "Last key code", "value": this.triggerKey, "readonly": true},
				{"name": "Last key name", "value": "'" + fixedStringFromCharCode(this.triggerKey) + "'", "readonly": true}
			]
		});
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	Cnds.prototype.IsKeyDown = function(key)
	{
		return this.keyMap[key];
	};
	
	Cnds.prototype.OnKey = function(key)
	{
		return (key === this.triggerKey);
	};
	
	Cnds.prototype.OnAnyKey = function(key)
	{
		return true;
	};
	
	Cnds.prototype.OnAnyKeyReleased = function(key)
	{
		return true;
	};
	
	Cnds.prototype.OnKeyReleased = function(key)
	{
		return (key === this.triggerKey);
	};
	
	Cnds.prototype.IsKeyCodeDown = function(key)
	{
		key = Math.floor(key);
		
		if (key < 0 || key >= this.keyMap.length)
			return false;
		
		return this.keyMap[key];
	};
	
	Cnds.prototype.OnKeyCode = function(key)
	{
		return (key === this.triggerKey);
	};
	
	Cnds.prototype.OnKeyCodeReleased = function(key)
	{
		return (key === this.triggerKey);
	};
	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.LastKeyCode = function (ret)
	{
		ret.set_int(this.triggerKey);
	};
	
	// Turns out Javascript's fromCharCode is nearly totally useless. Fix it with our own function.
	function fixedStringFromCharCode(kc)
	{
		kc = Math.floor(kc);
		
		// Alphanumerics work with fromCharCode, so just special case every other key
		switch (kc) {
		case 8:		return "backspace";
		case 9:		return "tab";
		case 13:	return "enter";
		case 16:	return "shift";
		case 17:	return "control";
		case 18:	return "alt";
		case 19:	return "pause";
		case 20:	return "capslock";
		case 27:	return "esc";
		case 33:	return "pageup";
		case 34:	return "pagedown";
		case 35:	return "end";
		case 36:	return "home";
		case 37:	return "←";
		case 38:	return "↑";
		case 39:	return "→";
		case 40:	return "↓";
		case 45:	return "insert";
		case 46:	return "del";
		case 91:	return "left window key";
		case 92:	return "right window key";
		case 93:	return "select";
		case 96:	return "numpad 0";
		case 97:	return "numpad 1";
		case 98:	return "numpad 2";
		case 99:	return "numpad 3";
		case 100:	return "numpad 4";
		case 101:	return "numpad 5";
		case 102:	return "numpad 6";
		case 103:	return "numpad 7";
		case 104:	return "numpad 8";
		case 105:	return "numpad 9";
		case 106:	return "numpad *";
		case 107:	return "numpad +";
		case 109:	return "numpad -";
		case 110:	return "numpad .";
		case 111:	return "numpad /";
		case 112:	return "F1";
		case 113:	return "F2";
		case 114:	return "F3";
		case 115:	return "F4";
		case 116:	return "F5";
		case 117:	return "F6";
		case 118:	return "F7";
		case 119:	return "F8";
		case 120:	return "F9";
		case 121:	return "F10";
		case 122:	return "F11";
		case 123:	return "F12";
		case 144:	return "numlock";
		case 145:	return "scroll lock";
		case 186:	return ";";
		case 187:	return "=";
		case 188:	return ",";
		case 189:	return "-";
		case 190:	return ".";
		case 191:	return "/";
		case 192:	return "'";
		case 219:	return "[";
		case 220:	return "\\";
		case 221:	return "]";
		case 222:	return "#";
		case 223:	return "`";
		default:	return String.fromCharCode(kc);
		}
	};
	
	Exps.prototype.StringFromKeyCode = function (ret, kc)
	{
		ret.set_string(fixedStringFromCharCode(kc));
	};
	
	pluginProto.exps = new Exps();

}());