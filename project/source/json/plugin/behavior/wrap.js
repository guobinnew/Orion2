// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.behaviors, "cr.behaviors not created");

/////////////////////////////////////
// Behavior class
cr.behaviors.wrap = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var behaviorProto = cr.behaviors.wrap.prototype;
		
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
		this.mode = this.properties[0];		// 0 = wrap to layout, 1 = wrap to viewport
	};

	behinstProto.tick = function ()
	{
		var inst = this.inst;
		inst.update_bbox();
		var bbox = inst.bbox;
		var layer = inst.layer;
		var layout = layer.layout;
		
		var lbound = 0, rbound = 0, tbound = 0, bbound = 0;
		
		// wrap to layout
		if (this.mode === 0)
		{
			rbound = layout.width;
			bbound = layout.height;
		}
		// wrap to viewport
		else
		{
			lbound = layer.viewLeft;
			rbound = layer.viewRight;
			tbound = layer.viewTop;
			bbound = layer.viewBottom;
		}
		
		if (bbox.right < lbound)
		{
			inst.x = (rbound - 1) + (inst.x - bbox.left);
			inst.set_bbox_changed();
		}
		else if (bbox.left > rbound)
		{
			inst.x = (lbound + 1) - (bbox.right - inst.x);
			inst.set_bbox_changed();
		}
		else if (bbox.bottom < tbound)
		{
			inst.y = (bbound - 1) + (inst.y - bbox.top);
			inst.set_bbox_changed();
		}
		else if (bbox.top > bbound)
		{
			inst.y = (tbound + 1) - (bbox.bottom - inst.y);
			inst.set_bbox_changed();
		}
	};
	
}());