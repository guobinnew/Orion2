// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.TiledBg = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var pluginProto = cr.plugins_.TiledBg.prototype;
		
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
		
		this.pattern = null;
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
			this.webGL_texture = this.runtime.glwrap.loadTexture(this.texture_img, true, this.runtime.linearSampling, this.texture_pixelformat);
		}
		
		var i, len;
		for (i = 0, len = this.instances.length; i < len; i++)
			this.instances[i].webGL_texture = this.webGL_texture;
	};
	
	typeProto.loadTextures = function ()
	{
		if (this.is_family || this.webGL_texture || !this.runtime.glwrap)
			return;
			
		this.webGL_texture = this.runtime.glwrap.loadTexture(this.texture_img, true, this.runtime.linearSampling, this.texture_pixelformat);
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

	instanceProto.onCreate = function()
	{
		this.visible = (this.properties[0] === 0);							// 0=visible, 1=invisible
		this.rcTex = new cr.rect(0, 0, 0, 0);
		
		this.has_own_texture = false;										// true if a texture loaded in from URL
		this.texture_img = this.type.texture_img;
		
		if (this.runtime.glwrap)
		{
			// Create WebGL texture if type doesn't have it yet
			this.type.loadTextures();
			
			this.webGL_texture = this.type.webGL_texture;
		}
		else
		{
			// Create the pattern if the type doesn't have one yet
			if (!this.type.pattern)
				this.type.pattern = this.runtime.ctx.createPattern(this.type.texture_img, "repeat");
			
			this.pattern = this.type.pattern;
		}
	};
	
	instanceProto.afterLoad = function ()
	{
		// If has own texture, reset to normal texture since the texture isn't saved with the state
		this.has_own_texture = false;
		this.texture_img = this.type.texture_img;
	};
	
	instanceProto.onDestroy = function ()
	{
		// WebGL renderer: if we have our own texture (via load from URL), delete it now
		if (this.runtime.glwrap && this.has_own_texture && this.webGL_texture)
		{
			this.runtime.glwrap.deleteTexture(this.webGL_texture);
			this.webGL_texture = null;
		}
	};

	instanceProto.draw = function(ctx)
	{
		ctx.globalAlpha = this.opacity;
			
		ctx.save();
		
		ctx.fillStyle = this.pattern;
		
		var myx = this.x;
		var myy = this.y;
		
		if (this.runtime.pixel_rounding)
		{
			myx = Math.round(myx);
			myy = Math.round(myy);
		}
		
		// Patterns tile from the origin no matter where you draw from.
		// Translate the canvas to align with the draw position, then offset the draw position as well.
		var drawX = -(this.hotspotX * this.width);
		var drawY = -(this.hotspotY * this.height);
		
		var offX = drawX % this.texture_img.width;
		var offY = drawY % this.texture_img.height;
		if (offX < 0)
			offX += this.texture_img.width;
		if (offY < 0)
			offY += this.texture_img.height;
			
		ctx.translate(myx, myy);
		ctx.rotate(this.angle);
		ctx.translate(offX, offY);
		
		ctx.fillRect(drawX - offX,
					 drawY - offY,
					 this.width,
					 this.height);
		
		ctx.restore();
	};
	
	instanceProto.drawGL = function(glw)
	{
		glw.setTexture(this.webGL_texture);
		glw.setOpacity(this.opacity);
		
		var rcTex = this.rcTex;
		rcTex.right = this.width / this.texture_img.width;
		rcTex.bottom = this.height / this.texture_img.height;
		
		var q = this.bquad;
		
		if (this.runtime.pixel_rounding)
		{
			var ox = Math.round(this.x) - this.x;
			var oy = Math.round(this.y) - this.y;
			
			glw.quadTex(q.tlx + ox, q.tly + oy, q.trx + ox, q.try_ + oy, q.brx + ox, q.bry + oy, q.blx + ox, q.bly + oy, rcTex);
		}
		else
			glw.quadTex(q.tlx, q.tly, q.trx, q.try_, q.brx, q.bry, q.blx, q.bly, rcTex);
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
	
	Acts.prototype.LoadURL = function (url_)
	{
		var img = new Image();
		var self = this;
		
		img.onload = function ()
		{
			self.texture_img = img;
			
			// WebGL renderer
			if (self.runtime.glwrap)
			{
				// Delete any previous own texture
				if (self.has_own_texture && self.webGL_texture)
					self.runtime.glwrap.deleteTexture(self.webGL_texture);
					
				self.webGL_texture = self.runtime.glwrap.loadTexture(img, true, self.runtime.linearSampling);
			}
			// Canvas2D renderer
			else
			{
				self.pattern = self.runtime.ctx.createPattern(img, "repeat");
			}
			
			self.has_own_texture = true;
			self.runtime.redraw = true;
			self.runtime.trigger(cr.plugins_.TiledBg.prototype.cnds.OnURLLoaded, self);
		};
		
		if (url_.substr(0, 5) !== "data:")
			img.crossOrigin = 'anonymous';
		
		img.src = url_;
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.ImageWidth = function (ret)
	{
		ret.set_float(this.texture_img.width);
	};
	
	Exps.prototype.ImageHeight = function (ret)
	{
		ret.set_float(this.texture_img.height);
	};
	
	pluginProto.exps = new Exps();

}());