function GetPluginSettings()
{
	return {
		"name":			"Tiled Background",
		"id":			"TiledBg",
		"version":		"1.0",
		"description":	"Automatically tile an image over an area.  Tiled Backgrounds are much faster than tiling Sprites.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/118/tiled-background",
		"category":		"General",
		"type":			"world",			// appears in layout
		"rotatable":	true,
		"defaultimage":	"default.png",
		"flags":		pf_texture | pf_position_aces | pf_size_aces | pf_angle_aces | pf_appearance_aces | pf_tiling | pf_zorder_aces | pf_effects | pf_predraw
	};
};

// Conditions, actions and expressions
AddCondition(0, cf_trigger, "On image URL loaded", "Web", "On image URL loaded", "Triggered after 'Load image from URL' when the image has finished loading.", "OnURLLoaded");

AddComboParamOption("Normal");
AddComboParamOption("Additive");
AddComboParamOption("XOR");
AddComboParamOption("Copy");
AddComboParamOption("Destination over");
AddComboParamOption("Destination over");
AddComboParamOption("Destination in");
AddComboParamOption("Source out");
AddComboParamOption("Destination out");
AddComboParamOption("Source atop");
AddComboParamOption("Destination atop");
AddComboParam("Blend mode", "Choose the new blend mode for this object.");
AddAction(1, 0, "Set blend mode", "Appearance", "Set blend mode to <i>{0}</i>", "Set the background blend mode for this object.", "SetEffect");

AddStringParam("URI", "Enter the URL on the web, or data URI, of an image to load.", "\"http://\"");
AddAction(2, 0, "Load image from URL", "Web", "Load image from <i>{0}</i>", "Load an image from a web address or data URI.", "LoadURL");

AddExpression(0, ef_return_number, "", "Image", "ImageWidth", "The width of the current image, in pixels.");
AddExpression(1, ef_return_number, "", "Image", "ImageHeight", "The height of the current image, in pixels.");

ACESDone();

// Property grid properties for this plugin
var property_list = [
	new cr.Property(ept_link,	"Image",				lang("project\\misc\\tiledbg-edit-link"), "Click to edit the object's image.", "firstonly"),
	new cr.Property(ept_combo,	"Initial visibility",	"Visible",	"Choose whether the object is visible when the layout starts.", "Visible|Invisible"),
	//new cr.Property(ept_combo,	"Effect",				"(none)",	"Choose an effect for this object.  (This does not preview in the layout, only when you run.)", "(none)|Additive|XOR|Copy|Destination over|Source in|Destination in|Source out|Destination out|Source atop|Destination atop"),
	new cr.Property(ept_combo,	"Hotspot",				"Top-left",	"Choose the location of the hot spot in the object.", "Top-left|Center")
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
	if (this.properties["Hotspot"] === "Top-left")
		this.instance.SetHotspot(new cr.vector2(0, 0));
	else
		this.instance.SetHotspot(new cr.vector2(0.5, 0.5));
}

IDEInstance.prototype.OnInserted = function()
{
	this.just_inserted = true;
}

IDEInstance.prototype.OnDoubleClicked = function()
{
	this.instance.EditTexture();
}

// Called by the IDE after a property has been changed
IDEInstance.prototype.OnPropertyChanged = function(property_name)
{
	// Edit image link
	if (property_name === "Image")
	{
		this.instance.EditTexture();
	}
	else if (property_name === "Hotspot")
	{
		if (this.properties["Hotspot"] === "Top-left")
			this.instance.SetHotspot(new cr.vector2(0, 0));
		else
			this.instance.SetHotspot(new cr.vector2(0.5, 0.5));
	}
}

IDEInstance.prototype.OnRendererInit = function(renderer)
{
	renderer.LoadTexture(this.instance.GetTexture());
}
	
// Called to draw self in the editor
IDEInstance.prototype.Draw = function(renderer)
{
	var texture = this.instance.GetTexture();
	renderer.SetTexture(this.instance.GetTexture());
	
	// First draw after insert: use 2x the size of the texture so user can see four tiles.
	// Done after SetTexture so the file is loaded and dimensions known, preventing
	// the file being loaded twice.
	if (this.just_inserted)
	{
		this.just_inserted = false;
		var sz = texture.GetImageSize();
		this.instance.SetSize(new cr.vector2(sz.x * 2, sz.y * 2));
		RefreshPropertyGrid();		// show new size
	}
	
	// Calculate tiling
	// This ignores cards without NPOT texture support but... meh.  Tiling by repeated quads is a massive headache.
	var texsize = texture.GetImageSize();
	var objsize = this.instance.GetSize();
	var uv = new cr.rect(0, 0, objsize.x / texsize.x, objsize.y / texsize.y);
	
	renderer.EnableTiling(true);
	renderer.Quad(this.instance.GetBoundingQuad(), this.instance.GetOpacity(), uv);
	renderer.EnableTiling(false);
}

IDEInstance.prototype.OnRendererReleased = function(renderer)
{
	renderer.ReleaseTexture(this.instance.GetTexture());
}