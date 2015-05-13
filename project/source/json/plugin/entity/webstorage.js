// ECMAScript 5 strict mode
"use strict";

assert2(cr,"cr namespace not created");
assert2(cr.plugins_,"cr.plugins_ not created");

cr.plugins_.WebStorage = function(runtime)
{
	this.runtime = runtime;
};

(function()
{
	var pluginProto = cr.plugins_.WebStorage.prototype;

	pluginProto.Type = function(plugin)
	{
		this.plugin = plugin;
		this.runtime = plugin.runtime;
	};
	
	var typeProto = pluginProto.Type.prototype;
	typeProto.onCreate = function()
	{
	};

	pluginProto.Instance = function(type)
	{
		this.type = type;
		this.runtime = type.runtime;
	};
	
	var instanceProto = pluginProto.Instance.prototype;

	var prefix = "";
	var is_arcade = (typeof window["is_scirra_arcade"] !== "undefined");
	
	if (is_arcade)
		prefix = "arcade" + window["scirra_arcade_id"];
		
	instanceProto.onCreate = function()
	{
		if (typeof localStorage === "undefined")
		{
			cr.logexport("[Construct 2] Webstorage plugin: local storage is not supported on this platform.");
		}
		
		if (typeof sessionStorage === "undefined")
		{
			cr.logexport("[Construct 2] Webstorage plugin: session storage is not supported on this platform.");
		}
	};
	
	/**BEGIN-PREVIEWONLY**/
	instanceProto.getDebuggerValues = function (propsections)
	{
		var props = [];
		
		var i, len, key;
		for (i = 0, len = localStorage.length; i < len; ++i)
		{
			key = localStorage.key(i);
			
			if (key !== "__c2_sidesize" && key !== "__c2_watch")
				props.push({"name": key, "value": localStorage.getItem(key)});
		}
		
		if (props.length)
		{
			propsections.push({
				"title": "Local storage data",
				"properties": props
			});
		}
	};
	
	instanceProto.onDebugValueEdited = function (header, name, value)
	{
		try {
			localStorage.setItem(prefix + name, value);
		}
		catch (e)
		{
			alert("Error setting WebStorage data. The storage quota may have been exceeded.");
		}
	};
	/**END-PREVIEWONLY**/

	function Cnds() {};

	Cnds.prototype.LocalStorageEnabled = function()
	{
		return true;
	};
	
	Cnds.prototype.SessionStorageEnabled = function()
	{
		return true;
	};
	
	Cnds.prototype.LocalStorageExists = function(key)
	{
		if (typeof localStorage === "undefined")
			return false;
		
		return localStorage.getItem(prefix + key) != null;
	};
	
	Cnds.prototype.SessionStorageExists = function(key)
	{
		if (typeof sessionStorage === "undefined")
			return false;
		
		return sessionStorage.getItem(prefix + key) != null;
	};
	
	Cnds.prototype.OnQuotaExceeded = function ()
	{
		return true;
	};
	
	Cnds.prototype.CompareKeyText = function (key, text_to_compare, case_sensitive)
	{
		if (typeof localStorage === "undefined")
			return false;
		
		var value = localStorage.getItem(prefix + key) || "";
		
		if (case_sensitive)
			return value == text_to_compare;
		else
			return cr.equals_nocase(value, text_to_compare);
	};
	
	Cnds.prototype.CompareKeyNumber = function (key, cmp, x)
	{
		if (typeof localStorage === "undefined")
			return false;
		
		var value = localStorage.getItem(prefix + key) || "";
		
		return cr.do_cmp(parseFloat(value), cmp, x);
	};
	
	pluginProto.cnds = new Cnds();

	function Acts() {};
	
	Acts.prototype.StoreLocal = function(key, data)
	{
		if (typeof localStorage === "undefined")
			return;
		
		try {
			localStorage.setItem(prefix + key, data);
		}
		catch (e)
		{
			this.runtime.trigger(cr.plugins_.WebStorage.prototype.cnds.OnQuotaExceeded, this);
		}
	};
	
	Acts.prototype.StoreSession = function(key,data)
	{
		if (typeof sessionStorage === "undefined")
			return;
		
		
		try {
			sessionStorage.setItem(prefix + key, data);
		}
		catch (e)
		{
			this.runtime.trigger(cr.plugins_.WebStorage.prototype.cnds.OnQuotaExceeded, this);
		}
	};
	
	Acts.prototype.RemoveLocal = function(key)
	{
		if (typeof localStorage === "undefined")
			return;
		
		localStorage.removeItem(prefix + key);
	};
	
	Acts.prototype.RemoveSession = function(key)
	{
		if (typeof sessionStorage === "undefined")
			return;
		
		sessionStorage.removeItem(prefix + key);
	};
	
	Acts.prototype.ClearLocal = function()
	{
		if (typeof localStorage === "undefined")
			return;
		
		if (!is_arcade)
			localStorage.clear();
	};
	
	Acts.prototype.ClearSession = function()
	{
		if (typeof sessionStorage === "undefined")
			return;
		
		if (!is_arcade)
			sessionStorage.clear();
	};
	
	Acts.prototype.JSONLoad = function (json_, mode_)
	{
		if (typeof localStorage === "undefined")
			return;
		
		var d;
		
		try {
			d = JSON.parse(json_);
		}
		catch(e) { return; }
		
		if (!d["c2dictionary"])			// presumably not a c2dictionary object
			return;
		
		var o = d["data"];
		
		if (mode_ === 0 && !is_arcade)	// 'set' mode: must clear webstorage first
			localStorage.clear();
			
		var p;
		for (p in o)
		{
			if (o.hasOwnProperty(p))
			{
				try {
					localStorage.setItem(prefix + p, o[p]);
				}
				catch (e)
				{
					this.runtime.trigger(cr.plugins_.WebStorage.prototype.cnds.OnQuotaExceeded, this);
					return;
				}
			}
		}
	};
	
	pluginProto.acts = new Acts();
	
	function Exps() {};
	
	Exps.prototype.LocalValue = function(ret,key)
	{
		if (typeof localStorage === "undefined")
		{
			ret.set_string("");
			return;
		}
		
		ret.set_string(localStorage.getItem(prefix + key) || "");
	};
	
	Exps.prototype.SessionValue = function(ret,key)
	{
		if (typeof sessionStorage === "undefined")
		{
			ret.set_string("");
			return;
		}
		
		ret.set_string(sessionStorage.getItem(prefix + key) || "");
	};
	
	Exps.prototype.LocalCount = function(ret)
	{
		if (typeof localStorage === "undefined")
		{
			ret.set_int(0);
			return;
		}
		
		ret.set_int(is_arcade ? 0 : localStorage.length);
	};
	
	Exps.prototype.SessionCount = function(ret)
	{
		if (typeof sessionStorage === "undefined")
		{
			ret.set_int(0);
			return;
		}
		
		ret.set_int(is_arcade ? 0 : sessionStorage.length);
	};
	
	Exps.prototype.LocalAt = function(ret,n)
	{
		if (is_arcade || typeof localStorage === "undefined")
			ret.set_string("");
		else
			ret.set_string(localStorage.getItem(localStorage.key(n)) || "");
	};
	
	Exps.prototype.SessionAt = function(ret,n)
	{
		if (is_arcade || typeof sessionStorage === "undefined")
			ret.set_string("");
		else
			ret.set_string(sessionStorage.getItem(sessionStorage.key(n)) || "");
	};
	
	Exps.prototype.LocalKeyAt = function(ret,n)
	{
		if (is_arcade || typeof localStorage === "undefined")
			ret.set_string("");
		else
			ret.set_string(localStorage.key(n) || "");
	};
	
	Exps.prototype.SessionKeyAt = function(ret,n)
	{
		if (is_arcade || typeof sessionStorage === "undefined")
			ret.set_string("");
		else
			ret.set_string(sessionStorage.key(n) || "");
	};
	
	Exps.prototype.AsJSON = function (ret)
	{
		if (typeof localStorage === "undefined")
		{
			ret.set_string("");
			return;
		}
		
		var o = {}, i, len, k;
		
		for (i = 0, len = localStorage.length; i < len; i++)
		{
			k = localStorage.key(i);
			
			if (is_arcade)
			{
				// Only include if has proper prefix, but remove prefix from result
				if (k.substr(0, prefix.length) === prefix)
				{
					o[k.substr(prefix.length)] = localStorage.getItem(k);
				}
			}
			else
				o[k] = localStorage.getItem(k);
		}
		
		ret.set_string(JSON.stringify({
			"c2dictionary": true,
			"data": o
		}));
	};
	
	pluginProto.exps = new Exps();
	
}());