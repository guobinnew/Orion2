// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.behaviors, "cr.behaviors not created");

/////////////////////////////////////
// Behavior class
cr.behaviors.Bullet = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var behaviorProto = cr.behaviors.Bullet.prototype;
		
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
		var speed = this.properties[0];
		this.acc = this.properties[1];
		this.g = this.properties[2];
		this.bounceOffSolid = (this.properties[3] !== 0);
		this.setAngle = (this.properties[4] !== 0);
		
		this.dx = Math.cos(this.inst.angle) * speed;
		this.dy = Math.sin(this.inst.angle) * speed;
		this.lastx = this.inst.x;
		this.lasty = this.inst.y;		
		this.lastKnownAngle = this.inst.angle;
		this.travelled = 0;
		
		this.enabled = (this.properties[5] !== 0);
	};
	
	behinstProto.saveToJSON = function ()
	{
		return {
			"acc": this.acc,
			"g": this.g,
			"dx": this.dx,
			"dy": this.dy,
			"lx": this.lastx,
			"ly": this.lasty,
			"lka": this.lastKnownAngle,
			"t": this.travelled,
			"e": this.enabled
		};
	};
	
	behinstProto.loadFromJSON = function (o)
	{
		this.acc = o["acc"];
		this.g = o["g"];
		this.dx = o["dx"];
		this.dy = o["dy"];
		this.lastx = o["lx"];
		this.lasty = o["ly"];
		this.lastKnownAngle = o["lka"];
		this.travelled = o["t"];
		this.enabled = o["e"];
	};

	behinstProto.tick = function ()
	{
		if (!this.enabled)
			return;
			
		var dt = this.runtime.getDt(this.inst);
		var s, a;
		var bounceSolid, bounceAngle;
		
		// Object had its angle changed: change angle of motion, providing 'Set angle' is enabled.
		if (this.inst.angle !== this.lastKnownAngle)
		{
			if (this.setAngle)
			{
				s = cr.distanceTo(0, 0, this.dx, this.dy);
				this.dx = Math.cos(this.inst.angle) * s;
				this.dy = Math.sin(this.inst.angle) * s;
			}
			
			this.lastKnownAngle = this.inst.angle;
		}
		
		// Apply acceleration
		if (this.acc !== 0)
		{
			s = cr.distanceTo(0, 0, this.dx, this.dy);
			
			if (this.dx === 0 && this.dy === 0)
				a = this.inst.angle;
			else
				a = cr.angleTo(0, 0, this.dx, this.dy);
				
			s += this.acc * dt;
			
			// Don't decelerate to negative speeds
			if (s < 0)
				s = 0;
			
			this.dx = Math.cos(a) * s;
			this.dy = Math.sin(a) * s;
		}
		
		// Apply gravity
		if (this.g !== 0)
			this.dy += this.g * dt;
			
		this.lastx = this.inst.x;
		this.lasty = this.inst.y;
		
		// Apply movement to the object
		if (this.dx !== 0 || this.dy !== 0)
		{
			this.inst.x += this.dx * dt;
			this.inst.y += this.dy * dt;
			this.travelled += cr.distanceTo(0, 0, this.dx * dt, this.dy * dt)
			
			if (this.setAngle)
			{
				this.inst.angle = cr.angleTo(0, 0, this.dx, this.dy);
				this.inst.set_bbox_changed();
				this.lastKnownAngle = this.inst.angle;
			}
			
			this.inst.set_bbox_changed();
			
			// Is bouncing off solid and has moved in to a solid
			if (this.bounceOffSolid)
			{
				bounceSolid = this.runtime.testOverlapSolid(this.inst);
				
				// Has hit a solid
				if (bounceSolid)
				{
					this.runtime.registerCollision(this.inst, bounceSolid);
					
					s = cr.distanceTo(0, 0, this.dx, this.dy);
					bounceAngle = this.runtime.calculateSolidBounceAngle(this.inst, this.lastx, this.lasty);
					this.dx = Math.cos(bounceAngle) * s;
					this.dy = Math.sin(bounceAngle) * s;
					this.inst.x += this.dx * dt;			// move out for one tick since the object can't have spent a tick in the solid
					this.inst.y += this.dy * dt;
					this.inst.set_bbox_changed();
					
					if (this.setAngle)
					{
						// Setting the object angle after a bounce may cause it to overlap a solid again.
						// Make sure it's pushed out.
						this.inst.angle = bounceAngle;
						this.lastKnownAngle = bounceAngle;
						this.inst.set_bbox_changed();
					}
					
					// Advance the object until it is outside the solid
					if (!this.runtime.pushOutSolid(this.inst, this.dx / s, this.dy / s, Math.max(s * 2.5 * dt, 30)))
						this.runtime.pushOutSolidNearest(this.inst, 100);
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
				{"name": "Overall speed", "value": cr.distanceTo(0, 0, this.dx, this.dy)},
				{"name": "Angle of motion", "value":cr.to_degrees(cr.angleTo(0, 0, this.dx, this.dy))},
				{"name": "Acceleration", "value": this.acc},
				{"name": "Gravity", "value": this.g},
				{"name": "Distance travelled", "value": this.travelled, "readonly": true},
				{"name": "Enabled", "value": this.enabled}
			]
		});
	};
	
	behinstProto.onDebugValueEdited = function (header, name, value)
	{
		var a, s;
		
		switch (name) {
		case "Vector X":				this.dx = value;				break;
		case "Vector Y":				this.dy = value;				break;
		case "Overall speed":
			a = cr.angleTo(0, 0, this.dx, this.dy);
			this.dx = Math.cos(a) * value;
			this.dy = Math.sin(a) * value;
			break;
		case "Angle of motion":
			a = cr.to_radians(value);
			s = cr.distanceTo(0, 0, this.dx, this.dy)
			this.dx = Math.cos(a) * s;
			this.dy = Math.sin(a) * s;
			break;
		case "Acceleration":			this.acc = value;				break;
		case "Gravity":					this.g = value;					break;
		case "Enabled":					this.enabled = value;			break;
		}
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	Cnds.prototype.CompareSpeed = function (cmp, s)
	{
		return cr.do_cmp(cr.distanceTo(0, 0, this.dx, this.dy), cmp, s);
	};
	
	Cnds.prototype.CompareTravelled = function (cmp, d)
	{
		return cr.do_cmp(this.travelled, cmp, d);
	};
	
	behaviorProto.cnds = new Cnds();

	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.SetSpeed = function (s)
	{
		var a = cr.angleTo(0, 0, this.dx, this.dy);
		this.dx = Math.cos(a) * s;
		this.dy = Math.sin(a) * s;
	};
	
	Acts.prototype.SetAcceleration = function (a)
	{
		this.acc = a;
	};
	
	Acts.prototype.SetGravity = function (g)
	{
		this.g = g;
	};
	
	Acts.prototype.SetAngleOfMotion = function (a)
	{
		a = cr.to_radians(a);
		var s = cr.distanceTo(0, 0, this.dx, this.dy)
		this.dx = Math.cos(a) * s;
		this.dy = Math.sin(a) * s;
	};
	
	Acts.prototype.Bounce = function (objtype)
	{
		if (!objtype)
			return;
		
		var otherinst = objtype.getFirstPicked(this.inst);
		
		if (!otherinst)
			return;
			
		var dt = this.runtime.getDt(this.inst);
		var s = cr.distanceTo(0, 0, this.dx, this.dy);
		var bounceAngle = this.runtime.calculateSolidBounceAngle(this.inst, this.lastx, this.lasty, otherinst);
		this.dx = Math.cos(bounceAngle) * s;
		this.dy = Math.sin(bounceAngle) * s;
		this.inst.x += this.dx * dt;			// move out for one tick since the object can't have spent a tick in the solid
		this.inst.y += this.dy * dt;
		this.inst.set_bbox_changed();
		
		if (this.setAngle)
		{
			// Setting the object angle after a bounce may cause it to overlap a solid again.
			// Make sure it's pushed out.
			this.inst.angle = bounceAngle;
			this.lastKnownAngle = bounceAngle;
			this.inst.set_bbox_changed();
		}
		
		// Advance the object until it is outside the solid
		if (this.bounceOffSolid)
		{
			if (!this.runtime.pushOutSolid(this.inst, this.dx / s, this.dy / s, Math.max(s * 2.5 * dt, 30)))
				this.runtime.pushOutSolidNearest(this.inst, 100);
		}
		else
		{
			this.runtime.pushOut(this.inst, this.dx / s, this.dy / s, Math.max(s * 2.5 * dt, 30), otherinst)
		}
	};
	
	Acts.prototype.SetEnabled = function (en)
	{
		this.enabled = (en === 1);
	};
	
	behaviorProto.acts = new Acts();

	//////////////////////////////////////
	// Expressions
	function Exps() {};

	Exps.prototype.Speed = function (ret)
	{
		var s = cr.distanceTo(0, 0, this.dx, this.dy);
		
		// Due to floating point inaccuracy is likely to return 99.9999999 when speed is set to 100.
		// So round to nearest millionth of a pixel per second.
		s = cr.round6dp(s);
		
		ret.set_float(s);
	};
	
	Exps.prototype.Acceleration = function (ret)
	{
		ret.set_float(this.acc);
	};
	
	Exps.prototype.AngleOfMotion = function (ret)
	{
		ret.set_float(cr.to_degrees(cr.angleTo(0, 0, this.dx, this.dy)));
	};
	
	Exps.prototype.DistanceTravelled = function (ret)
	{
		ret.set_float(this.travelled);
	};
	
	behaviorProto.exps = new Exps();
	
}());