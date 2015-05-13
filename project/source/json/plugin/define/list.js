function GetPluginSettings()
{
	return {
		"name":			"List",
		"id":			"List",
		"version":		"1.0",
		"description":	"A list box or dropdown list for choosing an option from.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/155/list",
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
				
AddCmpParam("Comparison", "Select how to compare the current selection.");
AddNumberParam("Index", "The index of the selected item to compare to.");
AddCondition(0, cf_none, "Compare selection", "List", "Selected item {0} <b>{1}</b>", "Compare the index of the currently selected item.", "CompareSelection");

AddCondition(1, cf_trigger, "On selection changed", "List", "On selection changed", "Triggered when the selected item changes.", "OnSelectionChanged");
AddCondition(2, cf_trigger, "On clicked", "List", "On clicked", "Triggered when the list is clicked.", "OnClicked");
AddCondition(3, cf_trigger, "On double-clicked", "List", "On double-clicked", "Triggered when the list is double-clicked.", "OnDoubleClicked");

AddStringParam("Text to compare", "Enter the text to compare with the selected item's text.", "\"\"");
AddComboParamOption("Ignore case");
AddComboParamOption("Case sensitive");
AddComboParam("Case sensitivity", "Choose whether capital letters count as different to lowercase.  If ignoring case, \"ABC\" matches \"abc\".", 0);
AddCondition(4, 0, "Compare selected item text", "List", "Selected item text is <b>{0}</b> <i>({1})</i>", "Compare the text of the currently selected item.", "CompareSelectedText");

AddNumberParam("Index", "The zero-based index of the item text to compare with.");
AddStringParam("Text to compare", "Enter the text to compare with the item's text.", "\"\"");
AddComboParamOption("Ignore case");
AddComboParamOption("Case sensitive");
AddComboParam("Case sensitivity", "Choose whether capital letters count as different to lowercase.  If ignoring case, \"ABC\" matches \"abc\".", 0);
AddCondition(5, 0, "Compare item text at", "List", "Item <b>{0}</b> text is <b>{1}</b> <i>({2})</i>", "Compare the text of an item in the list.", "CompareTextAt");

////////////////////////////////////////
// Actions

// AddAction(id,				// any positive integer to uniquely identify this action
//			 flags,				// (see docs) af_none, af_deprecated
//			 list_name,			// appears in event wizard list
//			 category,			// category in event wizard list
//			 display_str,		// as appears in event sheet - use {0}, {1} for parameters and also <b></b>, <i></i>
//			 description,		// appears in event wizard dialog when selected
//			 script_name);		// corresponding runtime function name

AddNumberParam("Index", "The zero-based index of the item to select.");
AddAction(0, af_none, "Set selection", "Items", "Select item <i>{0}</i>", "Select an item in the list.", "Select");

AddStringParam("Tooltip", "The tooltip to set on the list.");
AddAction(1, af_none, "Set tooltip", "List", "Set tooltip to <i>{0}</i>", "Set the list's tooltip.", "SetTooltip");

AddComboParamOption("Invisible");
AddComboParamOption("Visible");
AddComboParam("Visibility", "Choose whether to hide or show the list.");
AddAction(2, af_none, "Set visible", "Appearance", "Set <b>{0}</b>", "Hide or show the list.", "SetVisible");

AddComboParamOption("Disabled");
AddComboParamOption("Enabled");
AddComboParam("Mode", "Choose whether to enable or disable the list.");
AddAction(3, af_none, "Set enabled", "List", "Set <b>{0}</b>", "Disable or enable the list.", "SetEnabled");

AddAction(4, af_none, "Set focused", "List", "Set focused", "Set the input focus to the list.", "SetFocus");
AddAction(5, af_none, "Set unfocused", "List", "Set unfocused", "Remove the input focus from the list.", "SetBlur");

AddStringParam("Property name", "A CSS property name to set on the control.", "\"color\"");
AddStringParam("Value", "A string to assign as the value for this CSS property.", "\"red\"");
AddAction(6, af_none, "Set CSS style", "Appearance", "Set CSS style {0} to {1}", "Set a CSS style on the control.", "SetCSSStyle");

AddStringParam("Text", "The item text to add.");
AddAction(7, af_none, "Add item", "Items", "Add item <i>{0}</i>", "Append a new item to the list.", "AddItem");

AddNumberParam("Index", "The zero-based index of the item to insert before.");
AddStringParam("Text", "The item text to add.");
AddAction(8, af_none, "Add item at", "Items", "Add item <i>{1}</i> at index <i>{0}</i>", "Append a new item to a specific place in the list.", "AddItemAt");

AddNumberParam("Index", "The zero-based index of the item to remove.");
AddAction(9, af_none, "Remove", "Items", "Remove item <i>{0}</i>", "Remove an item from the list.", "Remove");

AddNumberParam("Index", "The zero-based index of the item to change.");
AddStringParam("Text", "The item text to set.");
AddAction(10, af_none, "Set item text", "Items", "Set item <i>{0}</i> text to <i>{1}</i>", "Set the text of an item in the list.", "SetItemText");

AddAction(11, af_none, "Clear", "Items", "Clear all items", "Remove all items from the list.", "Clear");

////////////////////////////////////////
// Expressions

// AddExpression(id,			// any positive integer to uniquely identify this expression
//				 flags,			// (see docs) ef_none, ef_deprecated, ef_return_number, ef_return_string,
//								// ef_return_any, ef_variadic_parameters (one return flag must be specified)
//				 list_name,		// currently ignored, but set as if appeared in event wizard
//				 category,		// category in expressions panel
//				 exp_name,		// the expression name after the dot, e.g. "foo" for "myobject.foo" - also the runtime function name
//				 description);	// description in expressions panel

AddExpression(0, ef_return_number, "", "List", "ItemCount", "The number of items in the list.");
AddExpression(1, ef_return_number, "", "List", "SelectedIndex", "The zero-based index of the currently selected item.");
AddExpression(2, ef_return_string, "", "List", "SelectedText", "The text of the currently selected item.");
AddExpression(3, ef_return_number, "", "List", "SelectedCount", "In a multi-select list box, the number of currently selected items.");

AddNumberParam("Index", "Selected item number to get.");
AddExpression(4, ef_return_number, "", "List", "SelectedIndexAt", "In a multi-select list box, the index of the Nth selected item.");

AddNumberParam("Index", "Selected item number to get.");
AddExpression(5, ef_return_string, "", "List", "SelectedTextAt", "In a multi-select list box, the text of the Nth selected item.");

AddNumberParam("Index", "Item number to get.");
AddExpression(6, ef_return_string, "", "List", "ItemTextAt", "The text of the Nth item in the list.");

ACESDone();

// Property grid properties for this plugin
var property_list = [
	new cr.Property(ept_text,	"Items",				"Item 1;Item 2;Item 3",			"The initial list of items, separated by semicolons."),
	new cr.Property(ept_text,	"Tooltip",				"",			"Display this text when hovering the mouse over the control."),
	new cr.Property(ept_combo,	"Initial visibility",	"Visible",	"Choose whether the list is visible on startup.", "Invisible|Visible"),
	new cr.Property(ept_combo,	"Enabled",				"Yes",		"Choose whether the list is enabled or disabled on startup.", "No|Yes"),
	new cr.Property(ept_combo,	"Type",					"Dropdown list",		"The kind of control to use.", "List box|Dropdown list"),
	new cr.Property(ept_combo,	"Multi-select",			"No",		"For list boxes, whether to allow multiple selections.", "No|Yes"),
	new cr.Property(ept_combo,	"Auto font size",		"Yes",		"Automatically set the font size depending on the layer scale.", "No|Yes"),
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
	this.font = null;
}

IDEInstance.prototype.OnCreate = function()
{
	this.instance.SetHotspot(new cr.vector2(0, 0));
}

IDEInstance.prototype.OnInserted = function()
{
	this.instance.SetSize(new cr.vector2(150, 22));
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
	if (!this.font)
		this.font = renderer.CreateFont("Arial", 14, false, false);
		
	renderer.SetTexture(null);
	var quad = this.instance.GetBoundingQuad();
	renderer.Fill(quad, this.properties["Enabled"] === "Yes" ? cr.RGB(255, 255, 255) : cr.RGB(224, 224, 224));
	renderer.Outline(quad, cr.RGB(0, 0, 0));
	
	cr.quad.prototype.offset.call(quad, 4, 2);
	
	if (this.properties["Type"] === "List box")
	{
		this.font.DrawText(this.properties["Items"].replace(/;/g, "\n"),
							quad,
							cr.RGB(0, 0, 0),
							ha_left);
	}
	else
	{
		var items = this.properties["Items"].split(";");
		
		if (items.length)
		{
			this.font.DrawText(items[0],
								quad,
								cr.RGB(0, 0, 0),
								ha_left);
			
			cr.quad.prototype.offset.call(quad, -4, -2);			
			quad.tlx = quad.trx - 20;
			quad.blx = quad.brx - 20;
			renderer.Fill(quad, cr.RGB(224, 224, 224));
			renderer.Outline(quad, cr.RGB(0, 0, 0));
			
			var arr = new cr.vector2(quad.trx - 10, (quad.try_ + quad.bly) / 2 + 3);
			var arr1 = new cr.vector2(arr.x - 6, arr.y - 6);
			var arr2 = new cr.vector2(arr.x + 6, arr.y - 6);
			renderer.Line(arr, arr1, cr.RGB(0, 0, 0));
			renderer.Line(arr, arr2, cr.RGB(0, 0, 0));
		}
	}
}

IDEInstance.prototype.OnRendererReleased = function(renderer)
{
	this.font = null;
}