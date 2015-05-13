// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.Particles = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	/////////////////////////////////////
	var pluginProto = cr.plugins_.Particles.prototype;
		
	/////////////////////////////////////
	// Object type class
	pluginProto.Type = function(plugin)
	{
		this.plugin = plugin;
		this.runtime = plugin.runtime;
	};

	var typeProto = pluginProto.Type.prototype;

	// called on startup for each object type
	typeProto.onCreate = function()
	{
		if (this.is_family)
			return;
			
		// Create the texture
		this.texture_img = new Image();
		this.texture_img.cr_filesize = this.texture_filesize;
		this.webGL_texture = null;
		
		// Tell runtime to wait for this to load
		this.runtime.waitForImageLoad(this.texture_img, this.texture_file);
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
	};
	
	typeProto.loadTextures = function ()
	{
		if (this.is_family || this.webGL_texture || !this.runtime.glwrap)
			return;
		
		this.webGL_texture = this.runtime.glwrap.loadTexture(this.texture_img, true, this.runtime.linearSampling, this.texture_pixelformat);
	};
	
	typeProto.unloadTextures = function ()
	{
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
	// Particle class
	function Particle(owner)
	{
		this.owner = owner;
		this.active = false;
		this.x = 0;
		this.y = 0;
		this.speed = 0;
		this.angle = 0;
		this.opacity = 1;
		this.grow = 0;
		this.size = 0;
		this.gs = 0;			// gravity speed
		this.age = 0;
		cr.seal(this);
	};
	
	Particle.prototype.init = function ()
	{
		var owner = this.owner;
		this.x = owner.x - (owner.xrandom / 2) + (Math.random() * owner.xrandom);
		this.y = owner.y - (owner.yrandom / 2) + (Math.random() * owner.yrandom);
		
		this.speed = owner.initspeed - (owner.speedrandom / 2) + (Math.random() * owner.speedrandom);
		this.angle = owner.angle - (owner.spraycone / 2) + (Math.random() * owner.spraycone);
		this.opacity = owner.initopacity;
		this.size = owner.initsize - (owner.sizerandom / 2) + (Math.random() * owner.sizerandom);
		this.grow = owner.growrate - (owner.growrandom / 2) + (Math.random() * owner.growrandom);
		this.gs = 0;
		this.age = 0;
	};
	
	Particle.prototype.tick = function (dt)
	{
		var owner = this.owner;
		
		// Move
		this.x += Math.cos(this.angle) * this.speed * dt;
		this.y += Math.sin(this.angle) * this.speed * dt;
		
		// Apply gravity
		this.y += this.gs * dt;
		
		// Adjust lifetime parameters
		this.speed += owner.acc * dt;
		this.size += this.grow * dt;
		this.gs += owner.g * dt;
		this.age += dt;
		
		// Destroy particle if shrunk to less than a pixel in size
		if (this.size < 1)
		{
			this.active = false;
			return;
		}
		
		if (owner.lifeanglerandom !== 0)
			this.angle += (Math.random() * owner.lifeanglerandom * dt) - (owner.lifeanglerandom * dt / 2);
			
		if (owner.lifespeedrandom !== 0)
			this.speed += (Math.random() * owner.lifespeedrandom * dt) - (owner.lifespeedrandom * dt / 2);
			
		if (owner.lifeopacityrandom !== 0)
		{
			this.opacity += (Math.random() * owner.lifeopacityrandom * dt) - (owner.lifeopacityrandom * dt / 2);
			
			if (this.opacity < 0)
				this.opacity = 0;
			else if (this.opacity > 1)
				this.opacity = 1;
		}
		
		// Make inactive after timeout for both fade and timeout settings
		if (owner.destroymode <= 1 && this.age >= owner.timeout)
		{
			this.active = false;
		}
		// Or make inactive when stopped
		if (owner.destroymode === 2 && this.speed <= 0)
		{
			this.active = false;
		}
	};
	
	Particle.prototype.draw = function (ctx)
	{
		var curopacity = this.owner.opacity * this.opacity;
		
		if (curopacity === 0)
			return;
		
		// Modify opacity for fade-out
		if (this.owner.destroymode === 0)
			curopacity *= 1 - (this.age / this.owner.timeout);
			
		ctx.globalAlpha = curopacity;
			
		var drawx = this.x - this.size / 2;
		var drawy = this.y - this.size / 2;
		
		if (this.owner.runtime.pixel_rounding)
		{
			drawx = (drawx + 0.5) | 0;
			drawy = (drawy + 0.5) | 0;
		}
		
		ctx.drawImage(this.owner.type.texture_img, drawx, drawy, this.size, this.size);
	};
	
	Particle.prototype.drawGL = function (glw)
	{
		var curopacity = this.owner.opacity * this.opacity;
		
		// Modify opacity for fade-out
		if (this.owner.destroymode === 0)
			curopacity *= 1 - (this.age / this.owner.timeout);
		
		var drawsize = this.size;
		var scaleddrawsize = drawsize * this.owner.particlescale;
		var drawx = this.x - drawsize / 2;
		var drawy = this.y - drawsize / 2;
		
		if (this.owner.runtime.pixel_rounding)
		{
			drawx = (drawx + 0.5) | 0;
			drawy = (drawy + 0.5) | 0;
		}
		
		// Don't bother issuing a quad for a particle smaller than 1px, it probably won't be visible anyway.
		if (scaleddrawsize < 1 || curopacity === 0)
			return;
			
		// Quad if outside the allowed point range, otherwise issue a point.  Hopefully there won't be too much
		// quad <-> point batch switching.  Note we have to manually scale particles which don't take in to account
		// the layout zoom etc. otherwise.
		if (scaleddrawsize < glw.minPointSize || scaleddrawsize > glw.maxPointSize)
		{
			glw.setOpacity(curopacity);
			glw.quad(drawx, drawy, drawx + drawsize, drawy, drawx + drawsize, drawy + drawsize, drawx, drawy + drawsize);
		}
		else
			glw.point(this.x, this.y, scaleddrawsize, curopacity);
	};
	
	Particle.prototype.left = function ()
	{
		return this.x - this.size / 2;
	};
	
	Particle.prototype.right = function ()
	{
		return this.x + this.size / 2;
	};
	
	Particle.prototype.top = function ()
	{
		return this.y - this.size / 2;
	};
	
	Particle.prototype.bottom = function ()
	{
		return this.y + this.size / 2;
	};

	/////////////////////////////////////
	// Instance class
	pluginProto.Instance = function(type)
	{
		this.type = type;
		this.runtime = type.runtime;
	};
	
	var instanceProto = pluginProto.Instance.prototype;
	
	// global array of particles to recycle
	var deadparticles = [];

	// called whenever an instance is created
	instanceProto.onCreate = function()
	{
		var props = this.properties;
		
		this.rate = props[0];
		this.spraycone = cr.to_radians(props[1]);
		this.spraytype = props[2];			// 0 = continuous, 1 = one-shot
		this.spraying = true;				// for continuous mode only
		
		this.initspeed = props[3];
		this.initsize = props[4];
		this.initopacity = props[5] / 100.0;
		this.growrate = props[6];
		this.xrandom = props[7];
		this.yrandom = props[8];
		this.speedrandom = props[9];
		this.sizerandom = props[10];
		this.growrandom = props[11];
		this.acc = props[12];
		this.g = props[13];
		this.lifeanglerandom = props[14];
		this.lifespeedrandom = props[15];
		this.lifeopacityrandom = props[16];
		this.destroymode = props[17];		// 0 = fade, 1 = timeout, 2 = stopped
		this.timeout = props[18];
		
		this.particleCreateCounter = 0;
		this.particlescale = 1;
		
		// Dynamically set the bounding box to surround all created particles
		this.particleBoxLeft = this.x;
		this.particleBoxTop = this.y;
		this.particleBoxRight = this.x;
		this.particleBoxBottom = this.y;
		
		this.add_bbox_changed_callback(function (self) {
			self.bbox.set(self.particleBoxLeft, self.particleBoxTop, self.particleBoxRight, self.particleBoxBottom);
			self.bquad.set_from_rect(self.bbox);
			self.bbox_changed = false;
			self.update_collision_cell();
		});
		
		// Check for recycling
		if (!this.recycled)
			this.particles = [];
		
		this.runtime.tickMe(this);
		
		this.type.loadTextures();
			
		// If in one-shot mode, create all particles now
		if (this.spraytype === 1)
		{
			for (var i = 0; i < this.rate; i++)
				this.allocateParticle().opacity = 0;
		}
		
		this.first_tick = true;		// for re-init'ing one-shot particles on first tick so they assume any new angle/position
	};
	
	instanceProto.saveToJSON = function ()
	{
		var o = {
			"r": this.rate,
			"sc": this.spraycone,
			"st": this.spraytype,
			"s": this.spraying,
			"isp": this.initspeed,
			"isz": this.initsize,
			"io": this.initopacity,
			"gr": this.growrate,
			"xr": this.xrandom,
			"yr": this.yrandom,
			"spr": this.speedrandom,
			"szr": this.sizerandom,
			"grnd": this.growrandom,
			"acc": this.acc,
			"g": this.g,
			"lar": this.lifeanglerandom,
			"lsr": this.lifespeedrandom,
			"lor": this.lifeopacityrandom,
			"dm": this.destroymode,
			"to": this.timeout,
			"pcc": this.particleCreateCounter,
			"ft": this.first_tick,
			"p": []
		};
		
		var i, len, p;
		var arr = o["p"];

		for (i = 0, len = this.particles.length; i < len; i++)
		{
			p = this.particles[i];
			arr.push([p.x, p.y, p.speed, p.angle, p.opacity, p.grow, p.size, p.gs, p.age]);
		}
		
		return o;
	};
	
	instanceProto.loadFromJSON = function (o)
	{
		this.rate = o["r"];
		this.spraycone = o["sc"];
		this.spraytype = o["st"];
		this.spraying = o["s"];
		this.initspeed = o["isp"];
		this.initsize = o["isz"];
		this.initopacity = o["io"];
		this.growrate = o["gr"];
		this.xrandom = o["xr"];
		this.yrandom = o["yr"];
		this.speedrandom = o["spr"];
		this.sizerandom = o["szr"];
		this.growrandom = o["grnd"];
		this.acc = o["acc"];
		this.g = o["g"];
		this.lifeanglerandom = o["lar"];
		this.lifespeedrandom = o["lsr"];
		this.lifeopacityrandom = o["lor"];
		this.destroymode = o["dm"];
		this.timeout = o["to"];
		this.particleCreateCounter = o["pcc"];
		this.first_tick = o["ft"];
		
		// recycle all particles then load by reallocating them
		deadparticles.push.apply(deadparticles, this.particles);
		this.particles.length = 0;
		
		var i, len, p, d;
		var arr = o["p"];
		
		for (i = 0, len = arr.length; i < len; i++)
		{
			p = this.allocateParticle();
			d = arr[i];
			p.x = d[0];
			p.y = d[1];
			p.speed = d[2];
			p.angle = d[3];
			p.opacity = d[4];
			p.grow = d[5];
			p.size = d[6];
			p.gs = d[7];
			p.age = d[8];
		}
	};
	
	instanceProto.onDestroy = function ()
	{
		// recycle all particles
		deadparticles.push.apply(deadparticles, this.particles);
		this.particles.length = 0;
	};
	
	instanceProto.allocateParticle = function ()
	{
		var p;
		
		if (deadparticles.length)
		{
			p = deadparticles.pop();
			p.owner = this;
		}
		else
			p = new Particle(this);
		
		this.particles.push(p);
		p.active = true;
		return p;
	};
	
	instanceProto.tick = function()
	{
		var dt = this.runtime.getDt(this);
		
		var i, len, p, n, j;
		
		// Create spray particles for this tick
		if (this.spraytype === 0 && this.spraying)
		{
			this.particleCreateCounter += dt * this.rate;
			
			n = cr.floor(this.particleCreateCounter);
			this.particleCreateCounter -= n;
			
			for (i = 0; i < n; i++)
			{
				p = this.allocateParticle();
				p.init();
			}
		}
		
		this.particleBoxLeft = this.x;
		this.particleBoxTop = this.y;
		this.particleBoxRight = this.x;
		this.particleBoxBottom = this.y;
		
		for (i = 0, j = 0, len = this.particles.length; i < len; i++)
		{
			p = this.particles[i];
			this.particles[j] = p;
			
			this.runtime.redraw = true;
			
			// If the first tick for one-shot particles, call init() now so the particles
			// assume any changed position or angle of the Particles object.
			if (this.spraytype === 1 && this.first_tick)
				p.init();
			
			p.tick(dt);
			
			// Particle is dead: move to deadparticles for later recycling
			if (!p.active)
			{
				deadparticles.push(p);
				continue;
			}
			
			// measure bounding box
			if (p.left() < this.particleBoxLeft)
				this.particleBoxLeft = p.left();
			if (p.right() > this.particleBoxRight)
				this.particleBoxRight = p.right();
			if (p.top() < this.particleBoxTop)
				this.particleBoxTop = p.top();
			if (p.bottom() > this.particleBoxBottom)
				this.particleBoxBottom = p.bottom();
			
			j++;
		}
		
		this.particles.length = j;
		
		// Update the bounding box based on active particles
		this.set_bbox_changed();
		
		this.first_tick = false;
		
		// If one-shot and all particles are dead, destroy the whole object
		if (this.spraytype === 1 && this.particles.length === 0)
			this.runtime.DestroyInstance(this);
	};
	
	// only called if a layout object - draw to a canvas 2D context
	instanceProto.draw = function (ctx)
	{
		var i, len, p, layer = this.layer;
		
		for (i = 0, len = this.particles.length; i < len; i++)
		{
			p = this.particles[i];
			
			// Only draw active and on-screen particles
			if (p.right() >= layer.viewLeft && p.bottom() >= layer.viewTop && p.left() <= layer.viewRight && p.top() <= layer.viewBottom)
			{
				p.draw(ctx);
			}
		}
	};
	
	// only called if a layout object in WebGL mode - draw to the WebGL context
	// 'glw' is not a WebGL context, it's a wrapper - you can find its methods in GLWrap.js in the install
	// directory or just copy what other plugins do.
	instanceProto.drawGL = function (glw)
	{
		this.particlescale = this.layer.getScale();
		glw.setTexture(this.type.webGL_texture);
		
		var i, len, p, layer = this.layer;
		
		for (i = 0, len = this.particles.length; i < len; i++)
		{
			p = this.particles[i];
			
			// Only draw active and on-screen particles
			if (p.right() >= layer.viewLeft && p.bottom() >= layer.viewTop && p.left() <= layer.viewRight && p.top() <= layer.viewBottom)
			{
				p.drawGL(glw);
			}
		}
	};
	
	/**BEGIN-PREVIEWONLY**/
	instanceProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": "Particles",
			"properties": [
				{"name": "Particle count", "value": this.particles.length, "readonly": true},
				{"name": "Type", "value": (this.spraytype === 0 ? "Continuous" : "One-shot"), "readonly": true},
				{"name": "Is spraying", "value": this.spraying},
				{"name": "Rate", "value": this.rate},
				{"name": "Spray cone", "value": cr.to_degrees(this.spraycone)},
				{"name": "Speed", "value": this.initspeed},
				{"name": "Size", "value": this.initsize},
				{"name": "Opacity", "value": this.initopacity * 100},
				{"name": "Grow rate", "value": this.growrate},
				{"name": "X randomiser", "value": this.xrandom},
				{"name": "Y randomiser", "value": this.yrandom},
				{"name": "Speed randomiser", "value": this.speedrandom},
				{"name": "Size randomiser", "value": this.sizerandom},
				{"name": "Grow randomiser", "value": this.growrandom},
				{"name": "Acceleration", "value": this.acc},
				{"name": "Gravity", "value": this.g},
				{"name": "Lifetime angle randomiser", "value": this.lifeanglerandom},
				{"name": "Lifetime speed randomiser", "value": this.lifespeedrandom},
				{"name": "Lifetime opacity randomiser", "value": this.lifeopacityrandom},
				{"name": "Timeout", "value": this.timeout}
			]
		});
	};
	
	instanceProto.onDebugValueEdited = function (header, name, value)
	{
		switch (name) {
			case "Is spraying":			this.spraying = value;			break;
			case "Rate":				this.rate = value;				break;
			case "Spray cone":			this.spraycone = cr.to_radians(value);		break;
			case "Speed":				this.initspeed = value;			break;
			case "Size":				this.initsize = value;			break;
			case "Opacity":				this.initopacity = value / 100;	break;
			case "Grow rate":			this.growrate = value;			break;
			case "X randomiser":		this.xrandom = value;			break;
			case "Y randomiser":		this.yrandom = value;			break;
			case "Speed randomiser":	this.speedrandom = value;		break;
			case "Size randomiser":		this.sizerandom = value;		break;
			case "Grow randomiser":		this.growrandom = value;		break;
			case "Acceleration":		this.acc = value;				break;
			case "Gravity":				this.g = value;					break;
			case "Lifetime angle randomiser": this.lifeanglerandom = value;	break;
			case "Lifetime speed randomiser": this.lifespeedrandom = value; break;
			case "Lifetime opacity randomiser": this.lifeopacityrandom = value; break;
			case "Timeout":				this.timeout = value;			break;
		}
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	// the example condition
	Cnds.prototype.IsSpraying = function ()
	{
		return this.spraying;
	};
	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.SetSpraying = function (set_)
	{
		this.spraying = (set_ !== 0);
	};
	
	Acts.prototype.SetEffect = function (effect)
	{
		this.compositeOp = cr.effectToCompositeOp(effect);
		cr.setGLBlend(this, effect, this.runtime.gl);
		this.runtime.redraw = true;
	};
	
	Acts.prototype.SetRate = function (x)
	{
		this.rate = x;
		var diff, i;
		
		// In one-shot mode, if still in the first tick, adjust the number of particles created 
		if (this.spraytype === 1 && this.first_tick)
		{
			// Reducing particle count
			if (x < this.particles.length)
			{
				diff = this.particles.length - x;
				
				for (i = 0; i < diff; i++)
					deadparticles.push(this.particles.pop());
			}
			// Increasing particle count
			else if (x > this.particles.length)
			{
				diff = x - this.particles.length;
				
				for (i = 0; i < diff; i++)
					this.allocateParticle().opacity = 0;
			}
		}
	};
	
	Acts.prototype.SetSprayCone = function (x)
	{
		this.spraycone = cr.to_radians(x);
	};
	
	Acts.prototype.SetInitSpeed = function (x)
	{
		this.initspeed = x;
	};
	
	Acts.prototype.SetInitSize = function (x)
	{
		this.initsize = x;
	};
	
	Acts.prototype.SetInitOpacity = function (x)
	{
		this.initopacity = x / 100;
	};
	
	Acts.prototype.SetGrowRate = function (x)
	{
		this.growrate = x;
	};
	
	Acts.prototype.SetXRandomiser = function (x)
	{
		this.xrandom = x;
	};
	
	Acts.prototype.SetYRandomiser = function (x)
	{
		this.yrandom = x;
	};
	
	Acts.prototype.SetSpeedRandomiser = function (x)
	{
		this.speedrandom = x;
	};
	
	Acts.prototype.SetSizeRandomiser = function (x)
	{
		this.sizerandom = x;
	};
	
	Acts.prototype.SetGrowRateRandomiser = function (x)
	{
		this.growrandom = x;
	};
	
	Acts.prototype.SetParticleAcc = function (x)
	{
		this.acc = x;
	};
	
	Acts.prototype.SetGravity = function (x)
	{
		this.g = x;
	};
	
	Acts.prototype.SetAngleRandomiser = function (x)
	{
		this.lifeanglerandom = x;
	};
	
	Acts.prototype.SetLifeSpeedRandomiser = function (x)
	{
		this.lifespeedrandom = x;
	};
	
	Acts.prototype.SetOpacityRandomiser = function (x)
	{
		this.lifeopacityrandom = x;
	};
	
	Acts.prototype.SetTimeout = function (x)
	{
		this.timeout = x;
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.ParticleCount = function (ret)
	{
		ret.set_int(this.particles.length);
	};
	
	Exps.prototype.Rate = function (ret)
	{
		ret.set_float(this.rate);
	};
	
	Exps.prototype.SprayCone = function (ret)
	{
		ret.set_float(cr.to_degrees(this.spraycone));
	};
	
	Exps.prototype.InitSpeed = function (ret)
	{
		ret.set_float(this.initspeed);
	};
	
	Exps.prototype.InitSize = function (ret)
	{
		ret.set_float(this.initsize);
	};
	
	Exps.prototype.InitOpacity = function (ret)
	{
		ret.set_float(this.initopacity * 100);
	};
	
	Exps.prototype.InitGrowRate = function (ret)
	{
		ret.set_float(this.growrate);
	};
	
	Exps.prototype.XRandom = function (ret)
	{
		ret.set_float(this.xrandom);
	};
	
	Exps.prototype.YRandom = function (ret)
	{
		ret.set_float(this.yrandom);
	};
	
	Exps.prototype.InitSpeedRandom = function (ret)
	{
		ret.set_float(this.speedrandom);
	};
	
	Exps.prototype.InitSizeRandom = function (ret)
	{
		ret.set_float(this.sizerandom);
	};
	
	Exps.prototype.InitGrowRandom = function (ret)
	{
		ret.set_float(this.growrandom);
	};
	
	Exps.prototype.ParticleAcceleration = function (ret)
	{
		ret.set_float(this.acc);
	};
	
	Exps.prototype.Gravity = function (ret)
	{
		ret.set_float(this.g);
	};
	
	Exps.prototype.ParticleAngleRandom = function (ret)
	{
		ret.set_float(this.lifeanglerandom);
	};
	
	Exps.prototype.ParticleSpeedRandom = function (ret)
	{
		ret.set_float(this.lifespeedrandom);
	};
	
	Exps.prototype.ParticleOpacityRandom = function (ret)
	{
		ret.set_float(this.lifeopacityrandom);
	};
	
	Exps.prototype.Timeout = function (ret)
	{
		ret.set_float(this.timeout);
	};
	
	pluginProto.exps = new Exps();

}());