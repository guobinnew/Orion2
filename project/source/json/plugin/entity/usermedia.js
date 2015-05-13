// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.UserMedia = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	/////////////////////////////////////
	var pluginProto = cr.plugins_.UserMedia.prototype;
		
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
	
	typeProto.onRestoreWebGLContext = function ()
	{
		// No need to create textures if no instances exist, will create on demand
		if (this.is_family || !this.instances.length)
			return;
			
		var i, len, inst;
		
		for (i = 0, len = this.instances.length; i < len; i++)
		{
			inst = this.instances[i];
			
			if (inst.video_active && inst.v.videoWidth >= 0 && inst.v.videoHeight >= 0)
			{
				inst.webGL_texture = this.runtime.glwrap.createEmptyTexture(inst.v.videoWidth, inst.v.videoHeight, this.runtime.linearSampling);
			}
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
	
	var c2getUserMedia = navigator["getUserMedia"] || navigator["webkitGetUserMedia"] ||
            navigator["mozGetUserMedia"] || navigator["msGetUserMedia"];
			
	var c2URL = window["URL"] || window["webkitURL"] || window["mozURL"] || window["msURL"];
	
	var audioSources = [];
	var videoSources = [];
	
	var isRequesting = false;
	var CJS_CAMWIDTH = 640;
	var CJS_CAMHEIGHT = 480;
	var supportsSpeechRecognition = !!(window["webkitSpeechRecognition"] || window["speechRecognition"]);
	var ambientlux = -1;
	
	// Speech synthesis
	var speechSynthesis = window["speechSynthesis"] || window["webkitSpeechSynthesis"] || window["mozSpeechSynthesis"] || window["msSpeechSynthesis"];
	var SpeechSynthesisUtterance = window["SpeechSynthesisUtterance"] || window["webkitSpeechSynthesisUtterance"] || window["mozSpeechSynthesisUtterance"] || window["msSpeechSynthesisUtterance"];
	var speechVoices = null;
	var requestedVoices = false;
	
	function lazyGetVoices()
	{
		if (requestedVoices || !speechSynthesis)
			return;
		
		speechVoices = speechSynthesis["getVoices"]();
		requestedVoices = true;
	};
	
	function getVoiceCount()
	{
		lazyGetVoices();
		return speechVoices ? speechVoices.length : 0;
	};
	
	function getVoiceAt(i)
	{
		lazyGetVoices();
		if (!speechVoices)
			return null;
		if (i < 0 || i >= speechVoices.length)
			return null;
		return speechVoices[i];
	};
	
	function getVoiceByURI(uri)
	{
		lazyGetVoices();
		if (!speechVoices)
			return null;
			
		var i, len, v;
		for (i = 0, len = speechVoices.length; i < len; ++i)
		{
			v = speechVoices[i];
			
			if (v["voiceURI"] === uri)
				return v;
		}
		
		return null;
	};

	// called whenever an instance is created
	instanceProto.onCreate = function()
	{
		var self = this;
		
		this.v = document.createElement("video");
		
		// CocoonJS does not support video
		if (this.v)
		{
			this.v.autoplay = true;
			this.v.addEventListener("canplaythrough", function ()
			{
				if (self.video_active)
					self.video_ready = true;
			});
		}
		
		cr.setGLBlend(this, 0, this.runtime.gl);		// normal alpha blend
		
		this.video_active = false;
		this.audio_active = false;
		this.media_stream = null;
		this.runtime.tickMe(this);
		this.snapshot_data = "";
		this.webGL_texture = null;
		this.lastDecodedFrame = -1;
		this.video_ready = false;
		
		this.speechRecognition = null;
		this.finalTranscript = "";
		this.interimTranscript = "";
		this.isRecognisingSpeech = false;
		this.lastSpeechError = "";
		
		if (this.runtime.isWinJS && !this.runtime.isWindows8Capable)
		{
			var lightSensor = Windows["Devices"]["Sensors"]["LightSensor"]["getDefault"]();
			
			if (lightSensor)
			{
				var minInterval = lightSensor["minimumReportInterval"];
				lightSensor["reportInterval"] = (minInterval > 16 ? minInterval : 16);
				lightSensor.addEventListener("readingchanged", function (e)
				{
					ambientlux = e["reading"]["illuminanceInLux"];
					self.runtime.trigger(cr.plugins_.UserMedia.prototype.cnds.OnDeviceLight, self);
				});
			}
		}
		else
		{
			window.addEventListener("devicelight", function (e) {
				ambientlux = e["value"];
				self.runtime.trigger(cr.plugins_.UserMedia.prototype.cnds.OnDeviceLight, self);
			});
		}
	};
	
	instanceProto.onDestroy = function()
	{
		// Stop feed and release texture
		if (this.media_stream)
		{
			this.media_stream.stop();
			this.media_stream = null;
			this.video_active = false;
			this.video_ready = false;
			this.audio_active = false;
		}
		
		if (this.runtime.glwrap && this.webGL_texture)
		{
			this.runtime.glwrap.deleteTexture(this.webGL_texture);
			this.webGL_texture = null;
		}
	};
	
	instanceProto.tick = function ()
	{
		if (this.video_active && this.video_ready)
			this.runtime.redraw = true;
	};
	
	// only called if a layout object - draw to a canvas 2D context
	instanceProto.draw = function(ctx)
	{		
		ctx.globalAlpha = this.opacity;
		
		ctx.save();
		
		ctx.fillStyle = "#000";
		ctx.fillRect(this.x, this.y, this.width, this.height);
		
		if (this.video_active && this.video_ready)
		{
			// Firefox seems to have a bug where the first few frames throw "Component is not available"
			try {
				ctx.drawImage(this.v, this.x, this.y, this.width, this.height);
			}
			catch (e) {}
		}
		
		ctx.restore();
	};
	
	// only called if a layout object in WebGL mode - draw to the WebGL context
	// 'glw' is not a WebGL context, it's a wrapper - you can find its methods in GLWrap.js in the install
	// directory or just copy what other plugins do.
	instanceProto.drawGL = function (glw)
	{
		glw.setBlend(this.srcBlend, this.destBlend);
		glw.setOpacity(this.opacity);
		
		var q = this.bquad;
		
		if (!this.video_active || !this.video_ready || this.v.videoWidth <= 0 || this.v.videoHeight <= 0)
		{
			glw.setTexture(null);
			glw.quad(q.tlx, q.tly, q.trx, q.try_, q.brx, q.bry, q.blx, q.bly);
			return;
		}
			
		// Need to create texture
		if (!this.webGL_texture)
		{
			this.webGL_texture = glw.createEmptyTexture(this.v.videoWidth, this.v.videoHeight, this.runtime.linearSampling);
		}
		
		// Don't upload new pixels unless video frame has changed
		var framecount = this.v["webkitDecodedFrameCount"] || this.v["mozParsedFrames"];
		var updatetexture = false;
		
		if (!framecount)
			updatetexture = true;
		else if (framecount > this.lastDecodedFrame)
		{
			updatetexture = true;
			this.lastDecodedFrame = framecount;
		}
		
		if (updatetexture)
		{
			glw.videoToTexture(this.v, this.webGL_texture);
		}
		
		glw.setTexture(this.webGL_texture);
		glw.quad(q.tlx, q.tly, q.trx, q.try_, q.brx, q.bry, q.blx, q.bly);
	};
	
	/**BEGIN-PREVIEWONLY**/
	instanceProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": "User media",
			"properties": [
				{"name": "Video active", "value": this.video_active, "readonly": true},
				{"name": "Audio active", "value": this.audio_active, "readonly": true},
				{"name": "Ambient light", "value": ambientlux.toString() + " lux", "readonly": true}
			]
		});
		
		propsections.push({
			"title": "Speech recognition",
			"properties": [
				{"name": "Recongising speech", "value": this.isRecognisingSpeech, "readonly": true},
				{"name": "Interim transcript", "value": this.interimTranscript, "readonly": true},
				{"name": "Final transcript", "value": this.finalTranscript, "readonly": true},
				{"name": "Last speech error", "value": this.lastSpeechError, "readonly": true}
			]
		});
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	Cnds.prototype.OnApproved = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnDeclined = function ()
	{
		return true;
	};
	
	Cnds.prototype.SupportsUserMedia = function ()
	{
		return !!c2getUserMedia;
	};
	
	Cnds.prototype.SupportsSpeechRecognition = function ()
	{
		return supportsSpeechRecognition;
	};
	
	Cnds.prototype.OnSpeechRecognitionStart = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnSpeechRecognitionEnd = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnSpeechRecognitionError = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnSpeechRecognitionResult = function ()
	{
		return true;
	};
	
	Cnds.prototype.IsRecognisingSpeech = function ()
	{
		return this.isRecognisingSpeech;
	};
	
	Cnds.prototype.OnDeviceLight = function ()
	{
		return true;
	};
	
	Cnds.prototype.SupportsSpeechSynthesis = function ()
	{
		return !!speechSynthesis && !!SpeechSynthesisUtterance;
	};
	
	Cnds.prototype.IsSpeaking = function ()
	{
		return speechSynthesis && speechSynthesis["speaking"];
	};
	
	Cnds.prototype.OnMediaSources = function ()
	{
		return true;
	};
	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.RequestCamera = function (source_index)
	{
		if (isRequesting || !c2getUserMedia)
			return;			// already has info bar up or user media not supported
			
		isRequesting = true;
		var self = this;
		source_index = Math.floor(source_index);
		
		var constraints = {};
		
		// No valid source index provided, or media sources not requested
		if (source_index < 0 || source_index >= videoSources.length)
		{
			// Use default
			constraints["video"] = true;
		}
		else
		{
			// Request to use the given video source ID
			constraints["video"] = {
				"optional": [{
					"sourceId": videoSources[source_index]["id"]
				}]
			};
		}
		
		c2getUserMedia.call(navigator, constraints,
			function (localMediaStream)		// success
			{
				self.media_stream = localMediaStream;
				self.video_ready = false;		// wait for canplaythrough event before rendering
				self.v.src = c2URL.createObjectURL(localMediaStream);
				self.video_active = true;
				isRequesting = false;
				self.lastDecodedFrame = -1;
				
				if (self.runtime.glwrap && self.webGL_texture)
				{
					self.runtime.glwrap.deleteTexture(self.webGL_texture);
					self.webGL_texture = null;
				}
				
				self.runtime.trigger(cr.plugins_.UserMedia.prototype.cnds.OnApproved, self);
			},
			function (e)					// fail
			{
				isRequesting = false;
				self.runtime.trigger(cr.plugins_.UserMedia.prototype.cnds.OnDeclined, self);
			});
	};
	
	Acts.prototype.RequestMic = function (tag, source_index)
	{
		if (isRequesting || !c2getUserMedia)
			return;			// already has info bar up or user media not supported
			
		isRequesting = true;
		var self = this;
		source_index = Math.floor(source_index);
		
		var constraints = {};
		
		// No valid source index provided, or media sources not requested
		if (source_index < 0 || source_index >= audioSources.length)
		{
			// Use default
			constraints["audio"] = true;
		}
		else
		{
			// Request to use the given video source ID
			constraints["audio"] = {
				"optional": [{
					"sourceId": audioSources[source_index]["id"]
				}]
			};
		}
		
		c2getUserMedia.call(navigator, constraints,
			function (localMediaStream)		// success
			{
				self.media_stream = localMediaStream;
				
				// notify Audio plugin
				if (window["c2OnAudioMicStream"])
					window["c2OnAudioMicStream"](localMediaStream, tag);
				
				self.audio_active = true;
				isRequesting = false;
				self.runtime.trigger(cr.plugins_.UserMedia.prototype.cnds.OnApproved, self);
			},
			function ()						// fail
			{
				isRequesting = false;
				self.runtime.trigger(cr.plugins_.UserMedia.prototype.cnds.OnDeclined, self);
			});
	};
	
	Acts.prototype.Stop = function ()
	{
		if (this.media_stream)
		{
			this.media_stream.stop();
			this.media_stream = null;
			this.video_active = false;
			this.video_ready = false;
			this.audio_active = false;
		}
	};
	
	Acts.prototype.Snapshot = function (format_, quality_)
	{
		if (this.video_active && this.video_ready)
		{
			var tmpcanvas = document.createElement("canvas");
			tmpcanvas.width = this.v.videoWidth;
			tmpcanvas.height = this.v.videoHeight;
			var tmpctx = tmpcanvas.getContext("2d");
			tmpctx.drawImage(this.v, 0, 0, this.v.videoWidth, this.v.videoHeight);
			this.snapshot_data = tmpcanvas.toDataURL(format_ === 0 ? "image/png" : "image/jpeg", quality_ / 100);
		}
	};
	
	Acts.prototype.RequestSpeechRecognition = function (lang_, mode_, results_)
	{
		if (!supportsSpeechRecognition)
			return;
		
		if (window["webkitSpeechRecognition"])
			this.speechRecognition = new window["webkitSpeechRecognition"]();
		else if (window["speechRecognition"])
			this.speechRecognition = new window["speechRecognition"]();
		
		this.speechRecognition["lang"] = lang_;
		this.speechRecognition["continuous"] = (mode_ === 0);
		this.speechRecognition["interimResults"] = (results_ === 0);
		
		this.isRecognisingSpeech = false;		// reset until onstart fires
		this.interimTranscript = "";
		this.finalTranscript = "";
		var self = this;
		
		this.speechRecognition["onstart"] = function ()
		{
			self.isRecognisingSpeech = true;
			self.runtime.trigger(cr.plugins_.UserMedia.prototype.cnds.OnSpeechRecognitionStart, self);
		};
		
		this.speechRecognition["onend"] = function ()
		{
			self.isRecognisingSpeech = false;
			self.runtime.trigger(cr.plugins_.UserMedia.prototype.cnds.OnSpeechRecognitionEnd, self);
		};
		
		this.speechRecognition["onerror"] = function (e)
		{
			self.isRecognisingSpeech = false;
			self.lastSpeechError = e["error"] || "unknown";
			self.runtime.trigger(cr.plugins_.UserMedia.prototype.cnds.OnSpeechRecognitionError, self);
		};
		
		this.speechRecognition["onresult"] = function (e)
		{
			var i;
			
			self.interimTranscript = "";
			for (i = e["resultIndex"]; i < e["results"].length; ++i)
			{
				if (e["results"][i]["isFinal"])
					self.finalTranscript += e["results"][i][0]["transcript"];
				else
					self.interimTranscript += e["results"][i][0]["transcript"];
			}
			
			self.runtime.trigger(cr.plugins_.UserMedia.prototype.cnds.OnSpeechRecognitionResult, self);
		};
		
		this.speechRecognition["start"]();
	};
	
	Acts.prototype.StepSpeechRecognition = function ()
	{
		if (this.speechRecognition)
		{
			this.speechRecognition["stop"]();
			this.speechRecognition = null;
		}
	};
	
	function dbToLinear(x)
	{
		var v = Math.pow(10, x / 20);
		if (v < 0)
			v = 0;
		if (v > 1)
			v = 1;
		return v;
	};
	
	Acts.prototype.SpeakText = function (text, lang, uri, vol, rate, pitch)
	{
		if (!speechSynthesis || !SpeechSynthesisUtterance)
			return;
		
		var s = new SpeechSynthesisUtterance(text);
		
		if (lang)
			s["lang"] = lang;
		
		if (uri)
		{
			s["voice"] = getVoiceByURI(uri);	// not in the standard but seems to be used by Chrome
			s["voiceURI"] = uri;
		}
		
		s["volume"] = dbToLinear(vol);
		s["rate"] = rate;
		s["pitch"] = pitch;
		
		speechSynthesis["speak"](s);
	};
	
	Acts.prototype.StopSpeaking = function ()
	{
		if (!speechSynthesis)
			return;
		
		speechSynthesis["cancel"]();
	};
	
	Acts.prototype.PauseSpeaking = function ()
	{
		if (!speechSynthesis)
			return;
		
		speechSynthesis["pause"]();
	};
	
	Acts.prototype.ResumeSpeaking = function ()
	{
		if (!speechSynthesis)
			return;
		
		speechSynthesis["resume"]();
	};
	
	Acts.prototype.GetMediaSources = function ()
	{
		var self = this;
		
		audioSources.length = 0;
		videoSources.length = 0;
		
		var handleMediaSourcesList = function (media_sources)
		{
			var i, len, source, kind;
			for (i = 0, len = media_sources.length; i < len; ++i)
			{
				source = media_sources[i];
				kind = source["kind"];
				
				if (kind === "audio" || kind === "audioinput")
				{
					audioSources.push(source);
				}
				else if (kind === "video" || kind === "videoinput")
				{
					videoSources.push(source);
				}
				
				self.runtime.trigger(cr.plugins_.UserMedia.prototype.cnds.OnMediaSources, self);
			}
		};
		
		// New style API
		if (navigator["mediaDevices"] && navigator["mediaDevices"]["enumerateDevices"])
		{
			navigator["mediaDevices"]["enumerateDevices"]().then(handleMediaSourcesList);
		}
		// Old style API
		else if (window["MediaStreamTrack"] && window["MediaStreamTrack"]["getSources"])
		{
			window["MediaStreamTrack"]["getSources"](handleMediaSourcesList);
		}
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.VideoWidth = function (ret)
	{
		ret.set_int(this.v ? this.v.videoWidth : 0);
	};
	
	Exps.prototype.VideoHeight = function (ret)
	{
		ret.set_int(this.v ? this.v.videoHeight : 0);
	};
	
	Exps.prototype.SnapshotURL = function (ret)
	{
		ret.set_string(this.snapshot_data);
	};
	
	Exps.prototype.FinalTranscript = function (ret)
	{
		ret.set_string(this.finalTranscript);
	};
	
	Exps.prototype.InterimTranscript = function (ret)
	{
		ret.set_string(this.interimTranscript);
	};
	
	Exps.prototype.SpeechError = function (ret)
	{
		ret.set_string(this.lastSpeechError);
	};
	
	Exps.prototype.AmbientLux = function (ret)
	{
		ret.set_float(ambientlux);
	};
	
	Exps.prototype.VoiceCount = function (ret)
	{
		ret.set_int(getVoiceCount());
	};
	
	Exps.prototype.VoiceNameAt = function (ret, i)
	{
		i = Math.floor(i);
		var v = getVoiceAt(i);
		ret.set_string(v ? v["name"] : "");
	};
	
	Exps.prototype.VoiceLangAt = function (ret, i)
	{
		i = Math.floor(i);
		var v = getVoiceAt(i);
		ret.set_string(v ? v["lang"] : "");
	};
	
	Exps.prototype.VoiceURIAt = function (ret, i)
	{
		i = Math.floor(i);
		var v = getVoiceAt(i);
		ret.set_string(v ? v["voiceURI"] : "");
	};
	
	Exps.prototype.AudioSourceCount = function (ret)
	{
		ret.set_int(audioSources.length);
	};
	
	Exps.prototype.AudioSourceLabelAt = function (ret, i)
	{
		i = Math.floor(i);
		
		if (i < 0 || i >= audioSources.length)
		{
			ret.set_string("");
			return;
		}
		
		ret.set_string(audioSources[i]["label"] || "");
	};
	
	Exps.prototype.CameraSourceCount = function (ret)
	{
		ret.set_int(videoSources.length);
	};
	
	Exps.prototype.CameraSourceLabelAt = function (ret, i)
	{
		i = Math.floor(i);
		
		if (i < 0 || i >= videoSources.length)
		{
			ret.set_string("");
			return;
		}
		
		ret.set_string(videoSources[i]["label"] || "");
	};
	
	Exps.prototype.CameraSourceFacingAt = function (ret, i)
	{
		i = Math.floor(i);
		
		if (i < 0 || i >= videoSources.length)
		{
			ret.set_string("");
			return;
		}
		
		ret.set_string(videoSources[i]["facing"] || "");
	};
	
	pluginProto.exps = new Exps();

}());