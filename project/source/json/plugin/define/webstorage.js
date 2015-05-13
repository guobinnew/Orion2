function GetPluginSettings()
{
	return {
		"name":	"WebStorage",
		"id": "WebStorage",
		"version":		"1.0",
		"description": "Allows you to store data in the browser session and local storage.",
		"author": "Zack0Wack0/Scirra",
		"help url": "http://www.scirra.com/manual/120/webstorage",
		"category": "Data & Storage",
		"type": "object",
		"rotatable": false,
		"flags": pf_singleglobal
	};
}

//////////////
// Conditions
AddCondition(0,cf_deprecated,"Local storage supported","Local","Local storage supported","Check if local storage is supported by the browser.","LocalStorageEnabled");

AddCondition(1,cf_deprecated,"Session storage supported","Session","Session storage supported","Check if session storage is supported by the browser.","SessionStorageEnabled");

AddStringParam("Key","Enter the name of the key to check.", "\"\"");
AddCondition(3,0,"Local key exists","Local","Local key {0} exists","Check if a key exists in local storage.","LocalStorageExists");

AddStringParam("Key","Enter the name of the key to check.", "\"\"");
AddCondition(4,0,"Session key exists","Session","Session key {0} exists","Check if a key exists in session storage.","SessionStorageExists");

AddCondition(5, cf_trigger, "On quota exceeded", "Storage", "On quota exceeded", "Triggered when the browser's limitations on how much data can be stored are exceeded.", "OnQuotaExceeded");

AddStringParam("Key", "Enter the name of the key to retrieve. The value is treated as a string.");
AddStringParam("Text to compare", "Enter the text to compare the value of the key to.");
AddComboParamOption("Ignore case");
AddComboParamOption("Case sensitive");
AddComboParam("Case sensitivity", "Choose whether capital letters count as different to lowercase.  If ignoring case, \"ABC\" matches \"abc\".", 0);
AddCondition(6, 0, "Compare key value (as text)", "Local", "Local key {0} is <b>{1}</b> <i>({2})</i>", "Retrieve the value of a key as a string, then compare it to some text.", "CompareKeyText");

AddStringParam("Key", "Enter the name of the key to retrieve. The value is treated as a number.");
AddCmpParam("Comparison", "Choose how to compare the value of the key.");
AddNumberParam("Value", "Enter the value to compare to.");
AddCondition(7, 0, "Compare key value (as number)", "Local", "Local key {0} {1} {2}", "Retrieve the value of a key as a number, then compare it.", "CompareKeyNumber");

//////////////
// Actions
AddStringParam("Key","Enter the name of the key to associate the value with.", "\"\"");
AddAnyTypeParam("Value","Enter the value to store.", "\"\"");
AddAction(0,0,"Set local value","Local","Set local key {0} to {1}","Store a value in local storage (available in any session).","StoreLocal");

AddStringParam("Key","Enter the name of the key to associate the value with.", "\"\"");
AddAnyTypeParam("Value","Enter the value to store.", "\"\"");
AddAction(1,0,"Set session value","Session","Set session key {0} to {1}","Store a value in session storage (only available in the current session).","StoreSession");

AddStringParam("Key","Enter the name of the key to remove.", "\"\"");
AddAction(2,0,"Remove local value","Local","Remove local key {0}","Remove a key from local storage.","RemoveLocal");

AddStringParam("Key","Enter the name of the key to remove.", "\"\"");
AddAction(3,0,"Remove session value","Session","Remove session key {0}","Remove a key from session storage.","RemoveSession");

AddAction(4,0,"Clear local storage","Local","Clear local storage","Remove all local keys.","ClearLocal");
AddAction(5,0,"Clear session storage","Session","Clear session storage","Remove all session keys.","ClearSession");

AddStringParam("JSON", "A string of the JSON data to load.");
AddComboParamOption("Set");
AddComboParamOption("Merge");
AddComboParam("Mode", "'Set' will clear local storage then add items.  'Merge' will add or overwrite items.");
AddAction(6, 0, "Load JSON", "Local", "{1} local storage from JSON string <i>{0}</i>", "Load data in to local storage from a JSON string.", "JSONLoad");

//////////////
// Expressions
AddStringParam("Key", "Key name", "\"\"");
AddExpression(0,ef_return_string,"Get local value","Local","LocalValue","Get the value from a key in local storage.");

AddStringParam("Key", "Key name", "\"\"");
AddExpression(1,ef_return_string,"Get session value","Session","SessionValue","Get the value from a key in session storage.");

AddExpression(2,ef_return_number,"Get number of local values","Local","LocalCount","Get the number of keys in local storage.");
AddExpression(3,ef_return_number,"Get number of session values","Session","SessionCount","Get the number of keys in session storage.");

AddNumberParam("Index", "Index", "0");
AddExpression(4,ef_return_string,"Get Nth local value", "Local", "LocalAt", "Get the value stored in the Nth local key.");

AddNumberParam("Index", "Index", "0");
AddExpression(5,ef_return_string,"Get Nth session value", "Session", "SessionAt", "Get the value stored in the Nth session key.");

AddNumberParam("Index", "Index", "0");
AddExpression(6,ef_return_string,"Get Nth local key name", "Local", "LocalKeyAt", "Get the Nth local key's name.");

AddNumberParam("Index", "Index", "0");
AddExpression(7,ef_return_string,"Get Nth session key name", "Session", "SessionKeyAt", "Get the Nth session key's name.");

AddExpression(8, ef_return_string, "Get as JSON", "Local", "AsJSON", "Return the contents of all local storage in JSON format.");

ACESDone();

var property_list = [
];

function CreateIDEObjectType()
{
	return new IDEObjectType();
}

function IDEObjectType()
{
	assert2(this instanceof arguments.callee,"Constructor called as a function");
}

IDEObjectType.prototype.CreateInstance = function(instance)
{
	return new IDEInstance(instance, this);
}

function IDEInstance(instance, type)
{
	assert2(this instanceof arguments.callee,"Constructor called as a function");
	
	this.instance = instance;
	this.type = type;
	
	this.properties = {};
	
	for(property in property_list)
		this.properties[property.name] = property.initial_value;
}

IDEInstance.prototype.OnCreate = function()
{
}

IDEInstance.prototype.OnPropertyChanged = function(property_name)
{
}

IDEInstance.prototype.Draw = function(renderer)
{
}

IDEInstance.prototype.OnRendererReleased = function()
{
}