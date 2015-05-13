function GetPluginSettings()
{
	return {
		"name":			"Scml",				// as appears in 'insert object' dialog, can be changed as long as "id" stays the same
		"id":			"Spriter",				// this is used to identify this plugin and is saved to the project; never change it
		"version":		"1.0",					// (float in x.y format) Plugin version - C2 shows compatibility warnings based on this
		"description":	"Load and playback SCML files from BrashMonkey's Spriter animation toolkit.",
		"author":		"BrashMonkey",
		"help url":		"<your website or a manual entry on Scirra.com>",
		"category":		"General",				// Prefer to re-use existing categories, but you can set anything here
		"type":			"world",				// either "world" (appears in layout and is drawn), else "object"
		"rotatable":	true,					// only used when "type" is "world".  Enables an angle property on the object.
		"flags":		0						// uncomment lines to enable flags...
					//	| pf_singleglobal		// exists project-wide, e.g. mouse, keyboard.  "type" must be "object".
					//	| pf_texture			// object has a single texture (e.g. tiled background)
						| pf_position_aces		// compare/set/get x, y...
					//	| pf_size_aces			// compare/set/get width, height...
						| pf_angle_aces			// compare/set/get angle (recommended that "rotatable" be set to true)
					//	| pf_appearance_aces	// compare/set/get visible, opacity...
					//	| pf_tiling				// adjusts image editor features to better suit tiled images (e.g. tiled background)
					//	| pf_animations			// enables the animations system.  See 'Sprite' for usage
						| pf_zorder_aces		// move to top, bottom, layer...
					//  | pf_nosize				// prevent resizing in the editor
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
				
// example	

		
AddStringParam("Animation", "Enter the name of the animation that has finished.");
AddCondition(0, cf_trigger, "On animation finished", "Animations", "On animation {0} finished", "Triggered when the animation has finished.", "OnAnimFinished");

AddCondition(1, cf_trigger, "On any finished", "Animations", "On any animation finished", "Triggered when any animation has finished.", "OnAnyAnimFinished");

AddCmpParam("Current Key Frame is ", "Is the current Key Frame <,>,=,etc to the value below");
AddNumberParam("Frame","The frame number to compare the current key frame to" ,"0")	;
AddCondition(2,0, "Compare Current Animation Frame", "Animations", "Current Key Frame is {0} {1}", "Compare the current key frame number.", "CompareCurrentKey");

AddCmpParam("Current Animation Time is ", "Is the current time <,>,=,etc to the value below");
AddNumberParam("Time","The time to compare the current key frame to" ,"0")	;
AddComboParamOption("milliseconds");
AddComboParamOption("ratio of the animation length");
AddComboParam("Time Format", "Is the 'Time' value above expressed in milliseconds or as a ratio",0);	
AddCondition(3,0, "Compare Current Time", "Animations", "Current Time is {0} {1} {2}", "Compare the current time.", "CompareCurrentTime");
		
AddStringParam("Animation", "Is this the current animation.");
AddCondition(4, 0, "Compare Current Animation", "Animations", "Is current animation {0}", "Compare the name of the current animation.", "CompareAnimation");

AddCondition(5, 0, "Is Paused", "Animations", "If animation is paused", "Is animation paused?", "AnimationPaused");

AddCondition(6, 0, "Is Looping", "Animations", "If animation is looping", "Is animation set to loop?", "AnimationLooping");

AddCondition(7, cf_trigger, "On sound triggered", "Sounds", "On sound triggered", "Triggered when a sound should begin playback.", "OnSoundTriggered");

AddCondition(8, cf_trigger, "On sound volume change triggered", "Sounds", "On sound volume change triggered", "Triggered when a sound's volume should change.", "OnSoundVolumeChangeTriggered");

AddCondition(9, cf_trigger, "On sound panning change triggered", "Sounds", "On sound panning change triggered", "Triggered when a sound's volume should change.", "OnSoundPanningChangeTriggered");
	
AddCondition(10, 0, "Is Mirrored (on x axis)", "Size & Position", "Mirrored on x axis", "True when the object is mirrored on the x axis", "isMirrored");

AddCondition(11, 0, "Is Flipped (on y axis)", "Size & Position", "Flipped on y axis", "True when the object is mirrored on the y axis", "isFlipped");
	
AddCondition(666, cf_trigger, "On initialised", "Initialisation", "On initialised", "Triggered when .SCML file has loaded and is ready to be associated with objects.", "readyForSetup");

AddStringParam("name", "The name of the point.");
AddCondition(12, 0, "Action Point exists on frame", "Action Points", "Action Point {0} exists on current frame", "True when the action point exists on the current frame", "actionPointExists");

AddCondition(13, 0, "Position is outside padded viewport", "Advanced : Optimization", "Position is outside padded viewport", "True when this object's x,y coordinate is outside the padded display box set through Set Automatic Pausing action.", "outsidePaddedViewport");

////////////////////////////////////////
// Actions

// AddAction(id,				// any positive integer to uniquely identify this action
//			 flags,				// (see docs) af_none, af_deprecated
//			 list_name,			// appears in event wizard list
//			 category,			// category in event wizard list
//			 display_str,		// as appears in event sheet - use {0}, {1} for parameters and also <b></b>, <i></i>
//			 description,		// appears in event wizard dialog when selected
//			 script_name);		// corresponding runtime function name

// example
AddObjectParam("Object", "Choose the object type you wish to associate.");
AddStringParam("Name", "The name Spriter uses for this object.");
AddAction(666, 0, "Associate object type", "Initialisation", "Associate {0} with name {1}", "Associate an object with its Spriter name.", "associateTypeWithName");

AddStringParam("Animation", "The name of the animation to set.");
AddComboParamOption("play from start");	
AddComboParamOption("play from current time");
AddComboParamOption("play from current time ratio");
AddComboParamOption("blend to start");
AddComboParamOption("blend at current time ratio");
AddComboParam("Where to start playing this animation", "Play this animation from the beginning? or play from current time or time ratio", 0);	
AddNumberParam("Blend Duration", "If a blend option is chosen above, the length of time (in milliseconds) to take to blend to the new animation", "0");
AddAction(0, 0, "Set animation", "Animations", "Set animation to {0} and {1} with a {2}ms blend", "Set the current animation", "setAnim");

AddNumberParam("Speed Ratio", "The new ratio of playback speed (1.0 is full speed. 0.5 is half speed, 2.0 is double speed)", "1.0");
AddAction(1, 0, "Set playback speed ratio", "Animations", "Set playback speed ratio to {0}", "Set the ratio of playback speed", "setPlaybackSpeedRatio");

AddNumberParam("Scale Ratio", "The new ratio of the object scale (1.0 is full size. 0.5 is half size, 2.0 is double size)", "1.0");
AddComboParamOption("don't flip x axis");	
AddComboParamOption("flip x axis");											
AddComboParam("Flip X Axis", "Flip on the X Axis and display with negative width", 0);	
AddComboParamOption("don't flip y axis");	
AddComboParamOption("flip y axis");											
AddComboParam("Flip Y Axis", "Flip on the Y Axis and display with negative height", 0);		
AddAction(2, 0, "Set object scale ratio", "Size & Position", "Set object scale to {0} - {1} - {2}", "Set the ratio of object scale", "setObjectScaleRatio");

AddComboParamOption("play once");	
AddComboParamOption("loop");											
AddComboParam("Loop animation?", "Loop animation, or play only once? (overrides animation settings from file)", 0);		
AddAction(3, 0, "Set animation looping", "Animations", "Set current animation to {0}", "Set whether or not an animation loops", "setAnimationLoop");

AddComboParamOption("milliseconds");	
AddComboParamOption("ratio of total length");											
AddComboParam("time units", "New position in current animation to be specified in milliseconds, or as a ration of the total length of the animation between 0.0 and 1.0?", 0);
AddNumberParam("New Position", "The new position in milliseconds or as a ratio from 0.0 to 1.0", "0")
AddAction(4, 0, "Set current time in animation", "Animations", "Set current time in animation to {1} {0}", "Set current time in animation", "setAnimationTime");

AddAction(5, 0, "Pause animation", "Animations", "Pause animation playback", "Pause animation playback", "pauseAnimation");

AddAction(6, 0, "Resume animation", "Animations", "Resume animation playback", "Resume animation playback", "resumeAnimation");

AddComboParamOption("to keyframe");	
AddComboParamOption("to time");
AddComboParamOption("to time ratio");												
AddComboParam("", "What type of value are specifying below to play the animation to", 0);	
AddNumberParam("Target", "The target time to play the current animation to (units specified above)", "0");
AddAction(7, 0, "Play current animation to...", "Animations", "Play current animation {0}: {1}", "Play animation from current time to...", "playAnimTo");

AddStringParam("Entity", "The name of the entity to set.");
AddStringParam("Animation", "The name of the animation to set. Leave blank to attempt to switch to animation of same name.");
AddAction(8, 0, "Set entity", "Entities", "Set entity to {0} : {1}", "Set the current entity", "setEnt");

AddAction(9, 0, "Remove all character maps", "Character Maps", "Remove all character maps", "Remove all character maps from the character", "removeAllCharMaps");

AddStringParam("Character Map","The character map to append to this character.");
AddAction(10, 0, "Append character map", "Character Maps", "Append character map {0}", "Append character map to this character", "appendCharMap");

AddComboParamOption("don't mirror x axis");	
AddComboParamOption("mirror x axis");											
AddComboParam("Mirror X Axis", "Flip on the X Axis and display with negative width", 0);	
AddAction(11, 0, "Set mirrored (on x axis)", "Size & Position", "{0}", "Set whether the object is mirrored on the x axis", "setObjectXFlip");

AddComboParamOption("don't flip y axis");	
AddComboParamOption("flip y axis");											
AddComboParam("Flip Y Axis", "Flip on the Y Axis and display with negative width", 0);	
AddAction(12, 0, "Set flipped (on y axis)", "Size & Position", "{0}", "Set whether the object is flipped on the y axis", "setObjectYFlip");

AddStringParam("Animation", "The name of the second animation to set.");												
AddAction(13, 0, "Set second (blended) animation", "Advanced : Animation Blending", "Set second(blended) animation to {0}", "Set the current second(blended) animation", "setSecondAnim");

AddNumberParam("Blend level","The new new ratio from 0.0 to 1.0 of the blend between the current and blended animations", "0");												
AddAction(14, 0, "Set animation blend ratio", "Advanced : Animation Blending", "Set animation blend ratio to {0}", "Set the blend ratio of the current animations", "setAnimBlendRatio");
											
AddAction(15, 0, "Stop blending second (blended) animation", "Advanced : Animation Blending", "Stop blending second(blended) animation", "Stops blending the second(blended) animation, and resumes normal playback of the current animation", "stopSecondAnim");

AddComboParamOption("Invisible");
AddComboParamOption("Visible");												
AddComboParam("Visibility", "Choose whether the object is hidden or shown", 1);	
AddAction(16, 0, "Set Visible", "Appearance", "Set {0}", "Choose whether the object is hidden or shown","setVisible");
	
AddComboParamOption("No automatic pausing.");												
AddComboParamOption("Pause all playback automatically when scml object position is outside padded display area");
AddComboParamOption("Pause all playback except sound automatically when scml object position is outside padded display area");
AddComboParam("Automatic Pause Settings", "Set the new automatic pausing settings", 0);
AddNumberParam("Left Padding","The object must be outside the visible area of the screen plus this padding to automatically pause", "0");												
AddNumberParam("Right Padding","The object must be outside the visible area of the screen plus this padding to automatically pause", "0");												
AddNumberParam("Top Padding","The object must be outside the visible area of the screen plus this padding to automatically pause", "0");												
AddNumberParam("Bottom Padding","The object must be outside the visible area of the screen plus this padding to automatically pause", "0");												
AddAction(17, 0, "Set Automatic Pausing", "Advanced : Optimization", "Set to {0}. Padding Left:{1} Right:{2} Top:{3} Bottom{4}", "Set when to automatically pause an animation for efficiency","setAutomaticPausing");
	
////////////////////////////////////////
// Expressions

// AddExpression(id,			// any positive integer to uniquely identify this expression
//				 flags,			// (see docs) ef_none, ef_deprecated, ef_return_number, ef_return_string,
//								// ef_return_any, ef_variadic_parameters (one return flag must be specified)
//				 list_name,		// currently ignored, but set as if appeared in event wizard
//				 category,		// category in expressions panel
//				 exp_name,		// the expression name after the dot, e.g. "foo" for "myobject.foo" - also the runtime function name
//				 description);	// description in expressions panel

// example
AddExpression(0, ef_return_number, "Current time in animation", "Animations", "time", "Returns the current time in the animation between 0 and animation length.");
AddExpression(1, ef_return_number, "Current keyframe in animation", "Animations", "key", "Returns the index of the current keyframe in the animation between 0 and the number of keys.");
AddExpression(2, ef_return_number, "Scale Ratio", "Size & Position", "ScaleRatio", "Returns the scale ratio of the object");
AddExpression(3, ef_return_number, "'Play To...' target time", "Animations", "PlayTo", "Returns the target time in the animation between 0 and animation length for the current 'Play animation to...' action, or -1 if there is no current 'Play animation to...' action");
AddExpression(4, ef_return_number, "'Play To...' distance left", "Animations", "PlayToTimeLeft", "Returns the remaining number of milliseconds(at normal playback speed) left to complete the current 'Play animation to...' action");
AddExpression(5, ef_return_string, "Current Animation Name", "Animations", "animationName", "Returns the name of the current animation");
AddExpression(6, ef_return_string, "Current Entity Name", "Entities", "entityName", "Returns the name of the current entity");
AddExpression(7, ef_return_number, "Current time in animation as a ratio", "Animations", "timeRatio", "Returns the current time as a ratio of the entire animation length between 0.0 and 1.0");

AddStringParam("name", "The name of the point.");
AddExpression(8, ef_return_number, "Action Point X", "Action Points", "pointX", "returns the current x position of an action point");

AddStringParam("name", "The name of the point.");
AddExpression(9, ef_return_number, "Action Point Y", "Action Points", "pointY", "returns the current y position of an action point");

AddStringParam("name", "The name of the point.");
AddExpression(10, ef_return_number, "Action Point Angle", "Action Points", "pointAngle", "returns the current angle of an action point");

AddExpression(11, ef_return_string, "Name of last triggered sound", "Sounds", "triggeredSound", "returns the name of the last triggered sound");
AddExpression(12, ef_return_string, "Name of last triggered sound's tag", "Sounds", "triggeredSoundTag", "returns the name of the last triggered sound's tag");

AddStringParam("name", "The name of the sound.");
AddExpression(13, ef_return_number, "Volume sound", "Sounds", "soundVolume", "returns the volume of the sound");

AddStringParam("name", "The name of the sound.");
AddExpression(14, ef_return_number, "Volume panning", "Sounds", "soundPanning", "returns the panning of the sound");

AddExpression(15, ef_return_number, "Blend Ratio", "Advanced : Animation Blending", "blendRatio", "returns the current blend ratio of animations 0(current animation) to 1(next animation)");
AddExpression(16, ef_return_string, "Second(blend) Animation", "Advanced : Animation Blending", "secondAnimationName", "returns the name of the second(blended) animation");

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
	//new cr.Property(ept_integer, 	"My property",		77,		"An example property."),
	 new cr.Property(ept_text,		"SCML file",	"",	"The source SCML file for your character"),
	 new cr.Property(ept_text,		"starting entity",	"",	"The character's name within the file"),
	 new cr.Property(ept_text,		"starting animation",	"",	"The animation to play on start")
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
	// this.myValue = 0...
}

// Called when inserted via Insert Object Dialog for the first time
IDEInstance.prototype.OnInserted = function()
{
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
}

// For rendered objects to release fonts or textures
IDEInstance.prototype.OnRendererReleased = function(renderer)
{
}