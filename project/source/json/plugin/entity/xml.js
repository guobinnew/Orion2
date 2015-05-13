// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.XML = function(runtime)
{
	this.runtime = runtime;
	
	// IE sucks
	if (this.runtime.isIE)
	{
		var x = {};
		window["XPathResult"] = x;
		x.NUMBER_TYPE = 1;
		x.STRING_TYPE = 2;
		x.UNORDERED_NODE_SNAPSHOT_TYPE = 6;
		x.ORDERED_NODE_SNAPSHOT_TYPE = 7;
	}
};

(function ()
{
	/////////////////////////////////////
	var pluginProto = cr.plugins_.XML.prototype;
		
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
		this.xmlDoc = null;
		this.nodeStack = [];
		
		if (this.runtime.isDomFree)
			cr.logexport("[Construct 2] The XML object is not supported on this platform.");
	};
	
	instanceProto.xpath_eval_one = function(xpath, result_type)
	{
		if (!this.xmlDoc)
			return;
		
		var root = this.nodeStack.length ? this.nodeStack[this.nodeStack.length - 1] : this.xmlDoc.documentElement;
		
		try {
			if (this.runtime.isIE)
				return root.selectSingleNode(xpath);
			else
				return this.xmlDoc.evaluate(xpath, root, null, result_type, null);
		}
		catch (e) { return null; }
	};
	
	instanceProto.xpath_eval_many = function(xpath, result_type)
	{
		if (!this.xmlDoc)
			return;
		
		var root = this.nodeStack.length ? this.nodeStack[this.nodeStack.length - 1] : this.xmlDoc.documentElement;
		
		try {
			if (this.runtime.isIE)
				return root.selectNodes(xpath);
			else
				return this.xmlDoc.evaluate(xpath, root, null, result_type, null);
		}
		catch (e) { return null; }
	};

	//////////////////////////////////////
	// Conditions
	function Cnds() {};
	
	instanceProto.doForEachIteration = function (current_event, item)
	{
		this.nodeStack.push(item);
		this.runtime.pushCopySol(current_event.solModifiers);
		current_event.retrigger();
		this.runtime.popSol(current_event.solModifiers);
		this.nodeStack.pop();
	};

	Cnds.prototype.ForEach = function (xpath)
	{
		if (this.runtime.isDomFree)
			return false;
		
		var current_event = this.runtime.getCurrentEventStack().current_event;
		var result = this.xpath_eval_many(xpath, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE);
		var i, len, x;
		
		if (!result)
			return false;
		else
		{
			var current_loop = this.runtime.pushLoopStack();
			
			if (this.runtime.isIE)
			{
				for (i = 0, len = result.length; i < len; i++)
				{
					current_loop.index = i;
					this.doForEachIteration(current_event, result[i]);
				}
			}
			else
			{
				for (i = 0, len = result.snapshotLength; i < len; i++)
				{
					current_loop.index = i;
					this.doForEachIteration(current_event, result.snapshotItem(i));
				}
			}
			
			this.runtime.popLoopStack();
		}
		
		return false;
	};
	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.Load = function (str)
	{
		if (this.runtime.isDomFree)
			return;
		
		var xml, tmp;
		var isWindows8 = !!(typeof window["c2isWindows8"] !== "undefined" && window["c2isWindows8"]);
		
		try {
			// MS have invented 2 different non-standard ways to load XML, make sure we use the right one
			// for Windows 8 apps, for IE browser, and for everyone else
			if (isWindows8)
	        {
	            xml = new Windows["Data"]["Xml"]["Dom"]["XmlDocument"]()
	            xml["loadXml"](str);
	        }
			else if (this.runtime.isIE)
			{
				var versions = ["MSXML2.DOMDocument.6.0",
                        "MSXML2.DOMDocument.3.0",
                        "MSXML2.DOMDocument"];

				for (var i = 0; i < 3; i++){
					try {
						xml = new ActiveXObject(versions[i]);
						
						if (xml)
							break;
					} catch (ex){
						xml = null;
					}
				}
				
				if (xml)
				{
					xml.async = "false";
					xml["loadXML"](str);
				}
			}
			else {
				tmp = new DOMParser();
				xml = tmp.parseFromString(str, "text/xml");
			}
		} catch(e) {
			xml = null;
		}
		
		if (xml)
		{
			this.xmlDoc = xml;
			
			if (this.runtime.isIE && !isWindows8)
				this.xmlDoc["setProperty"]("SelectionLanguage","XPath");
		}
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.NumberValue = function (ret, xpath)
	{
		if (this.runtime.isDomFree)
		{
			ret.set_int(0);
			return;
		}
		
		var result = this.xpath_eval_one(xpath, XPathResult.NUMBER_TYPE);
		
		if (!result)
			ret.set_int(0);
		else if (this.runtime.isIE)
			ret.set_int(parseInt(result.nodeValue, 10));
		else
			ret.set_int(result.numberValue || 0);
	};
	
	Exps.prototype.StringValue = function (ret, xpath)
	{
		if (this.runtime.isDomFree)
		{
			ret.set_string("");
			return;
		}
		
		var result;

		// Workaround Firefox's dumb 4096 text length limitation.  Request snapshot of all nodes matching
		// and concatenate their results.
		if (/firefox/i.test(navigator.userAgent))
		{
			result = this.xpath_eval_one(xpath, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE);
			
			if (!result)
				ret.set_string("");
			else
			{
				var i, len, totalstr = "";
				for (i = 0, len = result.snapshotLength; i < len; i++)
				{
					totalstr += result.snapshotItem(i).textContent;
				}
				
				ret.set_string(totalstr);
			}
		}
		else
		{
			result = this.xpath_eval_one(xpath, XPathResult.STRING_TYPE);
			
			if (!result)
				ret.set_string("");
			else if (this.runtime.isIE)
				ret.set_string(result.nodeValue || "");
			else
				ret.set_string(result.stringValue || "");
		}
	};
	
	Exps.prototype.NodeCount = function (ret, xpath)
	{
		if (this.runtime.isDomFree)
		{
			ret.set_int(0);
			return;
		}
		
		var result = this.xpath_eval_many(xpath, XPathResult.UNORDERED_NODE_SNAPSHOT_TYPE);
		
		if (!result)
			ret.set_int(0);
		else if (this.runtime.isIE)
			ret.set_int(result.length || 0);
		else
			ret.set_int(result.snapshotLength || 0);
	};
	
	pluginProto.exps = new Exps();

}());