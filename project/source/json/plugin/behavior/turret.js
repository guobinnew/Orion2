// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.behaviors, "cr.behaviors not created");

/////////////////////////////////////
// Behavior class
cr.behaviors.Turret = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var behaviorProto = cr.behaviors.Turret.prototype;
		
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
		this.targetTypes = [];						// object types to check for as targets
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
		// Load properties
		this.range = this.properties[0];
		this.rateOfFire = this.properties[1];
		this.rotateEnabled = (this.properties[2] !== 0);
		this.rotateSpeed = cr.to_radians(this.properties[3]);
		this.targetMode = this.properties[4];		// 0 = first, 1 = nearest
		this.predictiveAim = (this.properties[5] !== 0);
		this.projectileSpeed = this.properties[6];
		this.enabled = (this.properties[7] !== 0);
		this.useCollisionCells = (this.properties[8] !== 0);
		
		this.lastCheckTime = 0;						// last time checked for targets in range
		this.fireTimeCount = this.rateOfFire;		// counts up to rate of fire before shooting. starts in fully reloaded state
		this.currentTarget = null;					// current target object
		this.loadTargetUid = -1;
		this.oldTargetX = 0;
		this.oldTargetY = 0;
		this.lastSpeeds = [0, 0, 0, 0];
		this.speedsCount = 0;
		this.firstTickWithTarget = true;
		
		var self = this;
		
		// Need to know if target object gets destroyed
		if (!this.recycled)
		{
			this.myDestroyCallback = function(inst) {
										self.onInstanceDestroyed(inst);
									};
		}
										
		this.runtime.addDestroyCallback(this.myDestroyCallback);
	};
	
	behinstProto.saveToJSON = function ()
	{
		var o = {
			"r": this.range,
			"rof": this.rateOfFire,
			"re": this.rotateEnabled,
			"rs": this.rotateSpeed,
			"tm": this.targetMode,
			"pa": this.predictiveAim,
			"ps": this.projectileSpeed,
			"en": this.enabled,
			"lct": this.lastCheckTime,
			"ftc": this.fireTimeCount,
			"target": (this.currentTarget ? this.currentTarget.uid : -1),
			"ox": this.oldTargetX,
			"oy": this.oldTargetY,
			"ls": this.lastSpeeds,
			"sc": this.speedsCount,
			"targs": []
		};
		
		var i, len;
		for (i = 0, len = this.type.targetTypes.length; i < len; i++)
		{
			o["targs"].push(this.type.targetTypes[i].sid);
		}
		
		return o;
	};
	
	behinstProto.loadFromJSON = function (o)
	{
		this.range = o["r"];
		this.rateOfFire = o["rof"];
		this.rotateEnabled = o["re"];
		this.rotateSpeed = o["rs"];
		this.targetMode = o["tm"];
		this.predictiveAim = o["pa"];
		this.projectileSpeed = o["ps"];
		this.enabled = o["en"];
		this.lastCheckTime = o["lct"];
		this.fireTimeCount = o["ftc"] || 0;		// not in <r154
		this.loadTargetUid = o["target"];
		this.oldTargetX = o["ox"];
		this.oldTargetY = o["oy"];
		this.lastSpeeds = o["ls"];
		this.speedsCount = o["sc"];
		
		// Each behavior instance will do this repeatedly but oh well
		this.type.targetTypes.length = 0;
		var i, len, t;
		for (i = 0, len = o["targs"].length; i < len; i++)
		{
			t = this.runtime.getObjectTypeBySid(o["targs"][i]);
			if (t)
				this.type.targetTypes.push(t);
		}
	};
	
	behinstProto.afterLoad = function ()
	{
		if (this.loadTargetUid === -1)
			this.currentTarget = null;
		else
			this.currentTarget = this.runtime.getObjectByUID(this.loadTargetUid);
	};
	
	behinstProto.onInstanceDestroyed = function (inst)
	{
		// Floor object being destroyed
		if (this.currentTarget == inst)
			this.currentTarget = null;
	};
	
	behinstProto.onDestroy = function ()
	{
		this.currentTarget = null;
		this.runtime.removeDestroyCallback(this.myDestroyCallback);
	};
	
	behinstProto.addSpeed = function (s)
	{
		if (this.speedsCount < 4)
		{
			this.lastSpeeds[this.speedsCount] = s;
			this.speedsCount++;
		}
		else
		{
			this.lastSpeeds.shift();
			this.lastSpeeds.push(s);
		}
	};
	
	behinstProto.getSpeed = function ()
	{
		var ret = 0;
		var i = 0;
		for ( ; i < this.speedsCount; i++)
		{
			ret += this.lastSpeeds[i];
		}
		return ret / this.speedsCount;
	};
	
	behinstProto.isInRange = function (obj_)
	{
		var inst = this.inst;
		var dx = obj_.x - inst.x;
		var dy = obj_.y - inst.y;
		return dx * dx + dy * dy <= this.range * this.range;
	};
	
	var tmpRect = new cr.rect(0, 0, 0, 0);
	var candidates = [];
	
	behinstProto.lookForFirstTarget = function ()
	{
		var i, len, rinst;
		
		tmpRect.left = this.inst.x - this.range;
		tmpRect.top = this.inst.y - this.range;
		tmpRect.right = this.inst.x + this.range;
		tmpRect.bottom = this.inst.y + this.range;
		
		if (this.useCollisionCells)
		{
			this.runtime.getTypesCollisionCandidates(null, this.type.targetTypes, tmpRect, candidates);
		}
		else
		{
			for (i = 0, len = this.type.targetTypes.length; i < len; ++i)
			{
				cr.appendArray(candidates, this.type.targetTypes[i].instances);
			}
		}
		
		for (i = 0, len = candidates.length; i < len; ++i)
		{
			rinst = candidates[i];
			
			if (this.isInRange(rinst))
			{
				this.currentTarget = rinst;
				candidates.length = 0;
				return;
			}
		}
		
		candidates.length = 0;
	};
	
	behinstProto.lookForNearestTarget = function ()
	{
		var i, len, rinst, dist, dx, dy;
		var myx = this.inst.x;
		var myy = this.inst.y;
		var closest = this.range * this.range;
		this.currentTarget = null;
		
		tmpRect.left = myx - this.range;
		tmpRect.top = myy - this.range;
		tmpRect.right = myx + this.range;
		tmpRect.bottom = myy + this.range;
		
		if (this.useCollisionCells)
		{
			this.runtime.getTypesCollisionCandidates(null, this.type.targetTypes, tmpRect, candidates);
		}
		else
		{
			for (i = 0, len = this.type.targetTypes.length; i < len; ++i)
			{
				cr.appendArray(candidates, this.type.targetTypes[i].instances);
			}
		}
		
		for (i = 0, len = candidates.length; i < len; ++i)
		{
			rinst = candidates[i];
			
			dx = myx - rinst.x;
			dy = myy - rinst.y;
			dist = dx * dx + dy * dy;
			
			if (dist < closest)
			{
				this.currentTarget = rinst;
				closest = dist;
			}
		}
		
		candidates.length = 0;
	};

	behinstProto.tick = function ()
	{
		var dt = this.runtime.getDt(this.inst);
		var nowtime = this.runtime.kahanTime.sum;
		var inst = this.inst;
		
		if (!this.enabled)
			return;
		
		// Unacquire targets that go out of range
		if (this.currentTarget && !this.isInRange(this.currentTarget))
		{
			this.currentTarget = null;
			this.speedsCount = 0;
			this.firstTickWithTarget = true;
		}
			
		// Run range checks every 100ms to save CPU
		if (nowtime >= this.lastCheckTime + 0.1)
		{
			this.lastCheckTime = nowtime;
			
			// Looking for first target
			if (this.targetMode === 0 && !this.currentTarget)
			{
				this.lookForFirstTarget();
				
				// Found a target: trigger On Target Acquired
				if (this.currentTarget)
				{
					this.speedsCount = 0;
					this.firstTickWithTarget = true;
					this.oldTargetX = this.currentTarget.x;
					this.oldTargetY = this.currentTarget.y;
					this.runtime.trigger(cr.behaviors.Turret.prototype.cnds.OnTargetAcquired, this.inst);
				}
			}
			// Looking for nearest target (even if already got a target, a new one might be closer)
			else if (this.targetMode === 1)
			{
				var oldTarget = this.currentTarget;
				
				this.lookForNearestTarget();
				
				// Found a new or different target: trigger On Target Acquired
				if (this.currentTarget && this.currentTarget !== oldTarget)
				{
					this.speedsCount = 0;
					this.firstTickWithTarget = true;
					this.oldTargetX = this.currentTarget.x;
					this.oldTargetY = this.currentTarget.y;
					this.runtime.trigger(cr.behaviors.Turret.prototype.cnds.OnTargetAcquired, this.inst);
				}
			}
		}
		
		// Increment fire time counter
		this.fireTimeCount += dt;
		
		// Track targets
		if (this.currentTarget)
		{
			var targetAngle = cr.angleTo(inst.x, inst.y, this.currentTarget.x, this.currentTarget.y);
			
			// Determine predictive aim
			if (this.predictiveAim)
			{
				var Gx = inst.x;
				var Gy = inst.y;
				var Px = this.currentTarget.x;
				var Py = this.currentTarget.y;
				var h = cr.angleTo(Px, Py, this.oldTargetX, this.oldTargetY);
				
				// Smooth calculated speed over 4 frames to compensate for dt variance
				// Don't add a speed on the first tick with a target, since oldTargetX/Y are
				// initialised to the object's current position, and always log a zero speed
				if (!this.firstTickWithTarget)
					this.addSpeed(cr.distanceTo(Px, Py, this.oldTargetX, this.oldTargetY) / dt);
				var s = this.getSpeed();

				var q = Py - Gy;
				var r = Px - Gx;
				var w = (s * Math.sin(h) * (Gx - Px) - s * Math.cos(h) * (Gy - Py)) / this.projectileSpeed;

				var a = (Math.asin(w / Math.sqrt(q * q + r * r)) - Math.atan2(q, -r)) + Math.PI;

				// If result is NaN revert to dumb aim
				if (!isNaN(a))
					targetAngle = a;
			}
			
			// Rotate towards target
			if (this.rotateEnabled)
			{
				inst.angle = cr.angleRotate(inst.angle, targetAngle, this.rotateSpeed * dt);
				inst.set_bbox_changed();
			}
			
			// Shoot at the rate of fire if within 1 degree of target
			if ((this.fireTimeCount >= this.rateOfFire) &&
				cr.to_degrees(cr.angleDiff(inst.angle, targetAngle)) <= 0.1 &&
				(!this.predictiveAim || this.speedsCount >= 4))
			{
				this.fireTimeCount -= this.rateOfFire;
				
				// If the rate of fire was changed, we might still be over the counter.
				// Just reset to zero in this case.
				if (this.fireTimeCount >= this.rateOfFire)
					this.fireTimeCount = 0;
				
				this.runtime.trigger(cr.behaviors.Turret.prototype.cnds.OnShoot, this.inst);
			}
			
			this.oldTargetX = this.currentTarget.x;
			this.oldTargetY = this.currentTarget.y;
			this.firstTickWithTarget = false;
		}
		
		if (this.fireTimeCount > this.rateOfFire)
			this.fireTimeCount = this.rateOfFire;
	};
	
	/**BEGIN-PREVIEWONLY**/
	behinstProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": this.type.name,
			"properties": [
				{"name": "Range", "value": this.range},
				{"name": "Rate of fire", "value": this.rateOfFire},
				{"name": "Rotate speed", "value": cr.to_degrees(this.rotateSpeed)},
				{"name": "Predictive aiming", "value": this.predictiveAim},
				{"name": "Projectile speed", "value": this.projectileSpeed},
				{"name": "Has target", "value": !!this.currentTarget, "readonly": true},
				{"name": "Target UID", "value": this.currentTarget ? this.currentTarget.uid : 0, "readonly": true},
				{"name": "Enabled", "value": this.enabled}
			]
		});
	};
	
	behinstProto.onDebugValueEdited = function (header, name, value)
	{
		switch (name) {
		case "Range":					this.range = value;					break;
		case "Rate of fire":			this.rateOfFire = value;			break;
		case "Rotate speed":			this.rotateSpeed = cr.to_radians(value); break;
		case "Predictive aiming":		this.predictiveAim = value;			break;
		case "Projectile speed":		this.projectileSpeed = value;		break;
		case "Enabled":					this.enabled = value;				break;
		}
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	Cnds.prototype.HasTarget = function ()
	{
		return !!this.currentTarget;
	};
	
	Cnds.prototype.OnShoot = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnTargetAcquired = function ()
	{
		return true;
	};
	
	behaviorProto.cnds = new Cnds();

	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.AcquireTarget = function (obj_)
	{
		if (!obj_)
			return;
		
		// Check every selected instance of obj
		var instances = obj_.getCurrentSol().getObjects();
		var i, len, inst;
		
		for (i = 0, len = instances.length; i < len; ++i)
		{
			inst = instances[i];
			
			if (this.currentTarget !== inst && this.isInRange(inst))
			{
				this.currentTarget = inst;
				this.speedsCount = 0;
				this.firstTickWithTarget = true;
				this.oldTargetX = this.currentTarget.x;
				this.oldTargetY = this.currentTarget.y;
				this.runtime.trigger(cr.behaviors.Turret.prototype.cnds.OnTargetAcquired, this.inst);
				break;
			}
		}
	};
	
	Acts.prototype.AddTarget = function (obj_)
	{
		var targetTypes = this.type.targetTypes;
		
		// Check not already a target, we don't want to add twice
		if (targetTypes.indexOf(obj_) !== -1)
			return;
		
		// Check obj is not a member of a family that is already a target
		var i, len, t;
		for (i = 0, len = targetTypes.length; i < len; i++)
		{
			t = targetTypes[i];
			
			if (t.is_family && t.members.indexOf(obj_) !== -1)
				return;
		}
		
		targetTypes.push(obj_);
	};
	
	Acts.prototype.ClearTargets = function ()
	{
		this.type.targetTypes.length = 0;
	};
	
	Acts.prototype.UnacquireTarget = function ()
	{
		this.currentTarget = null;
		this.speedsCount = 0;
		this.firstTickWithTarget = true;
	};
	
	Acts.prototype.SetEnabled = function (e)
	{
		this.enabled = (e !== 0);
	};
	
	Acts.prototype.SetRange = function (r)
	{
		this.range = r;
	};
	
	Acts.prototype.SetRateOfFire = function (r)
	{
		this.rateOfFire = r;
	};
	
	Acts.prototype.SetRotate = function (r)
	{
		this.rotateEnabled = (r !== 0);
	};
	
	Acts.prototype.SetRotateSpeed = function (r)
	{
		this.rotateSpeed = cr.to_radians(r);
	};
	
	Acts.prototype.SetTargetMode = function (s)
	{
		this.targetMode = s;
	};
	
	Acts.prototype.SetPredictiveAim = function (s)
	{
		this.predictiveAim = (s !== 0);
	};
	
	Acts.prototype.SetProjectileSpeed = function (s)
	{
		this.projectileSpeed = s;
	};
	
	behaviorProto.acts = new Acts();

	//////////////////////////////////////
	// Expressions
	function Exps() {};

	Exps.prototype.TargetUID = function (ret)
	{
		ret.set_int(this.currentTarget ? this.currentTarget.uid : 0);
	};
	
	Exps.prototype.Range = function (ret)
	{
		ret.set_float(this.range);
	};
	
	Exps.prototype.RateOfFire = function (ret)
	{
		ret.set_float(this.rateOfFire);
	};
	
	Exps.prototype.RotateSpeed = function (ret)
	{
		ret.set_float(cr.to_degrees(this.rotateSpeed));
	};
	
	behaviorProto.exps = new Exps();
	
}());