// ECMAScript 5 strict mode
/* global cr,log,assert2 */
function GetPluginSettings()
{
	return {
		"name":			"Sprite font",
		"id":			"Spritefont2",
		"version":		"1.0",
		"description":	"Displays some text on the screen using a sprite sheet.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/166/sprite-font",
		"category":		"General",
		"type":			"world",			// appears in layout
		"rotatable":	true,				// can be rotated in layout
		"defaultimage":	"default_spritefont.png",
		"flags":		pf_position_aces | pf_size_aces | pf_angle_aces | pf_appearance_aces | pf_zorder_aces | pf_effects | pf_texture
						| pf_predraw
	};
}

// Conditions, actions and expressions
AddStringParam("Text to compare", "Enter the text to compare with the object's content.", "\"\"");
AddComboParamOption("Ignore case");
AddComboParamOption("Case sensitive");
AddComboParam("Case sensitivity", "Choose whether capital letters count as different to lowercase.  If ignoring case, \"ABC\" matches \"abc\".", 0);
AddCondition(0, 0, "Compare text", "Text", "Text is <b>{0}</b> <i>({1})</i>", "Compare the text in this object.", "CompareText");

////////////////////////
AddAnyTypeParam("Text", "Enter the text to set the object's content to.", "\"\"");
AddAction(0, 0, "Set text", "Text", "Set text to <i>{0}</i>", "Set the text of this object.", "SetText");

AddAnyTypeParam("Text", "Enter the text to append to the object's content.", "\"\"");
AddAction(1, 0, "Append text", "Text", "Append <i>{0}</i>", "Add text to the end of the existing text.", "AppendText");

AddNumberParam("Scale", "Enter the scale of the spritefont image.", "1.0");
AddAction(2, 0, "Set scale", "Text", "Set scale <i>{0}</i>", "Set the scale of the spritefont image.", "SetScale");

AddNumberParam("Character spacing", "Enter the number of pixels for the spacing between characters.", "0");
AddAction(3, 0, "Set character spacing", "Text", "Set spacing <i>{0}</i>", "Set spacing between each characters.", "SetCharacterSpacing");

AddNumberParam("Line height", "Enter the number of pixels for the spacing between lines.", "0");
AddAction(4, 0, "Set line height", "Text", "Set line height to <i>{0}</i>", "Set spacing between each lines.", "SetLineHeight");

AddStringParam("Character", "Enter one or more characters to change the width for (e.g. \"abcd\").", "\"\"");
AddNumberParam("Width", "Enter the number of pixels width you want the character(s) to occupy. This should be less than the cell width.", "8");
AddAction(5, 0, "Set character width", "Text", "Set character width to <i>{1}</i> for <i>{0}</i>", "Change the width of some characters.", "SetCharacterWidth");

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
AddAction(6, 0, "Set blend mode", "Appearance", "Set blend mode to <i>{0}</i>", "Set the background blend mode for this object.", "SetEffect");

AddComboParamOption("left");
AddComboParamOption("center");
AddComboParamOption("right");
AddComboParam("Horizontal alignment", "Choose the new horizontal alignment for the text.");
AddAction(7, 0, "Set horizontal alignment", "Text", "Set horizontal alignment to <i>{0}</i>", "Set the horizontal alignment of the text.", "SetHAlign");

AddComboParamOption("top");
AddComboParamOption("center");
AddComboParamOption("bottom");
AddComboParam("Vertical alignment", "Choose the new vertical alignment for the text.");
AddAction(8, 0, "Set vertical alignment", "Text", "Set vertical alignment to <i>{0}</i>", "Set the vertical alignment of the text.", "SetVAlign");


////////////////////////
AddStringParam("Character", "Character to get width for (empty for default)", "\"\"");
AddExpression(0,	ef_return_number,	"Get character width","Text",			"CharacterWidth",		"Get the width of a character.");
AddExpression(1,	ef_return_number,	"Get character height","Text",			"CharacterHeight",		"Get the height of a character.");
AddExpression(2,	ef_return_number,	"Get character scale","Text",			"CharacterScale",		"Get the scale of the characters.");
AddExpression(3,	ef_return_number,	"Get character spacing","Text",			"CharacterSpacing",		"Get the spacing between characters.");
AddExpression(4,	ef_return_number,	"Get line height","Text",				"LineHeight",			"Get the line height.");
AddExpression(5,	ef_return_string,	"Get text",			"Text",			"Text",		"Get the object's text.");
AddExpression(6,	ef_return_number,	"Get text width",	"Text",			"TextWidth", "Get the width extent of the text in the object in pixels.");
AddExpression(7,	ef_return_number,	"Get text height",	"Text",			"TextHeight", "Get the height extent of the text in the object in pixels.");


ACESDone();

// Property grid properties for this plugin
var property_list = [
	new cr.Property(ept_link,	"Sprite font",			"Edit",		"Click to edit the object's spritefont.", "firstonly"),
	new cr.Property(ept_integer,"Character width",		16,			"Width of each cell in the image."),
	new cr.Property(ept_integer,"Character height",		16,			"Height of each cell in the image."),
	new cr.Property(ept_text,	"Character set",		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,;:?!-_~#\"'&()[]|`\\/@°+=*$£€<>", "List of characters in the spritefont image, starting in the top-left."),
	new cr.Property(ept_text,	"Text",					"Text",		"Text to display."),
	new cr.Property(ept_float,	"Scale",				1.0,		"A multiplier to scale the drawn text with."),
	new cr.Property(ept_combo,	"Initial visibility",	"Visible",	"Choose whether the object is visible when the layout starts.", "Visible|Invisible"),
	new cr.Property(ept_combo,	"Horizontal alignment",	"Left",		"Horizontal alignment of the text.", "Left|Center|Right"),
	new cr.Property(ept_combo,	"Vertical alignment",	"Top",		"Vertical alignment of the text.", "Top|Center|Bottom"),
	new cr.Property(ept_combo,	"Hotspot",				"Top-left",	"Choose the location of the hot spot in the object.", "Top-left|Top|Top-right|Left|Center|Right|Bottom-left|Bottom|Bottom-right"),
	new cr.Property(ept_combo,	"Wrapping",				"Word",		"Wrap text by space-separated words or nearest character.", "Word|Character"),
	new cr.Property(ept_integer,"Character spacing",	0,			"Extra pixel width to add between characters."),
	new cr.Property(ept_float,	"Line height",			0,			"Offset to the default line height, in pixels. 0 is default line height.")
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

	// Properties for font
	this.charList = null;		// default font string
	this.lines = [];
}

IDEInstance.prototype.OnCreate = function()
{
	this.instance.SetHotspot(GetHotspot(this.properties["Hotspot"]));
};

// Called by the IDE after all initialization on this instance has been completed
IDEInstance.prototype.OnInserted = function()
{
	this.instance.SetSize(new cr.vector2(200, 30));
};

IDEInstance.prototype.OnDoubleClicked = function()
{
	this.instance.EditTexture();
};

// Called by the IDE after a property has been changed
IDEInstance.prototype.OnPropertyChanged = function(property_name)
{
	// Recreate font if font property changed
	if (property_name === "Sprite font")
	{
		this.instance.EditTexture();
	}
	else if (property_name === "Hotspot")
	{
		this.instance.SetHotspot(GetHotspot(this.properties["Hotspot"]));
	}
	else if (property_name === "Character set" ||
		property_name === "Character width" ||
		property_name === "Character height")
	{
		this.SplitSheet();
	}
};

IDEInstance.prototype.OnTextureEdited = function ()
{
	this.SplitSheet();
};

IDEInstance.prototype.SplitSheet = function() {
	var texture = this.instance.GetTexture();
	if (texture == null) return;

	var texSize      = texture.GetImageSize();
	var charWidth    = this.properties["Character width"];
	var charHeight   = this.properties["Character height"];
	var uvCharWidth  = charWidth/texSize.x;
	var uvCharHeight = charHeight/texSize.y;
	var charSet      = this.properties["Character set"];
	this.charList    = {};

	var cols = Math.floor(texSize.x/charWidth);
	var rows = Math.floor(texSize.y/charHeight);

	for ( var c = 0; c < charSet.length; c++) {
		// not enough texture space
		if  (c >= cols * rows) break;

		// create uvs for each characters
		var x = c%cols;
		var y = Math.floor(c/cols);
		this.charList[charSet.charAt(c)] =  new cr.rect(
			x * uvCharWidth,
			y * uvCharHeight,
			(x+1) * uvCharWidth,
			(y+1) * uvCharHeight
		);
	}
};


IDEInstance.prototype.OnRendererInit = function(renderer)
{
	renderer.LoadTexture(this.instance.GetTexture());
};

function rotateQuad(quad,cosa,sina) {
	var x_temp;

	x_temp   = (quad.tlx * cosa) - (quad.tly * sina);
	quad.tly = (quad.tly * cosa) + (quad.tlx * sina);
	quad.tlx = x_temp;

	x_temp    = (quad.trx * cosa) - (quad.try_ * sina);
	quad.try_ = (quad.try_ * cosa) + (quad.trx * sina);
	quad.trx  = x_temp;

	x_temp   = (quad.blx * cosa) - (quad.bly * sina);
	quad.bly = (quad.bly * cosa) + (quad.blx * sina);
	quad.blx = x_temp;

	x_temp    = (quad.brx * cosa) - (quad.bry * sina);
	quad.bry = (quad.bry * cosa) + (quad.brx * sina);
	quad.brx  = x_temp;

}

// Called by the IDE to draw this instance in the editor
IDEInstance.prototype.Draw = function(renderer)
{
	var texture = this.instance.GetTexture();
	var text    = this.properties["Text"];
	var scale   = this.properties["Scale"];
	var charWidth  = this.properties["Character width"] * scale;
	var charHeight = this.properties["Character height"] * scale;
	var EPSILON = 0.00001;
	
	if (text !== "" && texture != null) {
		if (this.charList === null) this.SplitSheet();
		renderer.SetTexture(texture);




		// we get all the parameters we need for computation
		var objsize = this.instance.GetSize();
		var wrapbyword = this.properties["Wrapping"] === "Word";
		var lines = [];
		WordWrap(this, text, this.lines, objsize.x, wrapbyword);

		var lineHeight = this.properties["Line height"];

		if (objsize.y < charHeight + lineHeight) {
			// not enough room to draw the first line
			return;
		}

		var hprop = this.properties["Horizontal alignment"];
		var vprop = this.properties["Vertical alignment"];

		// convert alignement properties to some usable values
		var ha = 0;
		ha = hprop === "Center" ? 0.5 : ha;
		ha = hprop === "Right"  ? 1.0 : ha;

		var va = 0;
		va = vprop === "Center" ? 0.5 : va;
		va = vprop === "Bottom" ? 1.0 : va;

		var texsize = texture.GetImageSize();
		var angle   = this.instance.GetAngle(); //radians
		var charSpace  = this.properties["Character spacing"];
		var lines = this.lines;
		var textHeight = lines.length * (charHeight + lineHeight);

		// we precompute cosine and sine 'cause we will use them a lot
		var cosa = Math.cos(angle);
		var sina = Math.sin(angle);

		// we compute the offsets for vertical alignement in object-space
		// but it can't be negative, else it would underflow the boundingbox
		var halign;
		var valign = va * Math.max(0,(objsize.y - textHeight));

		// we get the position of the top left corner of the bounding box
		var q = this.instance.GetBoundingQuad();
		var offx = q.tlx;
		var offy = q.tly;
		
		/**/
		if (angle == 0) {
			offx = Math.round(offx);
			offy = Math.round(offy);
		}
		//*/

		var drawX ;
		var drawY = valign;

		var dQuad = new cr.quad();

		for(var i = 0; i < lines.length; i++) {
			// for horizontal alignement, we need to work line by line
			var line       = lines[i].text;
			var lineWidth  = lines[i].width;

			// compute horizontal empty space
			// offset drawX according to horizontal alignement
			// indentation could be negative if long word in wrapbyword mode
			halign = ha * Math.max(0, objsize.x - lineWidth);
			drawX = halign;

			// we round to avoid pixel blurring
			drawY += lineHeight;
			for(var j = 0; j < line.length; j++) {

				var letter = line.charAt(j);
				// we skip unrecognized characters (creates a space)
				var clipUV = this.charList[letter];

				// check if next letter fits in bounding box
				if ( drawX + charWidth > objsize.x + EPSILON) {
					break;
				}

				if (clipUV !== undefined) {
					var clipWidth  = charWidth;
					var clipHeight = charHeight;

					// we build the quad
					dQuad.tlx  = drawX;
					dQuad.tly  = drawY;
					dQuad.trx  = drawX + clipWidth;
					dQuad.try_ = drawY ;
					dQuad.blx  = drawX;
					dQuad.bly  = drawY + clipHeight;
					dQuad.brx  = drawX + clipWidth;
					dQuad.bry  = drawY + clipHeight;

					// we then rotate the quad around 0,0
					rotateQuad(dQuad,cosa,sina);
					// we then apply the world space offset
					dQuad.offset(offx,offy);

					// and render
					renderer.Quad(dQuad, this.instance.GetOpacity(), clipUV);
				}
				drawX  += charWidth + charSpace;
			}
			drawY += charHeight;
			// check if next row fits in bounding box
			if ( drawY + charHeight + lineHeight > objsize.y) {
				break;
			}
		}
	}
};

IDEInstance.prototype.OnRendererReleased = function(renderer)
{
	renderer.ReleaseTexture(this.instance.GetTexture());
};



/*
 *	Word-Wrapping
 */
IDEInstance.prototype.measureWidth = function(text) {

	var scale     = this.properties["Scale"];
	var charWidth = this.properties["Character width"];
	var spacing   =  this.properties["Character spacing"];
	var len     = text.length;
	var width   = 0;
	for (var i = 0; i < len; i++) {
		width += charWidth * scale + spacing;
	}
	// we remove the trailing spacing
	width -= (width > 0) ? spacing : 0;
	return width;
};

function trimRight(text)
{
	return text.replace(/\s\s*$/, '');
}

function TokeniseWords(text)
{
	var wordsCache = [];

	var cur_word = "";
	var ch;

	// Loop every char
	var i = 0;

	while (i < text.length)
	{
		ch = text.charAt(i);

		if (ch === "\n")
		{
			// Dump current word if any
			if (cur_word.length)
			{
				wordsCache.push(cur_word);
				cur_word = "";
			}

			// Add newline word
			wordsCache.push("\n");

			++i;
		}
		// Whitespace or hyphen: swallow rest of whitespace and include in word
		else if (ch === " " || ch === "\t" || ch === "-")
		{
			do {
				cur_word += text.charAt(i);
				i++;
			}
			while (i < text.length && (text.charAt(i) === " " || text.charAt(i) === "\t"));

			wordsCache.push(cur_word);
			cur_word = "";
		}
		else if (i < text.length)
		{
			cur_word += ch;
			i++;
		}
	}

	// Append leftover word if any
	if (cur_word.length)
		wordsCache.push(cur_word);

	return wordsCache;
}

function addLine(inst,lines,lineIndex,cur_line) {
	var line;
	cur_line = trimRight(cur_line);
	// Recycle a line if possible
	line = {};
	line.text = cur_line;
	line.width = inst.measureWidth(cur_line);
	lines[lineIndex] = line;
}

function WordWrap(inst, text, lines, width, wrapbyword)
{
	lines.length = 0;
	if (!text || !text.length || width <= 2.0)
	{
		return;
	}

	// If under 100 characters (i.e. a fairly short string), try a short string optimisation: just measure the text
	// and see if it fits on one line, without going through the tokenise/wrap.
	// Text musn't contain a linebreak!
	var charWidth    = inst.properties["Character width"];
	var charScale    = inst.properties["Scale"];
	var charSpacing  = inst.properties["Character spacing"];
	if ( (text.length * (charWidth * charScale + charSpacing) - charSpacing) <= width && text.indexOf("\n") === -1)
	{
		var all_width = inst.measureWidth(text);

		if (all_width <= width)
		{
			// fits on one line
			lines.push({});
			lines[0].text = text;
			lines[0].width = all_width;
			return;
		}
	}

	this.WrapText(inst, text, lines, width, wrapbyword);
}

function WrapText(inst, text, lines, width, wrapbyword)
{
	var wordArray;
	lines.length = 0;
	if (wrapbyword) {
		wordArray = TokeniseWords(text);
	}
	else {
		wordArray = text;
	}

	var cur_line = "";
	var prev_line;
	var line_width;
	var i;
	var lineIndex = 0;
	var line;
	var ignore_newline = false;

	for (i = 0; i < wordArray.length; i++)
	{
		// Look for newline
		if (wordArray[i] === "\n")
		{
			if (ignore_newline === true) {
				// if a newline as been added by the wrapping
				// we ignore any happening just after
				ignore_newline = false;
			} else {
				// Flush line.  
				addLine(inst,lines,lineIndex,cur_line);
				lineIndex++;
			}
			cur_line = "";
			continue;
		}
		ignore_newline = false;

		// Otherwise add to line
		prev_line = cur_line;
		cur_line += wordArray[i];

		// Measure line
		line_width = inst.measureWidth(trimRight(cur_line));

		// Line too long: wrap the line before this word was added
		if (line_width > width)
		{

			if (prev_line === "") {
				// if it's the first word, we push it on the line
				// to avoid an unnecessary blank line
				// and since we are wrapping, we ignore the next newline if any
				addLine(inst,lines,lineIndex,cur_line);
				cur_line = "";
				ignore_newline = true;
			} else {
				// else we push the previous line
				addLine(inst,lines,lineIndex,prev_line);
				cur_line = wordArray[i];
			}

			lineIndex++;

			// Wrapping by character: avoid lines starting with spaces
			if (!wrapbyword && cur_line === " ")
				cur_line = "";
		}
	}

	// Add any leftover line
	if (trimRight(cur_line).length)
	{
		addLine(inst,lines,lineIndex,cur_line);

		lineIndex++;
	}
}

