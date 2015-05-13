// ECMAScript 5 strict mode
/* global cr,log,assert2 */
/* jshint globalstrict: true */
/* jshint strict: true */
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.Spritefont2 = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var pluginProto = cr.plugins_.Spritefont2.prototype;

	pluginProto.onCreate = function ()
	{
	};

	/////////////////////////////////////
	// Object type class
	pluginProto.Type = function(plugin)
	{
		this.plugin = plugin;
		this.runtime = plugin.runtime;
	};

	var typeProto = pluginProto.Type.prototype;

	typeProto.onCreate = function()
	{
		if (this.is_family)
			return;

		// Create the texture
		this.texture_img = new Image();

		// Tell runtime to wait for this to load
		this.runtime.waitForImageLoad(this.texture_img, this.texture_file);

		this.webGL_texture = null;
	};

	typeProto.onLostWebGLContext = function ()
	{
		if (this.is_family)
			return;

		this.webGL_texture = null;
	};

	typeProto.onRestoreWebGLContext = function ()
	{
		// No need to create textures if no instances exist, will create on demand
		if (this.is_family || !this.instances.length)
			return;

		if (!this.webGL_texture)
		{
			this.webGL_texture = this.runtime.glwrap.loadTexture(this.texture_img, false, this.runtime.linearSampling, this.texture_pixelformat);
		}

		var i, len;
		for (i = 0, len = this.instances.length; i < len; i++)
			this.instances[i].webGL_texture = this.webGL_texture;
	};

	typeProto.unloadTextures = function ()
	{
		// Don't release textures if any instances still exist, they are probably using them
		if (this.is_family || this.instances.length || !this.webGL_texture)
			return;

		this.runtime.glwrap.deleteTexture(this.webGL_texture);
		this.webGL_texture = null;
	};

	typeProto.preloadCanvas2D = function (ctx)
	{
		// draw to preload, browser should lazy load the texture
		ctx.drawImage(this.texture_img, 0, 0);
	};

	/////////////////////////////////////
	// Instance class
	pluginProto.Instance = function(type)
	{
		this.type = type;
		this.runtime = type.runtime;
	};

	var instanceProto = pluginProto.Instance.prototype;

	instanceProto.onDestroy = function()
	{
		// recycle the instance's objects
		freeAllLines (this.lines);
		freeAllClip  (this.clipList);
		freeAllClipUV(this.clipUV);
		cr.wipe(this.characterWidthList);
	};

	instanceProto.onCreate = function()
	{
		this.texture_img      = this.type.texture_img;
		this.characterWidth   = this.properties[0];
		this.characterHeight  = this.properties[1];
		this.characterSet     = this.properties[2];
		this.text             = this.properties[3];
		this.characterScale   = this.properties[4];
		this.visible          = (this.properties[5] === 0);	// 0=visible, 1=invisible
		this.halign           = this.properties[6]/2.0;			// 0=left, 1=center, 2=right
		this.valign           = this.properties[7]/2.0;			// 0=top, 1=center, 2=bottom
		this.wrapbyword       = (this.properties[9] === 0);	// 0=word, 1=character
		this.characterSpacing = this.properties[10];
		this.lineHeight       = this.properties[11];
		this.textWidth  = 0;
		this.textHeight = 0;

		// Use recycled properties to avoid garbage
		if (this.recycled)
		{
			this.lines.length = 0;
			cr.wipe(this.clipList);
			cr.wipe(this.clipUV);
			cr.wipe(this.characterWidthList);
		}
		else
		{
			this.lines = [];
			this.clipList = {};
			this.clipUV = {};
			this.characterWidthList = {};
		}
		
		// only update text if it changes
		this.text_changed = true;
		
		// only update line calculations if this change
		this.lastwrapwidth = this.width;

		if (this.runtime.glwrap)
		{
			// Create WebGL texture if type doesn't have it yet
			if (!this.type.webGL_texture)
			{
				this.type.webGL_texture = this.runtime.glwrap.loadTexture(this.type.texture_img, false, this.runtime.linearSampling, this.type.texture_pixelformat);
			}

			this.webGL_texture = this.type.webGL_texture;
		}

		this.SplitSheet();
	};

	instanceProto.saveToJSON = function ()
	{
		var save = {
			"t": this.text,
			"csc": this.characterScale,
			"csp": this.characterSpacing,
			"lh": this.lineHeight,
			"tw": this.textWidth,
			"th": this.textHeight,
			"lrt": this.last_render_tick,
			"cw": {}
		};

		for (var ch in this.characterWidthList)
			save["cw"][ch] = this.characterWidthList[ch];

		return save;
	};

	instanceProto.loadFromJSON = function (o)
	{
		this.text = o["t"];
		this.characterScale = o["csc"];
		this.characterSpacing = o["csp"];
		this.lineHeight = o["lh"];
		this.textWidth = o["tw"];
		this.textHeight = o["th"];
		this.last_render_tick = o["lrt"];

		for(var ch in o["cw"])
			this.characterWidthList[ch] = o["cw"][ch];

		this.text_changed = true;
		this.lastwrapwidth = this.width;
	};


	function trimRight(text)
	{
		return text.replace(/\s\s*$/, '');
	}

	// return what's in the cache
	// if the cache is empty, return a new object 
	// based on the given Constructor
	var MAX_CACHE_SIZE = 1000;
	function alloc(cache,Constructor)
	{
		if (cache.length)
			return cache.pop();
		else
			return new Constructor();
	}

	// store the data in the cache
	function free(cache,data)
	{
		if (cache.length < MAX_CACHE_SIZE)
		{
			cache.push(data);
		}
	}

	// store all the data from dataList in the cache
	// and wipe dataList
	function freeAll(cache,dataList,isArray)
	{
		if (isArray) {
			var i, len;
			for (i = 0, len = dataList.length; i < len; i++)
			{
				free(cache,dataList[i]);
			}
			dataList.length = 0;
		} else {
			var prop;
			for(prop in dataList) {
				if(Object.prototype.hasOwnProperty.call(dataList,prop)) {
					free(cache,dataList[prop]);
					delete dataList[prop];
				}
			}
		}
	}

	function addLine(inst,lineIndex,cur_line) {
		var lines = inst.lines;
		var line;
		cur_line = trimRight(cur_line);
		// Recycle a line if possible
		if (lineIndex >= lines.length)
			lines.push(allocLine());

		line = lines[lineIndex];
		line.text = cur_line;
		line.width = inst.measureWidth(cur_line);
		inst.textWidth = cr.max(inst.textWidth,line.width);
	}

	var linesCache = [];
	function allocLine()       { return alloc(linesCache,Object); }
	function freeLine(l)       { free(linesCache,l); }
	function freeAllLines(arr) { freeAll(linesCache,arr,true); }


	function addClip(obj,property,x,y,w,h) {
		if (obj[property] === undefined) {
			obj[property] = alloc(clipCache,Object);
		}

		obj[property].x = x;
		obj[property].y = y;
		obj[property].w = w;
		obj[property].h = h;
	}
	var clipCache = [];
	function allocClip()      { return alloc(clipCache,Object); }
	function freeAllClip(obj) { freeAll(clipCache,obj,false);}

	function addClipUV(obj,property,left,top,right,bottom) {
		if (obj[property] === undefined) {
			obj[property] = alloc(clipUVCache,cr.rect);
		}

		obj[property].left   = left;
		obj[property].top    = top;
		obj[property].right  = right;
		obj[property].bottom = bottom;
	}
	var clipUVCache = [];
	function allocClipUV()      { return alloc(clipUVCache,cr.rect);}
	function freeAllClipUV(obj) { freeAll(clipUVCache,obj,false);}


	instanceProto.SplitSheet = function() {
		// Create Clipping regions for each letters of the spritefont sheet
		var texture      = this.texture_img;
		var texWidth     = texture.width;
		var texHeight    = texture.height;
		var charWidth    = this.characterWidth;
		var charHeight   = this.characterHeight;
		var charU        = charWidth /texWidth;
		var charV        = charHeight/texHeight;
		var charSet      = this.characterSet ;

		var cols = Math.floor(texWidth/charWidth);
		var rows = Math.floor(texHeight/charHeight);

		for ( var c = 0; c < charSet.length; c++) {
			// not enough texture space
			if  (c >= cols * rows) break;

			// create clipping coordinates for each characters
			var x = c%cols;
			var y = Math.floor(c/cols);
			var letter = charSet.charAt(c);
			if (this.runtime.glwrap) {
				addClipUV(
					this.clipUV, letter,
					x * charU ,
					y * charV ,
					(x+1) * charU ,
					(y+1) * charV
				);
			} else {
				addClip(
					this.clipList, letter,
					x * charWidth,
					y * charHeight,
					charWidth,
					charHeight
				);
			}
		}
	};

	/*
     *	Word-Wrapping
     */

	var wordsCache = [];
	pluginProto.TokeniseWords = function (text)
	{
		wordsCache.length = 0;
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
	};


	pluginProto.WordWrap = function (inst)
	{
		var text = inst.text;
		var lines = inst.lines;

		if (!text || !text.length)
		{
			freeAllLines(lines);
			return;
		}

		var width = inst.width;
		if (width <= 2.0)
		{
			freeAllLines(lines);
			return;
		}


		// If under 100 characters (i.e. a fairly short string), try a short string optimisation: just measure the text
		// and see if it fits on one line, without going through the tokenise/wrap.
		// Text musn't contain a linebreak!
		var charWidth = inst.characterWidth;
		var charScale = inst.characterScale;
		var charSpacing = inst.characterSpacing;
		if ( (text.length * (charWidth * charScale + charSpacing) - charSpacing) <= width && text.indexOf("\n") === -1)
		{
			var all_width = inst.measureWidth(text);

			if (all_width <= width)
			{
				// fits on one line
				freeAllLines(lines);
				lines.push(allocLine());
				lines[0].text = text;
				lines[0].width = all_width;
				inst.textWidth  = all_width;
				inst.textHeight = inst.characterHeight * charScale + inst.lineHeight;
				return;
			}
		}

		var wrapbyword = inst.wrapbyword;

		this.WrapText(inst);
		inst.textHeight = lines.length * (inst.characterHeight * charScale + inst.lineHeight);
	};

	pluginProto.WrapText = function (inst)
	{
		var wrapbyword = inst.wrapbyword;
		var text       = inst.text;
		var lines      = inst.lines;
		var width      = inst.width;

		var wordArray;
		if (wrapbyword) {
			this.TokeniseWords(text);	// writes to wordsCache
			wordArray = wordsCache;
		} else {
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
					addLine(inst,lineIndex,cur_line);
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
					addLine(inst,lineIndex,cur_line);
					cur_line = "";
					ignore_newline = true;
				} else {
					// else we push the previous line
					addLine(inst,lineIndex,prev_line);
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
			addLine(inst,lineIndex,cur_line);

			lineIndex++;
		}

		// truncate lines to the number that were used. recycle any spare line objects
		for (i = lineIndex; i < lines.length; i++)
			freeLine(lines[i]);

		lines.length = lineIndex;
	};

	instanceProto.measureWidth = function(text) {
		var spacing = this.characterSpacing;
		var len     = text.length;
		var width   = 0;
		for (var i = 0; i < len; i++) {
			width += this.getCharacterWidth(text.charAt(i)) * this.characterScale + spacing;
		}
		// we remove the trailing spacing
		width -= (width > 0) ? spacing : 0;
		return width;
	};

	/***/


	instanceProto.getCharacterWidth = function(character) {
		var widthList = this.characterWidthList;
		if (widthList[character] !== undefined) {
			// special width
			return widthList[character];
		} else {
			// common width
			return this.characterWidth;
		}
	};

	instanceProto.rebuildText = function() {
		// If text has changed, run the word wrap.
		if (this.text_changed || this.width !== this.lastwrapwidth) {
			this.textWidth = 0;
			this.textHeight = 0;
			this.type.plugin.WordWrap(this);
			this.text_changed = false;
			this.lastwrapwidth = this.width;
		}
	};
    
    // to handle floating point imprecision
	var EPSILON = 0.00001;
	instanceProto.draw = function(ctx, glmode)
	{
		var texture = this.texture_img;
		if (this.text !== "" && texture != null) {

			//console.log("draw");

			this.rebuildText();

			// textWidth and textHeight needs to be calculated here
			// since we can early exit if bounding box is too tiny to draw anything
			// be we would still like to know the dimension of the text according to current width
			if (this.height < this.characterHeight*this.characterScale + this.lineHeight) {
				return;
			}

			ctx.globalAlpha = this.opacity;


			var myx = this.x;
			var myy = this.y;

			if (this.runtime.pixel_rounding)
			{
				myx = Math.round(myx);
				myy = Math.round(myy);
			}
			
			// Viewport dimensions
			var viewLeft = this.layer.viewLeft;
			var viewTop = this.layer.viewTop;
			var viewRight = this.layer.viewRight;
			var viewBottom = this.layer.viewBottom;

			ctx.save();
			ctx.translate(myx, myy);
			ctx.rotate(this.angle);

			// convert alignement properties to some usable values
			// useful parameters
			var ha         = this.halign;
			var va         = this.valign;
			var scale      = this.characterScale;
			var charHeight = this.characterHeight * scale;
			var lineHeight = this.lineHeight;
			var charSpace  = this.characterSpacing;
			var lines = this.lines;
			var textHeight = this.textHeight;
			var letterWidth;

			// we compute the offsets for vertical alignement in object-space
			// but it can't be negative, else it would underflow the boundingbox
			// horizontal alignement is evaluated for each line
			var halign;
			var valign = va * cr.max(0,(this.height - textHeight));

			// we get the position of the top left corner of the bounding box
			var offx = -(this.hotspotX * this.width);
			var offy = -(this.hotspotY * this.height);
			// we add to that any extra offset 
			// for vertical alignement
			offy += valign;

			var drawX ;
			var drawY = offy;
			var roundX, roundY;

			for(var i = 0; i < lines.length; i++) {
				// for horizontal alignement, we need to work line by line
				var line = lines[i].text;
				var len  = lines[i].width;

				// compute horizontal empty space
				// offset drawX according to horizontal alignement
				// indentation could be negative if long word in wrapbyword mode
				halign = ha * cr.max(0,this.width - len);		
				drawX = offx + halign;

				// we round to avoid pixel blurring
				drawY += lineHeight;
				
				// above viewport: skip rendering this line
				if (myy + drawY + charHeight < viewTop)
				{
					drawY += charHeight;
					continue;
				}
			
				for(var j = 0; j < line.length; j++) {

					var letter = line.charAt(j);
					letterWidth = this.getCharacterWidth(letter);
					// we skip unrecognized characters (creates a space)
					var clip = this.clipList[letter];
					
					// still off to the left of the viewport: skip drawing this character
					if (myx + drawX + letterWidth * scale + charSpace < viewLeft)
					{
						drawX += letterWidth * scale + charSpace;
						continue;
					}

					// check if next letter fits in bounding box
					if ( drawX + letterWidth * scale > this.width + EPSILON ) {
						break;
					}

					if (clip !== undefined) {

						roundX = drawX;
						roundY = drawY;
						
						if (this.angle === 0)
						{
							roundX = Math.round(roundX);
							roundY = Math.round(roundY);
						}
						
						ctx.drawImage( this.texture_img,
									 clip.x, clip.y, clip.w, clip.h,
									 roundX,roundY,clip.w*scale,clip.h*scale);
					}

					drawX += letterWidth * scale + charSpace;
					
					// Line extended off viewport to right: skip drawing rest of line
					if (myx + drawX > viewRight)
						break;
				}
				drawY += charHeight;

				// check if next row fits in bounding box and viewport and quit drawing if so
				if (drawY + charHeight + lineHeight > this.height || myy + drawY > viewBottom)
				{
					break;
				}
			}
			ctx.restore();
		}

	};

	// drawingQuad
	var dQuad = new cr.quad();

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

	instanceProto.drawGL = function(glw)
	{
		glw.setTexture(this.webGL_texture);
		glw.setOpacity(this.opacity);

		if (!this.text)
			return;

		// If text has changed, run the word wrap.
		this.rebuildText();

		// textWidth and textHeight needs to be calculated here
		// since we can early exit if bounding box is too tiny to draw anything
		// be we would still like to know the dimension of the text according to current width
		if (this.height < this.characterHeight*this.characterScale + this.lineHeight) {
			return;
		}

		this.update_bbox();
		var q = this.bquad;
		var ox = 0;
		var oy = 0;
		if (this.runtime.pixel_rounding)
		{
			ox = Math.round(this.x) - this.x;
			oy = Math.round(this.y) - this.y;
		}
		
		// Viewport dimensions
		var viewLeft = this.layer.viewLeft;
		var viewTop = this.layer.viewTop;
		var viewRight = this.layer.viewRight;
		var viewBottom = this.layer.viewBottom;

		// convert alignement properties to some usable values
		// useful parameters
		var angle      = this.angle;
		var ha         = this.halign;
		var va         = this.valign;
		var scale      = this.characterScale;
		var charHeight = this.characterHeight * scale;   // to precalculate in onCreate or on change
		var lineHeight = this.lineHeight;
		var charSpace  = this.characterSpacing;
		var lines = this.lines;
		var textHeight = this.textHeight;
		var letterWidth;

		var cosa,sina;
		if (angle !== 0) 
		{
			cosa = Math.cos(angle);
			sina = Math.sin(angle);
		}

		// we compute the offsets for vertical alignement in object-space
		// but it can't be negative, else it would underflow the boundingbox
		var halign;
		var valign = va * cr.max(0,(this.height - textHeight));

		// we get the position of the top left corner of the bounding box
		var offx = q.tlx + ox;
		var offy = q.tly + oy;


		var drawX ;
		var drawY = valign;
		var roundX, roundY;

		for(var i = 0; i < lines.length; i++) {
			// for horizontal alignement, we need to work line by line
			var line       = lines[i].text;
			var lineWidth  = lines[i].width;

			// compute horizontal empty space
			// offset drawX according to horizontal alignement
			// indentation could be negative if long word in wrapbyword mode
			halign = ha * cr.max(0,this.width - lineWidth);
			//halign = Math.floor(ha * cr.max(0,this.width - lineWidth));
			drawX = halign;

			// we round to avoid pixel blurring
			drawY += lineHeight;
			
			// above viewport: skip rendering this line
			if (angle === 0 && offy + drawY + charHeight < viewTop)
			{
				drawY += charHeight;
				continue;
			}
			
			for(var j = 0; j < line.length; j++) {

				var letter = line.charAt(j);
				letterWidth = this.getCharacterWidth(letter);
				var clipUV = this.clipUV[letter];
				
				// still off to the left of the viewport: skip drawing this character
				if (offx + drawX + letterWidth * scale + charSpace < viewLeft)
				{
					drawX += letterWidth * scale + charSpace;
					continue;
				}

				// check if next letter fits in bounding box
				if (drawX + letterWidth * scale > this.width + EPSILON)
				{
					break;
				}

				// we skip unrecognized characters (creates a space)
				if (clipUV !== undefined) {
					var clipWidth  = this.characterWidth*scale;
					var clipHeight = this.characterHeight*scale;

					roundX = drawX;
					roundY = drawY;
					
					if (angle === 0)
					{
						roundX = Math.round(roundX);
						roundY = Math.round(roundY);
					}
					
					// we build the quad
					dQuad.tlx  = roundX;
					dQuad.tly  = roundY;
					dQuad.trx  = roundX + clipWidth;
					dQuad.try_ = roundY ;
					dQuad.blx  = roundX;
					dQuad.bly  = roundY + clipHeight;
					dQuad.brx  = roundX + clipWidth;
					dQuad.bry  = roundY + clipHeight;

					// we then rotate the quad around 0,0
					// if necessary
					if(angle !== 0)
					{
						rotateQuad(dQuad,cosa,sina);
					}
					// we then apply the world space offset
					dQuad.offset(offx,offy);

					// and render
					glw.quadTex(
						dQuad.tlx, dQuad.tly,
						dQuad.trx, dQuad.try_,
						dQuad.brx, dQuad.bry,
						dQuad.blx, dQuad.bly,
						clipUV
					);
				}

				drawX += letterWidth * scale + charSpace;
				
				// Line extended off viewport to right: skip drawing rest of line
				if (angle === 0 && offx + drawX > viewRight)
					break;
			}
			
			drawY += charHeight;
			
			// check if next row fits in bounding box and viewport and quit drawing if so
			if (drawY + charHeight + lineHeight > this.height || offy + drawY > viewBottom)
			{
				break;
			}
		}

	};
	
	/**BEGIN-PREVIEWONLY**/
	instanceProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": "Sprite font",
			"properties": [
				{"name": "Text", "value": this.text},
				{"name": "Character width", "value": this.characterWidth},
				{"name": "Character height", "value": this.characterHeight},
				{"name": "Character scale", "value": this.characterScale},
				{"name": "Character spacing", "value": this.characterSpacing},
				{"name": "Line height", "value": this.lineHeight}
			]
		});
	};
	
	instanceProto.onDebugValueEdited = function (header, name, value)
	{
		switch (name) {
		case "Text":
			this.text = value;
			break;
		case "Character width":
			this.characterWidth = value;
			break;
		case "Character height":
			this.characterHeight = value;
			break;
		case "Character scale":
			this.characterScale = value;
			break;
		case "Character spacing":
			this.characterSpacing = value;
			break;
		case "Line height":
			this.lineHeight = value;
			break;
		}
		
		this.text_changed = true;
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {}

	Cnds.prototype.CompareText = function(text_to_compare, case_sensitive)
	{
		if (case_sensitive)
			return this.text == text_to_compare;
		else
			return cr.equals_nocase(this.text, text_to_compare);
	};

	pluginProto.cnds = new Cnds();

	//////////////////////////////////////
	// Actions
	function Acts() {}

	Acts.prototype.SetText = function(param)
	{
		if (cr.is_number(param) && param < 1e9)
			param = Math.round(param * 1e10) / 1e10;	// round to nearest ten billionth - hides floating point errors

		var text_to_set = param.toString();

		if (this.text !== text_to_set)
		{
			this.text = text_to_set;
			this.text_changed = true;
			this.runtime.redraw = true;
		}
	};

	Acts.prototype.AppendText = function(param)
	{
		if (cr.is_number(param))
			param = Math.round(param * 1e10) / 1e10;	// round to nearest ten billionth - hides floating point errors

		var text_to_append = param.toString();

		if (text_to_append)	// not empty
		{
			this.text += text_to_append;
			this.text_changed = true;
			this.runtime.redraw = true;
		}
	};

	Acts.prototype.SetScale = function(param)
	{
		if (param !== this.characterScale) {
			this.characterScale = param;
			this.text_changed = true;
			this.runtime.redraw = true;
		}
	};

	Acts.prototype.SetCharacterSpacing = function(param)
	{
		if (param !== this.CharacterSpacing) {
			this.characterSpacing = param;
			this.text_changed = true;
			this.runtime.redraw = true;
		}
	};

	Acts.prototype.SetLineHeight = function(param)
	{
		if (param !== this.lineHeight) {
			this.lineHeight = param;
			this.text_changed = true;
			this.runtime.redraw = true;
		}
	};

	instanceProto.SetCharWidth = function(character,width) {
		var w = parseInt(width,10);
		if (this.characterWidthList[character] !== w) {
			this.characterWidthList[character] = w;
			this.text_changed = true;
			this.runtime.redraw = true;
		}
	};

	Acts.prototype.SetCharacterWidth = function(characterSet,width)
	{
		if (characterSet !== "") {
			for(var c = 0; c < characterSet.length; c++) {
				this.SetCharWidth(characterSet.charAt(c),width);
			}
		}
	};
	
	Acts.prototype.SetEffect = function (effect)
	{
		this.compositeOp = cr.effectToCompositeOp(effect);
		cr.setGLBlend(this, effect, this.runtime.gl);
		this.runtime.redraw = true;
	};
	
	Acts.prototype.SetHAlign = function (a)
	{
		this.halign = a / 2.0;
		this.text_changed = true;
		this.runtime.redraw = true;
	};
	
	Acts.prototype.SetVAlign = function (a)
	{
		this.valign = a / 2.0;
		this.text_changed = true;
		this.runtime.redraw = true;
	};

	pluginProto.acts = new Acts();

	//////////////////////////////////////
	// Expressions
	function Exps() {}

	Exps.prototype.CharacterWidth = function(ret,character)
	{
		ret.set_int(this.getCharacterWidth(character));
	};

	Exps.prototype.CharacterHeight = function(ret)
	{
		ret.set_int(this.characterHeight);
	};

	Exps.prototype.CharacterScale = function(ret)
	{
		ret.set_float(this.characterScale);
	};

	Exps.prototype.CharacterSpacing = function(ret)
	{
		ret.set_int(this.characterSpacing);
	};

	Exps.prototype.LineHeight = function(ret)
	{
		ret.set_int(this.lineHeight);
	};

	Exps.prototype.Text = function(ret)
	{
		ret.set_string(this.text);
	};
	Exps.prototype.TextWidth = function (ret)
	{
		this.rebuildText();
		ret.set_float(this.textWidth);
	};

	Exps.prototype.TextHeight = function (ret)
	{
		this.rebuildText();
		ret.set_float(this.textHeight);
	};

	pluginProto.exps = new Exps();

}());