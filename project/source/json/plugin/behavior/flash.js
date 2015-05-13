// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.behaviors, "cr.behaviors not created");

/////////////////////////////////////
// Behavior class
cr.behaviors.Flash = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var behaviorProto = cr.behaviors.Flash.prototype;
		
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
		this.ontime = 0;
		this.offtime = 0;
		this.stage = 0;			// 0 = on, 1 = off
		this.stagetimeleft = 0;
		this.timeleft = 0;
	};
	
	behinstProto.saveToJSON = function ()
	{
		return {
			"ontime": this.ontime,
			"offtime": this.offtime,
			"stage": this.stage,
			"stagetimeleft": this.stagetimeleft,
			"timeleft": this.timeleft
		};
	};
	
	behinstProto.loadFromJSON = function (o)
	{
		this.ontime = o["ontime"];
		this.offtime = o["offtime"];
		this.stage = o["stage"];
		this.stagetimeleft = o["stagetimeleft"];
		this.timeleft = o["timeleft"];
	};

	behinstProto.tick = function ()
	{
		if (this.timeleft <= 0)
			return;		// not flashing
			
		var dt = this.runtime.getDt(this.inst);
		
		this.timeleft -= dt;
		
		// flash duration completed
		if (this.timeleft <= 0)
		{
			this.timeleft = 0;
			this.inst.visible = true; 
			this.runtime.redraw = true;
			this.runtime.trigger(cr.behaviors.Flash.prototype.cnds.OnFlashEnded, this.inst);
			return;
		}
		
		this.stagetimeleft -= dt;
		
		// switching
		if (this.stagetimeleft <= 0)
		{
			// is on and switching off
			if (this.stage === 0)
			{
				// switch off
				this.inst.visible = false;
				this.stage = 1;
				this.stagetimeleft += this.offtime;
			}
			// is off and switching on
			else
			{
				// switch on
				this.inst.visible = true;
				this.stage = 0;
				this.stagetimeleft += this.ontime;
			}
			
			this.runtime.redraw = true;
		}
	};
	
	/**BEGIN-PREVIEWONLY**/
	behinstProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": this.type.name,
			"properties": [
				{"name": "'On' time", "value": this.ontime},
				{"name": "'Off' time", "value": this.offtime},
				{"name": "Is flashing", "value": this.timeleft > 0, "readonly": true},
				{"name": "Time left", "value": this.timeleft, "readonly": true}
			]
		});
	};
	
	behinstProto.onDebugValueEdited = function (header, name, value)
	{
		switch (name) {
		case "'On' time":		this.ontime = value;			break;
		case "'Off' time":		this.offtime = value;			break;
		}
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};
	
	Cnds.prototype.IsFlashing = function ()
	{
		return this.timeleft > 0;
	};
	
	Cnds.prototype.OnFlashEnded = function ()
	{
		return true;
	};
	
	behaviorProto.cnds = new Cnds();

	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.Flash = function (on_, off_, dur_)
	{
		this.ontime = on_;
		this.offtime = off_;
		this.stage = 1;		// always start off
		this.stagetimeleft = off_;
		this.timeleft = dur_;
		
		this.inst.visible = false;
		this.runtime.redraw = true;
	};
	
	Acts.prototype.StopFlashing = function ()
	{
		this.timeleft = 0;
		this.inst.visible = true;
		this.runtime.redraw = true;
		return;
	};
	
	behaviorProto.acts = new Acts();

	//////////////////////////////////////
	// Expressions
	function Exps() {};
	behaviorProto.exps = new Exps();
	
}());