function GetPluginSettings()
{
	return {
		"name":			"User Media",			// as appears in 'insert object' dialog, can be changed as long as "id" stays the same
		"id":			"UserMedia",			// this is used to identify this plugin and is saved to the project; never change it
		"version":		"1.0",					// (float in x.y format) Plugin version - C2 shows compatibility warnings based on this
		"description":	"Get media from the user using a camera or microphone device, including speech recognition and synthesis.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/144/user-media",
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
						| pf_effects
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
				
AddCondition(0, cf_trigger, "On media request approved", "Media request", "On media request approved", "Triggered after the user approves the prompt from a media request.", "OnApproved");
AddCondition(1, cf_trigger, "On media request declined", "Media request", "On media request declined", "Triggered on an error, or if the user declines the prompt from a media request.", "OnDeclined");

AddCondition(2, cf_none, "Supports user media", "User media", "Supports user media", "True if the current browser supports accessing user media.", "SupportsUserMedia");

AddCondition(3, cf_none, "Supports speech recognition", "Speech recognition", "Supports speech recognition", "True if the current browser supports speech recognition.", "SupportsSpeechRecognition");

AddCondition(4, cf_trigger, "On start", "Speech recognition", "On speech recognition started", "Triggered when speech recognition begins.", "OnSpeechRecognitionStart");

AddCondition(5, cf_trigger, "On end", "Speech recognition", "On speech recognition ended", "Triggered when speech recognition ends.", "OnSpeechRecognitionEnd");

AddCondition(6, cf_trigger, "On error", "Speech recognition", "On speech recognition error", "Triggered if there is an error starting speech recongition, e.g. permission denied.", "OnSpeechRecognitionError");

AddCondition(7, cf_trigger, "On result", "Speech recognition", "On speech recognition result", "Triggered during speech recognition as results are detected.", "OnSpeechRecognitionResult");

AddCondition(8, cf_none, "Is recognising speech", "Speech recognition", "Is recognising speech", "True if speech recognition is currently active.", "IsRecognisingSpeech");

AddCondition(9, cf_trigger, "On reading update", "Ambient light", "On ambient light reading update", "Triggered when the device ambient light sensor (if present) reading changes.", "OnDeviceLight");

AddCondition(10, cf_none, "Supports speech synthesis", "Speech synthesis", "Supports speech synthesis", "True if the current browser supports speech synthesis.", "SupportsSpeechSynthesis");

AddCondition(11, cf_none, "Is speaking", "Speech synthesis", "Is speaking", "True if currently speaking some text.", "IsSpeaking");

AddCondition(12, cf_trigger, "On retrieved media sources", "Media request", "On retrieved media sources", "Triggered after 'Get media sources' when the media source list is available.", "OnMediaSources");

////////////////////////////////////////
// Actions

// AddAction(id,				// any positive integer to uniquely identify this action
//			 flags,				// (see docs) af_none, af_deprecated
//			 list_name,			// appears in event wizard list
//			 category,			// category in event wizard list
//			 display_str,		// as appears in event sheet - use {0}, {1} for parameters and also <b></b>, <i></i>
//			 description,		// appears in event wizard dialog when selected
//			 script_name);		// corresponding runtime function name

AddNumberParam("Source", "The zero-based index of the camera source to use.");
AddAction(0, af_none, "Request camera", "Media request", "Request camera source <i>{0}</i>", "Prompt the user for access to video input from a camera, if one is present.", "RequestCamera");

AddAction(1, af_none, "Stop", "Media request", "Stop", "Stop accessing the user media.", "Stop");

AddComboParamOption("PNG");
AddComboParamOption("JPEG");
AddComboParam("Format", "Choose the file format to save the snapshot.  PNG is lossless but JPEG files are smaller.");
AddNumberParam("JPEG quality", "If 'Format' is 'JPEG', the JPEG quality from 0 (worst) to 100 (best).  Ignored for PNG.", "75");
AddAction(2, af_none, "Snapshot", "Media request", "Take snapshot ({0}, quality <i>{1}</i>)", "Take a snapshot of the current image from the camera.  Use 'SnapshotURL' expression to get result.", "Snapshot");

AddStringParam("Tag", "A tag in the Audio object to route the microphone input to.", "\"mic\"");
AddNumberParam("Source", "The zero-based index of the audio source to use.");
AddAction(3, af_none, "Request microphone", "Media request", "Request microphone source <i>{1}</i> and route to Audio tag {0}", "Prompt the user for access to audio input from a microphone, if one is present. The Audio object must be added to the project.", "RequestMic");

AddStringParam("Language", "The language code to recognise, or empty for the browser's current language.", "\"en\"");
AddComboParamOption("Continuous");
AddComboParamOption("Single phrase");
AddComboParam("Mode", "Whether to keep detecting speech until stopped (continuous), or to automatically stop when speech stops (single phrase).");
AddComboParamOption("Interim");
AddComboParamOption("Final");
AddComboParam("Results", "Whether to allow uncertain (interim) results, or only final results.");
AddAction(4, af_none, "Request speech recognition", "Speech recognition", "Request speech recognition (language <i>{0}</i>, {1} mode, {2} results)", "Prompt the user for speech recognition input from a microphone, if one is present.", "RequestSpeechRecognition");

AddAction(5, af_none, "Stop speech recognition", "Speech recognition", "Stop speech recognition", "Stop any active speech recognition input.", "StepSpeechRecognition");

AddStringParam("Text", "The text to be spoken.");
AddStringParam("Language", "The language of the text, e.g. \"en-US\" for US English. Leave empty to use the default language.");
AddStringParam("Voice URI", "The voice to use. Leave empty to use the default voice.");
AddNumberParam("Volume", "The volume to speak the text at, in dB attenuation.");
AddNumberParam("Rate", "The relative speed at which to speak, e.g. 1 for normal, 0.5 for half as fast.", "1");
AddNumberParam("Pitch", "The relative pitch at which to speak, between 0 and 2.", "1");
AddAction(6, af_none, "Speak text", "Speech synthesis", "Speak text <b>{0}</b> (language <i>{1}</i>, voice URI <i>{2}</i>, volume <i>{3}</i> dB, rate <i>{4}</i>, pitch <i>{5}</i>", "Read out some text with speech synthesis.", "SpeakText");

AddAction(7, af_none, "Stop speaking", "Speech synthesis", "Stop speaking", "Cancel any active speech synthesis.", "StopSpeaking");

AddAction(8, af_none, "Pause speaking", "Speech synthesis", "Pause speaking", "Pause any active speech synthesis.", "PauseSpeaking");

AddAction(9, af_none, "Resume speaking", "Speech synthesis", "Resume speaking", "Resume a previously paused speech synthesis.", "ResumeSpeaking");

AddAction(10, af_none, "Get media sources", "Media request", "Get media sources", "Request a list of the available media sources. 'On retrieved media sources' will trigger shortly after.", "GetMediaSources");

////////////////////////////////////////
// Expressions

// AddExpression(id,			// any positive integer to uniquely identify this expression
//				 flags,			// (see docs) ef_none, ef_deprecated, ef_return_number, ef_return_string,
//								// ef_return_any, ef_variadic_parameters (one return flag must be specified)
//				 list_name,		// currently ignored, but set as if appeared in event wizard
//				 category,		// category in expressions panel
//				 exp_name,		// the expression name after the dot, e.g. "foo" for "myobject.foo" - also the runtime function name
//				 description);	// description in expressions panel

AddExpression(0, ef_return_number, "", "User Media", "VideoWidth", "Return the width of the image from the device camera.");
AddExpression(1, ef_return_number, "", "User Media", "VideoHeight", "Return the height of the image from the device camera.");
AddExpression(2, ef_return_string, "", "User Media", "SnapshotURL", "Return a data URI containing image data from a snapshot. Can be set to a Sprite or Tiled Background.");

AddExpression(3, ef_return_string, "", "Speech recognition", "FinalTranscript", "Return the final recognised speech.");

AddExpression(4, ef_return_string, "", "Speech recognition", "InterimTranscript", "Return the interim recognised speech, which can change.");

AddExpression(5, ef_return_string, "", "Speech recognition", "SpeechError", "Return a string describing the type of error in 'On error'.");

AddExpression(6, ef_return_number, "", "Ambient light", "AmbientLux", "Return the device ambient light reading, if available, in lux.");

AddExpression(7, ef_return_number, "", "Speech synthesis", "VoiceCount", "Return the number of available voices.");

AddNumberParam("Index", "Zero-based index of the voice.");
AddExpression(8, ef_return_string, "", "Speech synthesis", "VoiceNameAt", "Return the name of a voice at an index.");

AddNumberParam("Index", "Zero-based index of the voice.");
AddExpression(9, ef_return_string, "", "Speech synthesis", "VoiceLangAt", "Return the language of a voice at an index.");

AddNumberParam("Index", "Zero-based index of the voice.");
AddExpression(10, ef_return_string, "", "Speech synthesis", "VoiceURIAt", "Return the URI of a voice at an index.");

AddExpression(11, ef_return_number, "", "User Media", "AudioSourceCount", "Return the number of audio sources available.");

AddNumberParam("Index", "Zero-based index of the audio source.");
AddExpression(12, ef_return_string, "", "User Media", "AudioSourceLabelAt", "Return the label of a given audio source.");

AddExpression(13, ef_return_number, "", "User Media", "CameraSourceCount", "Return the number of camera sources available.");

AddNumberParam("Index", "Zero-based index of the camera source.");
AddExpression(14, ef_return_string, "", "User Media", "CameraSourceLabelAt", "Return the label of a given camera source.");

AddNumberParam("Index", "Zero-based index of the camera source.");
AddExpression(15, ef_return_string, "", "User Media", "CameraSourceFacingAt", "Return the facing (e.g. front/back) of a given camera source.");

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
	renderer.Line(tl, tr, cr.RGB(255, 0, 0));
	renderer.Line(tr, br, cr.RGB(255, 0, 0));
	renderer.Line(br, bl, cr.RGB(255, 0, 0));
	renderer.Line(bl, tl, cr.RGB(255, 0, 0));
	renderer.Line(tl, br, cr.RGB(255, 0, 0));
	renderer.Line(bl, tr, cr.RGB(255, 0, 0));
}

// For rendered objects to release fonts or textures
IDEInstance.prototype.OnRendererReleased = function(renderer)
{
}