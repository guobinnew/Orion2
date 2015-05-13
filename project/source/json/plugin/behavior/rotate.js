// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.behaviors, "cr.behaviors not created");

/////////////////////////////////////
// Behavior class
cr.behaviors.Rotate = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var behaviorProto = cr.behaviors.Rotate.prototype;
		
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
		this.speed = cr.to_radians(this.properties[0]);
		this.acc = cr.to_radians(this.properties[1]);
	};
	
	behinstProto.saveToJSON = function ()
	{
		return {
			"speed": this.speed,
			"acc": this.acc
		};
	};
	
	behinstProto.loadFromJSON = function (o)
	{
		this.speed = o["speed"];
		this.acc = o["acc"];
	};

	behinstProto.tick = function ()
	{
		var dt = this.runtime.getDt(this.inst);
		
		if (dt === 0)
			return;
			
		if (this.acc !== 0)
			this.speed += this.acc * dt;
			
		if (this.speed !== 0)
		{
			this.inst.angle = cr.clamp_angle(this.inst.angle + this.speed * dt);
			this.inst.set_bbox_changed();
		}
	};
	
	/**BEGIN-PREVIEWONLY**/
	behinstProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": this.type.name,
			"properties": [
				{"name": "Speed", "value": cr.to_degrees(this.speed)},
				{"name": "Acceleration", "value": cr.to_degrees(this.acc)}
			]
		});
	};
	
	behinstProto.onDebugValueEdited = function (header, name, value)
	{
		switch (name) {
		case "Speed":			this.speed = cr.to_radians(value);			break;
		case "Acceleration":	this.acc = cr.to_radians(value);			break;
		}
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};
	behaviorProto.cnds = new Cnds();

	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.SetSpeed = function (s)
	{
		this.speed = cr.to_radians(s);
	};
	
	Acts.prototype.SetAcceleration = function (a)
	{
		this.acc = cr.to_radians(a);
	};
	
	behaviorProto.acts = new Acts();

	//////////////////////////////////////
	// Expressions
	function Exps() {};

	Exps.prototype.Speed = function (ret)
	{
		ret.set_float(cr.to_degrees(this.speed));
	};
	
	Exps.prototype.Acceleration = function (ret)
	{
		ret.set_float(cr.to_degrees(this.acc));
	};
	
	behaviorProto.exps = new Exps();
	
}());