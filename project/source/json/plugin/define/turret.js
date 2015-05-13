function GetBehaviorSettings()
{
	return {
		"name":			"Turret",			// as appears in 'add behavior' dialog, can be changed as long as "id" stays the same
		"id":			"Turret",			// this is used to identify this behavior and is saved to the project; never change it
		"version":		"1.0",				// (float in x.y format) Behavior version - C2 shows compatibility warnings based on this
		"description":	"Detect objects in range, rotating towards them and firing.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/152/turret",
		"category":		"Movements",		// Prefer to re-use existing categories, but you can set anything here
		"flags":		0					// uncomment lines to enable flags...
					//	| bf_onlyone		// can only be added once to an object, e.g. solid
	};
};

////////////////////////////////////////
// Parameter types:
// AddNumberParam(label, description [, initial_string = "0"])			// a number
// AddStringParam(label, description [, initial_string = "\"\""])		// a string
// AddAnyTypeParam(label, description [, initial_string = "0"])			// accepts either a number or string
// AddCmpParam(label, description)										// combo with equal, not equal, less, etc.
// AddComboParamOption(text)											// (repeat before "AddComboParam" to add combo items)
// AddComboParam(label, description [, initial_selection = 0])			// a dropdown list parameter
// AddObjectParam(label, description)									// a button to click and pick an object type
// AddLayerParam(label, description)									// accepts either a layer number or name (string)
// AddLayoutParam(label, description)									// a dropdown list with all project layouts
// AddKeybParam(label, description)										// a button to click and press a key (returns a VK)
// AddAudioFileParam(label, description)								// a dropdown list with all imported project audio files

////////////////////////////////////////
// Conditions

// AddCondition(id,					// any positive integer to uniquely identify this condition
//				flags,				// (see docs) cf_none, cf_trigger, cf_fake_trigger, cf_static, cf_not_invertible,
//									// cf_deprecated, cf_incompatible_with_triggers, cf_looping
//				list_name,			// appears in event wizard list
//				category,			// category in event wizard list
//				display_str,		// as appears in event sheet - use {0}, {1} for parameters and also <b></b>, <i></i>, and {my} for the current behavior icon & name
//				description,		// appears in event wizard dialog when selected
//				script_name);		// corresponding runtime function name
							
AddCondition(0, cf_none, "Has target", "", "{my} has target", "True if the turret currently has a target to aim at.", "HasTarget");

AddCondition(1, cf_trigger, "On shoot", "", "On {my} shoot", "Triggered when the turret can shoot at its target.", "OnShoot");

AddCondition(2, cf_trigger, "On target acquired", "", "On {my} target acquired", "Triggered upon the turret finding a new target within range.", "OnTargetAcquired");

////////////////////////////////////////
// Actions

// AddAction(id,				// any positive integer to uniquely identify this action
//			 flags,				// (see docs) af_none, af_deprecated
//			 list_name,			// appears in event wizard list
//			 category,			// category in event wizard list
//			 display_str,		// as appears in event sheet - use {0}, {1} for parameters and also <b></b>, <i></i>
//			 description,		// appears in event wizard dialog when selected
//			 script_name);		// corresponding runtime function name

AddObjectParam("Target", "Choose an object for the turret to specifically target if in range.");
AddAction(0, af_none, "Acquire target", "", "{my} acquire target {0}", "Target a specific object if in range.", "AcquireTarget");

AddObjectParam("Target", "Choose an object or family for the turret to automatically detect as targets.");
AddAction(1, af_none, "Add object to target", "", "Add {my} target {0}", "Add an object or family to target automatically.", "AddTarget");

AddAction(2, af_none, "Clear targets", "", "Clear {my} targets", "Remove all objects to target automatically.", "ClearTargets");

AddAction(3, af_none, "Unacquire target", "", "Unacquire {my} target", "Unacquire current target so another target can be acquired.", "UnacquireTarget");

AddComboParamOption("disabled");
AddComboParamOption("enabled");
AddComboParam("State", "Choose whether to enable or disable the behavior.");
AddAction(5, af_none, "Set enabled", "Settings", "Set {my} {0}", "Enable or disable the behavior.", "SetEnabled");

AddNumberParam("Range", "The range to set, in pixels.");
AddAction(6, af_none, "Set range", "Settings", "Set {my} range to <i>{0}</i>", "Set the turret range.", "SetRange");

AddNumberParam("Rate of fire", "The rate of fire to set, in seconds.");
AddAction(7, af_none, "Set rate of fire", "Settings", "Set {my} rate of fire to <i>{0}</i>", "Set the turret rate of fire.", "SetRateOfFire");

AddComboParamOption("disabled");
AddComboParamOption("enabled");
AddComboParam("Rotate", "Choose whether to automatically rotate towards targets.");
AddAction(8, af_none, "Set rotate", "Settings", "Set {my} rotate {0}", "Enable or disable rotating to targets.", "SetRotate");

AddNumberParam("Rotate speed", "The rotation speed to set, in degrees per second.");
AddAction(9, af_none, "Set rotate speed", "Settings", "Set {my} rotate speed to <i>{0}</i>", "Set the rotation speed.", "SetRotateSpeed");

AddComboParamOption("first in range");
AddComboParamOption("nearest");
AddComboParam("Target", "Choose how to deal with multiple targets.");
AddAction(10, af_none, "Set target mode", "Settings", "Set {my} target mode to <i>{0}</i>", "Set how to deal with multiple targets.", "SetTargetMode");

AddComboParamOption("disabled");
AddComboParamOption("enabled");
AddComboParam("Predictive aim", "Choose whether or not to use predictive aim.");
AddAction(11, af_none, "Set predictive aim", "Settings", "Set {my} predictive aim {0}", "Set whether to use predictive aim.", "SetPredictiveAim");

AddNumberParam("Projectile speed", "The projectile speed to set for predictive aim, in pixels per second.");
AddAction(12, af_none, "Set projectile speed", "Settings", "Set {my} projectile speed to <i>{0}</i>", "Set the projectile speed for predictive aim to work.", "SetProjectileSpeed");

////////////////////////////////////////
// Expressions

// AddExpression(id,			// any positive integer to uniquely identify this expression
//				 flags,			// (see docs) ef_none, ef_deprecated, ef_return_number, ef_return_string,
//								// ef_return_any, ef_variadic_parameters (one return flag must be specified)
//				 list_name,		// currently ignored, but set as if appeared in event wizard
//				 category,		// category in expressions panel
//				 exp_name,		// the expression name after the dot, e.g. "foo" for "myobject.foo" - also the runtime function name
//				 description);	// description in expressions panel

AddExpression(0, ef_return_number, "", "", "TargetUID", "Get the unique ID (UID) of the current target object.");

AddExpression(1, ef_return_number, "", "", "Range", "Get the current range in pixels.");
AddExpression(2, ef_return_number, "", "", "RateOfFire", "Get the current rate of fire in seconds.");
AddExpression(3, ef_return_number, "", "", "RotateSpeed", "Get the current rotate speed in degrees per second.");

////////////////////////////////////////
ACESDone();

////////////////////////////////////////
// Array of property grid properties for this plugin
// new cr.Property(ept_integer,		name,	initial_value,	description)		// an integer value
// new cr.Property(ept_float,		name,	initial_value,	description)		// a float value
// new cr.Property(ept_text,		name,	initial_value,	description)		// a string
// new cr.Property(ept_combo,		name,	"Item 1",		description, "Item 1|Item 2|Item 3")	// a dropdown list (initial_value is string of initially selected item)

var property_list = [
	new cr.Property(ept_float, 	"Range",			300,		"The range in pixels the turret can detect targets."),
	new cr.Property(ept_float, 	"Rate of fire",		1,			"The time to shoot in seconds."),
	new cr.Property(ept_combo, 	"Rotate",			"Yes",		"Automatically rotate towards targets.", "No|Yes"),
	new cr.Property(ept_float, 	"Rotate speed",		180,		"Rotation speed in degrees per second, if 'Rotate' enabled."),
	new cr.Property(ept_combo, 	"Target mode",		"First in range",	"How to deal with multiple targets.", "First in range|Nearest"),
	new cr.Property(ept_combo, 	"Predictive aim",	"No",		"Aim to where targets are going, rather than where they are.", "No|Yes"),
	new cr.Property(ept_float, 	"Projectile speed",	500,		"The projectile speed for predictive aim to work, in pixels per second."),
	new cr.Property(ept_combo, "Initial state", "Enabled", "Whether to initially have the behavior enabled or disabled.", "Disabled|Enabled"),
	new cr.Property(ept_combo, "Use collision cells", "Yes", "Usually improves performance, but can be slower with very long ranges.", "No|Yes")
	];
	
// Called by IDE when a new behavior type is to be created
function CreateIDEBehaviorType()
{
	return new IDEBehaviorType();
}

// Class representing a behavior type in the IDE
function IDEBehaviorType()
{
	assert2(this instanceof arguments.callee, "Constructor called as a function");
}

// Called by IDE when a new behavior instance of this type is to be created
IDEBehaviorType.prototype.CreateInstance = function(instance)
{
	return new IDEInstance(instance, this);
}

// Class representing an individual instance of the behavior in the IDE
function IDEInstance(instance, type)
{
	assert2(this instanceof arguments.callee, "Constructor called as a function");
	
	// Save the constructor parameters
	this.instance = instance;
	this.type = type;
	
	// Set the default property values from the property table
	this.properties = {};
	
	for (var i = 0; i < property_list.length; i++)
		this.properties[property_list[i].name] = property_list[i].initial_value;
		
	// any other properties here, e.g...
	// this.myValue = 0;
}

// Called by the IDE after all initialization on this instance has been completed
IDEInstance.prototype.OnCreate = function()
{
}

// Called by the IDE after a property has been changed
IDEInstance.prototype.OnPropertyChanged = function(property_name)
{
}
