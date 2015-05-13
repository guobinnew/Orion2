// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.video = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	/////////////////////////////////////
	var pluginProto = cr.plugins_.video.prototype;
		
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
	};
	
	typeProto.onLostWebGLContext = function ()
	{
		if (this.is_family)
			return;
			
		var i, len, inst;
		
		// Release all WebGL textures
		for (i = 0, len = this.instances.length; i < len; ++i)
		{
			inst = this.instances[i];
			inst.webGL_texture = null;		// will lazy create again on next draw
		}
	};
	
	var tmpVideo = document.createElement("video");
	var can_play_webm = !!tmpVideo.canPlayType("video/webm");
	var can_play_ogv = !!tmpVideo.canPlayType("video/ogg");
	var can_play_mp4 = !!tmpVideo.canPlayType("video/mp4");
	tmpVideo = null;
	
	function isVideoPlaying(v)
	{
		return v && !v.paused && !v.ended && v.currentTime > 0;
	};

	/////////////////////////////////////
	// Instance class
	pluginProto.Instance = function(type)
	{
		this.type = type;
		this.runtime = type.runtime;
	};
	
	var instanceProto = pluginProto.Instance.prototype;
	
	// Work around gesture limitations on mobile devices
	var playOnNextTouch = [];
	
	document.addEventListener("touchstart", function ()
	{
		var i, len;
		for (i = 0, len = playOnNextTouch.length; i < len; ++i)
		{
			playOnNextTouch[i].play();
		}
		
		playOnNextTouch.length = 0;
		
	}, true);
		
	instanceProto.queueVideoPlay = function (add)
	{
		if (!this.video)
			return;
		
		var i;
		
		// Remove from video play queue if present
		if (!add)
		{
			i = playOnNextTouch.indexOf(this.video);
			
			if (i >= 0)
				playOnNextTouch.splice(i, 1);
			
			return;
		}
		
		// Can only play videos in user input event and this is not a user input event: queue
		// for playback
		if (this.useNextTouchWorkaround && !this.runtime.isInUserInputEvent)
		{
			i = playOnNextTouch.indexOf(this.video);

			if (i === -1)
				playOnNextTouch.push(this.video);
		}
		else	// otherwise can play right away
			this.video.play();
	};

		
	// called whenever an instance is created
	instanceProto.onCreate = function()
	{
		this.webm_src = this.properties[0];
		this.ogv_src = this.properties[1];
		this.mp4_src = this.properties[2];
		
		this.autoplay = this.properties[3];					// 0 = no, 1 = preload, 2 = yes
		this.playInBackground = (this.properties[4] !== 0);	// 0 = no, 1 = yes
		this.videoWasPlayingOnSuspend = false;
		
		this.video = document.createElement("video");
		
		// Chrome for Android: has bug on <=4.4 where crossOrigin must be set to
		// be able to render in to webgl
		if (this.runtime.isChrome && this.runtime.isAndroid)
			this.video.crossOrigin = "anonymous";
		
		this.webGL_texture = null;
		this.lastDecodedFrame = -1;
		
		this.currentTrigger = -1;
		
		// IE11 hack: WebGL doesn't support drawing videos here, so render to a canvas then upload the canvas to a texture!
		this.viaCanvas = null;
		this.viaCtx = null;
		this.useViaCanvasWorkaround = this.runtime.isIE;
		
		var self = this;
		
		this.video.addEventListener("canplay", function () {
			self.currentTrigger = 0;
			self.runtime.trigger(cr.plugins_.video.prototype.cnds.OnPlaybackEvent, self);
		});
		
		this.video.addEventListener("canplaythrough", function () {
			self.currentTrigger = 1;
			self.runtime.trigger(cr.plugins_.video.prototype.cnds.OnPlaybackEvent, self);
		});
		
		this.video.addEventListener("ended", function () {
			self.currentTrigger = 2;
			self.runtime.trigger(cr.plugins_.video.prototype.cnds.OnPlaybackEvent, self);
		});
		
		this.video.addEventListener("error", function () {
			self.currentTrigger = 3;
			self.runtime.trigger(cr.plugins_.video.prototype.cnds.OnPlaybackEvent, self);
		});
		
		this.video.addEventListener("loadstart", function () {
			self.currentTrigger = 4;
			self.runtime.trigger(cr.plugins_.video.prototype.cnds.OnPlaybackEvent, self);
		});
		
		this.video.addEventListener("playing", function () {
			self.currentTrigger = 5;
			self.runtime.trigger(cr.plugins_.video.prototype.cnds.OnPlaybackEvent, self);
		});
		
		this.video.addEventListener("pause", function () {
			self.currentTrigger = 6;
			self.runtime.trigger(cr.plugins_.video.prototype.cnds.OnPlaybackEvent, self);
		});
		
		this.video.addEventListener("stalled", function () {
			self.currentTrigger = 7;
			self.runtime.trigger(cr.plugins_.video.prototype.cnds.OnPlaybackEvent, self);
		});
		
		// Work around iOS & Android's requirement of having a user gesture start playback
		this.useNextTouchWorkaround = ((this.runtime.isiOS || (this.runtime.isAndroid && (this.runtime.isChrome || this.runtime.isAndroidStockBrowser))) && !this.runtime.isCrosswalk && !this.runtime.isDomFree);
		
		// Set autoplay/preload setting
		if (this.autoplay === 0)
		{
			this.video.autoplay = false;
			this.video.preload = "none";
		}
		else if (this.autoplay === 1)
		{
			this.video.autoplay = false;
			this.video.preload = "auto";
		}
		else if (this.autoplay === 2)
		{
			this.video.autoplay = true;
			
			if (this.useNextTouchWorkaround)
				this.queueVideoPlay(true);
		}
		
		this.setSource(this.webm_src, this.ogv_src, this.mp4_src);
		
		// Safari on iOS does not support rendering video to canvas - either by canvas2d or texImage2d.
		// Work around by adding the video to the DOM and positioning it over the canvas.
		this.useDom = ((this.runtime.isSafari && this.runtime.isiOS) && !this.runtime.isDomFree);
		this.element_hidden = false;
		
		if (this.useDom)
		{
			jQuery(this.video).appendTo(this.runtime.canvasdiv ? this.runtime.canvasdiv : "body");
			
			// Turn off controls, context menu, and enable inline playing
			this.video.addEventListener("contextmenu", function (e) { e.preventDefault(); e.stopPropagation(); return false; }, false);
			
			if (this.video.hasAttribute("controls"))
				this.video.removeAttribute("controls")   
			
			this.video.setAttribute("webkit-playsinline", "");
			
			if (this.properties[5] === 0)		// initially invisible
			{
				jQuery(this.video).hide();
				this.visible = false;
				this.element_hidden = true;
			}
		}
		else
		{
			this.visible = (this.properties[5] !== 0);
		}
		
		this.lastLeft = 0;
		this.lastTop = 0;
		this.lastRight = 0;
		this.lastBottom = 0;
		this.lastWinWidth = 0;
		this.lastWinHeight = 0;
		
		if (this.useDom)
			this.updatePosition(true);
		
		// Tick this object so we can set to redraw if the video is playing
		this.runtime.tickMe(this);
		
		// Get suspend callbacks if should not play in background
		if (!this.recycled)
		{
			var self = this;
			
			this.runtime.addSuspendCallback(function(s)
			{
				self.onSuspend(s);
			});
		}
	};
	
	instanceProto.onSuspend = function (s)
	{
		if (this.playInBackground || !this.video)
			return;
		
		if (s)
		{
			if (isVideoPlaying(this.video))
			{
				this.queueVideoPlay(false);
				this.video.pause();
				this.videoWasPlayingOnSuspend = true;
			}
		}
		else
		{
			if (this.videoWasPlayingOnSuspend)
			{
				this.queueVideoPlay(true);
				this.videoWasPlayingOnSuspend = false;
			}
		}
	};
	
	instanceProto.setSource = function (webm_src, ogv_src, mp4_src)
	{
		// Assign src depending on supported format and available sources
		if (can_play_webm && webm_src)
			this.video.src = webm_src;
		else if (can_play_ogv && ogv_src)
			this.video.src = ogv_src;
		else if (can_play_mp4 && mp4_src)
			this.video.src = mp4_src;
		
		this.lastDecodedFrame = -1;
		
		// Delete WebGL texture and 2d canvas - will be recreated at new video
		// size when new video ready
		if (this.runtime.glwrap && this.webGL_texture)
		{
			this.runtime.glwrap.deleteTexture(this.webGL_texture);
			this.webGL_texture = null;
		}
		
		this.viaCanvas = null;
		this.viaCtx = null;
	};
	
	// called whenever an instance is destroyed
	// note the runtime may keep the object after this call for recycling; be sure
	// to release/recycle/reset any references to other objects in this function.
	instanceProto.onDestroy = function ()
	{
		this.queueVideoPlay(false);
		
		if (this.runtime.glwrap && this.webGL_texture)
		{
			this.runtime.glwrap.deleteTexture(this.webGL_texture);
			this.webGL_texture = null;
		}
		
		if (this.useDom)
			jQuery(this.video).remove();
		
		this.viaCanvas = null;
		this.viaCtx = null;
		this.video = null;
	};
	
	instanceProto.tick = function ()
	{
		if (isVideoPlaying(this.video) && !this.useDom)
			this.runtime.redraw = true;
		
		if (this.useDom)
			this.updatePosition();
	};
	
	instanceProto.updatePosition = function (first)
	{
		if (this.runtime.isDomFree || !this.useDom)
			return;
		
		var videoWidth = this.video.videoWidth;
		var videoHeight = this.video.videoHeight;
		
		// Set default size if metadata not yet loaded
		if (videoWidth <= 0)
			videoWidth = 320;
		if (videoHeight <= 0)
			videoHeight = 240;
		
		var videoAspect = videoWidth / videoHeight;
		var dispWidth = this.width;
		var dispHeight = this.height;
		var dispAspect = dispWidth / dispHeight;
		var offx = 0;
		var offy = 0;
		var drawWidth = 0;
		var drawHeight = 0;
		
		// aspect scale the video to the object's draw area
		if (dispAspect > videoAspect)
		{
			drawWidth = dispHeight * videoAspect;
			drawHeight = dispHeight;
			offx = Math.floor((dispWidth - drawWidth) / 2);
			
			if (offx < 0)
				offx = 0;
		}
		else
		{
			drawWidth = dispWidth;
			drawHeight = dispWidth / videoAspect;
			offy = Math.floor((dispHeight - drawHeight) / 2);
			
			if (offy < 0)
				offy = 0;
		}
		
		var ax = this.x + offx;
		var ay = this.y + offy;
		
		var left = this.layer.layerToCanvas(ax, ay, true);
		var top = this.layer.layerToCanvas(ax, ay, false);
		var right = this.layer.layerToCanvas(ax + drawWidth, ay + drawHeight, true);
		var bottom = this.layer.layerToCanvas(ax + drawWidth, ay + drawHeight, false);
		
		// Is entirely offscreen or invisible: hide
		if (!this.visible || !this.layer.visible || right <= 0 || bottom <= 0 || left >= this.runtime.width || top >= this.runtime.height)
		{
			if (!this.element_hidden)
				jQuery(this.video).hide();
				
			this.element_hidden = true;
			return;
		}
		
		// Truncate to canvas size
		if (left < 1)
			left = 1;
		if (top < 1)
			top = 1;
		if (right >= this.runtime.width)
			right = this.runtime.width - 1;
		if (bottom >= this.runtime.height)
			bottom = this.runtime.height - 1;
		
		var curWinWidth = window.innerWidth;
		var curWinHeight = window.innerHeight;
			
		// Avoid redundant updates
		if (!first && this.lastLeft === left && this.lastTop === top && this.lastRight === right && this.lastBottom === bottom && this.lastWinWidth === curWinWidth && this.lastWinHeight === curWinHeight)
		{
			if (this.element_hidden)
			{
				jQuery(this.video).show();
				this.element_hidden = false;
			}
			
			// On iOS, make a redundant update every half a second or so, since otherwise
			// returning from the fullscreen player doesn't position it correctly
			if (!this.runtime.isiOS || this.runtime.tickcount % 30 !== 0)
				return;
		}
			
		this.lastLeft = left;
		this.lastTop = top;
		this.lastRight = right;
		this.lastBottom = bottom;
		this.lastWinWidth = curWinWidth;
		this.lastWinHeight = curWinHeight;
		
		if (this.element_hidden)
		{
			jQuery(this.video).show();
			this.element_hidden = false;
		}
		
		var offx = Math.round(left) + jQuery(this.runtime.canvas).offset().left;
		var offy = Math.round(top) + jQuery(this.runtime.canvas).offset().top;
		jQuery(this.video).css("position", "absolute");
		jQuery(this.video).offset({left: offx, top: offy});
		jQuery(this.video).width(Math.round(right - left));
		jQuery(this.video).height(Math.round(bottom - top));
	};
	
	// called when saving the full state of the game
	instanceProto.saveToJSON = function ()
	{
		// return a Javascript object containing information about your object's state
		// note you MUST use double-quote syntax (e.g. "property": value) to prevent
		// Closure Compiler renaming and breaking the save format
		return {
			// e.g.
			//"myValue": this.myValue
		};
	};
	
	// called when loading the full state of the game
	instanceProto.loadFromJSON = function (o)
	{
		// load from the state previously saved by saveToJSON
		// 'o' provides the same object that you saved, e.g.
		// this.myValue = o["myValue"];
		// note you MUST use double-quote syntax (e.g. o["property"]) to prevent
		// Closure Compiler renaming and breaking the save format
	};
	
	// only called if a layout object - draw to a canvas 2D context
	instanceProto.draw = function (ctx)
	{
		if (!this.video || this.useDom)
			return;		// no video to draw or using off-canvas DOM element
		
		// Render video with correct aspect ratio to the object rectangle area
		var videoWidth = this.video.videoWidth;
		var videoHeight = this.video.videoHeight;
		
		if (videoWidth <= 0 || videoHeight <= 0)
			return;		// not yet loaded metadata
		
		var videoAspect = videoWidth / videoHeight;
		var dispWidth = this.width;
		var dispHeight = this.height;
		var dispAspect = dispWidth / dispHeight;
		var offx = 0;
		var offy = 0;
		var drawWidth = 0;
		var drawHeight = 0;
		
		// aspect scale the video to the object's draw area
		if (dispAspect > videoAspect)
		{
			drawWidth = dispHeight * videoAspect;
			drawHeight = dispHeight;
			offx = Math.floor((dispWidth - drawWidth) / 2);
			
			if (offx < 0)
				offx = 0;
		}
		else
		{
			drawWidth = dispWidth;
			drawHeight = dispWidth / videoAspect;
			offy = Math.floor((dispHeight - drawHeight) / 2);
			
			if (offy < 0)
				offy = 0;
		}
		
		ctx.globalAlpha = this.opacity;
		ctx.drawImage(this.video, this.x + offx, this.y + offy, drawWidth, drawHeight);
	};
	
	var tmpRect = new cr.rect(0, 0, 0, 0);
	var tmpQuad = new cr.quad();
	
	// only called if a layout object in WebGL mode - draw to the WebGL context
	// 'glw' is not a WebGL context, it's a wrapper - you can find its methods in GLWrap.js in the install
	// directory or just copy what other plugins do.
	instanceProto.drawGL = function (glw)
	{
		if (!this.video || this.useDom)
			return;		// no video to draw or using off-canvas DOM element
		
		// Render video with correct aspect ratio to the object rectangle area
		var videoWidth = this.video.videoWidth;
		var videoHeight = this.video.videoHeight;
		
		if (videoWidth <= 0 || videoHeight <= 0)
			return;		// not yet loaded metadata
		
		var videoAspect = videoWidth / videoHeight;
		var dispWidth = this.width;
		var dispHeight = this.height;
		var dispAspect = dispWidth / dispHeight;
		var offx = 0;
		var offy = 0;
		var drawWidth = 0;
		var drawHeight = 0;
		
		// aspect scale the video to the object's draw area
		if (dispAspect > videoAspect)
		{
			drawWidth = dispHeight * videoAspect;
			drawHeight = dispHeight;
			offx = Math.floor((dispWidth - drawWidth) / 2);
			
			if (offx < 0)
				offx = 0;
		}
		else
		{
			drawWidth = dispWidth;
			drawHeight = dispWidth / videoAspect;
			offy = Math.floor((dispHeight - drawHeight) / 2);
			
			if (offy < 0)
				offy = 0;
		}
		
		var updatetexture = false;
		
		// Lazy-create texture
		if (!this.webGL_texture)
		{
			this.webGL_texture = glw.createEmptyTexture(videoWidth, videoHeight, this.runtime.linearSampling, false, false);
			updatetexture = true;
		}
		
		// Don't upload new pixels unless video frame has changed
		var framecount = this.video["webkitDecodedFrameCount"] || this.video["mozDecodedFrames"];
		
		if (!framecount)
		{
			updatetexture = true;		// frame count not available, have to update every frame
		}
		else if (framecount > this.lastDecodedFrame)
		{
			updatetexture = true;
			this.lastDecodedFrame = framecount;
		}
		
		if (updatetexture)
		{
			if (this.useViaCanvasWorkaround)
			{
				// Can't upload videos directly to a texture, so render via a canvas 2D context.
				// If the context does not yet exist, create it.
				if (!this.viaCtx)
				{
					this.viaCanvas = document.createElement("canvas");
					this.viaCanvas.width = videoWidth;
					this.viaCanvas.height = videoHeight;
					this.viaCtx = this.viaCanvas.getContext("2d");
				}
				
				this.viaCtx.drawImage(this.video, 0, 0);
				glw.videoToTexture(this.viaCanvas, this.webGL_texture);
			}
			else
			{
				glw.videoToTexture(this.video, this.webGL_texture);
			}
		}
		
		glw.setBlend(this.srcBlend, this.destBlend);
		glw.setOpacity(this.opacity);
		glw.setTexture(this.webGL_texture);
		
		tmpRect.set(this.x + offx, this.y + offy, this.x + offx + drawWidth, this.y + offy + drawHeight);
		tmpQuad.set_from_rect(tmpRect);
		glw.quad(tmpQuad.tlx, tmpQuad.tly, tmpQuad.trx, tmpQuad.try_, tmpQuad.brx, tmpQuad.bry, tmpQuad.blx, tmpQuad.bly);
	};
	
	// The comments around these functions ensure they are removed when exporting, since the
	// debugger code is no longer relevant after publishing.
	/**BEGIN-PREVIEWONLY**/
	instanceProto.getDebuggerValues = function (propsections)
	{
		// Append to propsections any debugger sections you want to appear.
		// Each section is an object with two members: "title" and "properties".
		// "properties" is an array of individual debugger properties to display
		// with their name and value, and some other optional settings.
		propsections.push({
			"title": "My debugger section",
			"properties": [
				// Each property entry can use the following values:
				// "name" (required): name of the property (must be unique within this section)
				// "value" (required): a boolean, number or string for the value
				// "html" (optional, default false): set to true to interpret the name and value
				//									 as HTML strings rather than simple plain text
				// "readonly" (optional, default false): set to true to disable editing the property
				
				// Example:
				// {"name": "My property", "value": this.myValue}
			]
		});
	};
	
	instanceProto.onDebugValueEdited = function (header, name, value)
	{
		// Called when a non-readonly property has been edited in the debugger. Usually you only
		// will need 'name' (the property name) and 'value', but you can also use 'header' (the
		// header title for the section) to distinguish properties with the same name.
		if (name === "My property")
			this.myProperty = value;
	};
	/**END-PREVIEWONLY**/
	
	function dbToLinear_nocap(x)
	{
		return Math.pow(10, x / 20);
	};
	
	function linearToDb_nocap(x)
	{
		return (Math.log(x) / Math.log(10)) * 20;
	};
	
	function dbToLinear(x)
	{
		var v = dbToLinear_nocap(x);
		if (v < 0)
			v = 0;
		if (v > 1)
			v = 1;
		return v;
	};
	
	function linearToDb(x)
	{
		if (x < 0)
			x = 0;
		if (x > 1)
			x = 1;
		return linearToDb_nocap(x);
	};

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	Cnds.prototype.IsPlaying = function ()
	{
		return isVideoPlaying(this.video);
	};
	
	Cnds.prototype.IsPaused = function ()
	{
		return this.video.paused;
	};
	
	Cnds.prototype.HasEnded = function ()
	{
		return this.video.ended;
	};
	
	Cnds.prototype.IsMuted = function ()
	{
		return this.video.muted;
	};
	
	Cnds.prototype.OnPlaybackEvent = function (trig)
	{
		return this.currentTrigger === trig;
	};
	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.SetSource = function (webm_src, ogv_src, mp4_src)
	{
		this.setSource(webm_src, ogv_src, mp4_src);
		this.video.load();
	};
	
	Acts.prototype.SetPlaybackTime = function (s)
	{
		try {
			this.video.currentTime = s;
		}
		catch (e)
		{
			if (console && console.error)
				console.error("Exception setting video playback time: ", e);
		}
	};
	
	Acts.prototype.SetLooping = function (l)
	{
		this.video.loop = (l !== 0);
	};
	
	Acts.prototype.SetMuted = function (m)
	{
		this.video.muted = (m !== 0);
	};
	
	Acts.prototype.SetVolume = function (v)
	{
		this.video.volume = dbToLinear(v);
	};
	
	Acts.prototype.Pause = function ()
	{
		this.queueVideoPlay(false);		// remove any play-on-next-touch queue, since we don't want it to be playing any more
		this.video.pause();
	};
	
	Acts.prototype.Play = function ()
	{
		this.queueVideoPlay(true);
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.PlaybackTime = function (ret)
	{
		ret.set_float(this.video.currentTime || 0);
	};
	
	Exps.prototype.Duration = function (ret)
	{
		ret.set_float(this.video.duration || 0);
	};
	
	Exps.prototype.Volume = function (ret)
	{
		ret.set_float(linearToDb(this.video.volume || 0));
	};
	
	pluginProto.exps = new Exps();

}());