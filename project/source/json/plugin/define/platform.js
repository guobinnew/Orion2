function GetBehaviorSettings()
{
	return {
		"name":			"Platform",
		"id":			"Platform",
		"version":		"1.0",
		"description":	"Jump and run between platforms (solid objects).",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/100/platform",
		"category":		"Movements",
		"flags":		0
	};
};

//////////////////////////////////////////////////////////////
// Conditions
AddCondition(0, 0, "Is moving", "", "{my} is moving", "True when the object is moving.", "IsMoving");

AddCmpParam("Comparison", "Choose the way to compare the current speed.");
AddNumberParam("Speed", "The speed, in pixels per second, to compare the current speed to.");
AddCondition(1, 0, "Compare speed", "", "{my} speed {0} {1}", "Compare the current speed of the object.", "CompareSpeed");

AddCondition(2, 0, "Is on floor", "", "{my} is on floor", "True when the object is on top of a solid or platform.", "IsOnFloor");

AddCondition(3, 0, "Is jumping", "", "{my} is jumping", "True when the object is moving upwards (i.e. jumping).", "IsJumping");

AddCondition(4, 0, "Is falling", "", "{my} is falling", "True when the object is moving downwards (i.e. falling).", "IsFalling");

AddCondition(5, cf_trigger, "On jump", "Animation triggers", "{my} On jump", "Triggered when jumping.", "OnJump");
AddCondition(6, cf_trigger, "On fall", "Animation triggers", "{my} On fall", "Triggered when falling.", "OnFall");
AddCondition(7, cf_trigger, "On stopped", "Animation triggers", "{my} On stopped", "Triggered when stopped moving.", "OnStop");
AddCondition(8, cf_trigger, "On moved", "Animation triggers", "{my} On moved", "Triggered when starting to move.", "OnMove");
AddCondition(9, cf_trigger, "On landed", "Animation triggers", "{my} On landed", "Triggered when first hitting the floor.", "OnLand");

AddComboParamOption("left");
AddComboParamOption("right");
AddComboParam("Side", "Select the side to test for a wall.");
AddCondition(10, 0, "Is by wall", "", "{my} has wall to {0}", "Test if the object has a wall to the left or right.", "IsByWall");

AddCondition(11, 0, "Is double-jump enabled", "", "{my} double-jump is enabled", "True if double-jump is currently enabled.", "IsDoubleJumpEnabled");

//////////////////////////////////////////////////////////////
// Actions
AddComboParamOption("Stop ignoring");
AddComboParamOption("Start ignoring");
AddComboParam("Input", "Set whether to ignore the controls for this movement.");
AddAction(0, 0, "Set ignoring input", "", "{0} {my} user input", "Set whether to ignore the controls for this movement.", "SetIgnoreInput");

AddNumberParam("Max Speed", "The new maximum speed of the object to set, in pixels per second.");
AddAction(1, 0, "Set max speed", "", "Set {my} maximum speed to <i>{0}</i>", "Set the object's maximum speed.", "SetMaxSpeed");

AddNumberParam("Acceleration", "The new acceleration of the object to set, in pixels per second per second.");
AddAction(2, 0, "Set acceleration", "", "Set {my} acceleration to <i>{0}</i>", "Set the object's acceleration.", "SetAcceleration");

AddNumberParam("Deceleration", "The new deceleration of the object to set, in pixels per second per second.");
AddAction(3, 0, "Set deceleration", "", "Set {my} deceleration to <i>{0}</i>", "Set the object's deceleration.", "SetDeceleration");

AddNumberParam("Jump strength", "The new speed at which jumps start, in pixels per second.");
AddAction(4, 0, "Set jump strength", "", "Set {my} jump strength to <i>{0}</i>", "Set the object's jump strength.", "SetJumpStrength");

AddNumberParam("Gravity", "The new acceleration from gravity, in pixels per second per second.");
AddAction(5, 0, "Set gravity", "", "Set {my} gravity to <i>{0}</i>", "Set the object's gravity.", "SetGravity");

AddNumberParam("Max fall speed", "The new maximum speed object can reach in freefall, in pixels per second.");
AddAction(6, 0, "Set max fall speed", "", "Set {my} max fall speed to <i>{0}</i>", "Set the object's maximum fall speed.", "SetMaxFallSpeed");

AddComboParamOption("Left");
AddComboParamOption("Right");
AddComboParamOption("Jump");
AddComboParam("Control", "The movement control to simulate pressing.");
AddAction(7, 0, "Simulate control", "", "Simulate {my} pressing {0}", "Control the movement by events.", "SimulateControl");

AddNumberParam("Vector X", "The new horizontal movement vector, in pixels per second.");
AddAction(8, 0, "Set vector X", "", "Set {my} vector X to <i>{0}</i>", "Set the X component of motion.", "SetVectorX");

AddNumberParam("Vector Y", "The new vertical movement vector, in pixels per second.");
AddAction(9, 0, "Set vector Y", "", "Set {my} vector Y to <i>{0}</i>", "Set the Y component of motion.", "SetVectorY");

AddNumberParam("Angle", "The angle of gravity in degrees.");
AddAction(10, 0, "Set angle of gravity", "", "Set {my} angle of gravity to <i>{0}</i> degrees", "Change the angle the player falls at.", "SetGravityAngle");

AddComboParamOption("Disabled");
AddComboParamOption("Enabled");
AddComboParam("State", "Set whether to enable or disable the behavior.");
AddAction(11, 0, "Set enabled", "", "Set {my} <b>{0}</b>", "Set whether this behavior is enabled.", "SetEnabled");

AddAction(12, 0, "Fall through", "", "Fall {my} down through jump-thru", "Fall through a jump-thru platform.", "FallThrough");

AddComboParamOption("Disabled");
AddComboParamOption("Enabled");
AddComboParam("State", "Set whether to enable or disable double jumps.");
AddAction(13, 0, "Set double-jump", "", "Set {my} double-jump <b>{0}</b>", "Set whether double-jump is enabled.", "SetDoubleJumpEnabled");

AddNumberParam("Jump sustain", "The new jump sustain, in milliseconds to sustain jump velocity for.");
AddAction(14, 0, "Set jump sustain", "", "Set {my} jump sustain to <b>{0}</b> ms", "Set the jump sustain property.", "SetJumpSustain");

//////////////////////////////////////////////////////////////
// Expressions
AddExpression(0, ef_return_number, "Get speed", "", "Speed", "The current object speed, in pixels per second.");
AddExpression(1, ef_return_number, "Get max speed", "", "MaxSpeed", "The maximum speed setting, in pixels per second.");
AddExpression(2, ef_return_number, "Get acceleration", "", "Acceleration", "The acceleration setting, in pixels per second per second.");
AddExpression(3, ef_return_number, "Get deceleration", "", "Deceleration", "The deceleration setting, in pixels per second per second.");
AddExpression(4, ef_return_number, "Get jump strength", "", "JumpStrength", "The jump strength setting, in pixels per second.");
AddExpression(5, ef_return_number, "Get gravity", "", "Gravity", "The gravity setting, in pixels per second per second.");
AddExpression(6, ef_return_number, "Get max fall speed", "", "MaxFallSpeed", "The maximum fall speed setting, in pixels per second.");
AddExpression(7, ef_return_number, "Get angle of motion", "", "MovingAngle", "The current angle of motion, in degrees.");
AddExpression(8, ef_return_number, "Get vector X", "", "VectorX", "The current X component of motion, in pixels.");
AddExpression(9, ef_return_number, "Get vector Y", "", "VectorY", "The current Y component of motion, in pixels.");

AddExpression(10, ef_return_number, "Get gravity angle", "", "GravityAngle", "The angle of gravity, in degrees.");

AddExpression(11, ef_return_number, "", "", "JumpSustain", "The jump sustain property, in milliseconds.");

ACESDone();

// Property grid properties for this plugin
var property_list = [
	new cr.Property(ept_float, "Max speed", 330, "The maximum speed, in pixels per second, the object can accelerate to."),
	new cr.Property(ept_float, "Acceleration", 1500, "The rate of acceleration, in pixels per second per second."),
	new cr.Property(ept_float, "Deceleration", 1500, "The rate of deceleration, in pixels per second per second."),
	new cr.Property(ept_float, "Jump strength", 650, "Speed at which jumps start, in pixels per second."),
	new cr.Property(ept_float, "Gravity", 1500, "Acceleration from gravity, in pixels per second per second."),
	new cr.Property(ept_float, "Max fall speed", 1000, "Maximum speed object can reach in freefall, in pixels per second."),
	new cr.Property(ept_combo, "Double jump", "Disabled", "Allow a single extra jump while in mid-air before landing.", "Disabled|Enabled"),
	new cr.Property(ept_float, "Jump sustain", 0, "Time in milliseconds to allow jump to sustain for, reaching higher if holding longer."),
	new cr.Property(ept_combo, "Default controls", "Yes", "If enabled, arrow keys control movement.  Otherwise, use the 'simulate control' action.", "No|Yes"),
	new cr.Property(ept_combo, "Initial state", "Enabled", "Whether to initially have the behavior enabled or disabled.", "Disabled|Enabled")
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

// Class representing an individual instance of an object in the IDE
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
}

// Called by the IDE after all initialization on this instance has been completed
IDEInstance.prototype.OnCreate = function()
{
}

// Called by the IDE after a property has been changed
IDEInstance.prototype.OnPropertyChanged = function(property_name)
{
	// Set initial value for "default controls" if empty (added r51)
	if (property_name === "Default controls" && !this.properties["Default controls"])
		this.properties["Default controls"] = "Yes";
}
