// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.behaviors, "cr.behaviors not created");

/////////////////////////////////////
// Behavior class
cr.behaviors.Timer = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var behaviorProto = cr.behaviors.Timer.prototype;
		
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
		this.timers = {};
	};
	
	behinstProto.onDestroy = function ()
	{
		cr.wipe(this.timers);
	};
	
	// called when saving the full state of the game
	behinstProto.saveToJSON = function ()
	{
		var o = {};
		
		var p, t;
		for (p in this.timers)
		{
			if (this.timers.hasOwnProperty(p))
			{
				t = this.timers[p];
				
				o[p] = {
					"c": t.current.sum,
					"t": t.total.sum,
					"d": t.duration,
					"r": t.regular
				};
			}
		}
		
		return o;
	};
	
	// called when loading the full state of the game
	behinstProto.loadFromJSON = function (o)
	{
		this.timers = {};
		
		var p;
		for (p in o)
		{
			if (o.hasOwnProperty(p))
			{
				this.timers[p] = {
					current: new cr.KahanAdder(),
					total: new cr.KahanAdder(),
					duration: o[p]["d"],
					regular: o[p]["r"]
				};
				
				this.timers[p].current.sum = o[p]["c"];
				this.timers[p].total.sum = o[p]["t"];
			}
		}
	};

	behinstProto.tick = function ()
	{
		var dt = this.runtime.getDt(this.inst);
		
		var p, t;
		
		for (p in this.timers)
		{
			if (this.timers.hasOwnProperty(p))
			{
				t = this.timers[p];
				t.current.add(dt);
				t.total.add(dt);
			}
		}
	};
	
	behinstProto.tick2 = function ()
	{
		// Reset any regular timers and remove any one-off timers
		var p, t;
		
		for (p in this.timers)
		{
			if (this.timers.hasOwnProperty(p))
			{
				t = this.timers[p];
				
				if (t.current.sum >= t.duration)
				{
					if (t.regular)
						t.current.sum -= t.duration;
					else
						delete this.timers[p];
				}
			}
		}
	};
	
	/**BEGIN-PREVIEWONLY**/
	behinstProto.getDebuggerValues = function (propsections)
	{
		var props = [], p, t;
		for (p in this.timers)
		{
			if (this.timers.hasOwnProperty(p))
			{
				t = this.timers[p];
				props.push({"name": p, "value": "" + (Math.round(t.current.sum * 100) / 100) + " s / " + (Math.round(t.duration * 100) / 100) + " s", "readonly": true});
			}
		}
		
		if (props.length)
		{
			propsections.push({
				"title": this.type.name,
				"properties": props
			});
		}
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	Cnds.prototype.OnTimer = function (tag_)
	{
		tag_ = tag_.toLowerCase();
		
		var t = this.timers[tag_];
		
		if (!t)
			return false;
		
		return t.current.sum >= t.duration;
	};

	behaviorProto.cnds = new Cnds();

	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.StartTimer = function (duration_, type_, tag_)
	{
		this.timers[tag_.toLowerCase()] = {
			current: new cr.KahanAdder(),
			total: new cr.KahanAdder(),
			duration: duration_,
			regular: (type_ === 1)
		};
	};
	
	Acts.prototype.StopTimer = function (tag_)
	{
		tag_ = tag_.toLowerCase();
		
		if (this.timers.hasOwnProperty(tag_))
			delete this.timers[tag_];
	};
	
	behaviorProto.acts = new Acts();

	//////////////////////////////////////
	// Expressions
	function Exps() {};

	Exps.prototype.CurrentTime = function (ret, tag_)
	{
		var t = this.timers[tag_.toLowerCase()];
		ret.set_float(t ? t.current.sum : 0);
	};
	
	Exps.prototype.TotalTime = function (ret, tag_)
	{
		var t = this.timers[tag_.toLowerCase()];
		ret.set_float(t ? t.total.sum : 0);
	};
	
	Exps.prototype.Duration = function (ret, tag_)
	{
		var t = this.timers[tag_.toLowerCase()];
		ret.set_float(t ? t.duration : 0);
	};
	
	behaviorProto.exps = new Exps();
	
}());