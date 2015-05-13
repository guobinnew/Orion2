function GetBehaviorSettings()
{
	return {
		"name":			"Bullet",
		"id":			"Bullet",
		"version":		"1.0",
		"description":	"Move an object ahead at its current angle.  Typically used for bullets and projectiles.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/90/bullet",
		"category":		"Movements",
		"flags":		0
	};
};

//////////////////////////////////////////////////////////////
// Conditions
AddCmpParam("Comparison", "Choose the way to compare the current speed.");
AddNumberParam("Speed", "The speed, in pixels per second, to compare the current speed to.");
AddCondition(0, 0, "Compare speed", "", "{my} speed {0} {1}", "Compare the current speed of the object.", "CompareSpeed");

AddCmpParam("Comparison", "Choose the way to compare the distance travelled.");
AddNumberParam("Distance", "The distance, in pixels, to compare to.");
AddCondition(1, 0, "Compare distance travelled", "", "{my} distance travelled {0} {1}", "Compare the distance the object has travelled.", "CompareTravelled");

//////////////////////////////////////////////////////////////
// Actions
AddNumberParam("Speed", "The new speed of the object to set, in pixels per second.");
AddAction(0, 0, "Set speed", "", "Set {my} speed to <i>{0}</i>", "Set the object's current speed.", "SetSpeed");

AddNumberParam("Acceleration", "The new acceleration of the object to set, in pixels per second per second.");
AddAction(1, 0, "Set acceleration", "", "Set {my} acceleration to <i>{0}</i>", "Set the object's acceleration.", "SetAcceleration");

AddNumberParam("Gravity", "The new acceleration caused by gravity, in pixels per second per second.");
AddAction(2, 0, "Set gravity", "", "Set {my} gravity to <i>{0}</i>", "Set the object's force of gravity.", "SetGravity");

AddNumberParam("Angle", "The new angle of motion, in degrees.");
AddAction(3, 0, "Set angle of motion", "", "Set {my} angle of motion to <i>{0}</i> degrees", "Set the angle the object is moving at.", "SetAngleOfMotion");

AddObjectParam("Object", "The object to bounce off.");
AddAction(4, 0, "Bounce off object", "", "Bounce {my} off {0}", "Bounce the object off an object it is currently touching.", "Bounce");

AddComboParamOption("Disabled");
AddComboParamOption("Enabled");
AddComboParam("State", "Set whether to enable or disable the behavior.");
AddAction(5, 0, "Set enabled", "", "Set {my} <b>{0}</b>", "Set whether this behavior is enabled.", "SetEnabled");

//////////////////////////////////////////////////////////////
// Expressions
AddExpression(0, ef_return_number, "Get speed", "", "Speed", "The current object speed, in pixels per second.");
AddExpression(1, ef_return_number, "Get acceleration", "", "Acceleration", "The acceleration setting, in pixels per second per second.");
AddExpression(2, ef_return_number, "Get angle of motion", "", "AngleOfMotion", "The angle the object is currently moving at, in degrees.");
AddExpression(3, ef_return_number, "Get distance travelled", "", "DistanceTravelled", "The distance, in pixels, the object has travelled so far.");

ACESDone();

// Property grid properties for this plugin
var property_list = [
	new cr.Property(ept_float, "Speed", 400, "The speed, in pixels per second, the object travels at."),
	new cr.Property(ept_float, "Acceleration", 0, "The rate of acceleration, in pixels per second per second.  Negative values decelerate."),
	new cr.Property(ept_float, "Gravity", 0, "Gravity on the bullet, in pixels per second per second."),
	new cr.Property(ept_combo, "Bounce off solids", "No", "Make the object bounce off solid objects when it collides with them.", "No|Yes"),
	new cr.Property(ept_combo, "Set angle", "Yes", "Set the object's angle to the angle of motion.", "No|Yes"),
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
}
