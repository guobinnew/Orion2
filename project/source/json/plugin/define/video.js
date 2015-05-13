function GetPluginSettings()
{
	return {
		"name":			"Video",				// as appears in 'insert object' dialog, can be changed as long as "id" stays the same
		"id":			"video",				// this is used to identify this plugin and is saved to the project; never change it
		"version":		"1.0",					// (float in x.y format) Plugin version - C2 shows compatibility warnings based on this
		"description":	"Play back a video in the game.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/181/video",
		"category":		"Media",				// Prefer to re-use existing categories, but you can set anything here
		"type":			"world",				// either "world" (appears in layout and is drawn), else "object"
		"rotatable":	false,					// only used when "type" is "world".  Enables an angle property on the object.
		"flags":		0						// uncomment lines to enable flags...
					//	| pf_singleglobal		// exists project-wide, e.g. mouse, keyboard.  "type" must be "object".
					//	| pf_texture			// object has a single texture (e.g. tiled background)
						| pf_position_aces		// compare/set/get x, y...
						| pf_size_aces			// compare/set/get width, height...
					//	| pf_angle_aces			// compare/set/get angle (recommended that "rotatable" be set to true)
						| pf_appearance_aces	// compare/set/get visible, opacity...
					//	| pf_tiling				// adjusts image editor features to better suit tiled images (e.g. tiled background)
					//	| pf_animations			// enables the animations system.  See 'Sprite' for usage
						| pf_zorder_aces		// move to top, bottom, layer...
					//  | pf_nosize				// prevent resizing in the editor
						| pf_effects			// allow WebGL shader effects to be added
					//  | pf_predraw			// set for any plugin which draws and is not a sprite (i.e. does not simply draw
												// a single non-tiling image the size of the object) - required for effects to work properly
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
							
AddCondition(0, cf_none, "Is playing", "Video", "Is playing", "True if video is currently playing.", "IsPlaying");

AddCondition(1, cf_none, "Is paused", "Video", "Is paused", "True if video is currently paused.", "IsPaused");

AddCondition(2, cf_none, "Has ended", "Video", "Has ended", "True if video has finished playing.", "HasEnded");

AddCondition(3, cf_none, "Is muted", "Video", "Is muted", "True if video sound is muted.", "IsMuted");

AddComboParamOption("Can play");			// 0
AddComboParamOption("Can play through");	// 1
AddComboParamOption("Ended");				// 2
AddComboParamOption("Error");				// 3
AddComboParamOption("Started loading");		// 4
AddComboParamOption("Played");				// 5
AddComboParamOption("Paused");				// 6
AddComboParamOption("Stalled");				// 7
AddComboParam("Event", "The playback event to check for.");
AddCondition(4, cf_trigger, "On playback event", "Video", "On {0}", "Triggered when a playback event occurs.", "OnPlaybackEvent");

////////////////////////////////////////
// Actions

// AddAction(id,				// any positive integer to uniquely identify this action
//			 flags,				// (see docs) af_none, af_deprecated
//			 list_name,			// appears in event wizard list
//			 category,			// category in event wizard list
//			 display_str,		// as appears in event sheet - use {0}, {1} for parameters and also <b></b>, <i></i>
//			 description,		// appears in event wizard dialog when selected
//			 script_name);		// corresponding runtime function name

AddStringParam("WebM source", "Name of the .webm video file using VP8 or VP9 codec.");
AddStringParam("Ogg Theora source", "Name of the .ogv video file using Theora/Vorbis codec.");
AddStringParam("H.264 source", "Name of the .mp4 video file using h.264 codec.");
AddAction(0, af_none, "Set source", "Video", "Set video source to <i>{0}</i> (WebM), <i>{1}</i> (Ogg Theora), <i>{2}</i> (H.264)", "Switch to a different video. This does not automatically start playing it.", "SetSource");

AddNumberParam("Time", "Playback time in seconds to seek to.");
AddAction(1, af_none, "Set playback time", "Video", "Set playback time to <b>{0}</b> seconds", "Set the current playback time in seconds.", "SetPlaybackTime");

AddComboParamOption("not looping");
AddComboParamOption("looping");
AddComboParam("Mode", "Whether or not the video should loop when it reaches the end.");
AddAction(2, af_none, "Set looping", "Video", "Set {0}", "Set whether the video loops when it reaches the end.", "SetLooping");

AddComboParamOption("not muted");
AddComboParamOption("muted");
AddComboParam("Mode", "Whether or not the audio should be muted.");
AddAction(3, af_none, "Set muted", "Video", "Set {0}", "Set whether the audio is muted.", "SetMuted");

AddNumberParam("Volume", "The volume of the audio to set, in dB attenuation.", "-6");
AddAction(4, af_none, "Set volume", "Video", "Set volume to <b>{0}</b> dB", "Set the volume of the accompanying audio.", "SetVolume");

AddAction(5, af_none, "Pause", "Video", "Pause", "Pause the current playback.", "Pause");

AddAction(6, af_none, "Play", "Video", "Play", "Start playing the video if stopped or paused. On mobile, may only work in a user input trigger.", "Play");

////////////////////////////////////////
// Expressions

// AddExpression(id,			// any positive integer to uniquely identify this expression
//				 flags,			// (see docs) ef_none, ef_deprecated, ef_return_number, ef_return_string,
//								// ef_return_any, ef_variadic_parameters (one return flag must be specified)
//				 list_name,		// currently ignored, but set as if appeared in event wizard
//				 category,		// category in expressions panel
//				 exp_name,		// the expression name after the dot, e.g. "foo" for "myobject.foo" - also the runtime function name
//				 description);	// description in expressions panel

AddExpression(0, ef_return_number, "", "Video", "PlaybackTime", "Current playback time in seconds.");

AddExpression(1, ef_return_number, "", "Video", "Duration", "Video duration in seconds, if known.");

AddExpression(2, ef_return_number, "", "Video", "Volume", "Current video volume in dB attenuation.");

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
	new cr.Property(ept_text, 	"WebM source",		"",		"Name of the .webm video file using VP8 or VP9 codec."),
	new cr.Property(ept_text, 	"Ogg Theora source", "",	"Name of the .ogv video file using Theora/Vorbis codec."),
	new cr.Property(ept_text, 	"H.264 source",		"",		"Name of the .mp4 video file using h.264 codec."),
	new cr.Property(ept_combo,	"Autoplay",			"Yes",	"Whether to do nothing, start downloading, or start playing upon creation.", "No|Preload|Yes"),
	new cr.Property(ept_combo,	"Play in background",	"No",	"Keep playing video even when the tab or app goes in to the background.", "No|Yes"),
	new cr.Property(ept_combo,	"Initial visibility",	"Visible",	"Choose whether the video is visible on startup.", "Invisible|Visible")
	];
	
// Called by IDE when a new object type is to be created
function CreateIDEObjectType()
{
	return new IDEObjectType();
};

// Class representing an object type in the IDE
function IDEObjectType()
{
	assert2(this instanceof arguments.callee, "Constructor called as a function");
};

// Called by IDE when a new object instance of this type is to be created
IDEObjectType.prototype.CreateInstance = function(instance)
{
	return new IDEInstance(instance);
};

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
};

IDEInstance.prototype.OnCreate = function()
{
	this.instance.SetHotspot(new cr.vector2(0, 0));
}

// Called when inserted via Insert Object Dialog for the first time
IDEInstance.prototype.OnInserted = function()
{
	this.instance.SetSize(new cr.vector2(320, 240));
}

// Called when double clicked in layout
IDEInstance.prototype.OnDoubleClicked = function()
{
}

// Called after a property has been changed in the properties bar
IDEInstance.prototype.OnPropertyChanged = function(property_name)
{
}

// For rendered objects to load fonts or textures
IDEInstance.prototype.OnRendererInit = function(renderer)
{
}

// Called to draw self in the editor if a layout object
IDEInstance.prototype.Draw = function(renderer)
{
	var q = this.instance.GetBoundingQuad();
	var tl = new cr.vector2(q.tlx, q.tly);
	var tr = new cr.vector2(q.trx, q.try_);
	var br = new cr.vector2(q.brx, q.bry);
	var bl = new cr.vector2(q.blx, q.bly);
	var c = cr.RGB(0, 0, 255)
	renderer.Line(tl, tr, c);
	renderer.Line(tr, br, c);
	renderer.Line(br, bl, c);
	renderer.Line(bl, tl, c);
	renderer.Line(tl, br, c);
	renderer.Line(bl, tr, c);
}

// For rendered objects to release fonts or textures
IDEInstance.prototype.OnRendererReleased = function(renderer)
{
}