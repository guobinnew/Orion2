// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.AJAX = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var isNWjs = false;
	var path = null;
	var fs = null;
	var nw_appfolder = "";
	
	var pluginProto = cr.plugins_.AJAX.prototype;
		
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
		
		this.lastData = "";
		this.curTag = "";
		this.progress = 0;
		this.timeout = -1;
		
		isNWjs = this.runtime.isNWjs;
		
		if (isNWjs)
		{
			path = require("path");
			fs = require("fs");
			nw_appfolder = path["dirname"](process["execPath"]) + "\\";
		}
	};
	
	var instanceProto = pluginProto.Instance.prototype;
	
	var theInstance = null;
	
	// For handling AJAX events in DC
	window["C2_AJAX_DCSide"] = function (event_, tag_, param_)
	{
		if (!theInstance)
			return;
		
		if (event_ === "success")
		{
			theInstance.curTag = tag_;
			theInstance.lastData = param_;
			theInstance.runtime.trigger(cr.plugins_.AJAX.prototype.cnds.OnComplete, theInstance);
		}
		else if (event_ === "error")
		{
			theInstance.curTag = tag_;
			theInstance.runtime.trigger(cr.plugins_.AJAX.prototype.cnds.OnError, theInstance);
		}
		else if (event_ === "progress")
		{
			theInstance.progress = param_;
			theInstance.curTag = tag_;
			theInstance.runtime.trigger(cr.plugins_.AJAX.prototype.cnds.OnProgress, theInstance);
		}
	};

	instanceProto.onCreate = function()
	{
		theInstance = this;
	};
	
	instanceProto.saveToJSON = function ()
	{
		return { "lastData": this.lastData };
	};
	
	instanceProto.loadFromJSON = function (o)
	{
		this.lastData = o["lastData"];
		this.curTag = "";
		this.progress = 0;
	};
	
	var next_request_headers = {};
	var next_override_mime = "";
	
	instanceProto.doRequest = function (tag_, url_, method_, data_)
	{
		// In directCanvas: forward request to webview layer
		if (this.runtime.isDirectCanvas)
		{
			AppMobi["webview"]["execute"]('C2_AJAX_WebSide("' + tag_ + '", "' + url_ + '", "' + method_ + '", ' + (data_ ? '"' + data_ + '"' : "null") + ');');
			return;
		}
		
		// Create a context object with the tag name and a reference back to this
		var self = this;
		var request = null;
		
		var doErrorFunc = function ()
		{
			self.curTag = tag_;
			self.runtime.trigger(cr.plugins_.AJAX.prototype.cnds.OnError, self);
		};
		
		var errorFunc = function ()
		{
			// In node-webkit, try looking up the file on disk instead since it wasn't found in the project.
			if (isNWjs)
			{
				var filepath = nw_appfolder + url_;
				
				if (fs["existsSync"](filepath))
				{
					fs["readFile"](filepath, {"encoding": "utf8"}, function (err, data) {
						if (err)
						{
							doErrorFunc();
							return;
						}
						
						self.lastData = data.replace(/\r\n/g, "\n")
						self.runtime.trigger(cr.plugins_.AJAX.prototype.cnds.OnComplete, self);
					});
				}
				else
					doErrorFunc();
			}
			else
				doErrorFunc();
		};
			
		var progressFunc = function (e)
		{
			if (!e["lengthComputable"])
				return;
				
			self.progress = e.loaded / e.total;
			self.curTag = tag_;
			self.runtime.trigger(cr.plugins_.AJAX.prototype.cnds.OnProgress, self);
		};
			
		try
		{
			// Windows Phone 8 can't AJAX local files using the standards-based API, but
			// can if we use the old-school ActiveXObject. So use ActiveX on WP8 only.
			if (this.runtime.isWindowsPhone8)
				request = new ActiveXObject("Microsoft.XMLHTTP");
			else
				request = new XMLHttpRequest();
			
			request.onreadystatechange = function()
			{
				if (request.readyState === 4)
				{
					self.curTag = tag_;
					
					if (request.responseText)
						self.lastData = request.responseText.replace(/\r\n/g, "\n");		// fix windows style line endings
					else
						self.lastData = "";
					
					if (request.status >= 400)
						self.runtime.trigger(cr.plugins_.AJAX.prototype.cnds.OnError, self);
					else
					{
						// In node-webkit, don't trigger 'on success' with empty string if file not found
						if (!isNWjs || self.lastData.length)
							self.runtime.trigger(cr.plugins_.AJAX.prototype.cnds.OnComplete, self);
					}
				}
			};
			
			if (!this.runtime.isWindowsPhone8)
			{
				request.onerror = errorFunc;
				request.ontimeout = errorFunc;
				request.onabort = errorFunc;
				request["onprogress"] = progressFunc;
			}
			
			request.open(method_, url_);
			
			if (!this.runtime.isWindowsPhone8)
			{
				// IE requires timeout be set after open()
				if (this.timeout >= 0 && typeof request["timeout"] !== "undefined")
					request["timeout"] = this.timeout;
			}
			
			// Workaround for CocoonJS bug: property exists but is not settable
			try {
				request.responseType = "text";
			} catch (e) {}
			
			if (data_)
			{
				if (request["setRequestHeader"] && !next_request_headers.hasOwnProperty("Content-Type"))
				{
					request["setRequestHeader"]("Content-Type", "application/x-www-form-urlencoded");
				}
			}
			
			// Apply custom headers
			if (request["setRequestHeader"])
			{
				var p;
				for (p in next_request_headers)
				{
					if (next_request_headers.hasOwnProperty(p))
					{
						try {
							request["setRequestHeader"](p, next_request_headers[p]);
						}
						catch (e) {}
					}
				}
				
				// Reset for next request
				next_request_headers = {};
			}
			
			// Apply MIME type override if one set
			if (next_override_mime && request["overrideMimeType"])
			{
				try {
					request["overrideMimeType"](next_override_mime);
				}
				catch (e) {}
				
				// Reset for next request
				next_override_mime = "";
			}

			if (data_)
				request.send(data_);
			else
				request.send();
			
		}
		catch (e)
		{
			errorFunc();
		}
	};
	
	/**BEGIN-PREVIEWONLY**/
	instanceProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": "AJAX",
			"properties": [
				{"name": "Last data", "value": this.lastData, "readonly": true}
			]
		});
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	Cnds.prototype.OnComplete = function (tag)
	{
		return cr.equals_nocase(tag, this.curTag);
	};
	
	Cnds.prototype.OnError = function (tag)
	{
		return cr.equals_nocase(tag, this.curTag);
	};
	
	Cnds.prototype.OnProgress = function (tag)
	{
		return cr.equals_nocase(tag, this.curTag);
	};
	
	pluginProto.cnds = new Cnds();

	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.Request = function (tag_, url_)
	{
		this.doRequest(tag_, url_, "GET");
	};
	
	Acts.prototype.RequestFile = function (tag_, file_)
	{
		this.doRequest(tag_, file_, "GET");
	};
	
	Acts.prototype.Post = function (tag_, url_, data_, method_)
	{
		this.doRequest(tag_, url_, method_, data_);
	};
	
	Acts.prototype.SetTimeout = function (t)
	{
		this.timeout = t * 1000;
	};
	
	Acts.prototype.SetHeader = function (n, v)
	{
		next_request_headers[n] = v;
	};
	
	Acts.prototype.OverrideMIMEType = function (m)
	{
		next_override_mime = m;
	};
	
	pluginProto.acts = new Acts();

	//////////////////////////////////////
	// Expressions
	function Exps() {};

	Exps.prototype.LastData = function (ret)
	{
		ret.set_string(this.lastData);
	};
	
	Exps.prototype.Progress = function (ret)
	{
		ret.set_float(this.progress);
	};
	
	pluginProto.exps = new Exps();

}());