// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.NinePatch = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var pluginProto = cr.plugins_.NinePatch.prototype;
		
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
		this.texture_img.cr_filesize = this.texture_filesize;
		
		// Tell runtime to wait for this to load
		this.runtime.waitForImageLoad(this.texture_img, this.texture_file);
		
		this.fillPattern = null;
		this.leftPattern = null;
		this.rightPattern = null;
		this.topPattern = null;
		this.bottomPattern = null;
		
		this.webGL_texture = null;
		this.webGL_fillTexture = null;
		this.webGL_leftTexture = null;
		this.webGL_rightTexture = null;
		this.webGL_topTexture = null;
		this.webGL_bottomTexture = null;
	};
	
	typeProto.onLostWebGLContext = function ()
	{
		if (this.is_family)
			return;
			
		this.webGL_texture = null;
		this.webGL_fillTexture = null;
		this.webGL_leftTexture = null;
		this.webGL_rightTexture = null;
		this.webGL_topTexture = null;
		this.webGL_bottomTexture = null;
	};
	
	typeProto.onRestoreWebGLContext = function ()
	{
		// No need to create textures if no instances exist, will create on demand
		if (this.is_family || !this.instances.length)
			return;
		
		if (!this.webGL_texture)
		{
			this.webGL_texture = this.runtime.glwrap.loadTexture(this.texture_img, true, this.runtime.linearSampling, this.texture_pixelformat);
		}
	};
	
	typeProto.unloadTextures = function ()
	{
		// Don't release textures if any instances still exist, they are probably using them
		if (this.is_family || this.instances.length)
			return;
		
		// WebGL renderer
		if (this.runtime.glwrap)
		{
			this.runtime.glwrap.deleteTexture(this.webGL_texture);
			this.runtime.glwrap.deleteTexture(this.webGL_fillTexture);
			this.runtime.glwrap.deleteTexture(this.webGL_leftTexture);
			this.runtime.glwrap.deleteTexture(this.webGL_rightTexture);
			this.runtime.glwrap.deleteTexture(this.webGL_topTexture);
			this.runtime.glwrap.deleteTexture(this.webGL_bottomTexture);
			
			this.webGL_texture = null;
			this.webGL_fillTexture = null;
			this.webGL_leftTexture = null;
			this.webGL_rightTexture = null;
			this.webGL_topTexture = null;
			this.webGL_bottomTexture = null;
		}
	};
	
	typeProto.slicePatch = function (x1, y1, x2, y2)
	{
		var tmpcanvas = document.createElement("canvas");
		var w = x2 - x1;
		var h = y2 - y1;
		tmpcanvas.width = w;
		tmpcanvas.height = h;
		var tmpctx = tmpcanvas.getContext("2d");
		tmpctx.drawImage(this.texture_img, x1, y1, w, h, 0, 0, w, h);
		return tmpcanvas;
	};
	
	typeProto.createPatch = function (lm, rm, tm, bm)
	{
		var iw = this.texture_img.width;
		var ih = this.texture_img.height;
		var re = iw - rm;
		var be = ih - bm;
		
		if (this.runtime.glwrap)
		{
			if (this.webGL_fillTexture)
				return;		// already created
				
			var glwrap = this.runtime.glwrap;
			var ls = this.runtime.linearSampling;
			var tf = this.texture_pixelformat;
			
			if (re > lm && be > tm)
				this.webGL_fillTexture = glwrap.loadTexture(this.slicePatch(lm, tm, re, be), true, ls, tf);
			
			if (lm > 0 && be > tm)
				this.webGL_leftTexture = glwrap.loadTexture(this.slicePatch(0, tm, lm, be), true, ls, tf, "repeat-y");
			if (rm > 0 && be > tm)
				this.webGL_rightTexture = glwrap.loadTexture(this.slicePatch(re, tm, iw, be), true, ls, tf, "repeat-y");
			if (tm > 0 && re > lm)
				this.webGL_topTexture = glwrap.loadTexture(this.slicePatch(lm, 0, re, tm), true, ls, tf, "repeat-x");
			if (bm > 0 && re > lm)
				this.webGL_bottomTexture = glwrap.loadTexture(this.slicePatch(lm, be, re, ih), true, ls, tf, "repeat-x");
		}
		else
		{
			if (this.fillPattern)
				return;		// already created
			
			var ctx = this.runtime.ctx;
			
			if (re > lm && be > tm)
				this.fillPattern = ctx.createPattern(this.slicePatch(lm, tm, re, be), "repeat");
			
			if (lm > 0 && be > tm)
				this.leftPattern = ctx.createPattern(this.slicePatch(0, tm, lm, be), "repeat");
			if (rm > 0 && be > tm)
				this.rightPattern = ctx.createPattern(this.slicePatch(re, tm, iw, be), "repeat");
			if (tm > 0 && re > lm)
				this.topPattern = ctx.createPattern(this.slicePatch(lm, 0, re, tm), "repeat");
			if (bm > 0 && re > lm)
				this.bottomPattern = ctx.createPattern(this.slicePatch(lm, be, re, ih), "repeat");
		}
	};

	/////////////////////////////////////
	// Instance class
	pluginProto.Instance = function(type)
	{
		this.type = type;
		this.runtime = type.runtime;
	};
	
	var instanceProto = pluginProto.Instance.prototype;

	instanceProto.onCreate = function()
	{
		this.leftMargin = this.properties[0];
		this.rightMargin = this.properties[1];
		this.topMargin = this.properties[2];
		this.bottomMargin = this.properties[3];
		this.edges = this.properties[4];					// 0=tile, 1=stretch
		this.fill = this.properties[5];						// 0=tile, 1=stretch, 2=transparent
		this.visible = (this.properties[6] === 0);			// 0=visible, 1=invisible
		this.seamless = (this.properties[8] !== 0);			// 1px overdraw to hide seams
		
		if (this.recycled)
			this.rcTex.set(0, 0, 0, 0);
		else
			this.rcTex = new cr.rect(0, 0, 0, 0);
		
		if (this.runtime.glwrap)
		{
			// Create WebGL texture if type doesn't have it yet
			if (!this.type.webGL_texture)
			{
				this.type.webGL_texture = this.runtime.glwrap.loadTexture(this.type.texture_img, false, this.runtime.linearSampling, this.type.texture_pixelformat);
			}
		}
		
		// lazy create patches
		this.type.createPatch(this.leftMargin, this.rightMargin, this.topMargin, this.bottomMargin);
	};
	
	// patterns tile their image content relative to the origin, which is really
	// annoying for drawing tiled content.  Work around to tile from draw co-ordinates
	// using translate.
	function drawPatternProperly(ctx, pattern, pw, ph, drawX, drawY, w, h, ox, oy)
	{
		ctx.save();
		ctx.fillStyle = pattern;
		
		var offX = drawX % pw;
		var offY = drawY % ph;
		if (offX < 0)
			offX += pw;
		if (offY < 0)
			offY += ph;
			
		ctx.translate(offX + ox, offY + oy);
		
		ctx.fillRect(drawX - offX - ox, drawY - offY - oy, w, h);
		
		ctx.restore();
	};

	instanceProto.draw = function(ctx)
	{
		var img = this.type.texture_img;
		var lm = this.leftMargin;
		var rm = this.rightMargin;
		var tm = this.topMargin;
		var bm = this.bottomMargin;
		var iw = img.width;
		var ih = img.height;
		var re = iw - rm;
		var be = ih - bm;
		
		ctx.globalAlpha = this.opacity;
			
		ctx.save();
		
		var myx = this.x;
		var myy = this.y;
		var myw = this.width;
		var myh = this.height;
		
		if (this.runtime.pixel_rounding)
		{
			myx = Math.round(myx);
			myy = Math.round(myy);
		}
		
		// Patterns tile from the origin no matter where you draw from.
		// Translate the canvas to align with the draw position, then offset the draw position as well.
		var drawX = -(this.hotspotX * this.width);
		var drawY = -(this.hotspotY * this.height);
		
		var offX = drawX % iw;
		var offY = drawY % ih;
		if (offX < 0)
			offX += iw;
		if (offY < 0)
			offY += ih;
			
		ctx.translate(myx + offX, myy + offY);
		
		var x = drawX - offX;
		var y = drawY - offY;
		
		var s = (this.seamless ? 1 : 0);
		
		// draw top-left, top-right, bottom-right and bottom-left corners
		// include an extra 1px from source image to avoid seams
		if (lm > 0 && tm > 0)
			ctx.drawImage(img, 0, 0, lm + s, tm + s, x, y, lm + s, tm + s);
			
		if (rm > 0 && tm > 0)
			ctx.drawImage(img, re - s, 0, rm + s, tm + s, x + myw - rm - s, y, rm + s, tm + s);
			
		if (rm > 0 && bm > 0)
			ctx.drawImage(img, re - s, be - s, rm + s, bm + s, x + myw - rm - s, y + myh - bm - s, rm + s, bm + s);
			
		if (lm > 0 && bm > 0)
			ctx.drawImage(img, 0, be - s, lm + s, bm + s, x, y + myh - bm - s, lm + s, bm + s);
		
		// draw edges, overlapping in to fill area by 1px to avoid seams unless fill is transparent
		if (this.edges === 0)		// tile edges
		{
			var off = (this.fill === 2 ? 0 : s);
			
			if (lm > 0 && be > tm)
				drawPatternProperly(ctx, this.type.leftPattern, lm, be - tm, x, y + tm, lm + off, myh - tm - bm, 0, 0);
				
			if (rm > 0 && be > tm)
				drawPatternProperly(ctx, this.type.rightPattern, rm, be - tm, x + myw - rm - off, y + tm, rm + off, myh - tm - bm, off, 0);
				
			if (tm > 0 && re > lm)
				drawPatternProperly(ctx, this.type.topPattern, re - lm, tm, x + lm, y, myw - lm - rm, tm + off, 0, 0);
				
			if (bm > 0 && re > lm)
				drawPatternProperly(ctx, this.type.bottomPattern, re - lm, bm, x + lm, y + myh - bm - off, myw - lm - rm, bm + off, 0, off);
		}
		else if (this.edges === 1)	// stretch edges
		{
			if (lm > 0 && be > tm && myh - tm - bm > 0)
				ctx.drawImage(img, 0, tm, lm, be - tm, x, y + tm, lm, myh - tm - bm);
				
			if (rm > 0 && be > tm && myh - tm - bm > 0)
				ctx.drawImage(img, re, tm, rm, be - tm, x + myw - rm, y + tm, rm, myh - tm - bm);
				
			if (tm > 0 && re > lm && myw - lm - rm > 0)
				ctx.drawImage(img, lm, 0, re - lm, tm, x + lm, y, myw - lm - rm, tm);
				
			if (bm > 0 && re > lm && myw - lm - rm > 0)
				ctx.drawImage(img, lm, be, re - lm, bm, x + lm, y + myh - bm, myw - lm - rm, bm);
		}
		
		// fill
		if (be > tm && re > lm)
		{
			if (this.fill === 0)		// tile fill
			{
				drawPatternProperly(ctx, this.type.fillPattern, re - lm, be - tm, x + lm, y + tm, myw - lm - rm, myh - tm - bm, 0, 0);
			}
			else if (this.fill === 1)	// stretch fill
			{
				if (myw - lm - rm > 0 && myh - tm - bm > 0)
				{
					ctx.drawImage(img, lm, tm, re - lm, be - tm, x + lm, y + tm, myw - lm - rm, myh - tm - bm);
				}
			}
			// else fill is 2, meaning transparent - don't draw
		}
		
		ctx.restore();
	};
	
	instanceProto.drawPatch = function(glw, tex, sx, sy, sw, sh, dx, dy, dw, dh)
	{
		glw.setTexture(tex);
		var rcTex = this.rcTex;
		rcTex.left = sx / tex.c2width;
		rcTex.top = sy / tex.c2height;
		rcTex.right = (sx + sw) / tex.c2width;
		rcTex.bottom = (sy + sh) / tex.c2height;
		glw.quadTex(dx, dy, dx + dw, dy, dx + dw, dy + dh, dx, dy + dh, rcTex);
	};
	
	instanceProto.tilePatch = function(glw, tex, dx, dy, dw, dh, ox, oy)
	{
		glw.setTexture(tex);
		var rcTex = this.rcTex;
		rcTex.left = -ox / tex.c2width;
		rcTex.top = -oy / tex.c2height;
		rcTex.right = (dw - ox) / tex.c2width;
		rcTex.bottom = (dh - oy) / tex.c2height;
		glw.quadTex(dx, dy, dx + dw, dy, dx + dw, dy + dh, dx, dy + dh, rcTex);
	};
	
	instanceProto.drawGL = function(glw)
	{
		var lm = this.leftMargin;
		var rm = this.rightMargin;
		var tm = this.topMargin;
		var bm = this.bottomMargin;
		var iw = this.type.texture_img.width;
		var ih = this.type.texture_img.height;
		var re = iw - rm;
		var be = ih - bm;
		
		glw.setOpacity(this.opacity);
		
		var rcTex = this.rcTex;
		var q = this.bquad;
		var myx = q.tlx;
		var myy = q.tly;
		var myw = this.width;
		var myh = this.height;
		
		if (this.runtime.pixel_rounding)
		{
			myx = Math.round(myx);
			myy = Math.round(myy);
		}
		
		var s = (this.seamless ? 1 : 0);
		
		// draw corners
		if (lm > 0 && tm > 0)
			this.drawPatch(glw, this.type.webGL_texture, 0, 0, lm + s, tm + s, myx, myy, lm + s, tm + s);
		
		if (rm > 0 && tm > 0)
			this.drawPatch(glw, this.type.webGL_texture, re - s, 0, rm + s, tm + s, myx + myw - rm - s, myy, rm + s, tm + s);
			
		if (rm > 0 && bm > 0)
			this.drawPatch(glw, this.type.webGL_texture, re - s, be - s, rm + s, bm + s, myx + myw - rm - s, myy + myh - bm - s, rm + s, bm + s);
			
		if (lm > 0 && bm > 0)
			this.drawPatch(glw, this.type.webGL_texture, 0, be - s, lm + s, bm + s, myx, myy + myh - bm - s, lm + s, bm + s);
		
		// draw edges
		if (this.edges === 0)		// tile edges
		{
			var off = (this.fill === 2 ? 0 : s);
			
			if (lm > 0 && be > tm)
				this.tilePatch(glw, this.type.webGL_leftTexture, myx, myy + tm, lm + off, myh - tm - bm, 0, 0);
			if (rm > 0 && be > tm)
				this.tilePatch(glw, this.type.webGL_rightTexture, myx + myw - rm - off, myy + tm, rm + off, myh - tm - bm, off, 0);
			if (tm > 0 && re > lm)
				this.tilePatch(glw, this.type.webGL_topTexture, myx + lm, myy, myw - lm - rm, tm + off, 0, 0);
			if (bm > 0 && re > lm)
				this.tilePatch(glw, this.type.webGL_bottomTexture, myx + lm, myy + myh - bm - off, myw - lm - rm, bm + off, 0, off);
		}
		else if (this.edges === 1)	// stretch edges
		{
			if (lm > 0 && be > tm)
				this.drawPatch(glw, this.type.webGL_texture, 0, tm, lm, be - tm, myx, myy + tm, lm, myh - tm - bm);
			if (rm > 0 && be > tm)
				this.drawPatch(glw, this.type.webGL_texture, re, tm, rm, be - tm, myx + myw - rm, myy + tm, rm, myh - tm - bm);
			if (tm > 0 && re > lm)
				this.drawPatch(glw, this.type.webGL_texture, lm, 0, re - lm, tm, myx + lm, myy, myw - lm - rm, tm);
			if (bm > 0 && re > lm)
				this.drawPatch(glw, this.type.webGL_texture, lm, be, re - lm, bm, myx + lm, myy + myh - bm, myw - lm - rm, bm);
		}
		
		if (be > tm && re > lm)
		{
			if (this.fill === 0)		// tile fill
			{
				this.tilePatch(glw, this.type.webGL_fillTexture, myx + lm, myy + tm, myw - lm - rm, myh - tm - bm, 0, 0);
			}
			else if (this.fill === 1)	// stretch fill
			{
				this.drawPatch(glw, this.type.webGL_texture, lm, tm, re - lm, be - tm, myx + lm, myy + tm, myw - lm - rm, myh - tm - bm);
			}
			// else fill is 2, meaning transparent - don't draw
		}
	};

	//////////////////////////////////////
	// Conditions
	function Cnds() {};
	
	Cnds.prototype.OnURLLoaded = function ()
	{
		return true;
	};
	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.SetEffect = function (effect)
	{	
		this.compositeOp = cr.effectToCompositeOp(effect);
		cr.setGLBlend(this, effect, this.runtime.gl);
		this.runtime.redraw = true;
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	pluginProto.exps = new Exps();

}());