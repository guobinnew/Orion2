// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.behaviors, "cr.behaviors not created");

/////////////////////////////////////
// Behavior class
cr.behaviors.Fade = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var behaviorProto = cr.behaviors.Fade.prototype;
		
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
		var active_at_start = this.properties[0] === 1;
		this.fadeInTime = this.properties[1];
		this.waitTime = this.properties[2];
		this.fadeOutTime = this.properties[3];
		this.destroy = this.properties[4];			// 0 = no, 1 = after fade out
		this.stage = active_at_start ? 0 : 3;		// 0 = fade in, 1 = wait, 2 = fade out, 3 = done
		
		if (this.recycled)
			this.stageTime.reset();
		else
			this.stageTime = new cr.KahanAdder();
		
		this.maxOpacity = (this.inst.opacity ? this.inst.opacity : 1.0);
		
		if (active_at_start)
		{
			// Skip fade-in
			if (this.fadeInTime === 0)
			{
				this.stage = 1;
				
				// Skip wait
				if (this.waitTime === 0)
					this.stage = 2;
			}
			// Otherwise we don't want it at default opacity for first tick.  Set to 0 opacity.
			else
			{
				this.inst.opacity = 0;
				this.runtime.redraw = true;
			}
		}
	};
	
	behinstProto.saveToJSON = function ()
	{
		return {
			"fit": this.fadeInTime,
			"wt": this.waitTime,
			"fot": this.fadeOutTime,
			"s": this.stage,
			"st": this.stageTime.sum,
			"mo": this.maxOpacity,
		};
	};
	
	behinstProto.loadFromJSON = function (o)
	{
		this.fadeInTime = o["fit"];
		this.waitTime = o["wt"];
		this.fadeOutTime = o["fot"];
		this.stage = o["s"];
		this.stageTime.reset();
		this.stageTime.sum = o["st"];
		this.maxOpacity = o["mo"];
	};

	behinstProto.tick = function ()
	{
		this.stageTime.add(this.runtime.getDt(this.inst));
		
		// Stage 0: fade-in
		if (this.stage === 0)
		{
			this.inst.opacity = (this.stageTime.sum / this.fadeInTime) * this.maxOpacity;
			this.runtime.redraw = true;
			
			// Fade-in completed
			if (this.inst.opacity >= this.maxOpacity)
			{
				this.inst.opacity = this.maxOpacity;
				this.stage = 1;	// wait stage
				this.stageTime.reset();
				
				// On fade-in end
				this.runtime.trigger(cr.behaviors.Fade.prototype.cnds.OnFadeInEnd, this.inst);
			}
		}
		
		// Stage 1: wait
		if (this.stage === 1)
		{
			// Wait time has elapsed
			if (this.stageTime.sum >= this.waitTime)
			{
				this.stage = 2;	// fade out stage
				this.stageTime.reset();
				
				// On wait end
				this.runtime.trigger(cr.behaviors.Fade.prototype.cnds.OnWaitEnd, this.inst);
			}
		}
		
		// Stage 2: fade out
		if (this.stage === 2)
		{
			if (this.fadeOutTime !== 0)
			{
				this.inst.opacity = this.maxOpacity - ((this.stageTime.sum / this.fadeOutTime) * this.maxOpacity);
				this.runtime.redraw = true;
				
				// Fade-out completed
				if (this.inst.opacity < 0)
				{
					this.inst.opacity = 0;
					this.stage = 3;	// done
					this.stageTime.reset();
					
					// On fade-out end
					this.runtime.trigger(cr.behaviors.Fade.prototype.cnds.OnFadeOutEnd, this.inst);
					
					// Destroy after fade out
					if (this.destroy === 1)
						this.runtime.DestroyInstance(this.inst);
				}
			}
		}
	};
	
	behinstProto.doStart = function ()
	{
		this.stage = 0;
		this.stageTime.reset();
		
		// Skip fade-in
		if (this.fadeInTime === 0)
		{
			this.stage = 1;
			
			// Skip wait
			if (this.waitTime === 0)
				this.stage = 2;
		}
		// Otherwise we don't want it at default opacity for first tick.  Set to 0 opacity.
		else
		{
			this.inst.opacity = 0;
			this.runtime.redraw = true;
		}
	};
	
	/**BEGIN-PREVIEWONLY**/
	var stages = ["fade-in", "wait", "fade-out", "done"];
	behinstProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": this.type.name,
			"properties": [
				{"name": "Fade in time", "value": this.fadeInTime},
				{"name": "Wait time", "value": this.waitTime},
				{"name": "Fade out time", "value": this.fadeOutTime},
				{"name": "Stage", "value": stages[this.stage], "readonly": true},
			]
		});
	};
	
	behinstProto.onDebugValueEdited = function (header, name, value)
	{
		switch (name) {
		case "Fade in time":		this.fadeInTime = value;		break;
		case "Wait time":			this.waitTime = value;			break;
		case "Fade out time":		this.fadeOutTime = value;		break;
		}
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	Cnds.prototype.OnFadeOutEnd = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnFadeInEnd = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnWaitEnd = function ()
	{
		return true;
	};
	
	behaviorProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};
	
	Acts.prototype.StartFade = function ()
	{
		if (this.stage === 3)
			this.doStart();
	};
	
	Acts.prototype.RestartFade = function ()
	{
		this.doStart();
	};
	
	Acts.prototype.SetFadeInTime = function (t)
	{
		if (t < 0)
			t = 0;
		
		this.fadeInTime = t;
	};
	
	Acts.prototype.SetWaitTime = function (t)
	{
		if (t < 0)
			t = 0;
		
		this.waitTime = t;
	};
	
	Acts.prototype.SetFadeOutTime = function (t)
	{
		if (t < 0)
			t = 0;
		
		this.fadeOutTime = t;
	};
	
	behaviorProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.FadeInTime = function (ret)
	{
		ret.set_float(this.fadeInTime);
	};
	
	Exps.prototype.WaitTime = function (ret)
	{
		ret.set_float(this.waitTime);
	};
	
	Exps.prototype.FadeOutTime = function (ret)
	{
		ret.set_float(this.fadeOutTime);
	};
	
	behaviorProto.exps = new Exps();

}());