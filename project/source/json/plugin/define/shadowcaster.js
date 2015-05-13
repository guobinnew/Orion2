function GetBehaviorSettings()
{
	return {
		"name":			"Shadow Caster",
		"id":			"shadowcaster",
		"version":		"1.0",
		"description":	"Make the object cast a shadow away from a Shadow Light object.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/177/shadow-caster",
		"category":		"Attributes",
		"flags":		bf_onlyone
	};
};


AddCondition(0, 0, "Is enabled", "", "{my} is enabled", "Test whether the behavior is currently enabled.", "IsEnabled");

AddCmpParam("Comparison", "How to compare the height.");
AddNumberParam("Height", "The object height to compare to.");
AddCondition(1, 0, "Compare height", "", "{my} height {0} {1}", "Compare the height of the object.", "CompareHeight");

//////////////////////////////////////
AddComboParamOption("Disabled");
AddComboParamOption("Enabled");
AddComboParam("State", "Set whether the object counts as solid or not.", 1);
AddAction(0, 0, "Set enabled", "", "Set {my} <b>{0}</b>", "Set whether the object counts as solid or not.", "SetEnabled");

AddNumberParam("Height", "The height to set the object for determining the length of the shadow it casts.");
AddAction(1, 0, "Set height", "", "Set {my} height to <i>{0}</i>", "Set the height of the object for determining the length shadow it casts.", "SetHeight");

AddStringParam("Tag", "The new tag to set for the shadow caster.");
AddAction(2, af_none, "Set tag", "", "Set {my} tag to <i>{0}</i>", "Set the tag for the shadow caster.", "SetTag");

//////////////////////////////////////
AddExpression(0, ef_return_number, "", "", "Height", "The shadow caster object height.");
AddExpression(1, ef_return_string, "", "", "Tag", "The tag of the object.");

ACESDone();

// Property grid properties for this plugin
var property_list = [
	new cr.Property(ept_combo, "Initial state", "Enabled", "Whether to initially have the behavior enabled or disabled.", "Disabled|Enabled"),
	new cr.Property(ept_integer, "Height", 100, "The height of the object in pixels, determining the length shadow it casts."),
	new cr.Property(ept_text, "Tag", "", "A tag to identify which lights to cast a shadow from.")
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
