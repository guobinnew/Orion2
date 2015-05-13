function GetPluginSettings()
{
	return {
		"name":			"Particles",			// as appears in 'insert object' dialog, can be changed as long as "id" stays the same
		"id":			"Particles",			// this is used to identify this plugin and is saved to the project; never change it
		"version":		"1.0",					// (float in x.y format) Plugin version - C2 shows compatibility warnings based on this
		"description":	"Create a spray of particles.  Useful for visual effects.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/135/particles",
		"category":		"General",				// Prefer to re-use existing categories, but you can set anything here
		"type":			"world",				// either "world" (appears in layout and is drawn), else "object"
		"rotatable":	true,					// only used when "type" is "world".  Enables an angle property on the object.
		"flags":		0						// uncomment lines to enable flags...
					//	| pf_singleglobal		// exists project-wide, e.g. mouse, keyboard.  "type" must be "object".
						| pf_texture			// object has a single texture (e.g. tiled background)
						| pf_position_aces		// compare/set/get x, y...
					//	| pf_size_aces			// compare/set/get width, height...
						| pf_angle_aces			// compare/set/get angle (recommended that "rotatable" be set to true)
						| pf_appearance_aces	// compare/set/get visible, opacity...
					//	| pf_tiling				// adjusts image editor features to better suit tiled images (e.g. tiled background)
					//	| pf_animations			// enables the animations system.  See 'Sprite' for usage
						| pf_zorder_aces		// move to top, bottom, layer...
						| pf_effects
						| pf_predraw
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
				
AddCondition(0, cf_none, "Is spraying", "Particle spray", "Is spraying", "True if the continuous spray is currently enabled.", "IsSpraying");

////////////////////////////////////////
// Actions

// AddAction(id,				// any positive integer to uniquely identify this action
//			 flags,				// (see docs) af_none, af_deprecated
//			 list_name,			// appears in event wizard list
//			 category,			// category in event wizard list
//			 display_str,		// as appears in event sheet - use {0}, {1} for parameters and also <b></b>, <i></i>
//			 description,		// appears in event wizard dialog when selected
//			 script_name);		// corresponding runtime function name

AddComboParamOption("not spraying");
AddComboParamOption("spraying");
AddComboParam("State", "Whether or not the spray is currently enabled.");
AddAction(0, af_none, "Set spraying", "Particle spray", "Set {0}", "Enable or disable the continuous spray.", "SetSpraying");

AddNumberParam("Rate", "Number of particles to create per second.");
AddAction(1, af_none, "Set rate", "Particle spray", "Set rate to {0}", "Set the number of particles created per second.", "SetRate");

AddNumberParam("Spray cone", "Number of degrees through which particles are created.");
AddAction(2, af_none, "Set spray cone", "Particle spray", "Set spray cone to {0} degrees", "Set the number of degrees through which particles are created.", "SetSprayCone");

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
AddAction(3, 0, "Set blend mode", "Appearance", "Set blend mode to <i>{0}</i>", "Set the background blend mode for this object.", "SetEffect");

AddNumberParam("Speed", "Initial particle speed, in pixels per second.");
AddAction(4, af_none, "Set speed", "Initial particle properties", "Set initial particle speed to {0}", "Set the initial particle speed.", "SetInitSpeed");

AddNumberParam("Size", "Initial size of each particle, in pixels.");
AddAction(5, af_none, "Set size", "Initial particle properties", "Set initial particle size to {0}", "Set the initial size of each particle.", "SetInitSize");

AddNumberParam("Opacity", "Initial opacity of each particle, from 0 (transparent) to 100 (opaque).");
AddAction(6, af_none, "Set opacity", "Initial particle properties", "Set initial particle opacity to {0}", "Set the initial particle opacity.", "SetInitOpacity");

AddNumberParam("Grow rate", "Rate the size changes over time, in pixels per second.");
AddAction(7, af_none, "Set grow rate", "Initial particle properties", "Set grow rate to {0}", "Set the initial particle grow rate.", "SetGrowRate");

AddNumberParam("X randomiser", "Random X co-ordinate offset on creation.");
AddAction(8, af_none, "Set X randomiser", "Initial particle properties", "Set initial particle X randomiser to {0}", "Set the initial particle X randomiser.", "SetXRandomiser");

AddNumberParam("Y randomiser", "Random Y co-ordinate offset on creation.");
AddAction(9, af_none, "Set Y randomiser", "Initial particle properties", "Set initial particle Y randomiser to {0}", "Set the initial particle Y randomiser.", "SetYRandomiser");

AddNumberParam("Speed randomiser", "Random addition to particle speed on creation.");
AddAction(10, af_none, "Set speed randomiser", "Initial particle properties", "Set initial particle speed randomiser to {0}", "Set the random addition to particle speed on creation.", "SetSpeedRandomiser");

AddNumberParam("Size randomiser", "Random addition to particle size on creation.");
AddAction(11, af_none, "Set size randomiser", "Initial particle properties", "Set initial particle size randomiser to {0}", "Set the random addition to particle size on creation.", "SetSizeRandomiser");

AddNumberParam("Grow rate randomiser", "Random addition to particle grow rate on creation.");
AddAction(12, af_none, "Set grow rate randomiser", "Initial particle properties", "Set initial particle grow rate randomiser to {0}", "Set the random addition to particle grow rate on creation.", "SetGrowRateRandomiser");

AddNumberParam("Acceleration", "Particle acceleration, in pixels per second per second.");
AddAction(13, af_none, "Set acceleration", "Particle lifetime properties", "Set particle acceleration to {0}", "Set the particle acceleration.", "SetParticleAcc");

AddNumberParam("Gravity", "Force of gravity, in pixels per second per second.");
AddAction(14, af_none, "Set gravity", "Particle lifetime properties", "Set particle gravity to {0}", "Set the force of gravity.", "SetGravity");

AddNumberParam("Angle randomiser", "Maximum random angle deflection in degrees per second.");
AddAction(15, af_none, "Set angle randomiser", "Particle lifetime properties", "Set particle angle randomiser to {0}", "Set the maximum random angle deflection per second.", "SetAngleRandomiser");

AddNumberParam("Speed randomiser", "Maximum random speed adjustment per second, in pixels per second per second.");
AddAction(16, af_none, "Set speed randomiser", "Particle lifetime properties", "Set particle speed randomiser to {0}", "Set the maximum random speed adjustment per second.", "SetLifeSpeedRandomiser");

AddNumberParam("Opacity randomiser", "Maximum random opacity adjustment per second.");
AddAction(17, af_none, "Set opacity randomiser", "Particle lifetime properties", "Set particle opacity randomiser to {0}", "Set the maximum random opacity adjustment per second.", "SetOpacityRandomiser");

AddNumberParam("Timeout", "Time in seconds for timeout or fade to invisible (depending on destroy mode).");
AddAction(18, af_none, "Set timeout", "Particle lifetime properties", "Set particle timeout to {0}", "Set the destroy timeout or fade to invisible time.", "SetTimeout");

////////////////////////////////////////
// Expressions

// AddExpression(id,			// any positive integer to uniquely identify this expression
//				 flags,			// (see docs) ef_none, ef_deprecated, ef_return_number, ef_return_string,
//								// ef_return_any, ef_variadic_parameters (one return flag must be specified)
//				 list_name,		// currently ignored, but set as if appeared in event wizard
//				 category,		// category in expressions panel
//				 exp_name,		// the expression name after the dot, e.g. "foo" for "myobject.foo" - also the runtime function name
//				 description);	// description in expressions panel

AddExpression(0, ef_return_number, "Get particle count", "Particle spray", "ParticleCount", "Return the number of particles.");
AddExpression(1, ef_return_number, "Get rate", "Particle spray", "Rate", "The number of particles created per second.");
AddExpression(2, ef_return_number, "Get spray cone", "Particle spray", "SprayCone", "The number of degrees through which particles are created.");

AddExpression(3, ef_return_number, "Get speed", "Initial particle properties", "InitSpeed",		"The initial particle speed, in pixels per second.");
AddExpression(4, ef_return_number, "Get size", "Initial particle properties", "InitSize",		"The initial particle size, in pixels.");
AddExpression(5, ef_return_number, "Get opacity", "Initial particle properties", "InitOpacity",		"The initial particle opacity, from 0 (transparent) to 100 (opaque).");
AddExpression(6, ef_return_number, "Get grow rate", "Initial particle properties", "InitGrowRate",		"The initial particle grow rate, in pixels per second.");
AddExpression(7, ef_return_number, "Get X randomiser", "Initial particle properties", "XRandom",		"The random X offset on creation.");
AddExpression(8, ef_return_number, "Get Y randomiser", "Initial particle properties", "YRandom",		"The random Y offset on creation.");
AddExpression(9, ef_return_number, "Get speed randomiser", "Initial particle properties", "InitSpeedRandom",		"The random addition to particle speed on creation.");
AddExpression(10, ef_return_number, "Get size randomiser", "Initial particle properties", "InitSizeRandom",		"The random addition to particle size on creation.");
AddExpression(11, ef_return_number, "Get grow rate randomiser", "Initial particle properties", "InitGrowRandom",		"The random addition to particle grow rate on creation.");

AddExpression(12, ef_return_number, "Get acceleration", "Particle lifetime properties", "ParticleAcceleration", "The particle acceleration in pixels per second per second.");
AddExpression(13, ef_return_number, "Get gravity", "Particle lifetime properties", "Gravity", "The force of gravity in pixels per second per second.");
AddExpression(14, ef_return_number, "Get angle randomiser", "Particle lifetime properties", "ParticleAngleRandom", "The maximum random angle deflection in degrees per second.");
AddExpression(15, ef_return_number, "Get speed randomiser", "Particle lifetime properties", "ParticleSpeedRandom", "The maximum random change in speed in pixels per second per second.");
AddExpression(16, ef_return_number, "Get opacity randomiser", "Particle lifetime properties", "ParticleOpacityRandom", "The maximum change in opacity per second.");
AddExpression(17, ef_return_number, "Get timeout", "Particle lifetime properties", "Timeout", "The destroy or fade to invisible timeout.");

////////////////////////////////////////
ACESDone();

////////////////////////////////////////
// Array of property grid properties for this plugin
// new cr.Property(ept_integer,		name,	initial_value,	description)		// an integer value
// new cr.Property(ept_float,		name,	initial_value,	description)		// a float value
// new cr.Property(ept_text,		name,	initial_value,	description)		// a string
// new cr.Property(ept_color,		name,	initial_value,	description)		// a color dropdown
// new cr.Property(ept_font,		name,	"Arial,-16", 	description)		// a font with the given face name and size
// new cr.Property(ept_combo,		name,	"Item 1",		description, "Item 1|Item 2|Item 3")	// a dropdown list (initial_value is string of initially selected item)
// new cr.Property(ept_link,		name,	link_text,		description, "firstonly")		// has no associated value; simply calls "OnPropertyChanged" on click

var property_list = [
	// Properties
	new cr.Property(ept_float, 	"Rate",			50,					"Number of particles to create per second, or total for one-shot."),
	new cr.Property(ept_float,	"Spray cone",	60,					"Number of degrees through which particles are created."),
	new cr.Property(ept_combo,	"Type",			"Continuous spray",	"Either a spray or a one-shot blast of particles.", "Continuous spray|One-shot"),
	new cr.Property(ept_link,	"Image",		lang("project\\misc\\particles-edit-link"), "Click to edit the particle image.", "firstonly"),
	//new cr.Property(ept_combo,	"Effect",		"(none)",			"Choose an effect for this object.  (This does not preview in the layout, only when you run.)", "(none)|Additive|XOR|Copy|Destination over|Source in|Destination in|Source out|Destination out|Source atop|Destination atop"),
	
	// Creation settings
	new cr.Property(ept_section, "Initial particle properties", "",	"Properties affecting the creation of each particle."),
	new cr.Property(ept_float,	"Speed",		200,				"Initial particle speed, in pixels per second."),
	new cr.Property(ept_float,	"Size",			32,					"Initial size of each particle, in pixels."),
	new cr.Property(ept_float,	"Opacity",		100,				"Initial opacity of each particle, from 0 (transparent) to 100 (opaque)."),
	new cr.Property(ept_float,	"Grow rate",	0,					"Rate the size changes over time, in pixels per second."),
	new cr.Property(ept_float,	"X randomiser",	0,					"Random X co-ordinate offset on creation."),
	new cr.Property(ept_float,	"Y randomiser",	0,					"Random Y co-ordinate offset on creation."),
	new cr.Property(ept_float,	"Speed randomiser ", 0,				"Random addition to particle speed on creation."),
	new cr.Property(ept_float,	"Size randomiser", 0,				"Random addition to particle size on creation."),
	new cr.Property(ept_float,	"Grow rate randomiser", 0,			"Random addition to particle grow rate on creation."),
	
	// Particle lifetime settings
	new cr.Property(ept_section, "Particle lifetime properties", "", "Properties affecting the behavior of each particle."),
	new cr.Property(ept_float,	"Acceleration",	-150,				"Acceleration of each particle, in pixels per second per second."),
	new cr.Property(ept_float,	"Gravity",		0,					"Force of gravity, in pixels per second per second."),
	new cr.Property(ept_float,	"Angle randomiser", 0,				"Maximum random angle deflection in degrees per second."),
	new cr.Property(ept_float,	"Speed randomiser", 800,			"Maximum random speed adjustment per second, in pixels per second per second."),
	new cr.Property(ept_float,	"Opacity randomiser", 0,			"Maximum random opacity adjustment per second."),
	new cr.Property(ept_combo,	"Destroy mode",	"Fade to invisible", "Choose when each individual particle is destroyed.", "Fade to invisible|Timeout expired|Particle stopped"),
	new cr.Property(ept_float,	"Timeout",		1,					"Time in seconds for timeout or fade to invisible (depending on destroy mode).")
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
}

IDEInstance.prototype.OnCreate = function()
{
	// Always use middle-left hotspot
	this.instance.SetHotspot(new cr.vector2(0, 0.5));
}

IDEInstance.prototype.OnInserted = function()
{
	this.instance.SetSize(new cr.vector2(128, 128));
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
	var sz = texture.GetImageSize();
	
	// Draw particle image in middle
	var q = this.instance.GetBoundingQuad();
	var imgx = cr.quad.prototype.midX.apply(q) - sz.x / 2;
	var imgy = cr.quad.prototype.midY.apply(q) - sz.y / 2;
	var q2 = new cr.quad();
	q2.set_from_rect(new cr.rect(imgx, imgy, imgx + sz.x, imgy + sz.y));
	
	renderer.Quad(q2, this.instance.GetOpacity());
	
	// Draw lines indicating spray cone, 100px long
	var origin = new cr.vector2((q.tlx + q.blx) / 2, (q.tly + q.bly) / 2);
	
	var a = this.instance.GetAngle();
	var da = cr.to_radians(this.properties["Spray cone"]) / 2;
	
	var end1 = new cr.vector2(origin.x + Math.cos(a - da) * 100, origin.y + Math.sin(a - da) * 100);
	var end2 = new cr.vector2(origin.x + Math.cos(a + da) * 100, origin.y + Math.sin(a + da) * 100);
	renderer.Line(origin, end1, cr.RGB(255, 0, 0));
	renderer.Line(origin, end2, cr.RGB(255, 0, 0));
}

IDEInstance.prototype.OnRendererReleased = function(renderer)
{
	renderer.ReleaseTexture(this.instance.GetTexture());
}