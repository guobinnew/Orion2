// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.behaviors, "cr.behaviors not created");

/////////////////////////////////////
// Behavior class
cr.behaviors.EightDir = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var behaviorProto = cr.behaviors.EightDir.prototype;
		
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
		
		// Key states
		this.upkey = false;
		this.downkey = false;
		this.leftkey = false;
		this.rightkey = false;
		this.ignoreInput = false;
		
		// Simulated key states
		this.simup = false;
		this.simdown = false;
		this.simleft = false;
		this.simright = false;
		
		// attempted workaround for sticky keys bug
		this.lastuptick = -1;
		this.lastdowntick = -1;
		this.lastlefttick = -1;
		this.lastrighttick = -1;
		
		// Movement
		this.dx = 0;
		this.dy = 0;
	};
	
	var behinstProto = behaviorProto.Instance.prototype;

	behinstProto.onCreate = function()
	{
		// Load properties
		this.maxspeed = this.properties[0];
		this.acc = this.properties[1];
		this.dec = this.properties[2];
		this.directions = this.properties[3];	// 0=Up & down, 1=Left & right, 2=4 directions, 3=8 directions"
		this.angleMode = this.properties[4];	// 0=No,1=90-degree intervals, 2=45-degree intervals, 3=360 degree (smooth)
		this.defaultControls = (this.properties[5] === 1);	// 0=no, 1=yes
		
		this.enabled = (this.properties[6] !== 0);
		
		// Only bind keyboard events via jQuery if default controls are in use
		if (this.defaultControls && !this.runtime.isDomFree)
		{
			jQuery(document).keydown(
				(function (self) {
					return function(info) {
						self.onKeyDown(info);
					};
				})(this)
			);
			
			jQuery(document).keyup(
				(function (self) {
					return function(info) {
						self.onKeyUp(info);
					};
				})(this)
			);
		}
	};
	
	behinstProto.saveToJSON = function ()
	{
		return {
			"dx": this.dx,
			"dy": this.dy,
			"enabled": this.enabled,
			"maxspeed": this.maxspeed,
			"acc": this.acc,
			"dec": this.dec,
			"ignoreInput": this.ignoreInput
		};
	};
	
	behinstProto.loadFromJSON = function (o)
	{
		this.dx = o["dx"];
		this.dy = o["dy"];
		this.enabled = o["enabled"];
		this.maxspeed = o["maxspeed"];
		this.acc = o["acc"];
		this.dec = o["dec"];
		this.ignoreInput = o["ignoreInput"];
		
		this.upkey = false;
		this.downkey = false;
		this.leftkey = false;
		this.rightkey = false;
		
		this.simup = false;
		this.simdown = false;
		this.simleft = false;
		this.simright = false;
		
		this.lastuptick = -1;
		this.lastdowntick = -1;
		this.lastlefttick = -1;
		this.lastrighttick = -1;
	};

	behinstProto.onKeyDown = function (info)
	{	
		var tickcount = this.runtime.tickcount;
		
		switch (info.which) {
		case 37:	// left
			info.preventDefault();
			
			// Workaround for sticky keys bug: reject if arriving on same tick as onKeyUp event
			if (this.lastlefttick < tickcount)
				this.leftkey = true;
			
			break;
		case 38:	// up
			info.preventDefault();
			
			if (this.lastuptick < tickcount)
				this.upkey = true;
				
			break;
		case 39:	// right
			info.preventDefault();
			
			if (this.lastrighttick < tickcount)
				this.rightkey = true;
				
			break;
		case 40:	// down
			info.preventDefault();
			
			if (this.lastdowntick < tickcount)
				this.downkey = true;
			
			break;
		}
	};

	behinstProto.onKeyUp = function (info)
	{
		var tickcount = this.runtime.tickcount;
		
		switch (info.which) {
		case 37:	// left
			info.preventDefault();
			this.leftkey = false;
			this.lastlefttick = tickcount;
			break;
		case 38:	// up
			info.preventDefault();
			this.upkey = false;
			this.lastuptick = tickcount;
			break;
		case 39:	// right
			info.preventDefault();
			this.rightkey = false;
			this.lastrighttick = tickcount;
			break;
		case 40:	// down
			info.preventDefault();
			this.downkey = false;
			this.lastdowntick = tickcount;
			break;
		}
	};

	behinstProto.tick = function ()
	{
		var dt = this.runtime.getDt(this.inst);
		
		var left = this.leftkey || this.simleft;
		var right = this.rightkey || this.simright;
		var up = this.upkey || this.simup;
		var down = this.downkey || this.simdown;
		this.simleft = false;
		this.simright = false;
		this.simup = false;
		this.simdown = false;
		
		if (!this.enabled)
			return;
		
		// Is already overlapping solid: must have moved itself in (e.g. by rotating or being crushed),
		// so push out
		var collobj = this.runtime.testOverlapSolid(this.inst);
		if (collobj)
		{
			this.runtime.registerCollision(this.inst, collobj);
			if (!this.runtime.pushOutSolidNearest(this.inst))
				return;		// must be stuck in solid
		}
		
		// Ignoring input: ignore all keys
		if (this.ignoreInput)
		{
			left = false;
			right = false;
			up = false;
			down = false;
		}
		
		// Up & down mode: ignore left & right keys
		if (this.directions === 0)
		{
			left = false;
			right = false;
		}
		// Left & right mode: ignore up & down keys
		else if (this.directions === 1)
		{
			up = false;
			down = false;
		}
		
		// 4 directions mode: up/down take priority over left/right
		if (this.directions === 2 && (up || down))
		{
			left = false;
			right = false;
		}
		
		// Apply deceleration when no arrow key pressed, for each axis
		if (left == right)	// both up or both down
		{
			if (this.dx < 0)
			{
				this.dx += this.dec * dt;
				
				if (this.dx > 0)
					this.dx = 0;
			}
			else if (this.dx > 0)
			{
				this.dx -= this.dec * dt;
				
				if (this.dx < 0)
					this.dx = 0;
			}
		}
		
		if (up == down)
		{
			if (this.dy < 0)
			{
				this.dy += this.dec * dt;
				
				if (this.dy > 0)
					this.dy = 0;
			}
			else if (this.dy > 0)
			{
				this.dy -= this.dec * dt;
				
				if (this.dy < 0)
					this.dy = 0;
			}
		}
		
		// Apply acceleration
		if (left && !right)
		{
			// Moving in opposite direction to current motion: add deceleration
			if (this.dx > 0)
				this.dx -= (this.acc + this.dec) * dt;
			else
				this.dx -= this.acc * dt;
		}
		
		if (right && !left)
		{
			if (this.dx < 0)
				this.dx += (this.acc + this.dec) * dt;
			else
				this.dx += this.acc * dt;
		}
		
		if (up && !down)
		{
			if (this.dy > 0)
				this.dy -= (this.acc + this.dec) * dt;
			else
				this.dy -= this.acc * dt;
		}
		
		if (down && !up)
		{
			if (this.dy < 0)
				this.dy += (this.acc + this.dec) * dt;
			else
				this.dy += this.acc * dt;
		}
		
		var ax, ay;
		
		if (this.dx !== 0 || this.dy !== 0)
		{
			// Limit to max speed
			var speed = Math.sqrt(this.dx * this.dx + this.dy * this.dy);
			
			if (speed > this.maxspeed)
			{
				// Limit vector magnitude to maxspeed
				var a = Math.atan2(this.dy, this.dx);
				this.dx = this.maxspeed * Math.cos(a);
				this.dy = this.maxspeed * Math.sin(a);
			}
			
			// Save old position and angle
			var oldx = this.inst.x;
			var oldy = this.inst.y;
			var oldangle = this.inst.angle;
			
			// Attempt X movement
			this.inst.x += this.dx * dt;
			this.inst.set_bbox_changed();
			
			collobj = this.runtime.testOverlapSolid(this.inst);
			if (collobj)
			{
				this.inst.x = oldx;
				this.dx = 0;
				this.inst.set_bbox_changed();
				this.runtime.registerCollision(this.inst, collobj);
			}
			
			this.inst.y += this.dy * dt;
			this.inst.set_bbox_changed();
			
			collobj = this.runtime.testOverlapSolid(this.inst);
			if (collobj)
			{
				this.inst.y = oldy;
				this.dy = 0;
				this.inst.set_bbox_changed();
				this.runtime.registerCollision(this.inst, collobj);
			}
			
			ax = cr.round6dp(this.dx);
			ay = cr.round6dp(this.dy);
			
			// Apply angle so long as object is still moving and isn't entirely blocked by a solid
			if (ax !== 0 || ay !== 0)
			{
				if (this.angleMode === 1)	// 90 degree intervals
					this.inst.angle = cr.to_clamped_radians(Math.round(cr.to_degrees(Math.atan2(ay, ax)) / 90.0) * 90.0);
				else if (this.angleMode === 2)	// 45 degree intervals
					this.inst.angle = cr.to_clamped_radians(Math.round(cr.to_degrees(Math.atan2(ay, ax)) / 45.0) * 45.0);
				else if (this.angleMode === 3)	// 360 degree
					this.inst.angle = Math.atan2(ay, ax);
			}
				
			this.inst.set_bbox_changed();
			
			if (this.inst.angle != oldangle)
			{
				collobj = this.runtime.testOverlapSolid(this.inst);
				if (collobj)
				{
					this.inst.angle = oldangle;
					this.inst.set_bbox_changed();
					this.runtime.registerCollision(this.inst, collobj);
				}
			}
		}
	};
	
	/**BEGIN-PREVIEWONLY**/
	behinstProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": this.type.name,
			"properties": [
				{"name": "Vector X", "value": this.dx},
				{"name": "Vector Y", "value": this.dy},
				{"name": "Overall speed", "value": Math.sqrt(this.dx * this.dx + this.dy * this.dy)},
				{"name": "Angle of motion", "value": cr.to_degrees(Math.atan2(this.dy, this.dx)), "readonly": true},
				{"name": "Max speed", "value": this.maxspeed},
				{"name": "Acceleration", "value": this.acc},
				{"name": "Deceleration", "value": this.dec},
				{"name": "Enabled", "value": this.enabled}
			]
		});
	};
	
	behinstProto.onDebugValueEdited = function (header, name, value)
	{
		var a;
		
		switch (name) {
		case "Vector X":			this.dx = value;			break;
		case "Vector Y":			this.dy = value;			break;
		case "Overall speed":
			if (value < 0)
				value = 0;
			if (value > this.maxspeed)
				value = this.maxspeed;
				
			a = Math.atan2(this.dy, this.dx);
			this.dx = value * Math.cos(a);
			this.dy = value * Math.sin(a);
			break;
		case "Max speed":			this.maxspeed = value;		break;
		case "Acceleration":		this.acc = value;			break;
		case "Deceleration":		this.dec = value;			break;
		case "Enabled":				this.enabled = value;		break;
		}
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	Cnds.prototype.IsMoving = function ()
	{
		return this.dx !== 0 || this.dy !== 0;
	};
	
	Cnds.prototype.CompareSpeed = function (cmp, s)
	{
		var speed = Math.sqrt(this.dx * this.dx + this.dy * this.dy);
		
		return cr.do_cmp(speed, cmp, s);
	};
	
	behaviorProto.cnds = new Cnds();

	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.Stop = function ()
	{
		this.dx = 0;
		this.dy = 0;
	};
	
	Acts.prototype.Reverse = function ()
	{
		this.dx *= -1;
		this.dy *= -1;
	};
	
	Acts.prototype.SetIgnoreInput = function (ignoring)
	{
		this.ignoreInput = ignoring;
	};
	
	Acts.prototype.SetSpeed = function (speed)
	{
		if (speed < 0)
			speed = 0;
		if (speed > this.maxspeed)
			speed = this.maxspeed;
			
		// Speed is new magnitude of vector of motion
		var a = Math.atan2(this.dy, this.dx);
		this.dx = speed * Math.cos(a);
		this.dy = speed * Math.sin(a);
	};
	
	Acts.prototype.SetMaxSpeed = function (maxspeed)
	{
		this.maxspeed = maxspeed;
		
		if (this.maxspeed < 0)
			this.maxspeed = 0;
	};
	
	Acts.prototype.SetAcceleration = function (acc)
	{
		this.acc = acc;
		
		if (this.acc < 0)
			this.acc = 0;
	};
	
	Acts.prototype.SetDeceleration = function (dec)
	{
		this.dec = dec;
		
		if (this.dec < 0)
			this.dec = 0;
	};
	
	Acts.prototype.SimulateControl = function (ctrl)
	{
		// 0=left, 1=right, 2=up, 3=down
		switch (ctrl) {
		case 0:		this.simleft = true;	break;
		case 1:		this.simright = true;	break;
		case 2:		this.simup = true;		break;
		case 3:		this.simdown = true;	break;
		}
	};
	
	Acts.prototype.SetEnabled = function (en)
	{
		this.enabled = (en === 1);
	};
	
	Acts.prototype.SetVectorX = function (x_)
	{
		this.dx = x_;
	};
	
	Acts.prototype.SetVectorY = function (y_)
	{
		this.dy = y_;
	};
	
	behaviorProto.acts = new Acts();

	//////////////////////////////////////
	// Expressions
	function Exps() {};

	Exps.prototype.Speed = function (ret)
	{
		ret.set_float(Math.sqrt(this.dx * this.dx + this.dy * this.dy));
	};
	
	Exps.prototype.MaxSpeed = function (ret)
	{
		ret.set_float(this.maxspeed);
	};
	
	Exps.prototype.Acceleration = function (ret)
	{
		ret.set_float(this.acc);
	};
	
	Exps.prototype.Deceleration = function (ret)
	{
		ret.set_float(this.dec);
	};
	
	Exps.prototype.MovingAngle = function (ret)
	{
		ret.set_float(cr.to_degrees(Math.atan2(this.dy, this.dx)));
	};
	
	Exps.prototype.VectorX = function (ret)
	{
		ret.set_float(this.dx);
	};
	
	Exps.prototype.VectorY = function (ret)
	{
		ret.set_float(this.dy);
	};
	
	behaviorProto.exps = new Exps();
	
}());