// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.Geolocation = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var pluginProto = cr.plugins_.Geolocation.prototype;
		
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
	
	var geoSupported = ("geolocation" in navigator);

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
		this.watchId = -1;
		this.errorMsg = "";
		this.timestamp = 0;
		this.latitude = 0;
		this.longitude = 0;
		this.altitude = 0;
		this.accuracy = 0;
		this.altitudeaccuracy = 0;
		this.heading = 0;
		this.speed = 0;
		
		var self = this;
		
		this.updateFunction = function (pos)
		{
			self.timestamp = pos.timestamp;
			var coords = pos.coords;
			self.latitude = coords.latitude || 0;
			self.longitude = coords.longitude || 0;
			self.altitude = coords.altitude || 0;
			self.accuracy = coords.accuracy || 0;
			self.altitudeaccuracy = coords.altitudeAccuracy || 0;
			self.heading = coords.heading || 0;
			self.speed = coords.speed || 0;
			self.runtime.trigger(cr.plugins_.Geolocation.prototype.cnds.OnUpdate, self);
		};
		
		this.errorFunction = function (err)
		{
			self.errorMsg = err["message"] || "Unknown error";
			self.runtime.trigger(cr.plugins_.Geolocation.prototype.cnds.OnError, self);
		};
	};
	
	// called whenever an instance is destroyed
	// note the runtime may keep the object after this call for recycling; be sure
	// to release/recycle/reset any references to other objects in this function.
	instanceProto.onDestroy = function ()
	{
	};
	
	// called when saving the full state of the game
	instanceProto.saveToJSON = function ()
	{
		// return a Javascript object containing information about your object's state
		// note you MUST use double-quote syntax (e.g. "property": value) to prevent
		// Closure Compiler renaming and breaking the save format
		return {
			// e.g.
			//"myValue": this.myValue
		};
	};
	
	// called when loading the full state of the game
	instanceProto.loadFromJSON = function (o)
	{
		// load from the state previously saved by saveToJSON
		// 'o' provides the same object that you saved, e.g.
		// this.myValue = o["myValue"];
		// note you MUST use double-quote syntax (e.g. o["property"]) to prevent
		// Closure Compiler renaming and breaking the save format
	};
	
	/**BEGIN-PREVIEWONLY**/
	instanceProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": "Geolocation",
			"properties": [
				{"name": "Is watching", "value": (this.watchId > -1), "readonly": true},
				{"name": "Last error message", "value": this.errorMsg, "readonly": true},
				{"name": "Latitude", "value": this.latitude, "readonly": true},
				{"name": "Longitude", "value": this.longitude, "readonly": true},
				{"name": "Altitude", "value": this.altitude, "readonly": true},
				{"name": "Accuracy", "value": this.accuracy, "readonly": true},
				{"name": "Altitude accuracy", "value": this.altitudeaccuracy, "readonly": true},
				{"name": "Heading", "value": this.heading, "readonly": true},
				{"name": "Speed", "value": this.speed, "readonly": true},
				{"name": "Timestamp", "value": this.timestamp, "readonly": true}
			]
		});
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	Cnds.prototype.IsSupported = function ()
	{
		return geoSupported;
	};
	
	Cnds.prototype.IsWatching = function ()
	{
		return this.watchId > -1;
	};
	
	Cnds.prototype.OnUpdate = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnError = function ()
	{
		return true;
	};

	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.RequestLocation = function (accuracy_, timeout_, maxage_)
	{
		if (!geoSupported)
			return;
		
		navigator.geolocation.getCurrentPosition(this.updateFunction, this.errorFunction, {
			"enableHighAccuracy": (accuracy_ !== 0),
			"timeout": timeout_ * 1000,		// seconds to milliseconds
			"maximumAge": maxage_ * 1000
		});
	};
	
	Acts.prototype.WatchLocation = function (accuracy_, timeout_, maxage_)
	{
		if (!geoSupported)
			return;
		
		if (this.watchId > -1)
			navigator.geolocation.clearWatch(this.watchId);
		
		this.watchId = navigator.geolocation.watchPosition(this.updateFunction, this.errorFunction, {
			"enableHighAccuracy": (accuracy_ !== 0),
			"timeout": timeout_ * 1000,		// seconds to milliseconds
			"maximumAge": maxage_ * 1000
		});
	};
	
	Acts.prototype.StopWatching = function ()
	{
		if (this.watchId > -1)
			navigator.geolocation.clearWatch(this.watchId);
			
		this.watchId = -1;
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.ErrorMessage = function (ret)
	{
		ret.set_string(this.errorMsg);
	};
	
	Exps.prototype.Timestamp = function (ret)
	{
		ret.set_float(this.timestamp);
	};
	
	Exps.prototype.Latitude = function (ret)
	{
		ret.set_float(this.latitude);
	};
	
	Exps.prototype.Longitude = function (ret)
	{
		ret.set_float(this.longitude);
	};
	
	Exps.prototype.Altitude = function (ret)
	{
		ret.set_float(this.altitude);
	};
	
	Exps.prototype.Accuracy = function (ret)
	{
		ret.set_float(this.accuracy);
	};
	
	Exps.prototype.AltitudeAccuracy = function (ret)
	{
		ret.set_float(this.altitudeaccuracy);
	};
	
	Exps.prototype.Heading = function (ret)
	{
		ret.set_float(this.heading);
	};
	
	Exps.prototype.Speed = function (ret)
	{
		ret.set_float(this.speed);
	};
	
	pluginProto.exps = new Exps();

}());