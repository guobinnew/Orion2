// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.behaviors, "cr.behaviors not created");

/////////////////////////////////////
// Behavior class
cr.behaviors.bound = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var behaviorProto = cr.behaviors.bound.prototype;
		
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
		this.mode = 0;
	};
	
	var behinstProto = behaviorProto.Instance.prototype;

	behinstProto.onCreate = function()
	{
		this.mode = this.properties[0];	// 0 = origin, 1 = edge
	};
	
	behinstProto.tick = function ()
	{
	};

	behinstProto.tick2 = function ()
	{
		this.inst.update_bbox();
		var bbox = this.inst.bbox;
		var layout = this.inst.layer.layout;
		var changed = false;
		
		if (this.mode === 0)	// origin
		{
			if (this.inst.x < 0)
			{
				this.inst.x = 0;
				changed = true;
			}
			if (this.inst.y < 0)
			{
				this.inst.y = 0;
				changed = true;
			}
			if (this.inst.x > layout.width)
			{
				this.inst.x = layout.width;
				changed = true;
			}
			if (this.inst.y > layout.height)
			{
				this.inst.y = layout.height;
				changed = true;
			}
		}
		// Bound by edge (bounding box) mode
		else
		{
			if (bbox.left < 0)
			{
				this.inst.x -= bbox.left;
				changed = true;
			}
			if (bbox.top < 0)
			{
				this.inst.y -= bbox.top;
				changed = true;
			}
			if (bbox.right > layout.width)
			{
				this.inst.x -= (bbox.right - layout.width);
				changed = true;
			}
			if (bbox.bottom > layout.height)
			{
				this.inst.y -= (bbox.bottom - layout.height);
				changed = true;
			}
		}
		
		if (changed)
			this.inst.set_bbox_changed();
	};
	
}());