function GetBehaviorSettings()
{
	return {
		"name":			"Scroll To",
		"id":			"scrollto",
		"version":		"1.0",
		"description":	"Always center the view on this object, or at the mid-point of multiple objects.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/102/scroll-to",
		"category":		"General",
		"flags":		bf_onlyone
	};
};

// ACEs
AddNumberParam("Magnitude", "The strength of the shake, in maximum pixels to scroll away.", "20");
AddNumberParam("Duration", "The time the shake should last, in seconds.", "0.4");
AddComboParamOption("Reducing magnitude");
AddComboParamOption("Constant magnitude");
AddComboParam("Mode", "Select whether the magnitude gradually reduces to zero over the duration, or stays constant.");
AddAction(0, 0, "Shake", "", "Shake {my} with magnitude <i>{0}</i> for <i>{1}</i> seconds ({2})", "Shake the screen for a duration of time.", "Shake");

AddComboParamOption("disabled");
AddComboParamOption("enabled");
AddComboParam("State", "Whether to enable or disable the behavior.");
AddAction(1, 0, "Set enabled", "", "Set {my} {0}", "Enable or disable the behavior.", "SetEnabled");

ACESDone();

// Property grid properties for this plugin
var property_list = [
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
