function GetPluginSettings()
{
	return {
		"name":			"File chooser",
		"id":			"filechooser",
		"version":		"1.0",
		"description":	"Allow a user to pick a file from their device, and access the chosen file.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/182/file-chooser",
		"category":		"Form controls",
		"type":			"world",			// appears in layout
		"rotatable":	false,
		"flags":		pf_position_aces | pf_size_aces
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
// AddAnimationParam(label, description)								// a string intended to specify an animation name
// AddAudioFileParam(label, description)								// a dropdown list with all imported project audio files

////////////////////////////////////////
// Conditions

// AddCondition(id,					// any positive integer to uniquely identify this condition
//				flags,				// (see docs) cf_none, cf_trigger, cf_fake_trigger, cf_static, cf_not_invertible,
//									// cf_deprecated, cf_incompatible_with_triggers, cf_looping
//				list_name,			// appears in event wizard list
//				category,			// category in event wizard list
//				display_str,		// as appears in event sheet - use {0}, {1} for parameters and also <b></b>, <i></i>
//				description,		// appears in event wizard dialog when selected
//				script_name);		// corresponding runtime function name
				
AddCondition(0, cf_trigger, "On changed", "File chooser", "On changed", "Triggered when a file selection is made.", "OnChanged");

////////////////////////////////////////
// Actions

// AddAction(id,				// any positive integer to uniquely identify this action
//			 flags,				// (see docs) af_none, af_deprecated
//			 list_name,			// appears in event wizard list
//			 category,			// category in event wizard list
//			 display_str,		// as appears in event sheet - use {0}, {1} for parameters and also <b></b>, <i></i>
//			 description,		// appears in event wizard dialog when selected
//			 script_name);		// corresponding runtime function name


AddComboParamOption("Invisible");
AddComboParamOption("Visible");
AddComboParam("Visibility", "Choose whether to hide or show the file chooser.");
AddAction(0, af_none, "Set visible", "Appearance", "Set <b>{0}</b>", "Hide or show the file chooser.", "SetVisible");

AddStringParam("Property name", "A CSS property name to set on the control.", "\"color\"");
AddStringParam("Value", "A string to assign as the value for this CSS property.", "\"red\"");
AddAction(1, af_none, "Set CSS style", "Appearance", "Set CSS style {0} to {1}", "Set a CSS style on the control.", "SetCSSStyle");

AddStringParam("File URL", "The URL of the file to release.", "");
AddAction(2, af_none, "Release file", "File chooser", "Release file <i>{0}</i>", "Release a file so it is no longer using memory.", "ReleaseFile");


////////////////////////////////////////
// Expressions

// AddExpression(id,			// any positive integer to uniquely identify this expression
//				 flags,			// (see docs) ef_none, ef_deprecated, ef_return_number, ef_return_string,
//								// ef_return_any, ef_variadic_parameters (one return flag must be specified)
//				 list_name,		// currently ignored, but set as if appeared in event wizard
//				 category,		// category in expressions panel
//				 exp_name,		// the expression name after the dot, e.g. "foo" for "myobject.foo" - also the runtime function name
//				 description);	// description in expressions panel

AddExpression(0, ef_return_number, "", "File chooser", "FileCount", "The number of chosen files.");

AddNumberParam("Index", "Index of file");
AddExpression(1, ef_return_string, "", "File chooser", "FileNameAt", "Name of chosen file at an index.");

AddNumberParam("Index", "Index of file");
AddExpression(2, ef_return_number, "", "File chooser", "FileSizeAt", "Size of chosen file at an index.");

AddNumberParam("Index", "Index of file");
AddExpression(3, ef_return_string, "", "File chooser", "FileTypeAt", "MIME type of chosen file at an index.");

AddNumberParam("Index", "Index of file");
AddExpression(4, ef_return_string, "", "File chooser", "FileURLAt", "A URL to access the data of the chosen file at an index.");

ACESDone();

// Property grid properties for this plugin
var property_list = [
	new cr.Property(ept_text,	"Accept",				"",			"File type filter, e.g. .png or image/*"),
	new cr.Property(ept_combo,	"Select",				"Single",	"Use single or multiple file selection.", "Single|Multiple"),
	new cr.Property(ept_combo,	"Initial visibility",	"Visible",	"Choose whether the progress bar is visible on startup.", "Invisible|Visible"),
	new cr.Property(ept_text,	"ID (optional)",		"",			"An ID for the control allowing it to be styled with CSS from the page HTML.")
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
	return new IDEInstance(instance);
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
		
	// Plugin-specific variables
	this.just_inserted = false;
}

IDEInstance.prototype.OnCreate = function()
{
	this.instance.SetHotspot(new cr.vector2(0, 0));
}

IDEInstance.prototype.OnInserted = function()
{
	this.instance.SetSize(new cr.vector2(200, 25));
}

IDEInstance.prototype.OnDoubleClicked = function()
{
}

// Called by the IDE after a property has been changed
IDEInstance.prototype.OnPropertyChanged = function(property_name)
{
}

IDEInstance.prototype.OnRendererInit = function(renderer)
{
}
	
// Called to draw self in the editor
IDEInstance.prototype.Draw = function(renderer)
{
	renderer.SetTexture(null);
	var quad = this.instance.GetBoundingQuad();

	renderer.Fill(quad, cr.RGB(224, 224, 224));
	renderer.Outline(quad, cr.RGB(0, 0, 0));
}

IDEInstance.prototype.OnRendererReleased = function(renderer)
{
}