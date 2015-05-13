// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.C2WebSocket = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var pluginProto = cr.plugins_.C2WebSocket.prototype;
		
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
	
	var isSupported = (typeof WebSocket !== "undefined");
	
	var last_url = "";

	// called whenever an instance is created
	instanceProto.onCreate = function()
	{
		this.ws = null;
		this.messageText = "";
		this.errorMsg = "";
		this.closeCode = 0;
		this.closeReason = "";
	};
	
	instanceProto.saveToJSON = function ()
	{
		return { "messageText": this.messageText, "errorMsg": this.errorMsg };
	};
	
	instanceProto.loadFromJSON = function (o)
	{
		this.messageText = o["messageText"];
		this.errorMsg = o["errorMsg"];
	};
	
	/**BEGIN-PREVIEWONLY**/
	instanceProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": "WebSocket",
			"properties": [
				{"name": "Server URL", "value": last_url, "readonly": true},
				{"name": "Is open", "value": !!(this.ws && this.ws.readyState === 1), "readonly": true},
				{"name": "Last error", "value": this.errorMsg, "readonly": true},
				{"name": "Last message", "value": this.messageText, "readonly": true}
			]
		});
	};
	/**END-PREVIEWONLY**/
	
	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	Cnds.prototype.OnOpened = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnClosed = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnError = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnMessage = function ()
	{
		return true;
	};
	
	Cnds.prototype.IsOpen = function ()
	{
		return this.ws && this.ws.readyState === 1 /* OPEN */;
	};
	
	Cnds.prototype.IsConnecting = function ()
	{
		return this.ws && this.ws.readyState === 0 /* CONNECTING */;
	};
	
	Cnds.prototype.IsSupported = function ()
	{
		return isSupported;
	};
	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.Connect = function (url_, requireProtocol_)
	{
		if (!isSupported)
			return;
		
		// Close existing connection if any
		if (this.ws)
			this.ws.close();
			
		var self = this;
		
		last_url = url_;
		
		try {
			if (requireProtocol_ === "")
				this.ws = new WebSocket(url_);
			else
				this.ws = new WebSocket(url_, requireProtocol_);
		}
		catch (e) {
			this.ws = null;
			self.errorMsg = "Unable to create a WebSocket with the given address and protocol.";
			self.runtime.trigger(cr.plugins_.C2WebSocket.prototype.cnds.OnError, self);
			return;
		}
		
		this.ws.binaryType = "arraybuffer";
		this.ws.onopen = function() {
			// Check required protocol is supported if any
			if (requireProtocol_.length && self.ws.protocol.indexOf(requireProtocol_) === -1)
			{
				self.errorMsg = "WebSocket required protocol '" + requireProtocol_ + "' not supported by server";
				self.runtime.trigger(cr.plugins_.C2WebSocket.prototype.cnds.OnError, self);
			}
			else
				self.runtime.trigger(cr.plugins_.C2WebSocket.prototype.cnds.OnOpened, self);
		};
		this.ws.onerror = function (err_) {
			if (cr.is_string(err_))
				self.errorMsg = err_;
			else
				self.errorMsg = (err_ && cr.is_string(err_.data) ? err_.data : "");
			
			self.runtime.trigger(cr.plugins_.C2WebSocket.prototype.cnds.OnError, self);
		};
		this.ws.onclose = function (e) {
			self.closeCode = e["code"] || 0;
			self.closeReason = e["reason"] || "";
			self.runtime.trigger(cr.plugins_.C2WebSocket.prototype.cnds.OnClosed, self);
		};
		this.ws.onmessage = function (msg_) {
			self.messageText = msg_.data || "";
			self.runtime.trigger(cr.plugins_.C2WebSocket.prototype.cnds.OnMessage, self);
		};
	};
	
	Acts.prototype.Close = function ()
	{
		if (this.ws)
			this.ws.close();
	};
	
	Acts.prototype.Send = function (msg_)
	{
		if (!this.ws || this.ws.readyState !== 1 /* OPEN */)
			return;
			
		this.ws.send(msg_);
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.MessageText = function (ret)
	{
		ret.set_string(this.messageText);
	};
	
	Exps.prototype.ErrorMsg = function (ret)
	{
		ret.set_string(cr.is_string(this.errorMsg) ? this.errorMsg : "");
	};	
	
	Exps.prototype.CloseCode = function (ret)
	{
		ret.set_int(this.closeCode);
	};
	
	Exps.prototype.CloseReason = function (ret)
	{
		ret.set_string(this.closeReason);
	};
	
	pluginProto.exps = new Exps();

}());