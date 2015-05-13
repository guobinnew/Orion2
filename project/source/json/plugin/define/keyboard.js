function GetPluginSettings()
{
	return {
		"name":			"Keyboard",
		"id":			"Keyboard",
		"version":		"1.0",
		"description":	"Retrieve input from the keyboard.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/113/keyboard",
		"category":		"Input",
		"type":			"object",			// not in layout
		"rotatable":	false,
		"flags":		pf_singleglobal
	};
};

//////////////////////////////////////////////////////////////
// Conditions
AddKeybParam("Key", "Choose a key.  Note that international users and users on different operating systems or devices may not have the same keys available.");
AddCondition(0, cf_trigger, "On key pressed", "Keyboard", "On <b>{0}</b> pressed", "Triggered when a keyboard key is pressed.", "OnKey");

AddCondition(1, cf_trigger, "On any key pressed", "Keyboard", "On any key pressed", "Triggered when any keyboard key is pressed.", "OnAnyKey");

AddCondition(7, cf_trigger, "On any key released", "Keyboard", "On any key released", "Triggered when any keyboard key is released.", "OnAnyKeyReleased");

AddKeybParam("Key", "Choose a key.  Note that international users and users on different operating systems or devices may not have the same keys available.");
AddCondition(2,	0,	"Key is down",	"Keyboard", "<b>{0}</b> is down", "Test if a keyboard key is currently held down.", "IsKeyDown");

AddKeybParam("Key", "Choose a key.  Note that international users and users on different operating systems or devices may not have the same keys available.");
AddCondition(3,	cf_trigger,	"On key released",	"Keyboard", "On <b>{0}</b> released", "Triggered when a keyboard key is released.", "OnKeyReleased");

AddNumberParam("Keycode", "Choose a numeric key code to test.");
AddCondition(4, cf_trigger, "On key code pressed", "Key codes", "On key code <b>{0}</b> pressed", "Test for a key pressed by its numeric key code.", "OnKeyCode");

AddNumberParam("Keycode", "Choose a numeric key code to test.");
AddCondition(5,	0,	"Key code is down",	"Key codes", "Key code <b>{0}</b> is down", "Test if a keyboard key is currently held down by its numeric key code.", "IsKeyCodeDown");

AddNumberParam("Keycode", "Choose a numeric key code to test.");
AddCondition(6,	cf_trigger,	"On key code released",	"Key codes", "On key code <b>{0}</b> released", "Test for a key released by its numeric key code.", "OnKeyCodeReleased");

//////////////////////////////////////////////////////////////
// Expressions
AddExpression(0, ef_return_number, "", "Key codes", "LastKeyCode", "Get the key code for the last pressed key.");

AddNumberParam("Key code", "The key code to get a string from");
AddExpression(1, ef_return_string, "", "Key codes", "StringFromKeyCode", "Get a character string representing a key code.");

ACESDone();

// Property grid properties for this plugin
var property_list = [
	];
	
// Called by IDE when a new object type is to be created
function CreateIDEObjectType()
{
	return new IDEObjectType();
}

// Class representing an object type in the IDE
function IDEObjectType()
{
	assert2(this instanceof arguments.callee, "Constructor called as a function");
}

// Called by IDE when a new object instance of this type is to be created
IDEObjectType.prototype.CreateInstance = function(instance)
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
	
// Called by the IDE to draw this instance in the editor
IDEInstance.prototype.Draw = function(renderer)
{
}

// Called by the IDE when the renderer has been released (ie. editor closed)
// All handles to renderer-created resources (fonts, textures etc) must be dropped.
// Don't worry about releasing them - the renderer will free them - just null out references.
IDEInstance.prototype.OnRendererReleased = function()
{
}
