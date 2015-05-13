function GetPluginSettings()
{
	return {
		"name":			"Tilemap",				// as appears in 'insert object' dialog, can be changed as long as "id" stays the same
		"id":			"Tilemap",				// this is used to identify this plugin and is saved to the project; never change it
		"version":		"1.0",					// (float in x.y format) Plugin version - C2 shows compatibility warnings based on this
		"description":	"Create tile-based level designs. TMX maps also supported.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/172/tilemap",
		"category":		"General",				// Prefer to re-use existing categories, but you can set anything here
		"type":			"world",				// either "world" (appears in layout and is drawn), else "object"
		"rotatable":	false,					// only used when "type" is "world".  Enables an angle property on the object.
		"defaultimage":	"default_tilemap.png",
		"flags":		0						// uncomment lines to enable flags...
					//	| pf_singleglobal		// exists project-wide, e.g. mouse, keyboard.  "type" must be "object".
						| pf_texture			// object has a single texture (e.g. tiled background)
						| pf_position_aces		// compare/set/get x, y...
						| pf_size_aces			// compare/set/get width, height...
					//	| pf_angle_aces			// compare/set/get angle (recommended that "rotatable" be set to true)
						| pf_appearance_aces	// compare/set/get visible, opacity...
					//	| pf_tiling				// adjusts image editor features to better suit tiled images (e.g. tiled background)
					//	| pf_animations			// enables the animations system.  See 'Sprite' for usage
						| pf_zorder_aces		// move to top, bottom, layer...
					//  | pf_nosize				// prevent resizing in the editor
						| pf_effects			// allow WebGL shader effects to be added
						| pf_predraw			// set for any plugin which draws and is not a sprite (i.e. does not simply draw
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

AddNumberParam("Tile X", "X index of the tile (in tiles).");
AddNumberParam("Tile Y", "Y index of the tile (in tiles).");
AddCmpParam("Comparison", "How to compare the tile at the given position.");
AddNumberParam("Tile", "The tile number to compare to.");
AddCondition(0, cf_none, "Compare tile at", "Tilemap", "Tile ({0}, {1}) {2} {3}", "Compare the tile number at a position.", "CompareTileAt");

AddNumberParam("Tile X", "X index of the tile (in tiles).");
AddNumberParam("Tile Y", "Y index of the tile (in tiles).");
AddComboParamOption("normal");
AddComboParamOption("flipped horizontal");
AddComboParamOption("flipped vertical");
AddComboParamOption("rotated 90°");
AddComboParamOption("rotated 180°");
AddComboParamOption("rotated 270°");
AddComboParamOption("rotated 90°, flipped vertical");
AddComboParamOption("rotated 270°, flipped vertical");
AddComboParam("State", "The state to check if the given tile matches.");
AddCondition(1, cf_none, "Compare tile state at", "Tilemap", "Tile ({0}, {1}) is {2}", "Compare the flipped or rotated state of a tile at a position.", "CompareTileStateAt");

AddCondition(2, cf_trigger, "On image URL loaded", "Web", "On image URL loaded", "Triggered after 'Load image from URL' when the image has finished loading.", "OnURLLoaded");

////////////////////////////////////////
// Actions

// AddAction(id,				// any positive integer to uniquely identify this action
//			 flags,				// (see docs) af_none, af_deprecated
//			 list_name,			// appears in event wizard list
//			 category,			// category in event wizard list
//			 display_str,		// as appears in event sheet - use {0}, {1} for parameters and also <b></b>, <i></i>
//			 description,		// appears in event wizard dialog when selected
//			 script_name);		// corresponding runtime function name

AddNumberParam("Tile X", "X index of the tile to erase (in tiles).");
AddNumberParam("Tile Y", "Y index of the tile to erase (in tiles).");
AddAction(0, af_none, "Erase tile", "Tilemap", "Erase tile (<i>{0}</i>, <i>{1}</i>)", "Erase a tile at a position.", "EraseTile");

AddNumberParam("Tile X", "X index of the tile to set (in tiles).");
AddNumberParam("Tile Y", "Y index of the tile to set (in tiles).");
AddNumberParam("Tile", "Tile number to set at the given position.");
AddComboParamOption("normal");
AddComboParamOption("flipped horizontal");
AddComboParamOption("flipped vertical");
AddComboParamOption("rotated 90°");
AddComboParamOption("rotated 180°");
AddComboParamOption("rotated 270°");
AddComboParamOption("rotated 90°, flipped vertical");
AddComboParamOption("rotated 270°, flipped vertical");
AddComboParam("State", "Whether to rotate or flip this tile.");
AddAction(1, af_none, "Set tile", "Tilemap", "Set tile (<i>{0}</i>, <i>{1}</i>) to tile <b>{2}</b> (<i>{3}</i>)", "Change a tile at a position.", "SetTile");

AddNumberParam("Tile X", "X index of the first tile to erase (in tiles).");
AddNumberParam("Tile Y", "Y index of the first tile to erase (in tiles).");
AddNumberParam("Width", "Number of tiles across to erase.", "1");
AddNumberParam("Height", "Number of tiles down to erase.", "1");
AddAction(2, af_none, "Erase tile range", "Tilemap", "Erase tile range (<i>{0}</i>, <i>{1}</i>) area <i>{2}</i> x <i>{3}</i>", "Erase a rectangular area of tiles.", "EraseTileRange");

AddNumberParam("Tile X", "X index of the first tile to set (in tiles).");
AddNumberParam("Tile Y", "Y index of the first tile to set (in tiles).");
AddNumberParam("Width", "Number of tiles across to set.", "1");
AddNumberParam("Height", "Number of tiles down to set.", "1");
AddNumberParam("Tile", "Tile number to set across the given range.");
AddComboParamOption("normal");
AddComboParamOption("flipped horizontal");
AddComboParamOption("flipped vertical");
AddComboParamOption("rotated 90°");
AddComboParamOption("rotated 180°");
AddComboParamOption("rotated 270°");
AddComboParamOption("rotated 90°, flipped vertical");
AddComboParamOption("rotated 270°, flipped vertical");
AddComboParam("State", "Whether to rotate or flip the tiles.");
AddAction(3, af_none, "Set tile range", "Tilemap", "Set tiles at (<i>{0}</i>, <i>{1}</i>) with area <i>{2}</i> x <i>{3}</i> to tile <b>{4}</b> (<i>{5}</i>)", "Set a rectangular area of tiles.", "SetTileRange");

AddNumberParam("Tile X", "X index of the tile to modify (in tiles).");
AddNumberParam("Tile Y", "Y index of the tile to modify (in tiles).");
AddComboParamOption("normal");
AddComboParamOption("flipped horizontal");
AddComboParamOption("flipped vertical");
AddComboParamOption("rotated 90°");
AddComboParamOption("rotated 180°");
AddComboParamOption("rotated 270°");
AddComboParamOption("rotated 90°, flipped vertical");
AddComboParamOption("rotated 270°, flipped vertical");
AddComboParam("State", "Whether to rotate or flip this tile.");
AddAction(6, af_none, "Set tile state", "Tilemap", "Set tile (<i>{0}</i>, <i>{1}</i>) state <i>{2}</i>", "Change a tile's flipped or rotated state.", "SetTileState");

AddNumberParam("Tile X", "X index of the first tile to modify (in tiles).");
AddNumberParam("Tile Y", "Y index of the first tile to modify (in tiles).");
AddNumberParam("Width", "Number of tiles across to set.", "1");
AddNumberParam("Height", "Number of tiles down to set.", "1");
AddComboParamOption("normal");
AddComboParamOption("flipped horizontal");
AddComboParamOption("flipped vertical");
AddComboParamOption("rotated 90°");
AddComboParamOption("rotated 180°");
AddComboParamOption("rotated 270°");
AddComboParamOption("rotated 90°, flipped vertical");
AddComboParamOption("rotated 270°, flipped vertical");
AddComboParam("State", "Whether to rotate or flip the tiles.");
AddAction(7, af_none, "Set tile state range", "Tilemap", "Set tiles at (<i>{0}</i>, <i>{1}</i>) with area <i>{2}</i> x <i>{3}</i> state <i>{4}</i>", "Change the flipped or rotated state of a rectangular area of tiles.", "SetTileStateRange");

AddStringParam("JSON", "A string to load tilemap tiles from in JSON format from a previous use of the TilesJSON expression.");
AddAction(4, af_none, "Load", "JSON", "Load tilemap from JSON <b>{0}</b>", "Load tiles from a JSON string.", "LoadFromJSON");

AddStringParam("Filename", "The name of the file to download.", "\"tilemap.json\"");
AddAction(5, 0, "Download", "JSON", "Download as JSON data with filename <i>{0}</i>", "Download the tilemap data as a JSON file.", "JSONDownload");

AddStringParam("URI", "Enter the URL on the web, or data URI, of an image to load.", "\"http://\"");
AddAction(8, 0, "Load image from URL", "Web", "Load image from <i>{0}</i>", "Load the tilemap source image from a web address or data URI.", "LoadURL");

////////////////////////////////////////
// Expressions

// AddExpression(id,			// any positive integer to uniquely identify this expression
//				 flags,			// (see docs) ef_none, ef_deprecated, ef_return_number, ef_return_string,
//								// ef_return_any, ef_variadic_parameters (one return flag must be specified)
//				 list_name,		// currently ignored, but set as if appeared in event wizard
//				 category,		// category in expressions panel
//				 exp_name,		// the expression name after the dot, e.g. "foo" for "myobject.foo" - also the runtime function name
//				 description);	// description in expressions panel

AddNumberParam("TileX", "X position in tiles");
AddNumberParam("TileY", "Y position in tiles");
AddExpression(0, ef_return_number, "", "Tilemap", "TileAt", "Return the tile number at a position in tiles.");

AddNumberParam("X", "X co-ordinate");
AddExpression(1, ef_return_number, "", "Tilemap", "PositionToTileX", "Convert an X co-ordinate to a tile X index.");

AddNumberParam("Y", "Y co-ordinate");
AddExpression(2, ef_return_number, "", "Tilemap", "PositionToTileY", "Convert a Y co-ordinate to a tile Y index.");

AddNumberParam("TileX", "X position in tiles");
AddExpression(3, ef_return_number, "", "Tilemap", "TileToPositionX", "Convert a tile X index to X co-ordinate.");

AddNumberParam("TileY", "Y position in tiles");
AddExpression(4, ef_return_number, "", "Tilemap", "TileToPositionY", "Convert a tile Y index to Y co-ordinate.");

AddNumberParam("X", "X co-ordinate");
AddExpression(5, ef_return_number, "", "Tilemap", "SnapX", "Snap an X co-ordinate to the nearest tile X.");

AddNumberParam("Y", "Y co-ordinate");
AddExpression(6, ef_return_number, "", "Tilemap", "SnapY", "Snap a Y co-ordinate to the nearest tile Y.");

AddExpression(7, ef_return_string, "", "JSON", "TilesJSON", "Get tilemap data as a JSON-formatted string.");

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
	new cr.Property(ept_link,	"Image",				"Edit",		"Click to edit the object's image.", "firstonly"),
	new cr.Property(ept_combo,	"Initial visibility",	"Visible",	"Choose whether the object is visible when the layout starts.", "Visible|Invisible"),
	new cr.Property(ept_integer, "Tile width",			32,			"Width in pixels of each tile."),
	new cr.Property(ept_integer, "Tile height",			32,			"Height in pixels of each tile."),
	new cr.Property(ept_integer, "Tile X offset",		0,			"X offset in image of first tile."),
	new cr.Property(ept_integer, "Tile Y offset",		0,			"Y offset in image of first tile."),
	new cr.Property(ept_integer, "Tile X spacing",		0,			"Horizontal gap in pixels between each tile."),
	new cr.Property(ept_integer, "Tile Y spacing",		0,			"Veritcal gap in pixels between each tile."),
	new cr.Property(ept_combo,	"Seamless mode",		"On",		"Use a slower but seamless rendering method.", "Off|On")
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
		
	// Plugin-specific variables
	// this.myValue = 0...
};

IDEInstance.prototype.OnCreate = function()
{
	this.instance.SetHotspot(new cr.vector2(0, 0));
};

// Called when inserted via Insert Object Dialog for the first time
IDEInstance.prototype.OnInserted = function()
{
	this.instance.SetHotspot(new cr.vector2(0, 0));
	this.instance.SetPosition(new cr.vector2(0, 0));
	this.instance.SetSize(this.instance.GetLayoutSize());
};

// Called when double clicked in layout
IDEInstance.prototype.OnDoubleClicked = function()
{
	this.instance.EditTexture();
};

// Called after a property has been changed in the properties bar
IDEInstance.prototype.OnPropertyChanged = function(property_name)
{
	// Edit image link
	if (property_name === "Image")
	{
		this.instance.EditTexture();
	}
};

// For rendered objects to load fonts or textures
IDEInstance.prototype.OnRendererInit = function(renderer)
{
	renderer.LoadTexture(this.instance.GetTexture());
};

// Called to draw self in the editor if a layout object
IDEInstance.prototype.Draw = function(renderer)
{
	var texture = this.instance.GetTexture();
	renderer.SetTexture(this.instance.GetTexture());
	
	// Tile rendering happens via magic on the editor side
	this.instance.DrawTilemap();
};

// For rendered objects to release fonts or textures
IDEInstance.prototype.OnRendererReleased = function(renderer)
{
	renderer.ReleaseTexture(this.instance.GetTexture());
};