// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.Spriter = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var pluginProto = cr.plugins_.Spriter.prototype;
	
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
		this.scmlFiles=[];
		this.objectArrays=[];
	};
	
	/////////////////////////////////////
	// Instance class

	pluginProto.Instance = function(type)
	{
		this.type = type;
		this.runtime = type.runtime;
		
		this.lastData = "";		
		this.progress = 0;
		
		this.entity = 0;
		this.entities = [];
		
		this.currentSpriterTime = 0;
		this.currentAdjustedTime = 0;
		this.secondTime = 0;
		
		this.start_time = cr.performance_now();
		this.lastKnownTime = this.getNowTime();
	};

	
	var instanceProto = pluginProto.Instance.prototype;
	instanceProto.setEntitiesToOtherEntities = function(otherEntities)
	{
		var NO_INDEX=-1;
		var entityTags = otherEntities;
		var att=0;
		for (var e = 0; e < entityTags.length; e++)
		{
			var entityTag=entityTags[e];
			att=entityTag;
			var entity = new SpriterEntity();
			att=entityTag;
			entity.name=att.name;
			var animationTags = entityTag.animations;
			for (var a = 0; a < animationTags.length; a++)
			{
				var animationTag = animationTags[a];
				att=animationTag;
				var animation = new SpriterAnimation();
				animation.name=att.name;
				animation.length=att.length;
				animation.looping = att.looping;
				animation.loopTo = att.loop_to;
				
				var mainlineTag = animationTag.mainlineKeys;
				
				var mainline = new SpriterTimeline();
				
				
				var keyTags = mainlineTag;		
				for (var k = 0; k<keyTags.length; k++)
				{
					var keyTag = keyTags[k];
					
					var key = new SpriterKey();
					att=keyTag;
					key.time = att.time;
					key.curveType=att.curveType;
					key.c1=att.c1;
								key.c2=att.c2;
								key.c3=att.c3;
								key.c4=att.c4;
					var boneRefTags = keyTag.bones;	
					if(boneRefTags)
					{
						for (var o = 0; o < boneRefTags.length; o++)
						{
							var boneRefTag=boneRefTags[o];
							att=boneRefTag;
							var boneRef = new SpriterObjectRef();
							boneRef.timeline=att.timeline;
							boneRef.key=att.key;
							boneRef.parent = att.parent;
							key.bones.push(boneRef);
						}		
					}
					
					var objectRefTags = keyTag.objects;	
					if(objectRefTags)
					{
						for (var o = 0; o < objectRefTags.length; o++)
						{
							var objectRefTag=objectRefTags[o];
							att=objectRefTag;
							var objectRef = new SpriterObjectRef();
							objectRef.timeline=att.timeline;
							objectRef.key=att.key;
							objectRef.parent = att.parent;
							key.objects.push(objectRef);
						}		
					}
					
					animation.mainlineKeys.push(key);
				}	
				
				animation.mainline=mainline;
				var timelineTags = animationTag.timelines;
				if(timelineTags)
				{
					for (var t = 0; t < timelineTags.length; t++)
					{
						var timelineTag=timelineTags[t];
						
						att=timelineTag;
						
						var timeline = new SpriterTimeline();
						timeline.objectType = att.objectType;
						
						var timelineName=att.name;
						timeline.name=timelineName;
						
						var keyTags = timelineTag.keys;		
						if(keyTags)
						{
							
							for (var k = 0; k<keyTags.length; k++)
							{
								var keyTag = keyTags[k];
								
								var key = new SpriterKey();
								att=keyTag;
								
								key.time = att.time;
								key.spin = (att.spin);
								key.curveType=att.curveType;
								key.c1=att.c1;
								key.c2=att.c2;
								key.c3=att.c3;
								key.c4=att.c4;
								var objectTags = keyTag.objects;
								if(objectTags)
								{
									for(var o=0; o<objectTags.length; o++)
									{			
										var objectTag=objectTags[o];
										var object=CloneObject(objectTag);
										key.objects.push(object);
									}
								}
								var boneTags = keyTag.bones;
								if(boneTags)
								{
									for(var o=0; o<boneTags.length; o++)
									{			
										var boneTag=boneTags[o];
										var bone=CloneObject(boneTag);
										key.bones.push(bone);
									}
								}
								timeline.keys.push(key);
							}		
						}
						timeline.c2Object=this.c2ObjectArray[findObjectItemInArray(timelineName,this.objectArray,entity.name)];
						animation.timelines.push(timeline);
					}
				}
				entity.animations.push(animation);

			}
			this.entities.push(entity);
			if(!this.entity||this.properties[1]===entity.name)
			{
				this.entity=entity;
			}
		}
		
		
	};
	instanceProto.getCharacterFromPreload = function()
	{
		if(this.type.scmlFiles.hasOwnProperty(this.properties[0]))
		{	
			this.setEntitiesToOtherEntities(this.type.scmlFiles[this.properties[0]]);
			if(this.type.objectArrays.hasOwnProperty(this.properties[0]))
			{
				this.objectArray=this.type.objectArrays[this.properties[0]];
			}
			
			this.c2ObjectArray=this.generateTestC2ObjectArray(this.objectArray);
			
			if(this.startingEntName)
			{
				this.setEntTo(this.startingEntName);
			}
			else
			{
				this.setEntTo(this.properties[1]);
			}
			
			this.associateAllTypes();
			this.initDOMtoPairedObjects();
			
			if(this.startingAnimName)
			{
				this.setAnimTo(this.startingAnimName);
			}
			else
			{
				this.setAnimTo(this.properties[2]);
			}	
			
			if(!this.currentAnimation&&this.entity&&this.entity.animations.length)
			{
				this.setAnimTo(this.entity.animations[0].name);
			}	
			if(this.startingLoopType&&this.currentAnimation)
			{
				this.currentAnimation.looping=startingLoopType;
			}
			return true;
		}
		//else
		return false;
	}
	var PAUSENEVER=0;
	var PAUSEALLOUTSIDEBUFFER=1;
	var PAUSEALLBUTSOUNDOUTSIDEBUFFER=2;
	
	instanceProto.onCreate = function()
	{
		//this.xmlDoc = null;
		this.nodeStack = [];
		this.isDestroyed=false;
		//this.cur_frame = 0;
		this.folders = [];
		
		this.currentAnimation = "";
		this.secondAnimation = "";
		this.animBlend=0.0;
		this.blendStartTime=0.0;
		this.blendEndTime=0.0;
		this.blendPoseTime=0.0;
		
		this.lastKnownInstDataAsObj = null;
		this.c2ObjectArray = [];
		this.objectArray=[];
		this.animPlaying = true;
		this.speedRatio=1.0;
		
		this.scaleRatio=this.width/50.0;
		this.xFlip=false;
		this.yFlip=false;
		this.playTo=-1;
		this.changeToStartFrom=0;
		this.runtime.tick2Me(this);
		this.startingEntName=null;
		this.startingAnimName=null;
		this.startingLoopType=null;
		
		this.leftBuffer=0;
		this.rightBuffer=0;
		this.topBuffer=0;
		this.bottomBuffer=0;
		this.pauseWhenOutsideBuffer=PAUSENEVER;
		
		this.properties[0]=this.properties[0].toLowerCase();
		if(this.properties[0].lastIndexOf(".scml")>-1)
		{
			this.properties[0]=this.properties[0].replace(".scml",".scon");
		}
		if(!this.getCharacterFromPreload())
		{
			var self=this;
			var request = null;
			
			var doErrorFunc = function ()
			{
				
			};
			
			var errorFunc = function ()
			{
				
			};
			
			var progressFunc = function (e)
			{
				
			};
			
			request = new XMLHttpRequest();
			var isNodeWebkit=this.runtime.isNodeWebkit;
			request.onreadystatechange = function() 
			{
				// Note: node-webkit leaves status as 0 for local AJAX requests, presumably because
				// they are local requests and don't have a HTTP response.  So interpret 0 as success
				// in this case.
				
				if (request.readyState === 4&&!self.isDestroyed)
				{
					if (request.status >= 400)
					{
						log("[Construct 2] scml object("+self.type.name+"): XMLHttpRequest failed with request status " + request.status + "when attempting to retrieve " + self.properties[0], "warn");
					}
					else
					{
						request.responseText.replace(/\r\n/g, "\n");// fix windows style line endings
						self.doRequest(JSON.parse(request.responseText));		
						if(self.startingEntName)
						{
							self.setEntTo(self.startingEntName);
						}
						
						if(self.startingAnimName)
						{
							self.setAnimTo(self.startingAnimName);
						}
						if(self.startingLoopType&&self.currentAnimation)
						{
							self.currentAnimation.looping=self.startingLoopType;
						}
					}
				}
			};
			request.onerror = errorFunc;
			request.ontimeout = errorFunc;
			request.onabort = errorFunc;
			request["onprogress"] = progressFunc;
			
			request.open("GET", this.properties[0]);
			request.send();

			
		}
		
		
		this.force=false;
		this.inAnimTrigger=false;
		this.changeAnimTo=null;
	};
		
	instanceProto.onDestroy = function ()
	{
		this.isDestroyed=true;
	};
	
	function SpriterEntity()
	{
		this.name = "";
		this.animations = [];
	}
	
	function SpriterAnimation()
	{
		this.name = "";
		this.length = 1;
		this.looping = "true";
		this.loopTo = 0;
		this.mainlineKeys = [];
		this.timelines = [];
		this.soundlines = [];
		this.cur_frame = 0;
		this.localTime=0;
	}
	
	function SpriterTimeline()
	{
		this.keys = [];
		this.name = "";
		this.c2Object = 0;
		this.objectType="sprite";
		this.currentObjectState={};
		this.currentMappedState={};
		this.lastTimeSoundCheck=0;
	}
	
	function SpriterKey()
	{
		this.bones = [];
		this.objects = [];
		this.time = 0;
		this.spin = 1;
		this.curveType="linear";
		this.c1=0;
		this.c2=0;
		this.c3=0;
		this.c4=0;
	}
	
	function SpriterObject()
	{
		this.type = "sprite";
		this.x = 0;
		this.y = 0;
		this.angle = 0;
		this.a = 1;
		this.xScale = 1;
		this.yScale = 1;
		this.pivotX = 0.0;
		this.pivotY = 0.0;
		this.defaultPivot = false;
		this.frame = 0;
		this.storedFrame = 0;
	}
	
	function SpriterSound()
	{
		this.type = "sound";
		this.name = "";
		this.trigger =true;
		this.panning=0.0;
		this.volume=1.0;
	}
	
	function CloneObject(other)
	{
		if(other)
		{
			var newObj=new SpriterObject();
			newObj.type = other.type;
			newObj.x = other.x;
			newObj.y = other.y;
			newObj.angle = other.angle;
			newObj.a = other.a;
			newObj.xScale = other.xScale;
			newObj.yScale = other.yScale;
			newObj.pivotX = other.pivotX;
			newObj.pivotY = other.pivotY;
			newObj.defaultPivot = other.defaultPivot;
			newObj.frame = other.frame; 
			newObj.storedFrame = newObj.storedFrame;
			return newObj;
		}
		else
		{
			return null;
		}
	}
	
	function sampleCurve(a,b,c,t) 
	{
		return ((a*t+b)*t+c)*t;
	}

	function sampleCurveDerivativeX(ax, bx, cx, t)  
	{
		return (3.0*ax*t+2.0*bx)*t+cx;
	}
	// The epsilon value to pass given that the animation is going to run over |dur| seconds. The longer the
	// animation, the more precision is needed in the timing function result to avoid ugly discontinuities.
	function solveEpsilon(duration)  
	{
		return 1.0/(200.0*duration);
	}
	function solve(ax,bx,cx,ay,by,cy,x,epsilon)  
	{
		return sampleCurve(ay,by,cy,solveCurveX(ax,bx,cx,x,epsilon));
	}
	// Given an x value, find a parametric value it came from.
	function fabs(n)  
	{	
		if(n>=0) 
		{
			return n;
		}
		else 
		{
			return 0-n;
		}
	}

	function solveCurveX(ax,bx,cx,x,epsilon) 
	{
		var t0;
		var t1;
		var t2;
		var x2;
		var d2;
		var i;

		// First try a few iterations of Newton's method -- normally very fast.
		for(t2=x, i=0; i<8; i++) 
		{
			x2=sampleCurve(ax,bx,cx,t2)-x; 
			if(fabs(x2)<epsilon) 
			{
				return t2;
			} 
			d2=sampleCurveDerivativeX(ax,bx,cx,t2); 
			if(fabs(d2)<1e-6) 
			{
				break;
			} 
			t2=t2-x2/d2;
		}
		// Fall back to the bisection method for reliability.
		t0=0.0; 
		t1=1.0; 
		t2=x; 
		if(t2<t0) 
		{
			return t0;
		} 
		if(t2>t1) 
		{
			return t1;
		}
		while(t0<t1) 
		{
			x2=sampleCurve(ax,bx,cx,t2); 
			if(fabs(x2-x)<epsilon) 
			{
				return t2;
			} 
			if(x>x2) 
			{
				t0=t2;
			}
			else 
			{
				t1=t2;
			} 
			t2=(t1-t0)*.5+t0;
		}
		return t2; // Failure.
	}

	// currently used function to determine time
	// 1:1 conversion to js from webkit source files
	// UnitBezier.h, WebCore_animation_AnimationBase.cpp
	function CubicBezierAtTime(t,p1x,p1y,p2x,p2y,duration) 
	{
		var ax=0;
		var bx=0;
		var cx=0;
		var ay=0;
		var by=0;
		var cy=0;
		// `ax t^3 + bx t^2 + cx t' expanded using Horner's rule.


		// Calculate the polynomial coefficients, implicit first and last control points are (0,0) and (1,1).
		cx=3.0*p1x;
		bx=3.0*(p2x-p1x)-cx;
		ax=1.0-cx-bx;
		cy=3.0*p1y;
		by=3.0*(p2y-p1y)-cy;
		ay=1.0-cy-by;
		// Convert from input time to parametric value in curve, then from that to output time.
		return solve(ax,bx,cx,ay,by,cy,t, solveEpsilon(duration));
	}
	function getT(a,b,x)
	{
		if(a===b)
		{
			return 0;
		}
		//else	
		return (x-a)/(b-a);
	}
	function qerp(a,b,c,t)
	{
		return cr.lerp(cr.lerp(a,b,t),cr.lerp(b,c,t),t);
	}
	function cerp(a,b,c,d,t)
	{
		return cr.lerp(qerp(a,b,c,t),qerp(b,c,d,t),t);
	}
	function quartic(a,b,c,d,e,t)
	{
		return cr.lerp(cerp(a,b,c,d,t),cerp(b,c,d,e,t),t);
	}
	function quintic(a,b,c,d,e,f,t)
	{
		return cr.lerp(quartic(a,b,c,d,e,t),quartic(b,c,d,e,f,t),t);
	}

	function trueT(key,t)
	{
		switch(key.curveType)
		{
		case "linear":
			return t;
		case "quadratic":
			return qerp(0,key.c1,1,t);
		case "cubic":
			return cerp(0,key.c1,key.c2,1,t);
		case "quartic":
			return quartic(0,key.c1,key.c2,key.c3,1,t);
		case "quintic":
			return quintic(key.c1,key.c2,key.c3,key.c4,t);
		case "bezier":
			 return CubicBezierAtTime(t,key.c1,key.c2,key.c3,key.c4,1.0);
			//return quintic(0,c.value(0,0),c.value(1,0),c.value(2,0),c.value(3,0),1,t);
		case "instant":
			if(t>=1)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		//case CURVETYPE_INSTANT or invalid thing;
		return 0;
	}
	
	function TweenedSpriterObject(a,b,t,spin,wFactor,hFactor)
	{
		wFactor = typeof wFactor !== 'undefined' ? wFactor : 1;
		hFactor = typeof hFactor !== 'undefined' ? hFactor : 1;
		var newObj=new SpriterObject();
		newObj.type = a.type;
		newObj.x = cr.lerp(a.x,b.x,t);
		newObj.y = cr.lerp(a.y,b.y,t);
		newObj.angle = anglelerp2(a.angle,b.angle,t,spin);
		newObj.a = cr.lerp(a.a,b.a,t);
		newObj.xScale = cr.lerp(a.xScale,b.xScale,t);
		newObj.yScale = cr.lerp(a.yScale,b.yScale,t);
		newObj.pivotX = a.pivotX;//cr.lerp(a.pivotX,b.pivotX,t);
		newObj.pivotY = a.pivotY;//cr.lerp(a.pivotY,b.pivotY,t);
		newObj.defaultPivot = a.defaultPivot;
		newObj.frame = a.frame;
		newObj.storedFrame=a.storedFrame;
		return newObj;
	}
	function TweenedSpriterSound(a,b,t)
	{
		var newSound=new SpriterSound();
		newSound.trigger = a.trigger;
		newSound.volume = cr.lerp(a.volume,b.volume,t);
		newSound.panning = cr.lerp(a.panning,b.panning,t);
		newSound.name =a.name;
		return newSound;
	}
	function SpriterObjectRef()
	{
		this.type = "reference";
		this.timeline = 0;
		this.key = 0;
		this.parent = -1;
	}
	
	function SpriterFolder()
	{
		this.files = [];
	}
	
	function SpriterFile()
	{
		this.fileName="";
		this.pivotX=0;
		this.pivotY=0;
		this.w=1;
		this.h=1;
	}
	
	function SetSpriteAnimFrame(sprite,framenumber,c2Object)
	{
		if(!sprite)
		{
			return;
		}
		if(c2Object&&c2Object.appliedMap[framenumber]!==undefined)
		{
			if(c2Object.appliedMap[framenumber]===-1)
			{
				sprite.visible=false;
			}
			else
			{
				framenumber=c2Object.appliedMap[framenumber];
			}
		}
		sprite.changeAnimFrame = framenumber;
		
		// start ticking if not already
		if (!sprite.isTicking)
		{
			sprite.runtime.tickMe(sprite);
			sprite.isTicking = true;
		}
		
		// not in trigger: apply immediately
		if (!sprite.inAnimTrigger)
		{
			sprite.doChangeAnimFrame();
		}
	}
	
	function anglelerp(a, b, x)
	{
		//a = cr.to_radians(a);
		//b = cr.to_radians(b);
		var diff = cr.angleDiff(a, b);
		
		// b clockwise from a
		if (cr.angleClockwise(b, a))
		{
			return (a + diff * x);
		}
		// b anticlockwise from a
		else
		{
			return (a - diff * x);
		}
	}
	function anglelerp2(a, b, x, spin)
	{
		//a = cr.to_radians(a);
		//b = cr.to_radians(b);
		if(spin===0)
		{
			return a;
		}
		var diff = cr.angleDiff(a, b);
		
		// b clockwise from a
		if (spin==-1)
		{
			return (a + diff * x);
		}
		// b anticlockwise from a
		else 
		{
			return (a - diff * x);
		}
	}
	function findInArray(item,arr)
	{
		for(var i=0;i<arr.length;i++)
		{
			if(arr[i]==item)
			{
				return i;
			}
		}
		return -1;
	}
	instanceProto.isOutsideViewportBox = function()
	{
		var layer = (this.layer);
		if(layer)
		{
			if(this.x<layer.viewLeft-this.leftBuffer)
			{
				return true;
			}
			if(this.x>layer.viewRight+this.rightBuffer)
			{
				return true;
			}
			if(this.y<layer.viewTop-this.topBuffer)
			{
				return true;
			}
			if(this.y>layer.viewBottom+this.bottomBuffer)
			{
				return true;
			}
		}
		return false;
	}
	instanceProto.MoveToLayer = function (inst,layerMove)
	{
		// no layer or same layer: don't do anything
		if (!layerMove || layerMove == inst.layer)
		{
		return;
		}
		
		// otherwise remove from current layer...
		cr.arrayRemove(inst.layer.instances, inst.get_zindex());
		inst.layer.zindices_stale = true;
		
		// ...and add to the top of the new layer (which can be done without making zindices stale)
		inst.layer = layerMove;
		inst.zindex = layerMove.instances.length;
		layerMove.instances.push(inst);
		
		inst.runtime.redraw = true;
	};
	
	instanceProto.animationFinish = function (reverse)
	{
		this.animTriggerName = this.currentAnimation.name;
		var animTrigger=this.inAnimTrigger;
		if(this.inAnimTrigger===false)
		{
			this.inAnimTrigger = true;
			this.runtime.trigger(cr.plugins_.Spriter.prototype.cnds.OnAnyAnimFinished, this);
			this.runtime.trigger(cr.plugins_.Spriter.prototype.cnds.OnAnimFinished, this);
			this.inAnimTrigger = false;
		}
	};
	
	instanceProto.clearAnimationState = function()
	{
		var anim=this.currentAnimation;
		if(anim)
		{
			for(var t=0;t<anim.timelines.length;t++)
			{
				anim.timelines[t].currentObjectState={};
			}
		}
	}
	
	instanceProto.getNowTime = function()
	{
		return (cr.performance_now() - this.start_time) / 1000.0;
	};
	
	instanceProto.doAnimChange = function()
	{
		if(this.currentAnimation)
		{
			var ratio=this.currentSpriterTime/this.currentAnimation.length;
		}
		var startFrom=this.changeToStartFrom;
		
		// startFrom
		// 0 play from start
		// 1 play from current time
		// 2 play from current time ratio
		// 3 blend to start
		// 4 blend at current time ratio
		
		if(startFrom<3)
		{
			this.currentAnimation=this.changeAnimTo;
			this.changeAnimTo=null;		
			if(startFrom===0)//play from start
			{
				if(this.speedRatio>0)
				{
					this.currentSpriterTime=0;				
				}
				else
				{
					this.currentSpriterTime=this.currentAnimation.length;				
				}
				this.lastKnownTime=this.getNowTime();
			}
			else if (startFrom==2)//play from current time ratio
			{
				this.currentSpriterTime=this.currentAnimation.length*ratio;
				this.lastKnownTime=this.getNowTime();
			}		
		}
	};
	instanceProto.endBlendAndSwap = function()
	{
		if(this.secondAnimation)
		{
			this.blendEndTime=0;
			this.blendStartTime=0;
			this.animBlend=0;
			this.blendPoseTime=0;
			if(this.changeToStartFrom===BLENDTOSTART)
			{
				this.currentSpriterTime=0;
			}
			this.currentSpriterTime=this.secondAnimation.localTime;//cr.lerp(0,this.secondAnimation.length,getT(0,this.currentAnimation.length,this.currentSpriterTime));
			this.changeAnimTo=null;
			this.currentAnimation=this.secondAnimation;
			this.secondAnimation=null;
			// startFrom
			// 0 play from start	
			// 1 play from current time
			// 2 play from current time ratio
			// 3 blend to start
			// 4 blend at current time ratio
			this.changeToStartFrom=1;//play from current time
			//this.doAnimChange();
			
			
			
			
		}
	};
	instanceProto.tickCurrentAnimationTime = function()
	{
		var lastKnownTime=this.lastKnownTime;
		var nowTime=this.getNowTime();
		this.lastKnownTime=nowTime;
		var lastSpriterTime=this.currentSpriterTime;
		var cur_timescale=this.runtime.timescale;
		var animation=this.currentAnimation;
		
	
		// Apply object's own time scale if any
		if (this.my_timescale !== -1.0)
		{
			cur_timescale=this.my_timescale;
		}
		if(this.animPlaying)
		{
			this.currentSpriterTime+=(this.getNowTime()-lastKnownTime)*1000*this.speedRatio*cur_timescale;
		}
		
		var playTo=this.playTo;
		var animFinished=false;
		if(playTo>=0)
		{	
			if(this.animPlaying)
			{
				if(((lastSpriterTime-playTo)*(this.currentSpriterTime-playTo))<0)
				{
					this.animPlaying=false;
					this.currentSpriterTime=this.playTo;
					this.playTo=-1;
					animFinished=true;
					
				}
			}
		}
		else
		{
			if(this.speedRatio>=0)
			{
				if(this.currentSpriterTime>=animation.length)
				{
					if(this.changeToStartFrom===BLENDTOSTART&&this.secondAnimation&&this.blendEndTime>0)
					{
						this.endBlendAndSwap();
					}
					else
					{
						if(animation.looping=="false")
						{
							this.currentSpriterTime=animation.length;
							this.animPlaying=false;
						}
						animFinished=true;
					}
				}
			}
			else
			{
				if(this.speedRatio<0)
				{
					if(this.currentSpriterTime<0)
					{
						if(this.changeToStartFrom===BLENDTOSTART&&this.secondAnimation&&this.blendEndTime>0)
						{
							this.endBlendAndSwap();
						}
						else
						{
							if(animation.looping=="false")
							{
								this.currentSpriterTime=0;
								this.animPlaying=false;
							}
							animFinished=true;
						}
					}
				}
			}
		}
		animation=this.currentAnimation;
		while(this.currentSpriterTime<0)
		{
			this.currentSpriterTime+=animation.length;
		}
		
		if(this.currentSpriterTime!==animation.length)
		{
			this.currentSpriterTime%=animation.length;	
		}
		
		
		if(this.secondAnimation)
		{
			if(this.changeToStartFrom===BLENDTOSTART)
			{
				this.secondAnimation.localTime=0;
			}
			else
			{
				this.secondAnimation.localTime=cr.lerp(0,this.secondAnimation.length,getT(0,this.currentAnimation.length,this.currentSpriterTime));
			}
		}
		var blendEndTime=this.blendEndTime;
		if(blendEndTime>0)
		{
			if(blendEndTime<=nowTime)
			{
				this.endBlendAndSwap();
			}
			else
			{
				this.animBlend=getT(this.blendStartTime,blendEndTime,this.lastKnownTime);
			}
		}
		return animFinished;
	};
	
	
	instanceProto.setMainlineKeyByTime = function(animation)
	{
		animation=(typeof animation!=='undefined')?animation:this.currentAnimation;
		//var animation=this.currentAnimation;
		if(animation)
		{
			var currentTime=0;
			if(animation===this.currentAnimation)
			{
				if(this.changeToStartFrom===BLENDTOSTART)
				{
					currentTime=this.blendPoseTime;
				}
				else
				{
					currentTime=this.currentSpriterTime;
				}
				
			}
			else
			{
				currentTime=animation.localTime;
			}
			var mainKeys=animation.mainlineKeys;
			animation.cur_frame=mainKeys.length;
			var secondTime=animation.length;
			for (var k=1;k<mainKeys.length;k++)
			{
				if (currentTime < mainKeys[k].time)
				{
					secondTime=mainKeys[k].time;
					animation.cur_frame = k - 1;
					break;
				}
			}
			var firstTime=0;
			if(animation===this.currentAnimation)
			{
				this.currentAdjustedTime=currentTime;
			}
			// Don't go out of bounds
			if (animation.cur_frame < 0)
			{
				animation.cur_frame = 0;
			}
			else if (animation.cur_frame >= animation.mainlineKeys.length)
			{
				animation.cur_frame = animation.mainlineKeys.length - 1;
			}
			var mainKey=mainKeys[animation.cur_frame];
			if(mainKey)
			{
				firstTime=mainKey.time;
				
				if(animation===this.currentAnimation)
				{
					var t=getT(firstTime,secondTime,this.currentSpriterTime);
					this.currentAdjustedTime=cr.lerp(firstTime,secondTime,trueT(mainKey,t));
				}
				else
				{
					var t=getT(firstTime,secondTime,animation.localTime);
					if(this.changeToStartFrom===BLENDTOSTART)
					{
						animation.localTime=0;
					}
					else
					{					
						animation.localTime=cr.lerp(firstTime,secondTime,trueT(mainKey,t));
					}
				}
			}
			
		}
	};
	instanceProto.doSecondAnimation = function()
	{
		return this.animBlend!==0&&this.secondAnimation;
	}
	function shortestSpin(a,b)
	{
		if(a===b)
			return 0;

		var pi=3.141592653589793;
		var rad=pi*2;
		while(b-a<-pi)
		{
			a-=rad;
		}
		while(b-a>pi)
		{
			b-=rad;
		}
		//while(b-a<-180)
		//{
		//	a-=360;
		//}
		//while(b-a>180)
		//{
		//	b-=360;
		//}
		//if true it's clockwise
		return b>a?-1:1;
	}
	instanceProto.currentTweenedBones = function(animation)
	{
		var tweenedBones = [];
		animation = (typeof animation !== 'undefined') ? animation : this.currentAnimation;
		//var animation=this.currentAnimation;
		var key = animation.mainlineKeys[animation.cur_frame];
		var nextTime = 0;
		var currentTime=0;
		if(animation===this.currentAnimation)
		{
			if(this.changeToStartFrom===BLENDTOSTART)
			{
				currentTime=this.blendPoseTime;
			}
			else
			{
				currentTime=this.currentAdjustedTime;
			}	
		}
		else
		{
			currentTime=animation.localTime;
		}
		for(var i = 0; i < key.bones.length; i++)
		{		
			var bone = key.bones[i];
			var nextBone = null;
			var nextFrame = null;
			var timelineIndex;
			var parent=bone.parent;
			if (bone.type == "reference")
			{
				
				var refTimeline = animation.timelines[bone.timeline];
				var namename=refTimeline.name;
				timelineIndex=bone.timeline;
				var refKey = refTimeline.keys[bone.key];
				var refKeyIndex = bone.key;
				lastTime = refKey.time;
				var nextFrame = null;
				var keysLength = refTimeline.keys.length;
				bone = refKey.bones[0];
				
				if(keysLength>1)
				{
					if(refKeyIndex+1>=keysLength&&animation.looping=="true")
					{
						nextFrame=refTimeline.keys[0];
						nextTime=nextFrame.time;
						if(currentTime>lastTime)
						{
							nextTime+=animation.length;
						}
						nextBone=nextFrame.bones[0];
					}
					else if(refKeyIndex+1<keysLength)
					{
						nextFrame=refTimeline.keys[refKeyIndex+1];
						nextTime=nextFrame.time;	
						nextBone=nextFrame.bones[0];
					}
				}
				var mirror_factor = (this.xFlip == 1 ? -1 : 1);
				var flip_factor = (this.yFlip == 1 ? -1 : 1);
				var flipMe=1;
				var parentBone="";
				if(nextBone&&key.curveType!=="instant"&&refKey.curveType!=="instant")
				{
					var lastTime=refKey.time;
					var t=0;
					if(currentTime<lastTime)
					{
						lastTime-=animation.length;
					}
					if(nextTime>lastTime)
					{
						t=(currentTime-lastTime)/(nextTime-lastTime);
					}
					t=trueT(refKey,t);

					tweenedBones[i]=TweenedSpriterObject(bone,nextBone,t,refKey.spin);					
				}
				else
				{
					tweenedBones[i]=CloneObject(bone);
				}
				refTimeline.currentMappedState=CloneObject(tweenedBones[i]);
				
				if(animation===this.currentAnimation)
				{
					if(this.animBlend!==0&&this.secondAnimation)
					{
						var secondTimeline=this.timelineFromName(refTimeline.name,this.secondAnimation);
						if(secondTimeline)
						{
							var secondBone=secondTimeline.currentObjectState;
							if(secondBone)
							{
								var firstBone=tweenedBones[i];
								tweenedBones[i]=TweenedSpriterObject(firstBone,secondBone,this.animBlend,shortestSpin(firstBone.angle,secondBone.angle));
							}
						}
					}
				}
				
				if(parent>-1)
				{
					if(animation===this.currentAnimation)
					{
						flipMe=tweenedBones[parent].xScale*tweenedBones[parent].yScale;
					}
					parentBone=tweenedBones[parent];
				}
				else
				{
					if(animation===this.currentAnimation)
					{
						tweenedBones[i].x*=mirror_factor*this.scaleRatio;			
						tweenedBones[i].y*=flip_factor*this.scaleRatio;
					
						tweenedBones[i].xScale*=mirror_factor*this.scaleRatio;
						tweenedBones[i].yScale*=flip_factor*this.scaleRatio;
						parentBone=this.objFromInst(this);
						flipMe=mirror_factor*flip_factor;
					}
					
				}
				
				
				if(animation===this.currentAnimation)
				{
					tweenedBones[i]=this.mapObjToObj(parentBone,tweenedBones[i],flipMe);
				}
				refTimeline.currentObjectState=CloneObject(tweenedBones[i]);
			}
		}
		return tweenedBones;
	};
	instanceProto.tick = function()
	{
	}
	instanceProto.tick2 = function()
	{
		if(this.secondAnimation===this.currentAnimation)
		{
			this.blendStartTime=0;
			this.blendEndTime=0;
			this.blendPoseTime=0;
			this.secondAnimation=null;
			this.changeAnimTo=0;
			if(this.changeToStartFrom===BLENDTOSTART)
			{
				this.changeToStartFrom=0;
			}
			else if(this.changeToStartFrom===BLENDATCURRENTTIMERATIO)
			{
				this.changeToStartFrom=1;
			}
		}
		
		if(this.changeAnimTo&&!this.inAnimTrigger)
		{
			this.doAnimChange();
		}
		
		var animation = this.currentAnimation;		
		if (!animation||this.inAnimTrigger)
		{
			return;
		}
		
		var changed=null;
		if(!this.animPlaying)
		{
			if(!this.lastKnownInstDataAsObj||!this.instsEqual(this.lastKnownInstDataAsObj,this))
			{
				changed=true;
				this.lastKnownInstDataAsObj=this.objFromInst(this);
			}
		}
		
		if(!changed&&this.force)
		{
			changed=true;
		}
		
		this.force=false;
		
		
		// var PAUSENEVER=0;
		// var PAUSEALLOUTSIDEBUFFER=1;
		// var PAUSEALLBUTSOUNDOUTSIDEBUFFER=2;
		// this.leftBuffer=0;
		// this.rightBuffer=0;
		// this.topBuffer=0;
		// this.bottomBuffer=0;
		// this.pauseWhenOutsideBuffer=0;
		var pauseAllButSound=false;
		var pauseAll=false;
		if(this.pauseWhenOutsideBuffer!=PAUSENEVER)
		{
			var outsideBuffer=this.isOutsideViewportBox();
			if(outsideBuffer)
			{
				if(this.pauseWhenOutsideBuffer==PAUSEALLOUTSIDEBUFFER)
				{
					pauseAll=true;
				}
				else if(this.pauseWhenOutsideBuffer==PAUSEALLBUTSOUNDOUTSIDEBUFFER)
				{
					pauseAllButSound=true;
				}
				this.setAllInvisible();
			}
		}
		if(this.animPlaying||changed)
		{		
			var animFinished=this.tickCurrentAnimationTime();			
			if(animFinished)
			{
				this.animationFinish(this.speedRatio<0);
				if(this.changeAnimTo&&!this.inAnimTrigger&&!this.animPlaying)
				{
					this.tick2();
					return;
				}
			}
		}
		else
		{
			return;
		}
		
		if(pauseAll)
		{
			return;
		}
		animation=this.currentAnimation;
		var c2ObjectArray=this.c2ObjectArray;
		this.setAllCollisionsAndVisibility(false);
		this.setMainlineKeyByTime();
		
		
		
		this.runtime.redraw = true;
		
		if (!animation.mainlineKeys[animation.cur_frame])
		{
			return;
		}
		
		
		
		if(!pauseAllButSound)
		{
			if(this.animBlend!==0&&this.secondAnimation)
			{
				if(this.changeToStartFrom===BLENDTOSTART)
				{
					this.secondAnimation.localTime=0;
				}
				else
				{					
					this.secondAnimation.localTime=(this.currentSpriterTime/this.currentAnimation.length)*this.secondAnimation.length;
				}
				this.setMainlineKeyByTime(this.secondAnimation);
				var secondTweenedBones=this.currentTweenedBones(this.secondAnimation);
				this.animateCharacter(secondTweenedBones,this.secondAnimation,false);
			}
			
			var tweenedBones = this.currentTweenedBones();
			this.animateCharacter(tweenedBones);
		}
		if(this.animPlaying)
		{
			this.animateSounds();
		}
	};
	
	instanceProto.animateCharacter = function(tweenedBones,animation,applyToInstances)
	{
		animation = (typeof animation !== 'undefined') ? animation : this.currentAnimation;
		if(!animation)
		{
			return;
		}
		var cur_frame = animation.cur_frame;
		var object;
		var objectRef;
		var nextObject = null;
		var lastTime = 0;
		var	nextTime = 0;
		var myx = 0;
		var myy = 0;
		var w = 0;
		var h = 0;
		var layer=this.layer;
		
		
		var entity = this.entity;
		var zIndex=findInArray(this,layer.instances);
		//var zIndex=layer.instances.indexOf(this);
		applyToInstances = (typeof applyToInstance !== 'undefined') ? applyToInstances : true;
		
		//var animation=this.currentAnimation;
		var key = animation.mainlineKeys[animation.cur_frame];
		var refKey;
		var instances=layer.instances;
		this.clearAnimationState();
		var currentTime=0;
		if(animation===this.currentAnimation)
		{
			if(this.changeToStartFrom===BLENDTOSTART)
			{
				currentTime=this.blendPoseTime;
			}
			else
			{
				currentTime=this.currentAdjustedTime;
			}
		}
		else
		{
			currentTime=animation.localTime;
		}
		if(applyToInstances)
		{
			var zOrder=[];
			for(var i = 0; i < key.objects.length; i++)
			{		
				object = key.objects[i];
				if (object.type == "reference")
				{
					var refTimeline = animation.timelines[object.timeline];
					var c2Obj=refTimeline.c2Object;
					if(c2Obj)
					{
						var inst=c2Obj.inst;
						if(inst)
						{
							//var currZ=instances.indexOf(inst);
							var currZ=findInArray(inst,instances);
							if(currZ>=1&&currZ<=zIndex)
							{
								instances.splice(currZ,1);
								zIndex--;
							}
						}
					}
				}
			}
			//var tempZ=instances.indexOf(this);
			var tempZ=findInArray(this,instances);
			if(zIndex!=tempZ)
			{
				instances.splice(tempZ,1);
				instances.splice(zIndex,0,this);
			}
			
			
		}
		
		var zCounter=0;
		
		for(var i = 0; i < key.objects.length; i++)
		{		
			object = key.objects[i];
			objectRef = key.objects[i];
			nextObject = null;
			var nextFrame = null;
			var timelineIndex;
			if (object.type == "reference")
			{
				var refTimeline = animation.timelines[object.timeline];
				timelineIndex=object.timeline;
				refKey = refTimeline.keys[object.key];
				var refKeyIndex = object.key;
				lastTime = refKey.time;
				var nextFrame = null;
				var keysLength = refTimeline.keys.length;
				object = refKey.objects[0];
				
				if(keysLength>1)
				{
					if(refKeyIndex+1>=keysLength&&animation.looping=="true")
					{
						nextFrame=refTimeline.keys[0];
						nextTime=nextFrame.time;
						if(currentTime>lastTime)
						{
							nextTime+=animation.length;
						}
						nextObject=nextFrame.objects[0];
					}
					else if(refKeyIndex+1<keysLength)
					{
						nextFrame=refTimeline.keys[refKeyIndex+1];
						nextTime=nextFrame.time;	
						nextObject=nextFrame.objects[0];
					}
				}
			}
			
			
			myx = this.x;			
			myy = this.y;	
			
			
			var c2Obj=refTimeline.c2Object;
			if(c2Obj||refTimeline.objectType==="point")
			{
				var inst;
				if(c2Obj)
				{
					inst=c2Obj.inst;
				}
				else
				{
					inst=null;
				}
				if(inst||refTimeline.objectType==="point")
				{
					if(applyToInstances&&inst)
					{
						inst.collisionsEnabled = true;
						inst.visible=this.visible;
						this.MoveToLayer(inst,this.layer);
					}
					if(applyToInstances&&c2Obj&&c2Obj.spriterType=="sprite")
					{
						SetSpriteAnimFrame(inst,object.frame,c2Obj);
					}
					
					var tweenedObj=null;
					if(nextObject&&key.curveType!=="instant"&&refKey.curveType!=="instant")
					{
						var t=0;
						if(currentTime<lastTime)
						{
							lastTime-=animation.length;
						}
						if((nextTime-lastTime)>0)
						{
							t=(currentTime-lastTime)/(nextTime-lastTime);
						}
						t=trueT(refKey,t);
						tweenedObj=TweenedSpriterObject(object,nextObject,t,refKey.spin);							
					}
					else
					{
						tweenedObj=CloneObject(object);	
					}
					if(inst)
					{
						var cur_frame = inst.curFrame;
					}
					var mirror_factor = (this.xFlip == 1 ? -1 : 1);
					var flip_factor = (this.yFlip == 1 ? -1 : 1);
					
					var parent=objectRef.parent;
					
					refTimeline.currentMappedState=CloneObject(tweenedObj);
					if(animation===this.currentAnimation)
					{
						if(this.animBlend!==0&&this.secondAnimation)
						{
							var secondTimeline=this.timelineFromName(refTimeline.name,this.secondAnimation);
							if(secondTimeline)
							{
								var secondBone=secondTimeline.currentObjectState;
								if(secondBone)
								{
									if(this.animBlend>0.5)
									{
										SetSpriteAnimFrame(inst,secondTimeline.currentObjectState.frame,c2Obj);
									}
									var firstBone=tweenedObj;
									tweenedObj=TweenedSpriterObject(firstBone,secondBone,this.animBlend,shortestSpin(firstBone.angle,secondBone.angle));
								}
							}
						}
					}
					
					if(animation===this.currentAnimation)
					{
						if(parent>-1)
						{
							tweenedObj=this.mapObjToObj(tweenedBones[parent],tweenedObj,tweenedBones[parent].xScale*tweenedBones[parent].yScale);			
							tweenedObj.xScale*=mirror_factor;
							tweenedObj.yScale*=flip_factor;
						}
						else
						{
							tweenedObj.x*=mirror_factor*this.scaleRatio;			
							tweenedObj.y*=flip_factor*this.scaleRatio;
							tweenedObj=this.mapObjToObj(this.objFromInst(this),tweenedObj,mirror_factor*flip_factor);
							
						}
					}
					if(inst&&applyToInstances)
					{
						this.applyObjToInst(tweenedObj,inst,parent>-1,c2Obj);
					}
					refTimeline.currentObjectState=tweenedObj;


					if(inst&&applyToInstances)
					{
						var instZOrder=zIndex+1+zCounter++;
						if(instances[instZOrder]!==inst)
						{
							//var currInstanceZ=instances.indexOf(inst);
							var currInstanceZ=findInArray(inst,instances);
							if(currInstanceZ>=0)
							{
								instances.splice(currInstanceZ,1);
							}
							instances.splice(instZOrder,0,inst);
						}
					}
				}
			}
		}
	};
	
	instanceProto.animateSounds = function(anim)
	{
		anim = (typeof anim !== 'undefined') ? anim : this.currentAnimation;
		//var anim=this.currentAnimation;
		if(anim)
		{
			for(var s=0;s<anim.soundlines.length;s++)
			{
				var soundLine=anim.soundlines[s];
				if(soundLine)
				{
					this.animateSound(soundLine,anim.length);
				}
			}
		}
	}
	
	instanceProto.animateSound = function(soundline,animLength,anim)
	{
		var soundKeys=soundline.keys;
		var curSoundFrame=soundKeys.length;
		var secondTime=animLength;
		var curSoundKey=soundKeys[0];
		var curSound;
		
		for (var k=1;k<soundKeys.length;k++)
		{
			if (this.currentAdjustedTime < soundKeys[k].time)
			{
				secondTime=soundKeys[k].time;
				curSoundFrame = k - 1;
				curSoundKey=soundKeys[curSoundFrame];
				
				break;
			}
		}
		if(curSoundKey&&curSoundKey.objects&&curSoundKey.objects[0])
				{
					curSound=curSoundKey.objects[0];
				}
		anim = (typeof anim !== 'undefined') ? anim : this.currentAnimation;
		//var anim=this.currentAnimation;
		var keysLength=soundline.keys.length;
		var nextFrame;
		var nextTime;
		var nextObject;
		if(keysLength>1)
		{
			if(curSoundFrame+1>=keysLength&&anim.looping=="true")
			{
				nextFrame=soundline.keys[0];
				nextTime=nextFrame.time;
				if(this.currentSpriterTime>lastTime)
				{
					nextTime+=anim.length;
				}
				nextObject=nextFrame.objects[0];
			}
			else if(curSoundFrame+1<keysLength)
			{
				nextFrame=soundline.keys[curSoundFrame+1];
				nextTime=nextFrame.time;	
				nextObject=nextFrame.objects[0];
			}
		}
				
				
		var time=this.currentAdjustedTime;
		var lastTime=soundline.lastTimeSoundCheck;
	
		for (var k=0;k<soundKeys.length;k++)
		{
			var soundKey=soundKeys[k];
			if(soundKey)
			{
				var soundToPlay=soundKey.objects[0];
				if(soundToPlay)
				{
					if(time===soundKey.time)
					{
						this.playSound(soundline,soundToPlay.name);
						break;
					}


					var t=-1;
					if((time-lastTime>0&&(time-lastTime<lastTime+(anim.length-time)))||(lastTime-time>time+(anim.length-lastTime)))
					{
						if((time-lastTime>0)&&(time-lastTime<lastTime+(anim.length-time)))
						{
							t=getT(lastTime,time,soundKey.time);
						}
						else
						{
							t=getT(lastTime-anim.length,time,soundKey.time);
						}
					}
					else if(time-lastTime<0||(lastTime-time<time+(anim.length-lastTime)))
					{
						if((time-lastTime<0))
						{
							t=getT(time,lastTime,soundKey.time);
						}
						else
						{
							t=getT(time-anim.length,lastTime,soundKey.time);
						}
					}

					if(t>0&&t<1)
					{
						this.playSound(soundline,soundToPlay.name);
					}
				}
			}
		}
		var tweenedSound;
		if(nextObject)
		{
			var t=0;
			var lastTime=curSoundKey.time;
			if(this.currentAdjustedTime<lastTime)
			{
				lastTime-=anim.length;
			}
			var t=getT(lastTime,nextTime,this.currentAdjustedTime);
			tweenedSound=TweenedSpriterSound(curSound,nextObject,trueT(curSoundKey,t));
		}
		else
		{
			tweenedSound=this.cloneSound(curSound);
		}
		
			this.changeVolume(soundline,tweenedSound.volume);
		
			this.changePanning(soundline,tweenedSound.panning);
		soundline.lastTimeSoundCheck=this.currentAdjustedTime;
		soundline.currentObjectState=tweenedSound;
	}
	
	instanceProto.draw = function (ctx)
	{
	};
	
	instanceProto.drawGL = function(glw)
	{
	};
	
	instanceProto.playSound = function(soundLine,name)
	{
		this.soundToTrigger=name;
		this.soundLineToTrigger=soundLine;
		this.runtime.trigger(cr.plugins_.Spriter.prototype.cnds.OnSoundTriggered, this);
	}
	instanceProto.changeVolume = function(soundLine,newVolume)
	{
		if(soundLine.currentObjectState.volume!=newVolume)
		{	
			soundLine.currentObjectState.volume=newVolume;
			this.soundToTrigger="";
		this.soundLineToTrigger=soundLine;
			this.runtime.trigger(cr.plugins_.Spriter.prototype.cnds.OnSoundVolumeChangeTriggered, this);
		}
	}
	instanceProto.changePanning = function(soundLine,newPanning)
	{
		if(soundLine.currentObjectState.panning!=newPanning)
		{	
			soundLine.currentObjectState.panning=newPanning;
			this.soundToTrigger="";
		this.soundLineToTrigger=soundLine;
			this.runtime.trigger(cr.plugins_.Spriter.prototype.cnds.OnSoundPanningChangeTriggered, this);
		}
	}
	
	function findObjectItemInArray(name, objectArray, entityName)
	{
		for (var o = 0; o < objectArray.length; o++)
		{
			var obj=objectArray[o];
			if (obj&&(obj.name===name||(obj.entityName===entityName&&obj.originalName===name)))
			{
			return o;
			}
		}
		
		return -1;
	}

	function SpriterObjectArrayItem(spritername, name, entityName, originalName)
	{
		this.name = name;
		this.fullTypeName = spritername + "_" + name;
		this.spriterType = "sprite";
		this.frames = [];
		this.pivots = [];
		this.imageSizes = [];
		this.charMaps = [];
		this.width = 0;
		this.height = 0;
		this.entityName = entityName;
		this.originalName = originalName;
		//charmap=[]
		//charmap.old=framenumber
		//charmap.new=framenumber
		
		//apply charmap
		//timeline.appliedmap[charmap.old]=charmap.new;
		//
	}
	instanceProto.timelineFromName = function(name,anim)
	{
		anim = typeof anim !== 'undefined' ? anim : this.currentAnimation;
		//var anim=this.currentAnimation;
		if(anim)
		{
			for(var t=0;t<anim.timelines.length;t++)
			{
				var timeline=anim.timelines[t];
				if(timeline&&timeline.name===name)
				{
					return timeline;
				}
			}
		}
	}
	
	instanceProto.findSprites = function(xml) //XMLDocument object, name of entityToLoad
	{
		if(!xml)
		{
			return;
		}
		
		var thisTypeName = this.type.name;
		var att;
		
		var json = xml;//.spriter_data")[0];
		var folderTags = json["folder"];
		for (var d=0;d<folderTags.length;d++)
		{	
			var folderTag=folderTags[d];
			this.folders.push(new SpriterFolder());
			var fileTags = folderTag["file"];			
			
			for (var f=0;f<fileTags.length;f++)
			{	
				var fileTag=fileTags[f];
				att=fileTag;
				
				var spriterFile=new SpriterFile();
				spriterFile.fileName=att["name"];
				if(fileTag.hasOwnProperty("pivot_x"))
				{ 
					spriterFile.pivotX = (att["pivot_x"]);
				}
				if(fileTag.hasOwnProperty("pivot_y"))
				{ 
					spriterFile.pivotY = 1.0-(att["pivot_y"]);
				}
				if(fileTag.hasOwnProperty("width"))
				{ 
					spriterFile.w = (att["width"]);
				}
				if(fileTag.hasOwnProperty("height"))
				{ 
					spriterFile.h = (att["height"]);
				}
				
				this.folders[d].files.push(spriterFile);
			}
		}
		
		var objectArray=[];
		
		var NO_INDEX=-1;
		var entityTags = json["entity"];
		for (var e = 0; e < entityTags.length; e++)
		{
			var entityTag=entityTags[e];
			att = entityTag;
			
			var objInfoTags = entityTag["obj_info"];
			var charMapTags = entityTag["character_map"];
			var entityName=att.name;
			if(objInfoTags)
			{
				for (var o = 0; o < objInfoTags.length; o++)
				{
					var infoTag = objInfoTags[o];
					if(infoTag&&(infoTag["type"]==="sprite"||infoTag["type"]==="box"))
					{
						var typeName=infoTag["name"];						
						var originalName=infoTag["realname"];
						objectArray.push(new SpriterObjectArrayItem(thisTypeName, typeName,entityName,originalName));
						var lastObj=objectArray[objectArray.length-1];
						if(infoTag["type"]==="box")
						{
							lastObj.width=infoTag["w"];
							lastObj.height=infoTag["h"];
							lastObj.isBox=true;
							lastObj.spriterType="box";
							var imageSize={};
										imageSize.w=lastObj.width;
										imageSize.h=lastObj.height;
							lastObj.imageSizes.push(imageSize);
						}
						else if(infoTag["type"]==="sprite")
						{
							var frames=infoTag["frames"];
							if(frames)
							{
								for (var f = 0; f < frames.length; f++)
								{
									var frame = frames[f];
									if(this.folders[frame["folder"]]&&this.folders[frame["folder"]].files[frame["file"]])
									{
										lastObj.frames.push(this.folders[frame["folder"]].files[frame["file"]].fileName);
										var pivot={};
										pivot.x=0;
										pivot.y=0;
										pivot.x=this.folders[frame["folder"]].files[frame["file"]].pivotX;
										pivot.y=this.folders[frame["folder"]].files[frame["file"]].pivotY;
										lastObj.pivots.push(pivot);
										
										
										var imageSize={};
										imageSize.w=1;
										imageSize.h=1;
										imageSize.w=this.folders[frame["folder"]].files[frame["file"]].w;
										imageSize.h=this.folders[frame["folder"]].files[frame["file"]].h;
										lastObj.imageSizes.push(imageSize);
									}
								}
							}
							if(charMapTags)
							{
								for (var c = 0; c < charMapTags.length; c++)
								{
									var charMapTag = charMapTags[c];
									var mapTags = charMapTag["map"];
									if(mapTags)
									{
										for (var m = 0; m < mapTags.length; m++)
										{
											var mapTag = mapTags[m];
											if(typeof mapTag["folder"]!=="undefined"&&typeof mapTag["file"]!=="undefined")
											{
												if(this.folders[mapTag["folder"]]&&this.folders[mapTag["folder"]].files[mapTag["file"]])
												{
													var charMap={};
													charMap.oldFrame=lastObj.frames.indexOf(this.folders[mapTag["folder"]].files[mapTag["file"]].fileName);
													if(charMap.oldFrame>-1)
													{
														charMap.newFrame=-1;
														if(typeof mapTag["target_folder"]!=="undefined"&&typeof mapTag["target_file"]!=="undefined")
														{
															charMap.newFrame=lastObj.frames.indexOf(this.folders[mapTag["target_folder"]].files[mapTag["target_file"]].fileName);
														}
														if(!lastObj.charMaps[charMapTag["name"]])
														{
															lastObj.charMaps[charMapTag["name"]]=[];
														}
														lastObj.charMaps[charMapTag["name"]].push(charMap);
													}
												}
											}
										}
									}
								}	
							}
						}
					}
				}
			}
		}
		return objectArray;
	};
	
	instanceProto.generateTestC2ObjectArray = function(objectArray)
	{	
		var c2Objects=[];
		var types=this.runtime.types;
		for(var o in objectArray)
		{
			var c2Object={};
			c2Object.type=types[objectArray[o].fullTypeName];
			c2Object.spriterType=objectArray[o].spriterType;
			c2Object.inst=null;
			c2Object.appliedMap=[];
			c2Object.obj=objectArray[o];
			c2Objects.push(c2Object);
		}
		return c2Objects;
	};
	instanceProto.cloneSound = function(other)
	{
		var sound = new SpriterSound();
		sound.trigger = other.trigger;
		sound.volume = other.volume;
		sound.panning = other.panning;
		sound.name = other.name;
		return sound;
	};
	instanceProto.cloneObject = function(other)
	{
		var folderIndex=-null;
		var fileIndex=null;
		var fileName=null;
		var NO_INDEX=-1;
		var object = new SpriterObject();
		object.type=other.type;
		object.frame=other.frame;
		object.storedFrame=other.storedFrame;
		object.x = (other.x);
		object.y = -(other.y);
		object.angle = ((other.angle));
		object.a = ((other.a));
		object.angle=other.angle;
		object.xScale = (other.xScale);
		object.yScale = (other.yScale);
		object.pivotX = (other.pivotX);
		object.pivotY = (other.pivotY);
		object.defaultPivot = other.defaultPivot;
		return object;
	};
	instanceProto.objectFromTag = function(objectTag,objectArray,timelineName,object_type,entityName)
	{
		var att=objectTag;
		
		var folderIndex=-null;
		var fileIndex=null;
		var fileName=null;
		var NO_INDEX=-1;
		var object = new SpriterObject();
		object.type=object_type;
		if(object_type==="sprite")
		{
			folderIndex=att["folder"];
			fileIndex=att["file"];
			file=this.folders[folderIndex].files[fileIndex];
			var objectItem=objectArray[findObjectItemInArray(timelineName,objectArray,entityName)];
			object.frame=objectItem.frames.indexOf(file.fileName);
			object.storedFrame=object.frame;
		}	
		
		if(objectTag.hasOwnProperty("x"))
		{ 
			object.x = (att["x"]);
		}
		if(objectTag.hasOwnProperty("y"))
		{ 
			object.y = -(att["y"]);
		}
		if(objectTag.hasOwnProperty("angle"))
		{ 
			object.angle = ((att["angle"]));
		}
		if(objectTag.hasOwnProperty("a"))
		{ 
			object.a = ((att["a"]));
		}
		object.angle=360-object.angle;
		object.angle/=360;
		
		if(object.angle>0.5)
		{
		object.angle-=1;
		}
		
		object.angle*=3.141592653589793*2;
		
		if(objectTag.hasOwnProperty("scale_x"))
		{ 
			object.xScale = (att["scale_x"]);
		}
		
		if(objectTag.hasOwnProperty("scale_y"))
		{ 
			object.yScale = (att["scale_y"]);
		}							
		
		if(objectTag.hasOwnProperty("pivot_x"))
		{ 
			object.pivotX = (att["pivot_x"]);
		}
		else if(object_type==="sprite")
		{
			var folders=this.folders;
			var folder=folders[folderIndex];
			object.defaultPivot=true;
			if(folder)
			{
				var file=folder.files[fileIndex];
				if(file)
				{
					object.pivotX=file.pivotX;
				}
			}
		}	
		
		if(objectTag.hasOwnProperty("pivot_y"))
		{ 
			object.pivotY = 1-(att["pivot_y"]);
		}
		else if(object_type==="sprite")
		{
			var folders=this.folders;
			var folder=folders[folderIndex];
			object.defaultPivot=true;
			if(folder)
			{
				var file=folder.files[fileIndex];
				if(file)
				{
					object.pivotY=file.pivotY;
				}
			}
		}	
		
		return object;
	};
	instanceProto.soundFromTag = function(soundTag)
	{
		var sound = new SpriterSound();
		if(soundTag["folder"]!==undefined&&soundTag["file"]!==undefined)
		{
			var file=this.folders[soundTag["folder"]].files[soundTag["file"]];
			if(file)
			{
				sound.name=file.fileName;
				sound.name=sound.name.substr(0,sound.name.lastIndexOf("."));
			}
		}	
		
		if(soundTag.hasOwnProperty("trigger"))
		{ 
			sound.trigger=soundTag["trigger"];
		}
		if(soundTag.hasOwnProperty("panning"))
		{ 
			sound.panning = soundTag["panning"];
		}
		if(soundTag.hasOwnProperty("volume"))
		{ 
			sound.volume = soundTag["volume"];
		}
		
		return sound;
	};
	instanceProto.initDOMtoPairedObjects = function()
	{
		var entities=this.entities;
		for(var e=0;e<entities.length;e++)
		{
			var entity=entities[e];
			if(entity)
			{
				var animations=entity.animations;
				if(animations)
				{
					for(var a=0;a<animations.length;a++)
					{
						var animation=animations[a];
						if(animation)
						{
							var timelines=animation.timelines;
							if(timelines)
							{
								for(var t=0;t<timelines.length;t++)
								{
									var timeline=timelines[t];
									if(timeline)
									{
										timeline.c2Object=this.c2ObjectArray[findObjectItemInArray(timeline.name,this.objectArray,entity.name)];
									}
								}
							}
						}
					}
				}
			}
		}
	};
	instanceProto.associateAllTypes = function ()
	{	
		var c2ObjectArray=this.c2ObjectArray;
		var objectArray=this.objectArray;
		
		for(var o in objectArray)
		{
			var obj=objectArray[o];
			
			var siblings=this.siblings;
			if(siblings.length>0)
			{
				for(var s=0;s<siblings.length;s++)
				{
					var sibling=siblings[s];
					if(sibling)
					{
						var type=sibling.type;
						if(type.name===obj.fullTypeName)
						{
							var c2Object=c2ObjectArray[o];
							c2Object.type=type;
							//var iid = this.get_iid(); // get my IID
							var paired_inst = sibling;
							c2Object.inst=paired_inst;
							var animations=this.entity.animations;
							var name=obj.name;
							for(var a=0;a<animations.length;a++)
							{
								var animation=animations[a];
								var timelines=animation.timelines;
								for(var t=0;t<timelines.length;t++)
								{
									var timeline=timelines[t];
									if(name==timeline.name)
									{
										timeline.c2Object=c2Object;
									}
								}
							}
							break;
						}
					}
				}
			}
			else
			{
				var obj=objectArray[o];
				var c2Object=c2ObjectArray[o];
				var type=c2Object.type;
				if(type)
				{
					obj.fullTypeName=type.name;
					c2Object.type=type;
					var iid = this.get_iid(); // get my IID
					var paired_inst = type.instances[iid];
					c2Object.inst=paired_inst;
					var animations=this.entity.animations;
				}
				break;
			}
		}
	};
	instanceProto.loadSCML = function (json_)
	{	
		this.objectArray=this.findSprites(json_);
		if(!this.type.objectArrays[this.properties[0]])
		{
			this.type.objectArrays[this.properties[0]]=this.objectArray;
		}
		this.c2ObjectArray=this.generateTestC2ObjectArray(this.objectArray);

		var thisTypeName=this.type.name;
		var att;
		
		var json=json_;
		var folderTags = json["folder"];

		var NO_INDEX=-1;
		var entityTags = json["entity"];
		for (var e = 0; e < entityTags.length; e++)
		{
			var entityTag=entityTags[e];
			att=entityTag;
			
			var entity = new SpriterEntity();
			att=entityTag;
			entity.name=att["name"];
			var animationTags = entityTag["animation"];
			for (var a = 0; a < animationTags.length; a++)
			{
				var animationTag = animationTags[a];
				att=animationTag;
				var animation = new SpriterAnimation();
				animation.name=att["name"];
				animation.length=att["length"];
				
				if(animationTag.hasOwnProperty("looping"))
				{ 
					animation.looping = att["looping"];
				}
				if(animationTag.hasOwnProperty("loop_to"))
				{ 
					animation.loopTo = att["loop_to"];
				}
				
				var mainlineTag = animationTag["mainline"];
				
				var mainline = new SpriterTimeline();
				
				
				var keyTags = mainlineTag["key"];		
				for (var k = 0; k<keyTags.length; k++)
				{
					var keyTag = keyTags[k];
					
					var key = new SpriterKey();
					att=keyTag;
					
					if(keyTag.hasOwnProperty("time"))
					{ 
						key.time = att["time"];
					}
					if(keyTag.hasOwnProperty("curve_type"))
					{ 
						key.curveType = att["curve_type"];
					}
					if(keyTag.hasOwnProperty("c1"))
								{ 
									key.c1 = att["c1"];
								}
								if(keyTag.hasOwnProperty("c2"))
								{ 
									key.c2 = att["c2"];
								}
								if(keyTag.hasOwnProperty("c3"))
								{ 
									key.c3 = att["c3"];
								}
								if(keyTag.hasOwnProperty("c4"))
								{ 
									key.c4 = att["c4"];
								}
					var boneRefTags = keyTag["bone_ref"];	
					if(boneRefTags)
					{
						for (var o = 0; o < boneRefTags.length; o++)
						{
							var boneRefTag=boneRefTags[o];
							att=boneRefTag;
							var boneRef = new SpriterObjectRef();
							boneRef.timeline=att["timeline"];
							boneRef.key=att["key"];
							if(boneRefTag.hasOwnProperty("parent"))
							{ 
								boneRef.parent = att["parent"];
							}
							key.bones.push(boneRef);
						}		
					}
					
					var objectRefTags = keyTag["object_ref"];	
					if(objectRefTags)
					{
						for (var o = 0; o < objectRefTags.length; o++)
						{
							var objectRefTag=objectRefTags[o];
							att=objectRefTag;
							var objectRef = new SpriterObjectRef();
							objectRef.timeline=att["timeline"];
							objectRef.key=att["key"];
							if(objectRefTag.hasOwnProperty("parent"))
							{ 
								objectRef.parent = att["parent"];
							}
							key.objects.push(objectRef);
						}		
					}
					
					animation.mainlineKeys.push(key);
				}	
				
				animation.mainline=mainline;
				var timelineTags = animationTag["timeline"];
				if(timelineTags)
				{
					for (var t = 0; t < timelineTags.length; t++)
					{
						var timelineTag=timelineTags[t];
						
						att=timelineTag;
						
						var timeline = new SpriterTimeline();

						if(timelineTag.hasOwnProperty("object_type"))
						{	 
							timeline.objectType = att["object_type"];
						}
						
						var timelineName=att["name"];
						timeline.name=timelineName;
						
						var keyTags = timelineTag["key"];		
						if(keyTags)
						{
							
							for (var k = 0; k<keyTags.length; k++)
							{
								var keyTag = keyTags[k];
								
								var key = new SpriterKey();
								att=keyTag;
								
								if(keyTag.hasOwnProperty("time"))
								{ 
									key.time = att["time"];
								}
								if(keyTag.hasOwnProperty("spin"))
								{
									key.spin = (att["spin"]);
								}
								if(keyTag.hasOwnProperty("curve_type"))
								{ 
									key.curveType = att["curve_type"];
								}
								if(keyTag.hasOwnProperty("c1"))
								{ 
									key.c1 = att["c1"];
								}
								if(keyTag.hasOwnProperty("c2"))
								{ 
									key.c2 = att["c2"];
								}
								if(keyTag.hasOwnProperty("c3"))
								{ 
									key.c3 = att["c3"];
								}
								if(keyTag.hasOwnProperty("c4"))
								{ 
									key.c4 = att["c4"];
								}
								var objectTags = keyTag["object"];
								if(objectTags)
								{		
									var objectTag=objectTags;
									var object=this.objectFromTag(objectTag,this.objectArray,timelineName,timeline.objectType,entity.name);
									key.objects.push(object);
								}
								var boneTags = keyTag["bone"];
								if(boneTags)
								{		
									var boneTag=boneTags;
									var bone=this.objectFromTag(boneTag,this.objectArray,timelineName,timeline.objectType,entity.name);
									key.bones.push(bone);
								}
								timeline.keys.push(key);
							}		
						}
						timeline.c2Object=this.c2ObjectArray[findObjectItemInArray(timelineName,this.objectArray,entity.name)];
						animation.timelines.push(timeline);
					}
				}
				var timelineTags = animationTag["soundline"];
				if(timelineTags)
				{
					for (var t = 0; t < timelineTags.length; t++)
					{
						var timelineTag=timelineTags[t];
						
						att=timelineTag;
						
						var timeline = new SpriterTimeline();

						timeline.objectType = "sound";
						
						var timelineName=att["name"];
						timeline.name=timelineName;
						
						var keyTags = timelineTag["key"];		
						if(keyTags)
						{
							
							for (var k = 0; k<keyTags.length; k++)
							{
								var keyTag = keyTags[k];
								
								var key = new SpriterKey();
								att=keyTag;
								
								if(keyTag.hasOwnProperty("time"))
								{ 
									key.time = att["time"];
								}
								if(keyTag.hasOwnProperty("spin"))
								{
									key.spin = (att["spin"]);
								}
								if(keyTag.hasOwnProperty("curve_type"))
								{ 
									key.curveType = att["curve_type"];
								}
								if(keyTag.hasOwnProperty("c1"))
								{ 
									key.c1 = att["c1"];
								}
								if(keyTag.hasOwnProperty("c2"))
								{ 
									key.c2 = att["c2"];
								}
								if(keyTag.hasOwnProperty("c3"))
								{ 
									key.c3 = att["c3"];
								}
								if(keyTag.hasOwnProperty("c4"))
								{ 
									key.c4 = att["c4"];
								}
								var soundTags = keyTag["object"];
								if(soundTags)
								{		
									var soundTag=soundTags;
									var sound=this.soundFromTag(soundTag);
									key.objects.push(sound);
								}
								timeline.keys.push(key);
							}		
						}
						animation.soundlines.push(timeline);
					}
				}
				entity.animations.push(animation);

			}
			this.entities.push(entity);
			if(!this.entity||this.properties[1]===entity.name)
			{
				this.entity=entity;
			}
		}		
	};
	
	instanceProto.doRequest = function (json,url_, method_)
	{
		// Create a context object with the tag name and a reference back to this
		var self = this;
		var request = null;
		
		var errorFunc = function () {
			//self.runtime.trigger(cr.plugins_.AJAX.prototype.cnds.OnError, self);
		};
		
		try
		{
			var data = json;
			self.loadSCML(data);
			self.type.scmlFiles[self.properties[0]]=self.entities;
			self.runtime.trigger(cr.plugins_.Spriter.prototype.cnds.readyForSetup, self);
			self.setAnimTo(self.properties[2],true);
			if(!self.currentAnimation&&self.entity&&self.entity.animations.length)
			{
				self.setAnimTo(self.entity.animations[0].name,true);
			}	
			
		}
		catch (e)
		{
			//errorFunc();
		}
		
	};
	
	instanceProto.getAnimFromEntity = function(animName)
	{
		for(var a in this.entity.animations)
		{
			if(this.entity.animations[a].name==animName)
			{
				return this.entity.animations[a];
			}
		}
	};
	
	instanceProto.mapObjToObj = function(parentObject,obj,flipAngle)
	{
		var returnObj=new SpriterObject();
		returnObj.xScale=obj.xScale*parentObject.xScale;
		returnObj.yScale=obj.yScale*parentObject.yScale;
		if(flipAngle<0)
		{
			returnObj.angle=((3.141592653589793*2)-obj.angle)+parentObject.angle;
		}
		else
		{
			returnObj.angle=obj.angle+parentObject.angle;
		}
		var x=obj.x*parentObject.xScale;
		var y=obj.y*parentObject.yScale;
		var angle=parentObject.angle;
		var s = 0;
		var c = 1;
		
		if(angle!=0)
		{
			s=Math.sin(angle);
			c=Math.cos(angle);
		}
		var xnew = (x * c) - (y * s);
		var ynew = (x * s) + (y * c);
		
		returnObj.a=parentObject.a*obj.a;
		returnObj.pivotX=obj.pivotX;
		returnObj.pivotY=obj.pivotY;
		returnObj.defaultPivot=obj.defaultPivot;
		returnObj.x=xnew+parentObject.x;
		returnObj.y=ynew+parentObject.y;
		
		return returnObj;
	};
	
	
	instanceProto.instsEqual = function(obj,inst)
	{
		return obj.x==inst.x&&obj.y==inst.y&&obj.a==inst.opacity&&obj.angle==inst.angle;
	};
	
	instanceProto.objFromInst = function(inst)
	{
		var obj = new SpriterObject();
		obj.pivotX=inst.hotspotX;
		obj.pivotY=inst.hotspotY;
		obj.defaultPivot=false;
		obj.x=inst.x;
		obj.y=inst.y;
		obj.a=inst.opacity;
		obj.angle=inst.angle;
		obj.frame=inst.curFrame;
		obj.storedFrame=inst.storedFrame;
		return obj;
	};
	instanceProto.currentFrame = function()
	{
		if(this.currentAnimation)
		{
			return this.currentAnimation.cur_frame;
		}
		return 0;
	}
	instanceProto.applyPivotToInst = function(inst,pivotX,pivotY,objWidth,objHeight)
	{
		var x=-1*pivotX*objWidth;
		var y=-1*pivotY*objHeight;
		var angle=inst.angle;
		var s = 0;
		var c = 1;
		
		if(angle!=0)
		{
			s = Math.sin(angle);
			c = Math.cos(angle);
		}
		var xnew = (x * c) - (y * s);
		var ynew = (x * s) + (y * c);
	
		inst.x=xnew+inst.x;
		inst.y=ynew+inst.y;
	};
	instanceProto.applyObjToInst = function(obj,inst,dontApplyGlobalScale,c2Object)
	{
		inst.angle=obj.angle;
		inst.opacity=obj.a;
		var cur_frame = inst.curFrame;
		
		// hotspots are set back to zero if the project was imported with older versions of c2
		// which could cause the bounding box to shift incorrectly
		// this is a better failure for users who haven't reimported their projects after the update
		inst.hotspotX=0;
		inst.hotspotY=0;
		inst.x=obj.x;
		inst.y=obj.y;
		
		var trueW=inst.width;
		var trueH=inst.height;
		if(c2Object.obj.isBox)
		{
			var test=true;
		}
		if(c2Object.obj.imageSizes&&c2Object.obj.imageSizes.length>inst.cur_frame)
		{
			trueW=c2Object.obj.imageSizes[inst.cur_frame].w;
			trueH=c2Object.obj.imageSizes[inst.cur_frame].h;
		}
		var mirror_factor = (this.xFlip == 1 ? -1 : 1);
		var flip_factor = (this.yFlip == 1 ? -1 : 1);
		if(obj.xScale>100)
		{
		var here=true;
		}
		var new_width = (dontApplyGlobalScale?1:this.scaleRatio) * trueW * obj.xScale * mirror_factor;
		var new_height = (dontApplyGlobalScale?1:this.scaleRatio) * trueH * obj.yScale * flip_factor;
		
		if (inst.width !== new_width || inst.height !== new_height)
		{
			inst.width = new_width;
			inst.height = new_height;			
		}
		var pivX=obj.pivotX;
		var pivY=obj.pivotY;
		if(obj.defaultPivot)
		{
			pivX=c2Object.obj.pivots[inst.cur_frame].x;
			pivY=c2Object.obj.pivots[inst.cur_frame].y;
		}
		this.applyPivotToInst(inst,pivX,pivY,new_width,new_height);
		inst.set_bbox_changed();
	};
	instanceProto.setEntTo = function(entName)
	{
		var entities=this.entities;
		for(var e in entities)
		{
			var entity=entities[e];
			if(entity&&entName==entity.name)
			{
			this.entity=entity;
			}
		}
		if(!this.entity&&this.entities.length)
		{
			this.entity=entities[0];
		}
		if(!this.entity)
		{
			this.startingEntName=entName;
		}
	};
	instanceProto.setAllCollisionsAndVisibility = function(newState)
	{		
		var c2ObjectArray=this.c2ObjectArray;
		if(c2ObjectArray)
		{
			for(var o=0;o<c2ObjectArray.length;o++)
			{
				var c2Object=c2ObjectArray[o];
				var inst=c2Object.inst;
				if(!inst)
				{
					this.associateAllTypes();
					inst=c2Object.inst;
				}
				if(inst)
				{
					inst.collisionsEnabled = newState;
					inst.visible=newState;
				}
			}
		}
	};
	
	instanceProto.setAllInvisible = function()
	{		
		var c2ObjectArray=this.c2ObjectArray;
		if(c2ObjectArray)
		{
			for(var o=0;o<c2ObjectArray.length;o++)
			{
				var c2Object=c2ObjectArray[o];
				var inst=c2Object.inst;
				if(!inst)
				{
					this.associateAllTypes();
					inst=c2Object.inst;
				}
				if(inst)
				{
					inst.visible=false;
				}
			}
		}
	};
	instanceProto.setAnimTo = function(animName,tick)
	{
		tick = (typeof tick !== 'undefined') ? tick : true;
		this.playTo=-1;
		this.changeAnimTo=this.getAnimFromEntity(animName);
		
		if(!this.changeAnimTo&&(!this.currentAnimation)&&this.entity)
		{
			this.changeAnimTo=this.entity.animations[0];
		}
			// startFrom
		// 0 play from start
		// 1 play from current time
		// 2 play from current time ratio
		// 3 blend to start
		// 4 blend at current time ratio
		
		if(!this.changeAnimTo)
		{
			this.startingAnimName=animName;
			this.changeToStartFrom=0;
			this.blendStartTime=0;
			this.blendEndTime=0;
			this.secondAnimation=null;
			this.blendPoseTime=0;
		}
		
		var anim=this.currentAnimation;
		if(anim)
		{
			this.animPlaying=true;
		}
		
		this.setAllCollisionsAndVisibility(false);
		
		this.runtime.tick2Me(this);
		if(tick)
		{
			this.tick2();
		}
	};

	instanceProto.soundlineFromName = function(name)
	{
		if(this.soundLineToTrigger&&this.soundLineToTrigger.name===name)
		{
			return this.soundLineToTrigger;
		}
		var anim=this.currentAnimation;
		if(anim)
		{
			for(var s=0;s<anim.soundlines.length;s++)
			{
				var soundline=anim.soundlines[s];
				if(soundline&&soundline.name===name)
				{
					return soundline;
				}
			}
		}
	}
	
	//////////////////////////////////////
	// Conditions
	function Cnds() {}
	
	Cnds.prototype.readyForSetup = function ()
	{
			return true;
	};
	
	Cnds.prototype.outsidePaddedViewport = function()
	{
		return this.isOutsideViewportBox();
	}
	
	Cnds.prototype.actionPointExists = function(pointName)
	{
		var timeline=this.timelineFromName(name);
		if(timeline&&timeline.currentObjectState)
		{
			if(timeline.currentObjectState.x!==undefined)
			{
					return true;
			}
		}
		return false;
	};
	Cnds.prototype.OnAnimFinished = function (animname)
	{
		return this.currentAnimation.name.toLowerCase() === animname.toLowerCase();
	};
	Cnds.prototype.OnSoundTriggered = function ()
	{
		return true;
	};
	Cnds.prototype.OnSoundVolumeChangeTriggered = function ()
	{
		return true;
	};
	Cnds.prototype.OnSoundPanningChangeTriggered = function ()
	{
		return true;
	};
	Cnds.prototype.OnAnyAnimFinished = function ()
	{
		//this.runtime.trigger(cr.plugins_.Spriter.prototype.cnds.OnAnimFinished, this);
		return true;
	};
	// AddCmpParam("Current Key Frame is ", "Is the current Key Frame <,>,=,etc to the value below");
	// AddNumberParam("Frame","The frame number to compare the current key frame to" ,"0")	;
	// AddCondition(2,0, "Compare Current Key Frame", "Key Frames", "Current Key Frame is {0} {1}", "Compare the current key frame number.", "CompareCurrentKey");
	Cnds.prototype.CompareCurrentKey = function (cmp,frame)
	{
		return cr.do_cmp(this.currentFrame(), cmp, frame);
	};
	// AddCmpParam("Current Animation Time is ", "Is the current time <,>,=,etc to the value below");
	// AddNumberParam("Time","The time to compare the current key frame to" ,"0")	;
	// AddComboParamOption("milliseconds");
	// AddComboParamOption("ratio of the animation length");
	// AddComboParam("Time Format", "Is the 'Time' value above expressed in milliseconds or as a ratio",0);	
	// AddCondition(3,0, "Compare Current Time", "Animations", "Current Time is {0} {1} {2}", "Compare the current time.", "CompareCurrentTime");
	Cnds.prototype.CompareCurrentTime = function (cmp,time,format)
	{
		if(format===0)//milliseconds
		{
			return cr.do_cmp(this.currentSpriterTime, cmp, time);
		}
		else
		{
			var anim=this.currentAnimation;
			if(anim)
			{
				return cr.do_cmp(this.currentSpriterTime/this.currentAnimation.length, cmp, time);
			}
			else
			{
				return false;
			}
		}
	};	
	
	// AddStringParam("Animation", "Is this the current animation.");
	// AddCondition(4, 0, "Compare Current Animation", "Animations", "Is current animation {0}", "Compare the name of the current animation.", "CompareAnimation");
	Cnds.prototype.CompareAnimation = function (name)
	{
		var blendingTo=this.secondAnimation;
		if(blendingTo&&blendingTo.name===name&&this.blendEndTime>0)
		{
			return true;
		}
		var anim=this.currentAnimation;
		if(anim&&anim.name===name)
		{
			return true;
		}
		else
		{
			return false;
		}
	};
	
	// AddCondition(5, 0, "Is Paused", "Animations", "If animation is paused", "Is animation paused?", "AnimationPaused");
	Cnds.prototype.AnimationPaused = function ()
	{
		return !this.animPlaying;
	};
	// AddCondition(6, 0, "Is Looping", "Animations", "If animation is looping", "Is animation set to loop?", "AnimationLooping");
	Cnds.prototype.AnimationLooping = function ()
	{
		var anim=this.currentAnimation;
		if(anim&&anim.looping==="true")
		{
			return true;
		}
		else
		{
			return false;
		}
	};

	Cnds.prototype.isMirrored = function ()
	{
		return this.xFlip;
	};
	
	Cnds.prototype.isFlipped = function ()
	{
		return this.yFlip;
	};
	
	pluginProto.cnds = new Cnds();

	//////////////////////////////////////
	// Actions
	function Acts() {}

	Acts.prototype.setPlaybackSpeedRatio = function (newSpeed)
	{		
		this.speedRatio=newSpeed;
	};
	
	Acts.prototype.setVisible = function (visible)
	{		
		if(visible===1)
		{
			this.visible=true;
		}
		else
		{
			this.visible=false;
		}
	};
	
	Acts.prototype.setAutomaticPausing = function (newPauseSetting,leftBuffer,rightBuffer,topBuffer,bottomBuffer)
	{
		this.pauseWhenOutsideBuffer=newPauseSetting;
		this.leftBuffer=leftBuffer;
		this.rightBuffer=rightBuffer;
		this.topBuffer=topBuffer;
		this.bottomBuffer=bottomBuffer;
	}
	Acts.prototype.setObjectScaleRatio = function (newScale,xFlip,yFlip)
	{		
		this.scaleRatio=newScale;
		this.xFlip=xFlip;
		this.yFlip=yFlip;
	};
	
	Acts.prototype.setObjectXFlip = function (xFlip)
	{	
		this.xFlip=xFlip;
	};
	
	Acts.prototype.setObjectYFlip = function (yFlip)
	{	
		this.yFlip=yFlip;
	};
	var PLAYFROMSTART=0;
	var PLAYFROMCURRENTTIME=1;
	var PLAYFROMCURRENTTIMERATIO=2;
	var BLENDTOSTART=3;
	var BLENDATCURRENTTIMERATIO=4;
	
	
	
	Acts.prototype.setAnim = function (animName,startFrom,blendDuration)
	{
		var ratio=0;
		// startFrom
		// 0 play from start	
		// 1 play from current time
		// 2 play from current time ratio
		// 3 blend to start
		// 4 blend at current time ratio
	
		if((startFrom==1||startFrom==2)&&this.currentAnimation&&animName==this.currentAnimation.name)
		{
			return;
		}
	
		if(startFrom>PLAYFROMCURRENTTIMERATIO&&blendDuration>0)
		{
			var secondAnim=this.getAnimFromEntity(animName);
			if(secondAnim===this.secondAnimation&&this.blendEndTime>0)
			{
				return;
			}
			
			
			if(secondAnim===this.currentAnimation)
			{
				if(!this.secondAnimation)
				{
					this.blendStartTime=0;
					this.blendEndTime=0;
					this.blendPoseTime=0;
					this.secondAnimation=null;
					this.animBlend=0;
					this.changeAnimTo=null;
					return;
				}
				else
				{
					this.currentAnimation=this.secondAnimation;
					this.animBlend=1.0-this.animBlend;
				}
			}
			else
			{
				this.animBlend=0;
			}
			this.secondAnimation=secondAnim;
			
			this.blendStartTime=this.getNowTime();
			this.blendPoseTime=0;
			if(startFrom===BLENDTOSTART&&this.currentAnimation.looping==="false")
			{
				blendDuration=Math.min(blendDuration,(this.currentAnimation.length-this.currentSpriterTime));
			}
			this.blendEndTime=this.blendStartTime+((blendDuration/1000)/this.runtime.timescale);
		}
		else
		{
			if(blendDuration<=0)
			{
				if(startFrom===BLENDATCURRENTTIMERATIO)
				{
					startFrom=2;
				}
				else if(startFrom==BLENDTOSTART)
				{
					startFrom=0;
				}
			}
			this.blendStartTime=0;
			this.blendEndTime=0;
			this.blendPoseTime=0;
			this.secondAnimation=null;
		}
		
	
		this.changeToStartFrom=startFrom;
		if(startFrom===BLENDTOSTART&&this.secondAnimation)
		{
			this.blendPoseTime=this.currentSpriterTime;
			this.secondAnimation.localTime=0;
			this.setMainlineKeyByTime(this.secondAnimation);
			var secondTweenedBones=this.currentTweenedBones(this.secondAnimation);
			this.animateCharacter(secondTweenedBones,this.secondAnimation,false);
		}
		this.setAnimTo(animName,false);		
		var animPlaying=this.animPlaying;
		this.animPlaying=false;
		this.tick2();
		this.animPlaying=animPlaying;
	};
	
	Acts.prototype.setSecondAnim = function (animName)
	{
		this.secondAnimation=this.getAnimFromEntity(animName);
		if(this.secondAnimation===this.currentAnimation)
		{
			this.secondAnimation=null;
		}		
	};
	Acts.prototype.stopSecondAnim = function (animName)
	{
		this.secondAnimation=null;
		this.animBlend=0;
	};
	Acts.prototype.setAnimBlendRatio = function (newBlend)
	{
		this.animBlend=newBlend;
	};
	Acts.prototype.setEnt = function (entName,animName)
	{
		var newAnimName=animName;
		if(this.entity&&this.currentAnimation&&this.entity.name==entName&&this.currentAnimation.name==animName)
		{
			return;
		}
		if(entName!==""&&((!this.entity)||entName!=this.entity.name))
		{
			this.setEntTo(entName);
		}
		if(this.currentAnimation&&newAnimName==="")
		{
			newAnimName=this.currentAnimation.name;		
		}
		this.setAnimTo(newAnimName);
		
	};
	
	Acts.prototype.playAnimTo = function (units,playTo)
	{
		if(units===0)// keyframes
		{
			var mainKeys=this.currentAnimation.mainlineKeys;
			if(mainKeys)
			{
				var key=mainKeys[playTo];
				if(key)
				{
					this.playTo=key.time;
				}
				else
				{
					this.playTo=-1;
					return;
				}
			}
		}
		else if(units==1)// milliseconds
		{
			this.playTo=playTo;
		}
		else if(units==2)// ratio
		{
			this.playTo=playTo*this.currentAnimation.length;
		}
		if(this.playTo==this.currentSpriterTime)
		{
			this.playTo=-1;
			return;
		}
		var reverseFactor=1;
		if(this.currentAnimation.looping=="true")
		{
			var forwardDistance=0;
			var backwardDistance=0;
			if(this.playTo>this.currentSpriterTime)
			{	
				forwardDistance=this.playTo-this.currentSpriterTime;
				backwardDistance=(this.currentAnimation.length-this.playTo)+this.currentSpriterTime;
			}
			else
			{
				forwardDistance=this.playTo+(this.currentAnimation.length-this.currentSpriterTime);
				backwardDistance=this.currentSpriterTime-this.playTo;
			}
			if(backwardDistance<forwardDistance)
			{
				reverseFactor=-1;
			}
		}
		else
		{
			if(this.playTo<this.currentSpriterTime)
			{
				reverseFactor=-1;	
			}
		}
		this.speedRatio=Math.abs(this.speedRatio)*reverseFactor;
		this.animPlaying=true;
		this.tick2();
	};
	
	Acts.prototype.associateTypeWithName = function (type,name)
	{	
		var c2ObjectArray=this.c2ObjectArray;
		var objectArray=this.objectArray;
		
		for(var o in objectArray)
		{
			var obj=objectArray[o];
			if(name==obj.name)
			{
				obj.fullTypeName=type.name;
				var c2Object=c2ObjectArray[o];
				c2Object.type=type;
				var iid = this.get_iid(); // get my IID
				var paired_inst = type.instances[iid];
				c2Object.inst=paired_inst;
				var animations=this.entity.animations;
				for(var a in animations)
				{
					var animation=animations[a];
					var timelines=animation.timelines;
					for(var t in timelines)
					{
						var timeline=timelines[t];
						if(name==timeline.name)
						{
							timeline.c2Object=c2Object;
						}
					}
				}
				break;
			}
		}
	};
	Acts.prototype.setAnimationLoop = function (loopOn)
	{
		var currentAnimation=this.currentAnimation;
		if(currentAnimation)
		{
			if(loopOn===0)
			{
				currentAnimation.looping="false";
			}
			else if(loopOn==1)
			{
				currentAnimation.looping="true";
			}
		}
		else
		{
			if(loopOn===0)
			{
				this.startingLoopType="false";
			}
			else if(loopOn==1)
			{
				this.startingLoopType="true";
			}
		}
	};
	Acts.prototype.setAnimationTime = function (units,time)
	{
		var currentAnimation=this.currentAnimation;
		var lastSpriterTime=this.currentSpriterTime;
		if(currentAnimation)
		{
			if(units===0)// milliseconds
			{
				this.currentSpriterTime=time;
			}
			else if(units==1)// ratio
			{
				this.currentSpriterTime=time*currentAnimation.length;
			}
		}
		if(lastSpriterTime!=this.currentSpriterTime)
		{
			this.force=true;
		}
	};
	Acts.prototype.pauseAnimation = function ()
	{
		this.animPlaying=false;
	};
	
	Acts.prototype.resumeAnimation = function ()
	{
		this.animPlaying=true;
		var anim=this.currentAnimation;
		if(anim)
		{
			if(this.speedRatio>0)
			{
				if(this.currentSpriterTime==anim.length)
				{
				this.currentSpriterTime=0;
				}
			}
			else if(this.currentSpriterTime===0)
			{
				this.currentSpriterTime=this.currentAnimation.length;
			}
		}
	};
	
	Acts.prototype.removeAllCharMaps = function ()
	{
		var c2Objs=this.c2ObjectArray;
		for(var c=0;c<c2Objs.length;c++)
		{
			var c2Obj=c2Objs[c];
			c2Obj.appliedMap=[];
		}
	};
	
	Acts.prototype.appendCharMap = function (mapName)
	{
		var c2Objs=this.c2ObjectArray;
		for(var c=0;c<c2Objs.length;c++)
		{
			var c2Obj=c2Objs[c];
			if(c2Obj)
			{
				if(c2Obj.obj)
				{
					var charMap=c2Obj.obj.charMaps[mapName];
					if(charMap)
					{
						for(var m=0;m<charMap.length;m++)
						{
							var map=charMap[m];
							if(map)
							{
								c2Obj.appliedMap[map.oldFrame]=map.newFrame;
							}
						}
					}
				}
			}
		}
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {}
	
	Exps.prototype.time = function (ret)
	{
		ret.set_int(this.currentSpriterTime);
	};
	
	Exps.prototype.pointX = function (ret,name)
	{
		var timeline=this.timelineFromName(name);
		if(timeline&&timeline.currentObjectState)
		{
			if(timeline.currentObjectState.x!==undefined)
			{
				ret.set_float(timeline.currentObjectState.x);
				return;
			}
		}
		ret.set_float(0);
	};
	
	Exps.prototype.pointY = function (ret,name)
	{
		var timeline=this.timelineFromName(name);
		if(timeline&&timeline.currentObjectState)
		{
		if(timeline.currentObjectState.y!==undefined)
			{
			ret.set_float(timeline.currentObjectState.y);
			return;
			}
		}
			ret.set_float(0);
	};
	
	Exps.prototype.pointAngle = function (ret,name)
	{
		var timeline=this.timelineFromName(name);
		if(timeline&&timeline.currentObjectState)
		{
		if(timeline.currentObjectState.angle!==undefined)
			{
			ret.set_float(cr.to_degrees(timeline.currentObjectState.angle));
			return;
		}
		}
			ret.set_float(0);
	};
	
	Exps.prototype.timeRatio = function (ret)
	{
		if(this.currentAnimation)
		{
			ret.set_float(this.currentSpriterTime/this.currentAnimation.length);
		}
		else
		{
			ret.set_float(0);
		}
	};
	
		
	Exps.prototype.ScaleRatio = function (ret)
	{
		ret.set_float(this.scaleRatio);
	};
	
	Exps.prototype.key = function (ret)
	{
		ret.set_int(this.currentframe());
	};
	
	Exps.prototype.PlayTo = function (ret)
	{
		ret.set_int(this.playTo);
	};
	
	Exps.prototype.animationName = function (ret)
	{
		if(this.changeAnimTo)
		{
			ret.set_string(this.changeAnimTo.name);
		}
		//else if(this.currentAnimation)
		else if(this.currentAnimation)
		{
			ret.set_string(this.currentAnimation.name);
		}
		else
		{
			ret.set_string("");
		}
	};
	
	Exps.prototype.secondAnimationName = function (ret)
	{
		if(this.secondAnimation)
		{
			ret.set_string(this.secondAnimation.name);
		}
		else
		{
			ret.set_string("");
		}
	};
	
	Exps.prototype.entityName = function (ret)
	{
		if(this.entity)
		{
		ret.set_string(this.entity.name);
		}
		else
		{
		ret.set_string("");
		}
	};
	
	Exps.prototype.PlayToTimeLeft = function (ret)
	{
		if(this.playTo<0)
		{
			return ret.set_float(0);
		}
		
		if(this.currentAnimation.looping=="true")
		{
			var forwardDistance=0;
			var backwardDistance=0;
			if(speedRatio>=0)
			{
				if(this.playTo>this.currentSpriterTime)
				{	
					return ret.set_float(this.playTo-this.currentSpriterTime);
				}
				else
				{
					return ret.set_float(this.playTo+(this.currentAnimation.length-this.currentSpriterTime));
				}
			}
			else
			{
				if(this.playTo>this.currentSpriterTime)
				{	
					return ret.set_float((this.currentAnimation.length-this.playTo)+this.currentSpriterTime);
				}
				else
				{
					return ret.set_float(this.currentSpriterTime-this.playTo);
				}
			}
		}
		else
		{	
			return ret.set_float(Math.abs(this.playTo-this.currentSpriterTime));
		}
		
	};
	Exps.prototype.triggeredSound = function (ret)
	{
		ret.set_string(this.soundToTrigger);
	};
	
	Exps.prototype.triggeredSoundTag = function (ret)
	{
		if(this.soundLineToTrigger)
		{
			ret.set_string(this.soundLineToTrigger.name);
			return;
		}
		//else
		ret.set_string("");
	};
	
	
	Exps.prototype.soundVolume = function (ret,soundTag)
	{
		var soundline=this.soundlineFromName(soundTag);
		if(soundline)
		{
			if(soundline.currentObjectState)
			{
				ret.set_float(soundline.currentObjectState.volume);
				return;
			}
		}
		ret.set_float(0);
	};
	
	Exps.prototype.soundPanning = function (ret,soundTag)
	{
		var soundline=this.soundlineFromName(soundTag);
		if(soundline)
		{
			if(soundline.currentObjectState)
			{
				ret.set_float(soundline.currentObjectState.panning);
				return;
			}
		}
		ret.set_float(0);
	};
	
	Exps.prototype.blendRatio = function (ret)
	{
		ret.set_float(this.animBlend);		
	};
	
	pluginProto.exps = new Exps();

}());