function GetPluginSettings()
{
	return {
		"name":			"Sprite",
		"id":			"Sprite",
		"version":		"1.0",
		"description":	"An animated object that is the building block of most projects.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/115/sprite",
		"category":		"General",
		"type":			"world",			// appears in layout
		"rotatable":	true,
		"flags":		pf_animations | pf_position_aces | pf_size_aces | pf_angle_aces | pf_appearance_aces | pf_zorder_aces | pf_effects
	};
};

////////////////////////////////////////
// Conditions
AddObjectParam("Object", "Select the object to test for a collision with.");
AddCondition(0, cf_fake_trigger | cf_static, "On collision with another object", "Collisions", "On collision with {0}", "Triggered when the object collides with another object.", "OnCollision");

AddObjectParam("Object", "Select the object to test for overlap with.");
AddCondition(1, 0, "Is overlapping another object", "Collisions", "Is overlapping {0}", "Test if the object is overlapping another object.", "IsOverlapping");

AddAnimationParam("Animation", "Enter the name of the animation to check if playing.")
AddCondition(2, 0, "Is playing", "Animations", "Is animation {0} playing", "Test which of the object's animations is currently playing.", "IsAnimPlaying");

AddCmpParam("Comparison", "How to compare the current animation frame number (0-based).");
AddNumberParam("Number", "The animation frame number to compare to (0-based).");
AddCondition(3, 0, "Compare frame", "Animations", "Animation frame {0} {1}", "Test which animation frame is currently showing.", "CompareFrame");

AddAnimationParam("Animation", "Enter the name of the animation that has finished.")
AddCondition(4, cf_trigger, "On finished", "Animations", "On animation {0} finished", "Triggered when an animation has finished.", "OnAnimFinished");

AddCondition(5, cf_trigger, "On any finished", "Animations", "On any animation finished", "Triggered when any animation has finished.", "OnAnyAnimFinished");

AddCondition(6, cf_trigger, "On frame changed", "Animations", "On frame changed", "Triggered when the current animation frame changes.", "OnFrameChanged");

AddCondition(7, 0, "Is mirrored", "Appearance", "Is mirrored", "True if the object has been mirrored with the 'Set Mirrored' action.", "IsMirrored");
AddCondition(8, 0, "Is flipped", "Appearance", "Is flipped", "True if the object has been flipped with the 'Set Flipped' action.", "IsFlipped");

AddObjectParam("Object", "Select the object to test for overlap with.");
AddNumberParam("Offset X", "The amount to offset the X co-ordinate (in pixels) before checking for a collision.");
AddNumberParam("Offset Y", "The amount to offset the Y co-ordinate (in pixels) before checking for a collision.");
AddCondition(9, 0, "Is overlapping at offset", "Collisions", "Is overlapping {0} at offset (<i>{1}</i>, <i>{2}</i>)", "Test if the object is overlapping another object at an offset position.", "IsOverlappingOffset");

AddCondition(10, cf_trigger, "On image URL loaded", "Web", "On image URL loaded", "Triggered after 'Load image from URL' when the image has finished loading.", "OnURLLoaded");

AddCondition(11, 0, "Collisions enabled", "Collisions", "Collisions enabled", "True if the object's collisions are enabled and will fire collision events.", "IsCollisionEnabled");

AddCmpParam("Comparison", "How to compare the current animation speed.");
AddNumberParam("Number", "The animation speed to compare to.");
AddCondition(12, 0, "Compare speed", "Animations", "Animation speed {0} {1}", "Compare the current animation speed.", "CompareAnimSpeed");

////////////////////////////////////////
// Actions
AddObjectParam("Object", "Choose the object type of the new instance to create.");
AddLayerParam("Layer", "The layer name or number to create the instance on.");
AddAnyTypeParam("Image point", "Use 0 for the object's origin, or the name or number of an image point to spawn the object from.", "0");
AddAction(0, 0, "Spawn another object", "Misc", "Spawn {0} on layer <b>{1}</b> <i>(image point {2})</i>", "Create another object at this object.", "Spawn");

AddComboParamOption("Normal");
AddComboParamOption("Additive");
AddComboParamOption("XOR");
AddComboParamOption("Copy");
AddComboParamOption("Destination over");
AddComboParamOption("Source in");
AddComboParamOption("Destination in");
AddComboParamOption("Source out");
AddComboParamOption("Destination out");
AddComboParamOption("Source atop");
AddComboParamOption("Destination atop");
AddComboParam("Blend mode", "Choose the new blend mode for this object.");
AddAction(1, 0, "Set blend mode", "Appearance", "Set blend mode to <i>{0}</i>", "Set the background blend mode for this object.", "SetEffect");

AddAction(2, 0, "Stop",		"Animations",	"Stop animation",	"Stop the current animation from playing.", "StopAnim");

AddComboParamOption("current frame");
AddComboParamOption("beginning");
AddComboParam("From", "Choose whether to resume or rewind the animation back to the first frame.");
AddAction(3, 0, "Start",	"Animations",	"Start animation from {0}",	"Start the current animation, if it was stopped.", "StartAnim");

AddAnimationParam("Animation", "The name of the animation to set.");
AddComboParamOption("current frame");
AddComboParamOption("beginning");
AddComboParam("From", "Choose whether to play from the same frame number or rewind the animation back to the first frame.", 1);
AddAction(4, 0, "Set animation", "Animations", "Set animation to <b>{0}</b> (play from {1})", "Set the current animation", "SetAnim");

AddNumberParam("Frame number", "The animation frame number to set (0-based).");
AddAction(5, 0, "Set frame", "Animations", "Set animation frame to <b>{0}</b>", "Set the current animation frame number.", "SetAnimFrame");

AddNumberParam("Speed", "The new animation speed, in animation frames per second.");
AddAction(6, 0, "Set speed", "Animations", "Set animation speed to <b>{0}</b>", "Set the current animation speed.", "SetAnimSpeed");

AddComboParamOption("Mirrored");
AddComboParamOption("Not mirrored");
AddComboParam("State", "Choose whether to horizontally mirror the object or set it back to normal.");
AddAction(7, 0, "Set mirrored", "Appearance", "Set <b>{0}</b>", "Set the object horizontally mirrored or back to normal.", "SetMirrored");

AddComboParamOption("Flipped");
AddComboParamOption("Not flipped");
AddComboParam("State", "Choose whether to vertically flip the object or set it back to normal.");
AddAction(8, 0, "Set flipped", "Appearance", "Set <b>{0}</b>", "Set the object vertically flipped or back to normal.", "SetFlipped");

AddNumberParam("Scale", "The object width and height to set, based on a multiple of its original dimensions, e.g. 1 = original size, 2 = double size, 0.5 = half size etc.", "1");
AddAction(9, 0, "Set scale", "Size & Position", "Set scale to <i>{0}</i>", "Set the width and height as a multiple of its original size.", "SetScale");

AddStringParam("URI", "Enter the URL on the web, or data URI, of an image to load.", "\"http://\"");
AddComboParamOption("Resize to image size");
AddComboParamOption("Keep current size");
AddComboParam("Size", "Whether to resize the sprite to the size of the loaded image, or stretch it to the current size.");
AddAction(10, 0, "Load image from URL", "Web", "Load image from <i>{0}</i> ({1})", "Replace the currently displaying animation frame with an image loaded from a web address or data URI.", "LoadURL");

AddComboParamOption("Disabled");
AddComboParamOption("Enabled");
AddComboParam("Collisions", "Whether to enable or disable collisions for this object.");
AddAction(11, 0, "Set collisions enabled", "Misc", "Set collisions <b>{0}</b>", "Set whether the object will register collision events or not.", "SetCollisions");

////////////////////////////////////////
// Expressions
AddExpression(0, ef_return_number, "Get animation frame", "Animations", "AnimationFrame", "The current animation frame number (0-based).");

AddExpression(1, ef_return_number, "Get animation frame count", "Animations", "AnimationFrameCount", "The number of animation frames in the current animation.");

AddExpression(2, ef_return_string, "Get animation name", "Animations", "AnimationName", "The name of the current animation.");

AddExpression(3, ef_return_number, "Get animation speed", "Animations", "AnimationSpeed", "The speed of the current animation, in animation frames per second.");

AddAnyTypeParam("ImagePoint", "Name or number of image point to get.");
AddExpression(4, ef_return_number, "Get image point X",		"Size & Position",	"ImagePointX", "The X position of one of the object's image points.");

AddAnyTypeParam("ImagePoint", "Name or number of image point to get.");
AddExpression(5, ef_return_number, "Get image point Y",		"Size & Position",	"ImagePointY", "The Y position of one of the object's image points.");

AddExpression(6, ef_return_number, "Get image width", "Animations", "ImageWidth", "The width of the current animation frame image, in pixels.");
AddExpression(7, ef_return_number, "Get image height", "Animations", "ImageHeight", "The height of the current animation frame image, in pixels.");

AddExpression(8, ef_return_number, "", "Size & Position", "ImagePointCount", "The number of image points the current frame has.");

ACESDone();

// Property grid properties for this plugin
var property_list = [
	new cr.Property(ept_link,	"Animations",			lang("project\\misc\\sprite-edit-link"), "Click to edit the object's animations.", "firstonly"),
	new cr.Property(ept_link,	"Size",					lang("project\\misc\\sprite-make11-link"), "Click to set the object to the same size as its image.", "worldundo"),
	new cr.Property(ept_combo,	"Initial visibility",	"Visible",	"Choose whether the object is visible when the layout starts.", "Visible|Invisible"),
	new cr.Property(ept_text,	"Initial animation",	"Default",	"The initial animation showing."),
	new cr.Property(ept_integer,"Initial frame",		0,			"The initial animation frame showing."),
	new cr.Property(ept_combo,	"Collisions",			"Enabled",	"Whether the object will register collision events or not.", "Disabled|Enabled")
	//new cr.Property(ept_combo,	"Effect",				"(none)",	"Choose an effect for this object.  (This does not preview in the layout, only when you run.)", "(none)|Additive|XOR|Copy|Destination over|Source in|Destination in|Source out|Destination out|Source atop|Destination atop")
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
	this.texture_loaded = false;
	this.last_imgsize = new cr.vector2(0, 0);
	this.last_texture = null;
	this.last_texture_id = "";
}

IDEInstance.prototype.OnAfterLoad = function ()
{
	// Must initialise last_imgsize for correct updating of sprites on layouts without a tab open
	var texture = this.instance.GetTexture(this.properties["Initial frame"], this.properties["Initial animation"]);
	this.last_imgsize = texture.GetImageSize();
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
	// Edit animations link
	if (property_name == "Animations")
	{
		this.instance.EditTexture();
	}
	// Make 1:1 link
	else if (property_name == "Size")
	{
		if (this.texture_loaded)
			this.just_inserted = true;		// will scale to texture size when redrawn and update property grid
		else
		{
			// The object could be resized, but we can't refresh the property grid here.
			// Just assume the layout is always open, and prompt if not.
			alert("The object cannot be scaled to original size unless the layout containing it is open.");
		}
	}
}

IDEInstance.prototype.OnRendererInit = function(renderer)
{
	this.last_texture = this.instance.GetTexture(this.properties["Initial frame"], this.properties["Initial animation"]);
	this.last_texture_id = this.last_texture.GetID();
	
	renderer.LoadTexture(this.last_texture);
	this.texture_loaded = true;
	
	this.instance.SetHotspot(this.last_texture.GetHotspot());
}
	
// Called to draw self in the editor
IDEInstance.prototype.Draw = function(renderer)
{
	var texture = this.instance.GetTexture(this.properties["Initial frame"], this.properties["Initial animation"]);
	var texture_id = texture.GetID();
	
	if (this.last_texture_id !== "" && this.last_texture_id !== texture_id)
	{
		// Texture has changed: unload old and reload new.
		if (this.last_texture)
			renderer.ReleaseTexture(this.last_texture);
			
		renderer.LoadTexture(texture);
		this.instance.SetHotspot(texture.GetHotspot());
	}
	
	this.last_texture = texture;
	this.last_texture_id = texture_id;
	
	renderer.SetTexture(texture);
	
	var imgsize = texture.GetImageSize();
	
	// First draw after insert: use size of texture.
	// Done after SetTexture so the file is loaded and dimensions known, preventing
	// the file being loaded twice.
	if (this.just_inserted)
	{
		this.just_inserted = false;
		this.instance.SetSize(imgsize);
		
		RefreshPropertyGrid();		// show new size
	}
	// If not just inserted and the sprite texture has been edited and changed size, scale the texture accordingly.
	else if ((imgsize.x !== this.last_imgsize.x || imgsize.y !== this.last_imgsize.y)
		&& (this.last_imgsize.x !== 0 && this.last_imgsize.y !== 0))
	{
		var sz = new cr.vector2(imgsize.x / this.last_imgsize.x, imgsize.y / this.last_imgsize.y);
		var instsize = this.instance.GetSize();
		
		sz.mul(instsize.x, instsize.y);
		this.instance.SetSize(sz);
		this.instance.SetHotspot(texture.GetHotspot());
		
		RefreshPropertyGrid();		// show new size
	}

	this.last_imgsize = imgsize;
	
	if (renderer.SupportsFullSmoothEdges())
	{
		// Get the object size and texture size
		var objsize = this.instance.GetSize();
		var texsize = texture.GetImageSize();
		
		// Calculate pixels per texel, then get a quad padded with a texel padding
		var pxtex = new cr.vector2(objsize.x / texsize.x, objsize.y / texsize.y);
		var q = this.instance.GetBoundingQuad(new cr.vector2(pxtex.x, pxtex.y));
		
		// Calculate the size of a texel in texture coordinates, then calculate texture coordinates
		// for the texel padded quad
		var tex = new cr.vector2(1.0 / texsize.x, 1.0 / texsize.y);
		var uv = new cr.rect(-tex.x, -tex.y, 1.0 + tex.x, 1.0 + tex.y);
		
		// Render a quad with a half-texel padding for smooth edges
		renderer.Quad(q, this.instance.GetOpacity(), uv);
	}
	else
	{
		// Fall back to half-smoothed or jagged edges, depending on what the renderer supports
		renderer.Quad(this.instance.GetBoundingQuad(), this.instance.GetOpacity());
	}
}

IDEInstance.prototype.OnRendererReleased = function(renderer)
{
	this.texture_loaded = false;
	renderer.ReleaseTexture(this.last_texture);
}

IDEInstance.prototype.OnTextureEdited = function ()
{
	var texture = this.instance.GetTexture(this.properties["Initial frame"], this.properties["Initial animation"]);
	this.instance.SetHotspot(texture.GetHotspot());
	
	var imgsize = texture.GetImageSize();
	
	// If sprite texture has been edited and changed size, scale the texture accordingly.
	if ((imgsize.x !== this.last_imgsize.x || imgsize.y !== this.last_imgsize.y)
		&& (this.last_imgsize.x !== 0 && this.last_imgsize.y !== 0))
	{
		var sz = new cr.vector2(imgsize.x / this.last_imgsize.x, imgsize.y / this.last_imgsize.y);
		var instsize = this.instance.GetSize();
		
		sz.mul(instsize.x, instsize.y);
		this.instance.SetSize(sz);
		
		this.last_imgsize = imgsize;
	}
}