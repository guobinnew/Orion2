function GetPluginSettings()
{
	return {
		"name":			"9-patch",
		"id":			"NinePatch",
		"version":		"1.0",
		"description":	"Create resizable boxes with an image border by splitting an image in to nine tiles.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/151/9-patch",
		"category":		"General",
		"type":			"world",			// appears in layout
		"rotatable":	false,
		"flags":		pf_texture | pf_position_aces | pf_size_aces | /*pf_angle_aces |*/ pf_appearance_aces | pf_zorder_aces | pf_effects | pf_predraw
	};
};

// Conditions, actions and expressions
AddCondition(0, cf_trigger, "On image URL loaded", "Web", "On image URL loaded", "Triggered after 'Load image from URL' when the image has finished loading.", "OnURLLoaded");

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

ACESDone();

// Property grid properties for this plugin
var property_list = [
	new cr.Property(ept_link,	"Image",				lang("project\\misc\\tiledbg-edit-link"), "Click to edit the object's image.", "firstonly"),
	new cr.Property(ept_integer, "Left margin",			16, 		"Left border margin on the image in pixels."),
	new cr.Property(ept_integer, "Right margin",		16, 		"Right border margin on the image in pixels."),
	new cr.Property(ept_integer, "Top margin",			16, 		"Top border margin on the image in pixels."),
	new cr.Property(ept_integer, "Bottom margin",		16, 		"Bottom border margin on the image in pixels."),
	new cr.Property(ept_combo,	 "Edges",				"Stretch",	"Choose whether to tile or stretch the edges.", "Tile|Stretch"),
	new cr.Property(ept_combo,	 "Fill",				"Stretch", 	"Choose how to display the inside of the box.", "Tile|Stretch|Transparent"),
	new cr.Property(ept_combo,	"Initial visibility",	"Visible",	"Choose whether the object is visible when the layout starts.", "Visible|Invisible"),
	new cr.Property(ept_combo,	"Hotspot",				"Top-left",	"Choose the location of the hot spot in the object.", "Top-left|Top|Top-right|Left|Center|Right|Bottom-left|Bottom|Bottom-right"),
	new cr.Property(ept_combo,	"Seams",				"Overlap",		"Overlap seams or meet exact. 'Overlap' is seamless unless opacity used.", "Exact|Overlap")
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
	this.instance.SetHotspot(GetHotspot(this.properties["Hotspot"]));
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
		this.instance.SetHotspot(GetHotspot(this.properties["Hotspot"]));
	}
}

IDEInstance.prototype.OnRendererInit = function(renderer)
{
	renderer.LoadTexture(this.instance.GetTexture());
}

var tmpQuad = new cr.quad();
var tmpTex = new cr.rect();
var tmpOpacity = 1;

function drawPatch(renderer, tex, iw, ih, sx, sy, sw, sh, dx, dy, dw, dh)
{
	if (tex)
		renderer.SetTexture(tex);

	if (sw <= 0 || sh <= 0 || dw <= 0 || dh <= 0)
		return;
	
	tmpTex.left = sx / iw;
	tmpTex.top = sy / ih;
	tmpTex.right = (sx + sw) / iw;
	tmpTex.bottom = (sy + sh) / ih;
	
	tmpQuad.tlx = dx;
	tmpQuad.tly = dy;
	tmpQuad.trx = dx + dw;
	tmpQuad.try_ = dy;
	tmpQuad.brx = dx + dw;
	tmpQuad.bry = dy + dh;
	tmpQuad.blx = dx;
	tmpQuad.bly = dy + dh;
	
	renderer.Quad(tmpQuad, tmpOpacity, tmpTex);
};

function tilePatch(renderer, tex, iw, ih, sx, sy, sw, sh, dx, dy, dw, dh)
{
	if (sw <= 0 || sh <= 0 || dw <= 0 || dh <= 0)
		return;
	
	renderer.SetTexture(tex);
	
	var x, y, endx, endy, w, h;
	endx = dx + dw;
	endy = dy + dh;
	
	for (x = dx; x < endx; x += sw)
	{
		w = endx - x;
		if (w > sw)
			w = sw;
		
		for (y = dy; y < endy; y += sh)
		{
			h = endy - y;
			if (h > sh)
				h = sh;
			
			drawPatch(renderer, null, iw, ih, sx, sy, w, h, x, y, w, h);
		}
	}
};
	
// Called to draw self in the editor
IDEInstance.prototype.Draw = function(renderer)
{
	var texture = this.instance.GetTexture();
	
	// First draw after insert: use 2x the size of the texture so user can see resized content.
	// Done after SetTexture so the file is loaded and dimensions known, preventing
	// the file being loaded twice.
	if (this.just_inserted)
	{
		this.just_inserted = false;
		var sz = texture.GetImageSize();
		this.instance.SetSize(new cr.vector2(sz.x * 2, sz.y * 2));
		RefreshPropertyGrid();		// show new size
	}
	
	var texsize = texture.GetImageSize();
	var objsize = this.instance.GetSize();
	var q = this.instance.GetBoundingQuad();
	
	var lm = this.properties["Left margin"];
	var rm = this.properties["Right margin"];
	var tm = this.properties["Top margin"];
	var bm = this.properties["Bottom margin"];
	var iw = texsize.x;
	var ih = texsize.y;
	var re = iw - rm;
	var be = ih - bm;
	var myx = q.tlx;
	var myy = q.tly;
	var myw = objsize.x;
	var myh = objsize.y;
	var edges = this.properties["Edges"];
	var fill = this.properties["Fill"];
	tmpOpacity = this.instance.GetOpacity();
	
	if (fill === "Tile")
	{
		tilePatch(renderer, texture, iw, ih, lm, tm, re - lm, be - tm, myx + lm, myy + tm, myw - lm - rm, myh - tm - bm);
	}
	else if (fill === "Stretch")	// stretch fill
	{
		drawPatch(renderer, texture, iw, ih, lm, tm, re - lm, be - tm, myx + lm, myy + tm, myw - lm - rm, myh - tm - bm);
	}
	
	// draw edges
	if (edges === "Tile")
	{
		tilePatch(renderer, texture, iw, ih, 0, tm, lm, be - tm, myx, myy + tm, lm, myh - tm - bm);
		tilePatch(renderer, texture, iw, ih, re, tm, rm, be - tm, myx + myw - rm, myy + tm, rm, myh - tm - bm);
		tilePatch(renderer, texture, iw, ih, lm, 0, re - lm, tm, myx + lm, myy, myw - lm - rm, tm);
		tilePatch(renderer, texture, iw, ih, lm, be, re - lm, bm, myx + lm, myy + myh - bm, myw - lm - rm, bm);
	}
	else if (edges === "Stretch")
	{
		drawPatch(renderer, texture, iw, ih, 0, tm, lm, be - tm, myx, myy + tm, lm, myh - tm - bm);
		drawPatch(renderer, texture, iw, ih, re, tm, rm, be - tm, myx + myw - rm, myy + tm, rm, myh - tm - bm);
		drawPatch(renderer, texture, iw, ih, lm, 0, re - lm, tm, myx + lm, myy, myw - lm - rm, tm);
		drawPatch(renderer, texture, iw, ih, lm, be, re - lm, bm, myx + lm, myy + myh - bm, myw - lm - rm, bm);
	}
	
	// draw corners
	drawPatch(renderer, texture, iw, ih, 0, 0, lm, tm, myx, myy, lm, tm);
	drawPatch(renderer, texture, iw, ih, re, 0, rm, tm, myx + myw - rm, myy, rm, tm);
	drawPatch(renderer, texture, iw, ih, re, be, rm, bm, myx + myw - rm, myy + myh - bm, rm, bm);
	drawPatch(renderer, texture, iw, ih, 0, be, lm, bm, myx, myy + myh - bm, lm, bm);
}

IDEInstance.prototype.OnRendererReleased = function(renderer)
{
	renderer.ReleaseTexture(this.instance.GetTexture());
}