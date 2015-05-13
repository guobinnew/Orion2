// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.behaviors, "cr.behaviors not created");

/////////////////////////////////////
// Behavior class
cr.behaviors.LOS = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var behaviorProto = cr.behaviors.LOS.prototype;
		
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
		this.obstacleTypes = [];						// object types to check for as obstructions
	};
	
	behtypeProto.findLosBehavior = function (inst)
	{
		var i, len, b;
		for (i = 0, len = inst.behavior_insts.length; i < len; ++i)
		{
			b = inst.behavior_insts[i];
			
			if (b instanceof cr.behaviors.LOS.prototype.Instance && b.type === this)
				return b;
		}
		
		return null;
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
		this.obstacleMode = this.properties[0];		// 0 = solids, 1 = custom
		this.range = this.properties[1];
		this.cone = cr.to_radians(this.properties[2]);
		this.useCollisionCells = (this.properties[3] !== 0);
	};
	
	behinstProto.onDestroy = function ()
	{
	};
	
	// called when saving the full state of the game
	behinstProto.saveToJSON = function ()
	{
		var o = {
			"r": this.range,
			"c": this.cone,
			"t": []
		};
		
		var i, len;
		for (i = 0, len = this.type.obstacleTypes.length; i < len; i++)
		{
			o["t"].push(this.type.obstacleTypes[i].sid);
		}
		
		return o;
	};
	
	// called when loading the full state of the game
	behinstProto.loadFromJSON = function (o)
	{
		this.range = o["r"];
		this.cone = o["c"];
		
		// Each behavior instance will do this repeatedly but oh well
		this.type.obstacleTypes.length = 0;
		var i, len, t;
		for (i = 0, len = o["t"].length; i < len; i++)
		{
			t = this.runtime.getObjectTypeBySid(o["t"][i]);
			if (t)
				this.type.obstacleTypes.push(t);
		}
	};

	behinstProto.tick = function ()
	{
	};
	
	var candidates = [];
	var tmpRect = new cr.rect(0, 0, 0, 0);
	
	behinstProto.hasLOSto = function (x_, y_)
	{
		var startx = this.inst.x;
		var starty = this.inst.y;
		
		var myangle = this.inst.angle;
		
		// If object mirrored, flip the angle
		if (this.inst.width < 0)
			myangle += Math.PI;
		
		if (cr.distanceTo(startx, starty, x_, y_) > this.range)
			return false;		// too far away
			
		var a = cr.angleTo(startx, starty, x_, y_);
		
		if (cr.angleDiff(myangle, a) > this.cone / 2)
			return false;		// outside cone of view
		
		var i, leni, rinst, solid;
		tmpRect.set(startx, starty, x_, y_);
		tmpRect.normalize();
		
		// Solids are obstacles
		if (this.obstacleMode === 0)
		{
			if (this.useCollisionCells)
			{
				this.runtime.getSolidCollisionCandidates(this.inst.layer, tmpRect, candidates);
			}
			else
			{
				solid = this.runtime.getSolidBehavior();
				
				if (solid)
					cr.appendArray(candidates, solid.my_instances.valuesRef());
			}

			for (i = 0, leni = candidates.length; i < leni; ++i)
			{
				rinst = candidates[i];
				
				if (!rinst.extra["solidEnabled"] || rinst === this.inst)
					continue;
				
				if (this.runtime.testSegmentOverlap(startx, starty, x_, y_, rinst))
				{
					candidates.length = 0;
					return false;
				}
			}
		}
		// Custom types are obstacles
		else
		{
			if (this.useCollisionCells)
			{
				this.runtime.getTypesCollisionCandidates(this.inst.layer, this.type.obstacleTypes, tmpRect, candidates);
			}
			else
			{
				for (i = 0, leni = this.type.obstacleTypes.length; i < leni; ++i)
				{
					cr.appendArray(candidates, this.type.obstacleTypes[i].instances);
				}
			}
			
			for (i = 0, leni = candidates.length; i < leni; ++i)
			{
				rinst = candidates[i];
				
				if (rinst === this.inst)
					continue;
				
				if (this.runtime.testSegmentOverlap(startx, starty, x_, y_, rinst))
				{
					candidates.length = 0;
					return false;
				}
			}
		}
		
		candidates.length = 0;
		return true;
	};
	
	/**BEGIN-PREVIEWONLY**/
	behinstProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": this.type.name,
			"properties": [
				{"name": "Range", "value": this.range},
				{"name": "Cone of view", "value": cr.to_degrees(this.cone)}
			]
		});
	};
	
	behinstProto.onDebugValueEdited = function (header, name, value)
	{
		switch (name) {
		case "Range":				this.range = value;					break;
		case "Cone of view":		this.cone = cr.to_radians(value);	break;
		}
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};
	
	var ltopick = new cr.ObjectSet();
	var rtopick = new cr.ObjectSet();

	// Static condition
	Cnds.prototype.HasLOSToObject = function (obj_)
	{
		if (!obj_)
			return false;
		
		var i, j, leni, lenj, linst, losbeh, rinst, pick;
		var lsol = this.objtype.getCurrentSol();
		var rsol = obj_.getCurrentSol();
		var linstances = lsol.getObjects();
		var rinstances = rsol.getObjects();
		
		if (lsol.select_all)
			lsol.else_instances.length = 0;
		if (rsol.select_all)
			rsol.else_instances.length = 0;
		
		var inverted = this.runtime.getCurrentCondition().inverted;
		
		for (i = 0, leni = linstances.length; i < leni; ++i)
		{
			linst = linstances[i];
			pick = false;
			losbeh = this.findLosBehavior(linst);
			assert2(losbeh, "Can't find LOS behavior on instance");
			
			for (j = 0, lenj = rinstances.length; j < lenj; ++j)
			{
				rinst = rinstances[j];
				
				if (linst !== rinst && cr.xor(losbeh.hasLOSto(rinst.x, rinst.y), inverted))
				{
					pick = true;
					rtopick.add(rinst);
				}
			}
			
			if (pick)
				ltopick.add(linst);
		}
		
		var lpicks = ltopick.valuesRef();
		var rpicks = rtopick.valuesRef();
		
		lsol.select_all = false;
		rsol.select_all = false;
		cr.shallowAssignArray(lsol.instances, lpicks);
		cr.shallowAssignArray(rsol.instances, rpicks);
		
		ltopick.clear();
		rtopick.clear();
		
		return lsol.hasObjects();
	};
	
	Cnds.prototype.HasLOSToPosition = function (x_, y_)
	{
		return this.hasLOSto(x_, y_);
	};
	
	behaviorProto.cnds = new Cnds();

	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.SetRange = function (r)
	{
		this.range = r;
	};
	
	Acts.prototype.SetCone = function (c)
	{
		this.cone = cr.to_radians(c);
	};
	
	Acts.prototype.AddObstacle = function (obj_)
	{
		var obstacleTypes = this.type.obstacleTypes;
		
		// Check not already a target, we don't want to add twice
		if (obstacleTypes.indexOf(obj_) !== -1)
			return;
		
		// Check obj is not a member of a family that is already a target
		var i, len, t;
		for (i = 0, len = obstacleTypes.length; i < len; i++)
		{
			t = obstacleTypes[i];
			
			if (t.is_family && t.members.indexOf(obj_) !== -1)
				return;
		}
		
		obstacleTypes.push(obj_);
	};
	
	Acts.prototype.ClearObstacles = function ()
	{
		this.type.obstacleTypes.length = 0;
	};
	
	behaviorProto.acts = new Acts();

	//////////////////////////////////////
	// Expressions
	function Exps() {};

	Exps.prototype.Range = function (ret)
	{
		ret.set_float(this.range);
	};
	
	Exps.prototype.ConeOfView = function (ret)
	{
		ret.set_float(cr.to_degrees(this.cone));
	};
	
	behaviorProto.exps = new Exps();
	
}());