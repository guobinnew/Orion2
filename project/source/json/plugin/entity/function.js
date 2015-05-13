// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.Function = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var pluginProto = cr.plugins_.Function.prototype;
		
	/////////////////////////////////////
	// Object type class
	pluginProto.Type = function(plugin)
	{
		this.plugin = plugin;
		this.runtime = plugin.runtime;
	};

	var typeProto = pluginProto.Type.prototype;

	typeProto.onCreate = function()
	{
	};

	/////////////////////////////////////
	// Instance class
	pluginProto.Instance = function(type)
	{
		this.type = type;
		this.runtime = type.runtime;
	};
	
	var instanceProto = pluginProto.Instance.prototype;
	
	var funcStack = [];
	var funcStackPtr = -1;
	var isInPreview = false;	// set in onCreate
	
	function FuncStackEntry()
	{
		this.name = "";
		this.retVal = 0;
		this.params = [];
	};
	
	function pushFuncStack()
	{
		funcStackPtr++;
		
		if (funcStackPtr === funcStack.length)
			funcStack.push(new FuncStackEntry());
			
		return funcStack[funcStackPtr];
	};
	
	function getCurrentFuncStack()
	{
		if (funcStackPtr < 0)
			return null;
			
		return funcStack[funcStackPtr];
	};
	
	function getOneAboveFuncStack()
	{
		if (!funcStack.length)
			return null;
		
		var i = funcStackPtr + 1;
		
		if (i >= funcStack.length)
			i = funcStack.length - 1;
			
		return funcStack[i];
	};
	
	function popFuncStack()
	{
		assert2(funcStackPtr >= 0, "Popping empty function stack");
		
		funcStackPtr--;
	};

	instanceProto.onCreate = function()
	{
		isInPreview = (typeof cr_is_preview !== "undefined");
		
		// Attach a global function to run functions so JS integrators don't have to
		// resort to appalling hacks
		var self = this;
		
		window["c2_callFunction"] = function (name_, params_)
		{
			var i, len, v;
			var fs = pushFuncStack();
			fs.name = name_.toLowerCase();
			fs.retVal = 0;
			
			if (params_)
			{
				// copy only number and string types; all others just set to 0
				fs.params.length = params_.length;
				
				for (i = 0, len = params_.length; i < len; ++i)
				{
					v = params_[i];
					
					if (typeof v === "number" || typeof v === "string")
						fs.params[i] = v;
					else if (typeof v === "boolean")
						fs.params[i] = (v ? 1 : 0);
					else
						fs.params[i] = 0;
				}
			}
			else
			{
				fs.params.length = 0;
			}
			
			// Note: executing fast trigger path based on fs.name
			self.runtime.trigger(cr.plugins_.Function.prototype.cnds.OnFunction, self, fs.name);
			
			popFuncStack();
			
			return fs.retVal;
		};
	};
	
	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	Cnds.prototype.OnFunction = function (name_)
	{
		var fs = getCurrentFuncStack();
		
		if (!fs)
			return false;
		
		return cr.equals_nocase(name_, fs.name);
	};
	
	Cnds.prototype.CompareParam = function (index_, cmp_, value_)
	{
		var fs = getCurrentFuncStack();
		
		if (!fs)
			return false;
		
		index_ = cr.floor(index_);
		
		if (index_ < 0 || index_ >= fs.params.length)
			return false;
			
		return cr.do_cmp(fs.params[index_], cmp_, value_);
	};
	
	pluginProto.cnds = new Cnds();

	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.CallFunction = function (name_, params_)
	{
		var fs = pushFuncStack();
		fs.name = name_.toLowerCase();
		fs.retVal = 0;
		cr.shallowAssignArray(fs.params, params_);
		
		// Note: executing fast trigger path based on fs.name
		var ran = this.runtime.trigger(cr.plugins_.Function.prototype.cnds.OnFunction, this, fs.name);
		
		// In preview mode, log to the console if nothing was triggered
		if (isInPreview && !ran)
		{
			log("[Construct 2] Function object: called function '" + name_ + "', but no event was triggered. Is the function call spelt incorrectly or no longer used?", "warn");
		}
		
		popFuncStack();
	};
	
	Acts.prototype.SetReturnValue = function (value_)
	{
		var fs = getCurrentFuncStack();
		
		if (fs)
			fs.retVal = value_;
		else
			log("[Construct 2] Function object: used 'Set return value' when not in a function call", "warn");
	};
	
	Acts.prototype.CallExpression = function (unused)
	{
		// no-op: the function will have been called during parameter evaluation.
	};
	
	pluginProto.acts = new Acts();

	//////////////////////////////////////
	// Expressions
	function Exps() {};

	Exps.prototype.ReturnValue = function (ret)
	{
		// The previous function has already popped - so check one level up the function stack
		var fs = getOneAboveFuncStack();
		
		if (fs)
			ret.set_any(fs.retVal);
		else
			ret.set_int(0);
	};
	
	Exps.prototype.ParamCount = function (ret)
	{
		var fs = getCurrentFuncStack();
		
		if (fs)
			ret.set_int(fs.params.length);
		else
		{
			log("[Construct 2] Function object: used 'ParamCount' expression when not in a function call", "warn");
			ret.set_int(0);
		}
	};
	
	Exps.prototype.Param = function (ret, index_)
	{
		index_ = cr.floor(index_);
		var fs = getCurrentFuncStack();
		
		if (fs)
		{
			if (index_ >= 0 && index_ < fs.params.length)
			{
				ret.set_any(fs.params[index_]);
			}
			else
			{
				log("[Construct 2] Function object: in function '" + fs.name + "', accessed parameter out of bounds (accessed index " + index_ + ", " + fs.params.length + " params available)", "warn");
				ret.set_int(0);
			}
		}
		else
		{
			log("[Construct 2] Function object: used 'Param' expression when not in a function call", "warn");
			ret.set_int(0);
		}
	};
	
	Exps.prototype.Call = function (ret, name_)
	{
		var fs = pushFuncStack();
		fs.name = name_.toLowerCase();
		fs.retVal = 0;
		
		// Copy rest of parameters from arguments
		fs.params.length = 0;
		var i, len;
		for (i = 2, len = arguments.length; i < len; i++)
			fs.params.push(arguments[i]);
		
		// Note: executing fast trigger path based on fs.name
		var ran = this.runtime.trigger(cr.plugins_.Function.prototype.cnds.OnFunction, this, fs.name);
		
		// In preview mode, log to the console if nothing was triggered
		if (isInPreview && !ran)
		{
			log("[Construct 2] Function object: expression Function.Call('" + name_ + "' ...) was used, but no event was triggered. Is the function call spelt incorrectly or no longer used?", "warn");
		}
		
		popFuncStack();

		ret.set_any(fs.retVal);
	};
	
	pluginProto.exps = new Exps();

}());