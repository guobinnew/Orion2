function GetPluginSettings()
{
	return {
		"name":			"Array",
		"id":			"Arr",
		"version":		"1.0",
		"description":	"Store an array of values in up to 3 dimensions.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/108/array",
		"category":		"Data & Storage",
		"type":			"object",			// not in layout
		"rotatable":	false,
		"flags":		0
	};
};

//////////////////////////////////////////////////////////////
// Conditions
AddNumberParam("X", "X index (0-based) of array value to compare.", "0");
AddCmpParam("Comparison", "How to compare the value.");
AddAnyTypeParam("Value", "The value to compare the array value to.", "0");
AddCondition(0,	0, "Compare at X", "Array", "Value at <b>{0}</b> {1} <b>{2}</b>", "Compare the value at an X position (1D) in the array.", "CompareX");

AddNumberParam("X", "X index (0-based) of array value to compare.", "0");
AddNumberParam("Y", "Y index (0-based) of array value to compare.", "0");
AddCmpParam("Comparison", "How to compare the value.");
AddAnyTypeParam("Value", "The value to compare the array value to.", "0");
AddCondition(1,	0, "Compare at XY", "Array", "Value at (<b>{0}</b>, <b>{1}</b>) {2} <b>{3}</b>", "Compare the value at an X,Y position (2D) in the array.", "CompareXY");

AddNumberParam("X", "X index (0-based) of array value to compare.", "0");
AddNumberParam("Y", "Y index (0-based) of array value to compare.", "0");
AddNumberParam("Z", "Z index (0-based) of array value to compare.", "0");
AddCmpParam("Comparison", "How to compare the value.");
AddAnyTypeParam("Value", "The value to compare the array value to.", "0");
AddCondition(2,	0, "Compare at XYZ", "Array", "Value at (<b>{0}</b>, <b>{1}</b>, <b>{2}</b>) {3} <b>{4}</b>", "Compare the value at an X,Y,Z position (3D) in the array.", "CompareXYZ");

AddComboParamOption("XYZ");
AddComboParamOption("XY");
AddComboParamOption("X");
AddComboParam("Axes", "Select which dimensions to iterate.  For example, if you have a 3D array, but only want to iterate in 2D, choose 'XY'.");
AddCondition(3, cf_looping, "For each element", "For Each", "For each {0} element", "Repeat the event for each element in the array.", "ArrForEach");

AddCmpParam("Comparison", "How to compare the current value in the for-each loop.");
AddAnyTypeParam("Value", "The value to compare the array value to.", "0");
AddCondition(4,	0, "Compare current value", "For Each", "Current value {0} <b>{1}</b>", "Compare the value of the current element in a for-each loop.", "CompareCurrent");

AddAnyTypeParam("Value", "The value to test if in the array.", "0");
AddCondition(5,	0, "Contains value", "Array", "Contains value {0}", "Test if the array contains a value (at any element).", "Contains");

AddCondition(6,	0, "Is empty", "Array", "Is empty", "Test if the array is empty and contains no elements.", "IsEmpty");

AddComboParamOption("X");
AddComboParamOption("Y");
AddComboParamOption("Z");
AddComboParam("Axis", "Select the axis to compare.");
AddCmpParam("Comparison", "How to compare the size of the given axis.");
AddNumberParam("Value", "The value to compare to.");
AddCondition(7, cf_none, "Compare size", "Array", "{0} size {1} <b>{2}</b>", "Compare the size of one of the array dimensions.", "CompareSize");

//////////////////////////////////////////////////////////////
// Actions
AddAction(0, 0, "Clear", "Array", "Clear", "Reset all values to zero.", "Clear");

AddNumberParam("Width", "The new number of elements on the X axis.", "1");
AddNumberParam("Height", "The new number of elements on the Y axis.", "1");
AddNumberParam("Depth", "The new number of elements on the Z axis.", "1");
AddAction(1, 0, "Set size", "Array", "Set size to (<i>{0}</i>, <i>{1}</i>, <i>{2}</i>)", "Set the number of elements on each axis of the array.", "SetSize");

AddNumberParam("X", "The X index (0-based) of the array value to set.", "0");
AddAnyTypeParam("Value", "The value to store in the array.", "0");
AddAction(2, 0, "Set at X", "Array", "Set value at <i>{0}</i> to <i>{1}</i>", "Set the value at an X position (1D) in the array.", "SetX");

AddNumberParam("X", "The X index (0-based) of the array value to set.", "0");
AddNumberParam("Y", "The Y index (0-based) of the array value to set.", "0");
AddAnyTypeParam("Value", "The value to store in the array.", "0");
AddAction(3, 0, "Set at XY", "Array", "Set value at (<i>{0}</i>, <i>{1}</i>) to <i>{2}</i>", "Set the value at an X,Y position (2D) in the array.", "SetXY");

AddNumberParam("X", "The X index (0-based) of the array value to set.", "0");
AddNumberParam("Y", "The Y index (0-based) of the array value to set.", "0");
AddNumberParam("Z", "The Z index (0-based) of the array value to set.", "0");
AddAnyTypeParam("Value", "The value to store in the array.", "0");
AddAction(4, 0, "Set at XYZ", "Array", "Set value at (<i>{0}</i>, <i>{1}</i>, <i>{2}</i>) to <i>{3}</i>", "Set the value at an X,Y,Z position (3D) in the array.", "SetXYZ");

AddComboParamOption("back");
AddComboParamOption("front");
AddComboParam("Where", "Whether to insert at the beginning or the end of the array.");
AddAnyTypeParam("Value", "The value to insert to the array.", "0");
AddComboParamOption("X");
AddComboParamOption("Y");
AddComboParamOption("Z");
AddComboParam("Axis", "The axis to insert on.");
AddAction(5, 0, "Push", "Manipulation", "Push {0} <i>{1}</i> on {2} axis", "Add a new element to the front or back of an axis.", "Push");

AddComboParamOption("back");
AddComboParamOption("front");
AddComboParam("Where", "Whether to remove at the beginning or the end of the array.");
AddComboParamOption("X");
AddComboParamOption("Y");
AddComboParamOption("Z");
AddComboParam("Axis", "The axis to insert on.");
AddAction(6, 0, "Pop", "Manipulation", "Pop {0} on {1} axis", "Remove an element from the front or back of an axis.", "Pop");

AddComboParamOption("X");
AddComboParamOption("Y");
AddComboParamOption("Z");
AddComboParam("Axis", "The axis to reverse.");
AddAction(7, 0, "Reverse", "Manipulation", "Reverse {0} axis", "Reverse the order of elements on an axis.", "Reverse");

AddComboParamOption("X");
AddComboParamOption("Y");
AddComboParamOption("Z");
AddComboParam("Axis", "The axis to sort.");
AddAction(8, 0, "Sort", "Manipulation", "Sort {0} axis", "Sort the elements on an axis.", "Sort");

AddNumberParam("Index", "The zero-based index to delete.");
AddComboParamOption("X");
AddComboParamOption("Y");
AddComboParamOption("Z");
AddComboParam("Axis", "The axis to delete from.");
AddAction(9, 0, "Delete", "Manipulation", "Delete index {0} from {1} axis", "Delete an element from an axis.", "Delete");

AddAnyTypeParam("Value", "The value to insert to the array.");
AddNumberParam("Index", "The zero-based index to insert to.");
AddComboParamOption("X");
AddComboParamOption("Y");
AddComboParamOption("Z");
AddComboParam("Axis", "The axis to insert to.");
AddAction(10, 0, "Insert", "Manipulation", "Insert <i>{0}</i> at index <i>{1}</i> on {2} axis", "Insert a new element at an index on an axis.", "Insert");

AddStringParam("JSON", "A string of the JSON data to load.");
AddAction(11, 0, "Load", "JSON", "Load from JSON string <i>{0}</i>", "Load from an array previously encoded in JSON format.", "JSONLoad");

AddStringParam("Filename", "The name of the file to download.", "\"data.json\"");
AddAction(12, 0, "Download", "JSON", "Download as JSON data with filename <i>{0}</i>", "Download the contents of the array as a JSON file.", "JSONDownload");

//////////////////////////////////////////////////////////////
// Expressions
AddNumberParam("X", "The X index (0-based) of the array value to get.", "0");
AddExpression(0, ef_return_any | ef_variadic_parameters, "Get value at", "Array", "At", "Get value from the array.  Add second or third parameters to specify Y and Z indices.");

AddExpression(1, ef_return_number, "Get width", "Array", "Width", "Get the number of elements on the X axis of the array.");
AddExpression(2, ef_return_number, "Get height", "Array", "Height", "Get the number of elements on the Y axis of the array.");
AddExpression(3, ef_return_number, "Get depth", "Array", "Depth", "Get the number of elements on the Z axis of the array.");

AddExpression(4, ef_return_number, "Current X", "For Each", "CurX", "Get the current X index in a For Each loop.");
AddExpression(5, ef_return_number, "Current Y", "For Each", "CurY", "Get the current Y index in a For Each loop.");
AddExpression(6, ef_return_number, "Current Z", "For Each", "CurZ", "Get the current Z index in a For Each loop.");
AddExpression(7, ef_return_any, "Current Value", "For Each", "CurValue", "Get the current value in a For Each loop.");

AddExpression(8, ef_return_any, "Front value", "Array", "Front", "Get the front value on the X axis (at 0, 0, 0).");
AddExpression(9, ef_return_any, "Back value", "Array", "Back", "Get the back value on the X axis (at w-1, 0, 0).");

AddAnyTypeParam("Value", "Value to search for.");
AddExpression(10, ef_return_number, "Index of", "Array", "IndexOf", "Find first X index of matching value, or -1 if not found.");

AddAnyTypeParam("Value", "Value to search for.");
AddExpression(11, ef_return_number, "Last index of", "Array", "LastIndexOf", "Find last X index of matching value, or -1 if not found.");

AddExpression(12, ef_return_string, "Get as JSON", "JSON", "AsJSON", "Return the contents of the array in JSON format.");

ACESDone();

// Property grid properties for this plugin
var property_list = [
		new cr.Property(ept_integer,		"Width",		10,			"Initial number of elements on the X axis."),
		new cr.Property(ept_integer,		"Height",		1,			"Initial number of elements on the Y axis."),
		new cr.Property(ept_integer,		"Depth",		1,			"Initial number of elements on the Z axis."),
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
	if (this.properties["Width"] < 0)
		this.properties["Width"] = 0;
		
	if (this.properties["Height"] < 0)
		this.properties["Height"] = 0;
		
	if (this.properties["Depth"] < 0)
		this.properties["Depth"] = 0;
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
