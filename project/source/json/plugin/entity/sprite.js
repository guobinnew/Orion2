// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.Sprite = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var pluginProto = cr.plugins_.Sprite.prototype;
		
	/////////////////////////////////////
	// Object type class
	pluginProto.Type = function(plugin)
	{
		this.plugin = plugin;
		this.runtime = plugin.runtime;
	};

	var typeProto = pluginProto.Type.prototype;
	
	function frame_getDataUri()
	{
		if (this.datauri.length === 0)
		{		
			// Get Sprite image as data URI
			var tmpcanvas = document.createElement("canvas");
			tmpcanvas.width = this.width;
			tmpcanvas.height = this.height;
			var tmpctx = tmpcanvas.getContext("2d");
			
			if (this.spritesheeted)
			{
				tmpctx.drawImage(this.texture_img, this.offx, this.offy, this.width, this.height,
										 0, 0, this.width, this.height);
			}
			else
			{
				tmpctx.drawImage(this.texture_img, 0, 0, this.width, this.height);
			}
			
			this.datauri = tmpcanvas.toDataURL("image/png");
		}
		
		return this.datauri;
	};

	typeProto.onCreate = function()
	{
		if (this.is_family)
			return;
			
		var i, leni, j, lenj;
		var anim, frame, animobj, frameobj, wt, uv;
		
		this.all_frames = [];
		this.has_loaded_textures = false;
		
		// Load all animation frames
		for (i = 0, leni = this.animations.length; i < leni; i++)
		{
			anim = this.animations[i];
			animobj = {};
			animobj.name = anim[0];
			animobj.speed = anim[1];
			animobj.loop = anim[2];
			animobj.repeatcount = anim[3];
			animobj.repeatto = anim[4];
			animobj.pingpong = anim[5];
			animobj.sid = anim[6];
			animobj.frames = [];
			
			for (j = 0, lenj = anim[7].length; j < lenj; j++)
			{
				frame = anim[7][j];
				frameobj = {};
				frameobj.texture_file = frame[0];
				frameobj.texture_filesize = frame[1];
				frameobj.offx = frame[2];
				frameobj.offy = frame[3];
				frameobj.width = frame[4];
				frameobj.height = frame[5];
				frameobj.duration = frame[6];
				frameobj.hotspotX = frame[7];
				frameobj.hotspotY = frame[8];
				frameobj.image_points = frame[9];
				frameobj.poly_pts = frame[10];
				frameobj.pixelformat = frame[11];
				frameobj.spritesheeted = (frameobj.width !== 0);
				frameobj.datauri = "";		// generated on demand and cached
				frameobj.getDataUri = frame_getDataUri;
				
				uv = {};
				uv.left = 0;
				uv.top = 0;
				uv.right = 1;
				uv.bottom = 1;
				frameobj.sheetTex = uv;
				
				frameobj.webGL_texture = null;
				
				// Sprite sheets may mean multiple frames reference one image
				// Ensure image is not created in duplicate
				wt = this.runtime.findWaitingTexture(frame[0]);
				
				if (wt)
				{
					frameobj.texture_img = wt;
				}
				else
				{
					frameobj.texture_img = new Image();
					frameobj.texture_img.cr_src = frame[0];
					frameobj.texture_img.cr_filesize = frame[1];
					frameobj.texture_img.c2webGL_texture = null;
					
					// Tell runtime to wait on this texture
					this.runtime.waitForImageLoad(frameobj.texture_img, frame[0]);
				}
				
				cr.seal(frameobj);
				animobj.frames.push(frameobj);
				this.all_frames.push(frameobj);
			}
			
			cr.seal(animobj);
			this.animations[i] = animobj;		// swap array data for object
		}
	};
	
	typeProto.updateAllCurrentTexture = function ()
	{
		var i, len, inst;
		for (i = 0, len = this.instances.length; i < len; i++)
		{
			inst = this.instances[i];
			inst.curWebGLTexture = inst.curFrame.webGL_texture;
		}
	};
	
	typeProto.onLostWebGLContext = function ()
	{
		if (this.is_family)
			return;
			
		var i, len, frame;
		
		// Release all animation frames
		for (i = 0, len = this.all_frames.length; i < len; ++i)
		{
			frame = this.all_frames[i];
			frame.texture_img.c2webGL_texture = null;
			frame.webGL_texture = null;
		}
		
		this.has_loaded_textures = false;
		
		this.updateAllCurrentTexture();
	};
	
	typeProto.onRestoreWebGLContext = function ()
	{
		// No need to create textures if no instances exist, will create on demand
		if (this.is_family || !this.instances.length)
			return;
			
		var i, len, frame;
		
		// Re-load all animation frames
		for (i = 0, len = this.all_frames.length; i < len; ++i)
		{
			frame = this.all_frames[i];
			
			frame.webGL_texture = this.runtime.glwrap.loadTexture(frame.texture_img, false, this.runtime.linearSampling, frame.pixelformat);
		}
		
		this.updateAllCurrentTexture();
	};
	
	typeProto.loadTextures = function ()
	{
		if (this.is_family || this.has_loaded_textures || !this.runtime.glwrap)
			return;
			
		var i, len, frame;
		for (i = 0, len = this.all_frames.length; i < len; ++i)
		{
			frame = this.all_frames[i];
			
			frame.webGL_texture = this.runtime.glwrap.loadTexture(frame.texture_img, false, this.runtime.linearSampling, frame.pixelformat);
		}
		
		this.has_loaded_textures = true;
	};
	
	typeProto.unloadTextures = function ()
	{
		// Don't release textures if any instances still exist, they are probably using them
		if (this.is_family || this.instances.length || !this.has_loaded_textures)
			return;
			
		var i, len, frame;
		for (i = 0, len = this.all_frames.length; i < len; ++i)
		{
			frame = this.all_frames[i];
			
			this.runtime.glwrap.deleteTexture(frame.webGL_texture);
			frame.webGL_texture = null;
		}
		
		this.has_loaded_textures = false;
	};
	
	var already_drawn_images = [];
	
	typeProto.preloadCanvas2D = function (ctx)
	{
		var i, len, frameimg;
		already_drawn_images.length = 0;
		
		for (i = 0, len = this.all_frames.length; i < len; ++i)
		{
			frameimg = this.all_frames[i].texture_img;
			
			if (already_drawn_images.indexOf(frameimg) !== -1)
					continue;
				
			// draw to preload, browser should lazy load the texture
			ctx.drawImage(frameimg, 0, 0);
			already_drawn_images.push(frameimg);
		}
	};

	/////////////////////////////////////
	// Instance class
	pluginProto.Instance = function(type)
	{
		this.type = type;
		this.runtime = type.runtime;
		
		// Physics needs to see the collision poly before onCreate
		var poly_pts = this.type.animations[0].frames[0].poly_pts;
		
		if (this.recycled)
			this.collision_poly.set_pts(poly_pts);
		else
			this.collision_poly = new cr.CollisionPoly(poly_pts);
	};
	
	var instanceProto = pluginProto.Instance.prototype;

	instanceProto.onCreate = function()
	{
		this.visible = (this.properties[0] === 0);	// 0=visible, 1=invisible
		this.isTicking = false;
		this.inAnimTrigger = false;
		this.collisionsEnabled = (this.properties[3] !== 0);
		
		// Tick this object to change animation frame, but never tick single-animation, single-frame objects.
		// Also don't tick zero speed animations until the speed or animation is changed, which saves ticking
		// on tile sprites.
		if (!(this.type.animations.length === 1 && this.type.animations[0].frames.length === 1) && this.type.animations[0].speed !== 0)
		{
			this.runtime.tickMe(this);
			this.isTicking = true;
		}
		
		this.cur_animation = this.getAnimationByName(this.properties[1]) || this.type.animations[0];
		this.cur_frame = this.properties[2];
		
		if (this.cur_frame < 0)
			this.cur_frame = 0;
		if (this.cur_frame >= this.cur_animation.frames.length)
			this.cur_frame = this.cur_animation.frames.length - 1;
			
		// Update poly and hotspot for the starting frame.
		var curanimframe = this.cur_animation.frames[this.cur_frame];
		this.collision_poly.set_pts(curanimframe.poly_pts);
		this.hotspotX = curanimframe.hotspotX;
		this.hotspotY = curanimframe.hotspotY;
			
		this.cur_anim_speed = this.cur_animation.speed;
		
		if (this.recycled)
			this.animTimer.reset();
		else
			this.animTimer = new cr.KahanAdder();
		
		this.frameStart = this.getNowTime();
		this.animPlaying = true;
		this.animRepeats = 0;
		this.animForwards = true;
		this.animTriggerName = "";
		
		this.changeAnimName = "";
		this.changeAnimFrom = 0;
		this.changeAnimFrame = -1;
		
		// Ensure type has textures loaded
		this.type.loadTextures();
		
		// Iterate all animations and frames ensuring WebGL textures are loaded and sizes are set
		var i, leni, j, lenj;
		var anim, frame, uv, maintex;
		
		for (i = 0, leni = this.type.animations.length; i < leni; i++)
		{
			anim = this.type.animations[i];
			
			for (j = 0, lenj = anim.frames.length; j < lenj; j++)
			{
				frame = anim.frames[j];
				
				// If size is zero, image is not on a sprite sheet.  Determine size now.
				if (frame.width === 0)
				{
					frame.width = frame.texture_img.width;
					frame.height = frame.texture_img.height;
				}
				
				// If frame is spritesheeted update its uv coords
				if (frame.spritesheeted)
				{
					maintex = frame.texture_img;
					uv = frame.sheetTex;
					uv.left = frame.offx / maintex.width;
					uv.top = frame.offy / maintex.height;
					uv.right = (frame.offx + frame.width) / maintex.width;
					uv.bottom = (frame.offy + frame.height) / maintex.height;

					// Check if frame is in fact a complete-frame spritesheet
					if (frame.offx === 0 && frame.offy === 0 && frame.width === maintex.width && frame.height === maintex.height)
					{
						frame.spritesheeted = false;
					}
				}
			}
		}
		
		this.curFrame = this.cur_animation.frames[this.cur_frame];
		this.curWebGLTexture = this.curFrame.webGL_texture;
	};
	
	instanceProto.saveToJSON = function ()
	{
		var o = {
			"a": this.cur_animation.sid,
			"f": this.cur_frame,
			"cas": this.cur_anim_speed,
			"fs": this.frameStart,
			"ar": this.animRepeats,
			"at": this.animTimer.sum
		};
		
		if (!this.animPlaying)
			o["ap"] = this.animPlaying;
			
		if (!this.animForwards)
			o["af"] = this.animForwards;
		
		return o;
	};
	
	instanceProto.loadFromJSON = function (o)
	{
		var anim = this.getAnimationBySid(o["a"]);
		
		if (anim)
			this.cur_animation = anim;
		
		this.cur_frame = o["f"];
		
		if (this.cur_frame < 0)
			this.cur_frame = 0;
		if (this.cur_frame >= this.cur_animation.frames.length)
			this.cur_frame = this.cur_animation.frames.length - 1;
		
		this.cur_anim_speed = o["cas"];
		this.frameStart = o["fs"];
		this.animRepeats = o["ar"];
		this.animTimer.reset();
		this.animTimer.sum = o["at"];
		this.animPlaying = o.hasOwnProperty("ap") ? o["ap"] : true;
		this.animForwards = o.hasOwnProperty("af") ? o["af"] : true;
			
		this.curFrame = this.cur_animation.frames[this.cur_frame];
		this.curWebGLTexture = this.curFrame.webGL_texture;
		this.collision_poly.set_pts(this.curFrame.poly_pts);
		this.hotspotX = this.curFrame.hotspotX;
		this.hotspotY = this.curFrame.hotspotY;
	};
	
	instanceProto.animationFinish = function (reverse)
	{
		// stop
		this.cur_frame = reverse ? 0 : this.cur_animation.frames.length - 1;
		this.animPlaying = false;
		
		// trigger finish events
		this.animTriggerName = this.cur_animation.name;
		
		this.inAnimTrigger = true;
		this.runtime.trigger(cr.plugins_.Sprite.prototype.cnds.OnAnyAnimFinished, this);
		this.runtime.trigger(cr.plugins_.Sprite.prototype.cnds.OnAnimFinished, this);
		this.inAnimTrigger = false;
			
		this.animRepeats = 0;
	};
	
	instanceProto.getNowTime = function()
	{
		return this.animTimer.sum;
	};
	
	instanceProto.tick = function()
	{
		this.animTimer.add(this.runtime.getDt(this));
		
		// Change any animation or frame that was queued
		if (this.changeAnimName.length)
			this.doChangeAnim();
		if (this.changeAnimFrame >= 0)
			this.doChangeAnimFrame();
		
		var now = this.getNowTime();
		var cur_animation = this.cur_animation;
		var prev_frame = cur_animation.frames[this.cur_frame];
		var next_frame;
		var cur_frame_time = prev_frame.duration / this.cur_anim_speed;
		
		if (this.animPlaying && now >= this.frameStart + cur_frame_time)
		{			
			// Next frame
			if (this.animForwards)
			{
				this.cur_frame++;
				//log("Advancing animation frame forwards");
			}
			else
			{
				this.cur_frame--;
				//log("Advancing animation frame backwards");
			}
				
			this.frameStart += cur_frame_time;
			
			// Reached end of frames
			if (this.cur_frame >= cur_animation.frames.length)
			{
				//log("At end of frames");
				
				if (cur_animation.pingpong)
				{
					this.animForwards = false;
					this.cur_frame = cur_animation.frames.length - 2;
					//log("Ping pong looping from end");
				}
				// Looping: wind back to repeat-to frame
				else if (cur_animation.loop)
				{
					this.cur_frame = cur_animation.repeatto;
				}
				else
				{					
					this.animRepeats++;
					
					if (this.animRepeats >= cur_animation.repeatcount)
					{
						//log("Number of repeats reached; ending animation");
						
						this.animationFinish(false);
					}
					else
					{
						//log("Repeating");
						this.cur_frame = cur_animation.repeatto;
					}
				}
			}
			// Ping-ponged back to start
			if (this.cur_frame < 0)
			{
				if (cur_animation.pingpong)
				{
					this.cur_frame = 1;
					this.animForwards = true;
					//log("Ping ponging back forwards");
					
					if (!cur_animation.loop)
					{
						this.animRepeats++;
							
						if (this.animRepeats >= cur_animation.repeatcount)
						{
							//log("Number of repeats reached; ending animation");
							
							this.animationFinish(true);
						}
					}
				}
				// animation running backwards
				else
				{
					if (cur_animation.loop)
					{
						this.cur_frame = cur_animation.repeatto;
					}
					else
					{
						this.animRepeats++;
						
						// Reached number of repeats
						if (this.animRepeats >= cur_animation.repeatcount)
						{
							//log("Number of repeats reached; ending animation");
							
							this.animationFinish(true);
						}
						else
						{
							//log("Repeating");
							this.cur_frame = cur_animation.repeatto;
						}
					}
				}
			}
			
			// Don't go out of bounds
			if (this.cur_frame < 0)
				this.cur_frame = 0;
			else if (this.cur_frame >= cur_animation.frames.length)
				this.cur_frame = cur_animation.frames.length - 1;
				
			// If frameStart is still more than a whole frame away, we must've fallen behind.  Instead of
			// going catch-up (cycling one frame per tick), reset the frame timer to now.
			if (now > this.frameStart + (cur_animation.frames[this.cur_frame].duration / this.cur_anim_speed))
			{
				//log("Animation can't keep up, resetting timer");
				this.frameStart = now;
			}
				
			next_frame = cur_animation.frames[this.cur_frame];
			this.OnFrameChanged(prev_frame, next_frame);
				
			this.runtime.redraw = true;
		}
	};
	
	instanceProto.getAnimationByName = function (name_)
	{
		var i, len, a;
		for (i = 0, len = this.type.animations.length; i < len; i++)
		{
			a = this.type.animations[i];
			
			if (cr.equals_nocase(a.name, name_))
				return a;
		}
		
		return null;
	};
	
	instanceProto.getAnimationBySid = function (sid_)
	{
		var i, len, a;
		for (i = 0, len = this.type.animations.length; i < len; i++)
		{
			a = this.type.animations[i];
			
			if (a.sid === sid_)
				return a;
		}
		
		return null;
	};
	
	instanceProto.doChangeAnim = function ()
	{
		var prev_frame = this.cur_animation.frames[this.cur_frame];
		
		// Find the animation by name
		var anim = this.getAnimationByName(this.changeAnimName);
		
		this.changeAnimName = "";
		
		// couldn't find by name
		if (!anim)
			return;
			
		// don't change if setting same animation and the animation is already playing
		if (cr.equals_nocase(anim.name, this.cur_animation.name) && this.animPlaying)
			return;
			
		this.cur_animation = anim;
		this.cur_anim_speed = anim.speed;
		
		if (this.cur_frame < 0)
			this.cur_frame = 0;
		if (this.cur_frame >= this.cur_animation.frames.length)
			this.cur_frame = this.cur_animation.frames.length - 1;
			
		// from beginning
		if (this.changeAnimFrom === 1)
			this.cur_frame = 0;
			
		this.animPlaying = true;
		this.frameStart = this.getNowTime();
		this.animForwards = true;
		
		this.OnFrameChanged(prev_frame, this.cur_animation.frames[this.cur_frame]);
		
		this.runtime.redraw = true;
	};
	
	instanceProto.doChangeAnimFrame = function ()
	{
		var prev_frame = this.cur_animation.frames[this.cur_frame];
		var prev_frame_number = this.cur_frame;
		
		this.cur_frame = cr.floor(this.changeAnimFrame);
		
		if (this.cur_frame < 0)
			this.cur_frame = 0;
		if (this.cur_frame >= this.cur_animation.frames.length)
			this.cur_frame = this.cur_animation.frames.length - 1;
			
		if (prev_frame_number !== this.cur_frame)
		{
			this.OnFrameChanged(prev_frame, this.cur_animation.frames[this.cur_frame]);
			this.frameStart = this.getNowTime();
			this.runtime.redraw = true;
		}
		
		this.changeAnimFrame = -1;
	};
	
	instanceProto.OnFrameChanged = function (prev_frame, next_frame)
	{
		// Has the frame size changed?  Resize the object proportionally
		var oldw = prev_frame.width;
		var oldh = prev_frame.height;
		var neww = next_frame.width;
		var newh = next_frame.height;
		
		if (oldw != neww)
			this.width *= (neww / oldw);
		if (oldh != newh)
			this.height *= (newh / oldh);
			
		// Update hotspot, collision poly and bounding box
		this.hotspotX = next_frame.hotspotX;
		this.hotspotY = next_frame.hotspotY;
		this.collision_poly.set_pts(next_frame.poly_pts);
		this.set_bbox_changed();
		
		// Update webGL texture if any
		this.curFrame = next_frame;
		this.curWebGLTexture = next_frame.webGL_texture;
		
		// Notify behaviors
		var i, len, b;
		for (i = 0, len = this.behavior_insts.length; i < len; i++)
		{
			b = this.behavior_insts[i];
			
			if (b.onSpriteFrameChanged)
				b.onSpriteFrameChanged(prev_frame, next_frame);
		}
		
		// Trigger 'on frame changed'
		this.runtime.trigger(cr.plugins_.Sprite.prototype.cnds.OnFrameChanged, this);
	};

	instanceProto.draw = function(ctx)
	{
		ctx.globalAlpha = this.opacity;
			
		// The current animation frame to draw
		var cur_frame = this.curFrame;
		var spritesheeted = cur_frame.spritesheeted;
		var cur_image = cur_frame.texture_img;
		
		var myx = this.x;
		var myy = this.y;
		var w = this.width;
		var h = this.height;
		
		// Object not rotated: can draw without transformation.
		if (this.angle === 0 && w >= 0 && h >= 0)
		{
			myx -= this.hotspotX * w;
			myy -= this.hotspotY * h;
			
			if (this.runtime.pixel_rounding)
			{
				myx = Math.round(myx);
				myy = Math.round(myy);
			}
			
			if (spritesheeted)
			{
				ctx.drawImage(cur_image, cur_frame.offx, cur_frame.offy, cur_frame.width, cur_frame.height,
										 myx, myy, w, h);
			}
			else
			{
				ctx.drawImage(cur_image, myx, myy, w, h);
			}
		}
		else
		{
			// Only pixel round the x/y position, otherwise objects don't rotate smoothly
			if (this.runtime.pixel_rounding)
			{
				myx = Math.round(myx);
				myy = Math.round(myy);
			}
			
			// Angle applied; we need to transform the canvas.  Save state.
			ctx.save();
			
			var widthfactor = w > 0 ? 1 : -1;
			var heightfactor = h > 0 ? 1 : -1;
			
			// Translate to object's position, then rotate by its angle.
			ctx.translate(myx, myy);
			
			if (widthfactor !== 1 || heightfactor !== 1)
				ctx.scale(widthfactor, heightfactor);
			
			ctx.rotate(this.angle * widthfactor * heightfactor);
			
			var drawx = 0 - (this.hotspotX * cr.abs(w))
			var drawy = 0 - (this.hotspotY * cr.abs(h));
			
			// Draw the object; canvas origin is at hot spot.
			if (spritesheeted)
			{
				ctx.drawImage(cur_image, cur_frame.offx, cur_frame.offy, cur_frame.width, cur_frame.height,
										 drawx, drawy, cr.abs(w), cr.abs(h));
			}
			else
			{
				ctx.drawImage(cur_image, drawx, drawy, cr.abs(w), cr.abs(h));
			}
			
			// Restore previous state.
			ctx.restore();
		}
			
		//////////////////////////////////////////
		// Draw collision poly (for debug)
		/*
		ctx.strokeStyle = "#f00";
		ctx.lineWidth = 3;
		ctx.beginPath();
		this.collision_poly.cache_poly(this.width, this.height, this.angle);
		var i, len, ax, ay, bx, by;
		for (i = 0, len = this.collision_poly.pts_count; i < len; i++)
		{
			ax = this.collision_poly.pts_cache[i*2] + this.x;
			ay = this.collision_poly.pts_cache[i*2+1] + this.y;
			bx = this.collision_poly.pts_cache[((i+1)%len)*2] + this.x;
			by = this.collision_poly.pts_cache[((i+1)%len)*2+1] + this.y;
			
			ctx.moveTo(ax, ay);
			ctx.lineTo(bx, by);
		}
		
		ctx.stroke();
		ctx.closePath();
		*/
		// Draw physics polys (for debug)
		/*
		if (this.behavior_insts.length >= 1 && this.behavior_insts[0].draw)
		{
			this.behavior_insts[0].draw(ctx);
		}
		*/
		//////////////////////////////////////////
	};
	
	instanceProto.drawGL = function(glw)
	{
		glw.setTexture(this.curWebGLTexture);
		glw.setOpacity(this.opacity);
		var cur_frame = this.curFrame;
		
		var q = this.bquad;
		
		if (this.runtime.pixel_rounding)
		{
			var ox = Math.round(this.x) - this.x;
			var oy = Math.round(this.y) - this.y;
			
			if (cur_frame.spritesheeted)
				glw.quadTex(q.tlx + ox, q.tly + oy, q.trx + ox, q.try_ + oy, q.brx + ox, q.bry + oy, q.blx + ox, q.bly + oy, cur_frame.sheetTex);
			else
				glw.quad(q.tlx + ox, q.tly + oy, q.trx + ox, q.try_ + oy, q.brx + ox, q.bry + oy, q.blx + ox, q.bly + oy);
		}
		else
		{
			if (cur_frame.spritesheeted)
				glw.quadTex(q.tlx, q.tly, q.trx, q.try_, q.brx, q.bry, q.blx, q.bly, cur_frame.sheetTex);
			else
				glw.quad(q.tlx, q.tly, q.trx, q.try_, q.brx, q.bry, q.blx, q.bly);
		}
	};
	
	instanceProto.getImagePointIndexByName = function(name_)
	{
		var cur_frame = this.curFrame;
		
		var i, len;
		for (i = 0, len = cur_frame.image_points.length; i < len; i++)
		{
			if (cr.equals_nocase(name_, cur_frame.image_points[i][0]))
				return i;
		}
		
		return -1;
	};
	
	instanceProto.getImagePoint = function(imgpt, getX)
	{
		var cur_frame = this.curFrame;
		var image_points = cur_frame.image_points;
		var index;
		
		if (cr.is_string(imgpt))
			index = this.getImagePointIndexByName(imgpt);
		else
			index = imgpt - 1;	// 0 is origin
			
		index = cr.floor(index);
		if (index < 0 || index >= image_points.length)
			return getX ? this.x : this.y;	// return origin
			
		// get position scaled and relative to origin in pixels
		var x = (image_points[index][1] - cur_frame.hotspotX) * this.width;
		var y = image_points[index][2];
		
		y = (y - cur_frame.hotspotY) * this.height;
		
		// rotate by object angle
		var cosa = Math.cos(this.angle);
		var sina = Math.sin(this.angle);
		var x_temp = (x * cosa) - (y * sina);
		y = (y * cosa) + (x * sina);
		x = x_temp;
		x += this.x;
		y += this.y;
		return getX ? x : y;
	};
	
	/**BEGIN-PREVIEWONLY**/
	instanceProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": "Sprite animation",
			"properties": [
				{"name": "Current animation", "value": this.cur_animation.name},
				{"name": "Current frame", "value": this.cur_frame},
				{"name": "Playing", "value": this.animPlaying},
				{"name": "Speed", "value": this.cur_anim_speed},
				{"name": "Repeats", "value": this.animRepeats}
			]
		});
	};
	
	instanceProto.onDebugValueEdited = function (header, name, value)
	{
		if (header === "Sprite animation")
		{
			if (name === "Current animation")
			{
				this.changeAnimName = value;
				this.changeAnimFrom = 0;		// from current frame
				
				// start ticking if not already
				if (!this.isTicking)
				{
					this.runtime.tickMe(this);
					this.isTicking = true;
				}
				
				// not in trigger: apply immediately
				if (!this.inAnimTrigger)
					this.doChangeAnim();
			}
			else if (name === "Current frame")
			{
				this.changeAnimFrame = value;
		
				// start ticking if not already
				if (!this.isTicking)
				{
					this.runtime.tickMe(this);
					this.isTicking = true;
				}
				
				// not in trigger: apply immediately
				if (!this.inAnimTrigger)
					this.doChangeAnimFrame();
			}
			else if (name === "Playing")
			{
				this.animPlaying = value;
				
				if (this.animPlaying)
				{
					this.frameStart = this.getNowTime();
					
					// start ticking if not already
					if (!this.isTicking)
					{
						this.runtime.tickMe(this);
						this.isTicking = true;
					}
				}
			}
			else if (name === "Speed")
			{
				this.cur_anim_speed = cr.abs(value);
				this.animForwards = (value >= 0);
				
				// start ticking if not already
				if (!this.isTicking)
				{
					this.runtime.tickMe(this);
					this.isTicking = true;
				}
			}
			else if (name === "Repeats")
			{
				this.animRepeats = value;
			}
		}
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	// For the collision memory in 'On collision'.
	var arrCache = [];
	
	function allocArr()
	{
		if (arrCache.length)
			return arrCache.pop();
		else
			return [0, 0, 0];
	};
	
	function freeArr(a)
	{
		a[0] = 0;
		a[1] = 0;
		a[2] = 0;
		arrCache.push(a);
	};
	
	function makeCollKey(a, b)
	{
		// comma separated string with lowest value first
		if (a < b)
			return "" + a + "," + b;
		else
			return "" + b + "," + a;
	};
	
	function collmemory_add(collmemory, a, b, tickcount)
	{
		var a_uid = a.uid;
		var b_uid = b.uid;

		var key = makeCollKey(a_uid, b_uid);
		
		if (collmemory.hasOwnProperty(key))
		{
			// added already; just update tickcount
			collmemory[key][2] = tickcount;
			return;
		}
		
		var arr = allocArr();
		arr[0] = a_uid;
		arr[1] = b_uid;
		arr[2] = tickcount;
		collmemory[key] = arr;
	};
	
	function collmemory_remove(collmemory, a, b)
	{
		var key = makeCollKey(a.uid, b.uid);
		
		if (collmemory.hasOwnProperty(key))
		{
			freeArr(collmemory[key]);
			delete collmemory[key];
		}
	};
	
	function collmemory_removeInstance(collmemory, inst)
	{
		var uid = inst.uid;
		var p, entry;
		for (p in collmemory)
		{
			if (collmemory.hasOwnProperty(p))
			{
				entry = collmemory[p];
				
				// Referenced in either UID: must be removed
				if (entry[0] === uid || entry[1] === uid)
				{
					freeArr(collmemory[p]);
					delete collmemory[p];
				}
			}
		}
	};
	
	var last_coll_tickcount = -2;
	
	function collmemory_has(collmemory, a, b)
	{
		var key = makeCollKey(a.uid, b.uid);
		
		if (collmemory.hasOwnProperty(key))
		{
			last_coll_tickcount = collmemory[key][2];
			return true;
		}
		else
		{
			last_coll_tickcount = -2;
			return false;
		}
	};
	
	var candidates1 = [];
	
	Cnds.prototype.OnCollision = function (rtype)
	{	
		if (!rtype)
			return false;
			
		var runtime = this.runtime;
			
		// Static condition: perform picking manually.
		// Get the current condition.  This is like the 'is overlapping' condition
		// but with a built in 'trigger once' for the l instances.
		var cnd = runtime.getCurrentCondition();
		var ltype = cnd.type;
		
		// Create the collision memory, which remembers pairs of collisions that
		// are already overlapping
		if (!cnd.extra["collmemory"])
		{
			cnd.extra["collmemory"] = {};
			
			// Since this is one-time initialisation, also add a destroy callback
			// to remove any instance references from memory
			runtime.addDestroyCallback((function (collmemory) {
				return function(inst) {
					collmemory_removeInstance(collmemory, inst);
				};
			})(cnd.extra["collmemory"]));
		}
		
		var collmemory = cnd.extra["collmemory"];
		
		// Get the currently active SOLs for both objects involved in the overlap test
		var lsol = ltype.getCurrentSol();
		var rsol = rtype.getCurrentSol();
		var linstances = lsol.getObjects();
		var rinstances;
		
		// Iterate each combination of instances
		var l, linst, r, rinst;
		var curlsol, currsol;
		
		var tickcount = this.runtime.tickcount;
		var lasttickcount = tickcount - 1;
		var exists, run;
		
		var current_event = runtime.getCurrentEventStack().current_event;
		var orblock = current_event.orblock;
		
		// Note: don't cache lengths of linstances or rinstances. They can change if objects get destroyed in the event
		// retriggering.
		for (l = 0; l < linstances.length; l++)
		{
			linst = linstances[l];
			
			if (rsol.select_all)
			{
				linst.update_bbox();
				this.runtime.getCollisionCandidates(linst.layer, rtype, linst.bbox, candidates1);
				rinstances = candidates1;
			}
			else
				rinstances = rsol.getObjects();
			
			for (r = 0; r < rinstances.length; r++)
			{
				rinst = rinstances[r];
				
				if (runtime.testOverlap(linst, rinst) || runtime.checkRegisteredCollision(linst, rinst))
				{
					exists = collmemory_has(collmemory, linst, rinst);
					run = (!exists || (last_coll_tickcount < lasttickcount));
					
					// objects are still touching so update the tickcount
					collmemory_add(collmemory, linst, rinst, tickcount);
					
					if (run)
					{						
						runtime.pushCopySol(current_event.solModifiers);
						curlsol = ltype.getCurrentSol();
						currsol = rtype.getCurrentSol();
						curlsol.select_all = false;
						currsol.select_all = false;
						
						// If ltype === rtype, it's the same object (e.g. Sprite collides with Sprite)
						// In which case, pick both instances
						if (ltype === rtype)
						{
							curlsol.instances.length = 2;	// just use lsol, is same reference as rsol
							curlsol.instances[0] = linst;
							curlsol.instances[1] = rinst;
							ltype.applySolToContainer();
						}
						else
						{
							// Pick each instance in its respective SOL
							curlsol.instances.length = 1;
							currsol.instances.length = 1;
							curlsol.instances[0] = linst;
							currsol.instances[0] = rinst;
							ltype.applySolToContainer();
							rtype.applySolToContainer();
						}
						
						current_event.retrigger();
						runtime.popSol(current_event.solModifiers);
					}
				}
				else
				{
					// Pair not overlapping: ensure any record removed (mainly to save memory)
					collmemory_remove(collmemory, linst, rinst);
				}
			}
			
			candidates1.length = 0;
		}
		
		// We've aleady run the event by now.
		return false;
	};
	
	var rpicktype = null;
	var rtopick = new cr.ObjectSet();
	var needscollisionfinish = false;
	
	var candidates2 = [];
	var temp_bbox = new cr.rect(0, 0, 0, 0);
	
	function DoOverlapCondition(rtype, offx, offy)
	{
		if (!rtype)
			return false;
			
		var do_offset = (offx !== 0 || offy !== 0);
		var oldx, oldy, ret = false, r, lenr, rinst;
		var cnd = this.runtime.getCurrentCondition();
		var ltype = cnd.type;
		var inverted = cnd.inverted;
		var rsol = rtype.getCurrentSol();
		var orblock = this.runtime.getCurrentEventStack().current_event.orblock;
		var rinstances;
		
		if (rsol.select_all)
		{
			this.update_bbox();
			
			// Make sure queried box is offset the same as the collision offset so we look in
			// the right cells
			temp_bbox.copy(this.bbox);
			temp_bbox.offset(offx, offy);
			this.runtime.getCollisionCandidates(this.layer, rtype, temp_bbox, candidates2);
			rinstances = candidates2;
		}
		else if (orblock)
			rinstances = rsol.else_instances;
		else
			rinstances = rsol.instances;
		
		rpicktype = rtype;
		needscollisionfinish = (ltype !== rtype && !inverted);
		
		if (do_offset)
		{
			oldx = this.x;
			oldy = this.y;
			this.x += offx;
			this.y += offy;
			this.set_bbox_changed();
		}
		
		for (r = 0, lenr = rinstances.length; r < lenr; r++)
		{
			rinst = rinstances[r];
			
			// objects overlap: true for this instance, ensure both are picked
			// (if ltype and rtype are same, e.g. "Sprite overlaps Sprite", don't pick the other instance,
			// it will be picked when it gets iterated to itself)
			if (this.runtime.testOverlap(this, rinst))
			{
				ret = true;
				
				// Inverted condition: just bail out now, don't pick right hand instance -
				// also note we still return true since the condition invert flag makes that false
				if (inverted)
					break;
					
				if (ltype !== rtype)
					rtopick.add(rinst);
			}
		}
		
		if (do_offset)
		{
			this.x = oldx;
			this.y = oldy;
			this.set_bbox_changed();
		}
		
		candidates2.length = 0;
		return ret;
	};
	
	typeProto.finish = function (do_pick)
	{
		if (!needscollisionfinish)
			return;
		
		if (do_pick)
		{
			var orblock = this.runtime.getCurrentEventStack().current_event.orblock;
			var sol = rpicktype.getCurrentSol();
			var topick = rtopick.valuesRef();
			var i, len, inst;
			
			if (sol.select_all)
			{
				// All selected: filter down to just those in topick
				sol.select_all = false;
				sol.instances.length = topick.length;
			
				for (i = 0, len = topick.length; i < len; i++)
				{
					sol.instances[i] = topick[i];
				}
				
				// In OR blocks, else_instances must also be filled with objects not in topick
				if (orblock)
				{
					sol.else_instances.length = 0;
					
					for (i = 0, len = rpicktype.instances.length; i < len; i++)
					{
						inst = rpicktype.instances[i];
						
						if (!rtopick.contains(inst))
							sol.else_instances.push(inst);
					}
				}
			}
			else
			{
				if (orblock)
				{
					var initsize = sol.instances.length;
					sol.instances.length = initsize + topick.length;
				
					for (i = 0, len = topick.length; i < len; i++)
					{
						sol.instances[initsize + i] = topick[i];
						cr.arrayFindRemove(sol.else_instances, topick[i]);
					}
				}
				else
				{
					cr.shallowAssignArray(sol.instances, topick);
				}
			}
			
			rpicktype.applySolToContainer();
		}
		
		rtopick.clear();
		needscollisionfinish = false;
	};
	
	Cnds.prototype.IsOverlapping = function (rtype)
	{
		return DoOverlapCondition.call(this, rtype, 0, 0);
	};
	
	Cnds.prototype.IsOverlappingOffset = function (rtype, offx, offy)
	{
		return DoOverlapCondition.call(this, rtype, offx, offy);
	};
	
	Cnds.prototype.IsAnimPlaying = function (animname)
	{
		// If awaiting a change of animation to really happen next tick, compare to that now
		if (this.changeAnimName.length)
			return cr.equals_nocase(this.changeAnimName, animname);
		else
			return cr.equals_nocase(this.cur_animation.name, animname);
	};
	
	Cnds.prototype.CompareFrame = function (cmp, framenum)
	{
		return cr.do_cmp(this.cur_frame, cmp, framenum);
	};
	
	Cnds.prototype.CompareAnimSpeed = function (cmp, x)
	{
		var s = (this.animForwards ? this.cur_anim_speed : -this.cur_anim_speed);
		return cr.do_cmp(s, cmp, x);
	};
	
	Cnds.prototype.OnAnimFinished = function (animname)
	{
		return cr.equals_nocase(this.animTriggerName, animname);
	};
	
	Cnds.prototype.OnAnyAnimFinished = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnFrameChanged = function ()
	{
		return true;
	};
	
	Cnds.prototype.IsMirrored = function ()
	{
		return this.width < 0;
	};
	
	Cnds.prototype.IsFlipped = function ()
	{
		return this.height < 0;
	};
	
	Cnds.prototype.OnURLLoaded = function ()
	{
		return true;
	};
	
	Cnds.prototype.IsCollisionEnabled = function ()
	{
		return this.collisionsEnabled;
	};
	
	pluginProto.cnds = new Cnds();

	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.Spawn = function (obj, layer, imgpt)
	{
		if (!obj || !layer)
			return;
			
		var inst = this.runtime.createInstance(obj, layer, this.getImagePoint(imgpt, true), this.getImagePoint(imgpt, false));
		
		if (!inst)
			return;
		
		if (typeof inst.angle !== "undefined")
		{
			inst.angle = this.angle;
			inst.set_bbox_changed();
		}
		
		this.runtime.isInOnDestroy++;
		
		var i, len, s;
		this.runtime.trigger(Object.getPrototypeOf(obj.plugin).cnds.OnCreated, inst);
		
		if (inst.is_contained)
		{
			for (i = 0, len = inst.siblings.length; i < len; i++)
			{
				s = inst.siblings[i];
				this.runtime.trigger(Object.getPrototypeOf(s.type.plugin).cnds.OnCreated, s);
			}
		}
		
		this.runtime.isInOnDestroy--;
		
		// This action repeats for all picked instances.  We want to set the current
		// selection to all instances that are created by this action.  Therefore,
		// reset the SOL only for the first instance.  Determine this by the last tick count run.
		// HOWEVER loops and the 'on collision' event re-triggers events, re-running the action
		// with the same tickcount.  To get around this, triggers and re-triggering events increment
		// the 'execcount', so each execution of the action has a different execcount even if not
		// the same tickcount.
		var cur_act = this.runtime.getCurrentAction();
		var reset_sol = false;
		
		if (cr.is_undefined(cur_act.extra["Spawn_LastExec"]) || cur_act.extra["Spawn_LastExec"] < this.runtime.execcount)
		{
			reset_sol = true;
			cur_act.extra["Spawn_LastExec"] = this.runtime.execcount;
		}
		
		var sol;
		
		// Pick just this instance, as long as it's a different type (else the SOL instances array is
		// potentially modified while in use)
		if (obj != this.type)
		{
			sol = obj.getCurrentSol();
			sol.select_all = false;
			
			if (reset_sol)
			{
				sol.instances.length = 1;
				sol.instances[0] = inst;
			}
			else
				sol.instances.push(inst);
				
			// Siblings aren't in instance lists yet, pick them manually
			if (inst.is_contained)
			{
				for (i = 0, len = inst.siblings.length; i < len; i++)
				{
					s = inst.siblings[i];
					sol = s.type.getCurrentSol();
					sol.select_all = false;
					
					if (reset_sol)
					{
						sol.instances.length = 1;
						sol.instances[0] = s;
					}
					else
						sol.instances.push(s);
				}
			}
		}
	};
	
	Acts.prototype.SetEffect = function (effect)
	{
		this.compositeOp = cr.effectToCompositeOp(effect);
		cr.setGLBlend(this, effect, this.runtime.gl);
		this.runtime.redraw = true;
	};
	
	Acts.prototype.StopAnim = function ()
	{
		this.animPlaying = false;
		//log("Stopping animation");
	};
	
	Acts.prototype.StartAnim = function (from)
	{
		this.animPlaying = true;
		this.frameStart = this.getNowTime();
		//log("Starting animation");
		
		// from beginning
		if (from === 1 && this.cur_frame !== 0)
		{
			this.changeAnimFrame = 0;
			
			if (!this.inAnimTrigger)
				this.doChangeAnimFrame();
		}
		
		// start ticking if not already
		if (!this.isTicking)
		{
			this.runtime.tickMe(this);
			this.isTicking = true;
		}
	};
	
	Acts.prototype.SetAnim = function (animname, from)
	{
		this.changeAnimName = animname;
		this.changeAnimFrom = from;
		
		// start ticking if not already
		if (!this.isTicking)
		{
			this.runtime.tickMe(this);
			this.isTicking = true;
		}
		
		// not in trigger: apply immediately
		if (!this.inAnimTrigger)
			this.doChangeAnim();
	};
	
	Acts.prototype.SetAnimFrame = function (framenumber)
	{
		this.changeAnimFrame = framenumber;
		
		// start ticking if not already
		if (!this.isTicking)
		{
			this.runtime.tickMe(this);
			this.isTicking = true;
		}
		
		// not in trigger: apply immediately
		if (!this.inAnimTrigger)
			this.doChangeAnimFrame();
	};
	
	Acts.prototype.SetAnimSpeed = function (s)
	{
		this.cur_anim_speed = cr.abs(s);
		this.animForwards = (s >= 0);
		
		//this.frameStart = this.runtime.kahanTime.sum;
		
		// start ticking if not already
		if (!this.isTicking)
		{
			this.runtime.tickMe(this);
			this.isTicking = true;
		}
	};
	
	Acts.prototype.SetMirrored = function (m)
	{
		var neww = cr.abs(this.width) * (m === 0 ? -1 : 1);
		
		if (this.width === neww)
			return;
			
		this.width = neww;
		this.set_bbox_changed();
	};
	
	Acts.prototype.SetFlipped = function (f)
	{
		var newh = cr.abs(this.height) * (f === 0 ? -1 : 1);
		
		if (this.height === newh)
			return;
			
		this.height = newh;
		this.set_bbox_changed();
	};
	
	Acts.prototype.SetScale = function (s)
	{
		var cur_frame = this.curFrame;
		var mirror_factor = (this.width < 0 ? -1 : 1);
		var flip_factor = (this.height < 0 ? -1 : 1);
		var new_width = cur_frame.width * s * mirror_factor;
		var new_height = cur_frame.height * s * flip_factor;
		
		if (this.width !== new_width || this.height !== new_height)
		{
			this.width = new_width;
			this.height = new_height;
			this.set_bbox_changed();
		}
	};
	
	Acts.prototype.LoadURL = function (url_, resize_)
	{
		var img = new Image();
		var self = this;
		var curFrame_ = this.curFrame;
		
		img.onload = function ()
		{
			// If this action was used on multiple instances, they will each try to create a
			// separate image or texture, which is a waste of memory. So if the same image has
			// already been loaded, ignore this callback.
			if (curFrame_.texture_img.src === img.src)
			{
				// Still may need to switch to using the image's texture in WebGL renderer
				if (self.runtime.glwrap && self.curFrame === curFrame_)
					self.curWebGLTexture = curFrame_.webGL_texture;
				
				// Still need to trigger 'On loaded'
				self.runtime.redraw = true;
				self.runtime.trigger(cr.plugins_.Sprite.prototype.cnds.OnURLLoaded, self);
			
				return;
			}
			
			curFrame_.texture_img = img;
			curFrame_.offx = 0;
			curFrame_.offy = 0;
			curFrame_.width = img.width;
			curFrame_.height = img.height;
			curFrame_.spritesheeted = false;
			curFrame_.datauri = "";
			curFrame_.pixelformat = 0;	// reset to RGBA, since we don't know what type of image will have come in
										// and it could be different to what the exporter set for the original image
			
			// WebGL renderer: need to create texture (canvas2D just draws with img directly)
			if (self.runtime.glwrap)
			{
				if (curFrame_.webGL_texture)
					self.runtime.glwrap.deleteTexture(curFrame_.webGL_texture);
					
				curFrame_.webGL_texture = self.runtime.glwrap.loadTexture(img, false, self.runtime.linearSampling);
				
				if (self.curFrame === curFrame_)
					self.curWebGLTexture = curFrame_.webGL_texture;
				
				// Need to update other instance's curWebGLTexture
				self.type.updateAllCurrentTexture();
			}
			
			// Set size if necessary
			if (resize_ === 0)		// resize to image size
			{
				self.width = img.width;
				self.height = img.height;
				self.set_bbox_changed();
			}
			
			self.runtime.redraw = true;
			self.runtime.trigger(cr.plugins_.Sprite.prototype.cnds.OnURLLoaded, self);
		};
		
		if (url_.substr(0, 5) !== "data:")
			img["crossOrigin"] = "anonymous";
		
		img.src = url_;
	};
	
	Acts.prototype.SetCollisions = function (set_)
	{
		if (this.collisionsEnabled === (set_ !== 0))
			return;		// no change
		
		this.collisionsEnabled = (set_ !== 0);
		
		if (this.collisionsEnabled)
			this.set_bbox_changed();		// needs to be added back to cells
		else
		{
			// remove from any current cells and restore to uninitialised state
			if (this.collcells.right >= this.collcells.left)
				this.type.collision_grid.update(this, this.collcells, null);
			
			this.collcells.set(0, 0, -1, -1);
		}
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.AnimationFrame = function (ret)
	{
		ret.set_int(this.cur_frame);
	};
	
	Exps.prototype.AnimationFrameCount = function (ret)
	{
		ret.set_int(this.cur_animation.frames.length);
	};
	
	Exps.prototype.AnimationName = function (ret)
	{
		ret.set_string(this.cur_animation.name);
	};
	
	Exps.prototype.AnimationSpeed = function (ret)
	{
		ret.set_float(this.animForwards ? this.cur_anim_speed : -this.cur_anim_speed);
	};
	
	Exps.prototype.ImagePointX = function (ret, imgpt)
	{
		ret.set_float(this.getImagePoint(imgpt, true));
	};
	
	Exps.prototype.ImagePointY = function (ret, imgpt)
	{
		ret.set_float(this.getImagePoint(imgpt, false));
	};
	
	Exps.prototype.ImagePointCount = function (ret)
	{
		ret.set_int(this.curFrame.image_points.length);
	};
	
	Exps.prototype.ImageWidth = function (ret)
	{
		ret.set_float(this.curFrame.width);
	};
	
	Exps.prototype.ImageHeight = function (ret)
	{
		ret.set_float(this.curFrame.height);
	};
	
	pluginProto.exps = new Exps();

}());