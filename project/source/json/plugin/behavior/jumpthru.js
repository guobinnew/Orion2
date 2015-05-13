// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.behaviors, "cr.behaviors not created");

/////////////////////////////////////
// Behavior class
cr.behaviors.jumpthru = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var behaviorProto = cr.behaviors.jumpthru.prototype;
		
	/////////////////////////////////////
	// Behavior type class
	behaviorProto.Type = function(behavior, objtype)
	{
		this.behavior = behavior;
		this.objtype = objtype;
		this.runtime = behavior.runtime;
	};

	var behtypeProto = behaviorProto.Type.prototype;

	behtypeProto.onCreate = function()
	{
	};

	/////////////////////////////////////
	// Behavior instance class
	behaviorProto.Instance = function(type, inst)
	{
		this.type = type;
		this.behavior = type.behavior;
		this.inst = inst;				// associated object instance to modify
		this.runtime = type.runtime;
	};
	
	var behinstProto = behaviorProto.Instance.prototype;

	behinstProto.onCreate = function()
	{
		this.inst.extra["jumpthruEnabled"] = (this.properties[0] !== 0);
	};

	behinstProto.tick = function ()
	{
	};
	
	/**BEGIN-PREVIEWONLY**/
	behinstProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": this.type.name,
			"properties": [
				{"name": "Enabled", "value": this.inst.extra["jumpthruEnabled"]}
			]
		});
	};
	
	behinstProto.onDebugValueEdited = function (header, name, value)
	{
		if (name === "Enabled")
			this.inst.extra["jumpthruEnabled"] = value;
	};
	/**END-PREVIEWONLY**/
	
	function Cnds() {};
	
	Cnds.prototype.IsEnabled = function ()
	{
		return this.inst.extra["jumpthruEnabled"];
	};
	
	behaviorProto.cnds = new Cnds();
	
	function Acts() {};
	
	Acts.prototype.SetEnabled = function (e)
	{
		this.inst.extra["jumpthruEnabled"] = !!e;
	};
	
	behaviorProto.acts = new Acts();
	
}());