function GetPluginSettings()
{
	return {
		"name":			"AJAX",
		"id":			"AJAX",
		"version":		"1.0",
		"description":	"Request and receive other web pages.  Note cross-domain requests must be allowed for requests to work in preview (from localhost).",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/107/ajax",
		"category":		"Web",
		"type":			"object",			// not in layout
		"rotatable":	false,
		"flags":		pf_singleglobal
	};
};

//////////////////////////////////////////////////////////////
// Conditions
AddStringParam("Tag", "A tag, which can be anything you like, to distinguish between different AJAX requests.", "\"\"");
AddCondition(0,	cf_trigger, "On completed", "AJAX", "On <b>{0}</b> completed", "Triggered when an AJAX request completes successfully.", "OnComplete");

AddStringParam("Tag", "A tag, which can be anything you like, to distinguish between different AJAX requests.", "\"\"");
AddCondition(1,	cf_trigger, "On error", "AJAX", "On <b>{0}</b> error", "Triggered when an AJAX request fails.", "OnError");

AddStringParam("Tag", "A tag, which can be anything you like, to distinguish between different AJAX requests.", "\"\"");
AddCondition(2,	cf_trigger, "On progress", "AJAX", "On <b>{0}</b> progress", "Triggered when an AJAX request has a progress update.", "OnProgress");

//////////////////////////////////////////////////////////////
// Actions
AddStringParam("Tag", "A tag, which can be anything you like, to distinguish between different AJAX requests.", "\"\"");
AddStringParam("URL", "The URL to request.  Note: most browsers prevent cross-domain requests.", "\"http://\"");
AddAction(0, 0, "Request URL", "AJAX", "Request <b>{1}</b> (tag <i>{0}</i>)", "Request a URL by a GET request and retrieve the server response.", "Request");

AddStringParam("Tag", "A tag, which can be anything you like, to distinguish between different AJAX requests.", "\"\"");
AddFileParam("File", "Select a project file to request.");
AddAction(1, 0, "Request project file", "AJAX", "Request <b>{1}</b> (tag <i>{0}</i>)", "Request a file in the project and retrieve its contents.", "RequestFile");

AddStringParam("Tag", "A tag, which can be anything you like, to distinguish between different AJAX requests.", "\"\"");
AddStringParam("URL", "The URL to post to.  Note: most browsers prevent cross-domain posts.", "\"http://\"");
AddStringParam("Data", "The data to post, in query string form.  Be sure to URLEncode any user-entered data.");
AddStringParam("Method", "The HTTP method to use, typically \"POST\".", "\"POST\"");
AddAction(2, 0, "Post to URL", "AJAX", "Send <b>{2}</b> to URL <b>{1}</b> (method <i>{3}</i>, tag <i>{0}</i>)", "Send data with a request to a URL and retrieve the server response.", "Post");

AddNumberParam("Timeout", "The timeout for AJAX requests in seconds. Use -1 for no timeout.");
AddAction(3, 0, "Set timeout", "AJAX", "Set timeout to <i>{0}</i> seconds", "Set the maximum time before a request is considered to have failed.", "SetTimeout");

AddStringParam("Header", "The HTTP header name to set on the request.");
AddStringParam("Value", "A string of the value to set the header to.");
AddAction(4, 0, "Set request header", "AJAX", "Set request header <i>{0}</i> to <i>{1}</i>", "Set a HTTP header on the next request that is made.", "SetHeader");

AddStringParam("MIME type", "The MIME type to interpret the response as.");
AddAction(5, 0, "Override MIME type", "AJAX", "Override MIME type with <b>{0}</b>", "In the next request, override the MIME type indicated by the response.", "OverrideMIMEType");

//////////////////////////////////////////////////////////////
// Expressions
AddExpression(0, ef_return_string, "Get last data", "AJAX", "LastData", "Get the data returned by the last successful request.");
AddExpression(1, ef_return_number, "Get progress", "AJAX", "Progress", "Get the progress, from 0 to 1, of the request in 'On progress'.");

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
