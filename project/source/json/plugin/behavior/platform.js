// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.behaviors, "cr.behaviors not created");

/////////////////////////////////////
// Behavior class
cr.behaviors.Platform = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	var behaviorProto = cr.behaviors.Platform.prototype;
		
	/////////////////////////////////////
	// Behavior type class
	behaviorProto.Type = function(behavior, objtype)
	{
		this.behavior = behavior;
		this.objtype = objtype;
		this.runtime = behavior.runtime;
	};

	var behtypeProto = behaviorProto.Type.prototype;

	behtypeProto.onCreate = function()
	{
	};

	/////////////////////////////////////
	// Behavior instance class
	
	// animation modes
	var ANIMMODE_STOPPED = 0;
	var ANIMMODE_MOVING = 1;
	var ANIMMODE_JUMPING = 2;
	var ANIMMODE_FALLING = 3;
	
	behaviorProto.Instance = function(type, inst)
	{
		this.type = type;
		this.behavior = type.behavior;
		this.inst = inst;				// associated object instance to modify
		this.runtime = type.runtime;
		
		// Key states
		this.leftkey = false;
		this.rightkey = false;
		this.jumpkey = false;
		this.jumped = false;			// prevent bunnyhopping
		this.doubleJumped = false;
		this.canDoubleJump = false;
		this.ignoreInput = false;
		
		// Simulated controls
		this.simleft = false;
		this.simright = false;
		this.simjump = false;
		
		// Last floor object for moving platform
		this.lastFloorObject = null;
		this.loadFloorObject = -1;
		this.lastFloorX = 0;
		this.lastFloorY = 0;
		this.floorIsJumpthru = false;
		
		this.animMode = ANIMMODE_STOPPED;
		
		this.fallthrough = 0;			// fall through jump-thru.  >0 to disable, lasts a few ticks
		this.firstTick = true;
		
		// Movement
		this.dx = 0;
		this.dy = 0;
	};

	var behinstProto = behaviorProto.Instance.prototype;
	
	behinstProto.updateGravity = function()
	{
		// down vector
		this.downx = Math.cos(this.ga);
		this.downy = Math.sin(this.ga);
		
		// right vector
		this.rightx = Math.cos(this.ga - Math.PI / 2);
		this.righty = Math.sin(this.ga - Math.PI / 2);
		
		// get rid of any sin/cos small errors
		this.downx = cr.round6dp(this.downx);
		this.downy = cr.round6dp(this.downy);
		this.rightx = cr.round6dp(this.rightx);
		this.righty = cr.round6dp(this.righty);
		
		this.g1 = this.g;
		
		// gravity is negative (up): flip the down vector and make gravity positive
		// (i.e. change the angle of gravity instead)
		if (this.g < 0)
		{
			this.downx *= -1;
			this.downy *= -1;
			this.g = Math.abs(this.g);
		}
	};

	behinstProto.onCreate = function()
	{
		// Load properties
		this.maxspeed = this.properties[0];
		this.acc = this.properties[1];
		this.dec = this.properties[2];
		this.jumpStrength = this.properties[3];
		this.g = this.properties[4];
		this.g1 = this.g;
		this.maxFall = this.properties[5];
		this.enableDoubleJump = (this.properties[6] !== 0);	// 0=disabled, 1=enabled
		this.jumpSustain = (this.properties[7] / 1000);		// convert ms to s
		this.defaultControls = (this.properties[8] === 1);	// 0=no, 1=yes
		this.enabled = (this.properties[9] !== 0);
		this.wasOnFloor = false;
		this.wasOverJumpthru = this.runtime.testOverlapJumpThru(this.inst);
		this.loadOverJumpthru = -1;
		
		this.sustainTime = 0;				// time of jump sustain remaining

		// Angle of gravity
		this.ga = cr.to_radians(90);
		this.updateGravity();
		
		var self = this;
		
		// Only bind keyboard events via jQuery if default controls are in use
		if (this.defaultControls && !this.runtime.isDomFree)
		{
			jQuery(document).keydown(function(info) {
						self.onKeyDown(info);
					});
			
			jQuery(document).keyup(function(info) {
						self.onKeyUp(info);
					});
		}
		
		// Need to know if floor object gets destroyed
		if (!this.recycled)
		{
			this.myDestroyCallback = function(inst) {
										self.onInstanceDestroyed(inst);
									};
		}
										
		this.runtime.addDestroyCallback(this.myDestroyCallback);
		
		this.inst.extra["isPlatformBehavior"] = true;
	};
	
	behinstProto.saveToJSON = function ()
	{
		return {
			"ii": this.ignoreInput,
			"lfx": this.lastFloorX,
			"lfy": this.lastFloorY,
			"lfo": (this.lastFloorObject ? this.lastFloorObject.uid : -1),
			"am": this.animMode,
			"en": this.enabled,
			"fall": this.fallthrough,
			"ft": this.firstTick,
			"dx": this.dx,
			"dy": this.dy,
			"ms": this.maxspeed,
			"acc": this.acc,
			"dec": this.dec,
			"js": this.jumpStrength,
			"g": this.g,
			"g1": this.g1,
			"mf": this.maxFall,
			"wof": this.wasOnFloor,
			"woj": (this.wasOverJumpthru ? this.wasOverJumpthru.uid : -1),
			"ga": this.ga,
			"edj": this.enableDoubleJump,
			"cdj": this.canDoubleJump,
			"dj": this.doubleJumped,
			"sus": this.jumpSustain
		};
	};
	
	behinstProto.loadFromJSON = function (o)
	{
		this.ignoreInput = o["ii"];
		this.lastFloorX = o["lfx"];
		this.lastFloorY = o["lfy"];
		this.loadFloorObject = o["lfo"];
		this.animMode = o["am"];
		this.enabled = o["en"];
		this.fallthrough = o["fall"];
		this.firstTick = o["ft"];
		this.dx = o["dx"];
		this.dy = o["dy"];
		this.maxspeed = o["ms"];
		this.acc = o["acc"];
		this.dec = o["dec"];
		this.jumpStrength = o["js"];
		this.g = o["g"];
		this.g1 = o["g1"];
		this.maxFall = o["mf"];
		this.wasOnFloor = o["wof"];
		this.loadOverJumpthru = o["woj"];
		this.ga = o["ga"];
		this.enableDoubleJump = o["edj"];
		this.canDoubleJump = o["cdj"];
		this.doubleJumped = o["dj"];
		this.jumpSustain = o["sus"];
		
		this.leftkey = false;
		this.rightkey = false;
		this.jumpkey = false;
		this.jumped = false;
		this.simleft = false;
		this.simright = false;
		this.simjump = false;
		this.sustainTime = 0;
		this.updateGravity();
	};
	
	behinstProto.afterLoad = function ()
	{
		if (this.loadFloorObject === -1)
			this.lastFloorObject = null;
		else
			this.lastFloorObject = this.runtime.getObjectByUID(this.loadFloorObject);
			
		if (this.loadOverJumpthru === -1)
			this.wasOverJumpthru = null;
		else
			this.wasOverJumpthru = this.runtime.getObjectByUID(this.loadOverJumpthru);
	};
	
	behinstProto.onInstanceDestroyed = function (inst)
	{
		// Floor object being destroyed
		if (this.lastFloorObject == inst)
			this.lastFloorObject = null;
	};
	
	behinstProto.onDestroy = function ()
	{
		this.lastFloorObject = null;
		this.runtime.removeDestroyCallback(this.myDestroyCallback);
	};

	behinstProto.onKeyDown = function (info)
	{	
		switch (info.which) {
		case 38:	// up
			info.preventDefault();
			this.jumpkey = true;
			break;
		case 37:	// left
			info.preventDefault();
			this.leftkey = true;
			break;
		case 39:	// right
			info.preventDefault();
			this.rightkey = true;
			break;
		}
	};

	behinstProto.onKeyUp = function (info)
	{
		switch (info.which) {
		case 38:	// up
			info.preventDefault();
			this.jumpkey = false;
			this.jumped = false;
			break;
		case 37:	// left
			info.preventDefault();
			this.leftkey = false;
			break;
		case 39:	// right
			info.preventDefault();
			this.rightkey = false;
			break;
		}
	};
	
	behinstProto.getGDir = function ()
	{
		if (this.g < 0)
			return -1;
		else
			return 1;
	};

	behinstProto.isOnFloor = function ()
	{
		var ret = null;
		var ret2 = null;
		var i, len, j;
		
		// Move object one pixel down
		var oldx = this.inst.x;
		var oldy = this.inst.y;
		this.inst.x += this.downx;
		this.inst.y += this.downy;
		this.inst.set_bbox_changed();
		
		// See if still overlapping last floor object (if any)
		if (this.lastFloorObject && this.runtime.testOverlap(this.inst, this.lastFloorObject))
		{
			// Put the object back
			this.inst.x = oldx;
			this.inst.y = oldy;
			this.inst.set_bbox_changed();
			return this.lastFloorObject;
		}
		else
		{
			ret = this.runtime.testOverlapSolid(this.inst);
			
			if (!ret && this.fallthrough === 0)
				ret2 = this.runtime.testOverlapJumpThru(this.inst, true);
			
			// Put the object back
			this.inst.x = oldx;
			this.inst.y = oldy;
			this.inst.set_bbox_changed();
			
			if (ret)		// was overlapping solid
			{
				// If the object is still overlapping the solid one pixel up, it
				// must be stuck inside something.  So don't count it as floor.
				if (this.runtime.testOverlap(this.inst, ret))
					return null;
				else
				{
					this.floorIsJumpthru = false;
					return ret;
				}
			}
			
			// Is overlapping one or more jumpthrus
			if (ret2 && ret2.length)
			{
				// Filter out jumpthrus it is still overlapping one pixel up
				for (i = 0, j = 0, len = ret2.length; i < len; i++)
				{
					ret2[j] = ret2[i];
					
					if (!this.runtime.testOverlap(this.inst, ret2[i]))
						j++;
				}
				
				// All jumpthrus it is only overlapping one pixel down are floor pieces/tiles.
				// Return first in list.
				if (j >= 1)
				{
					this.floorIsJumpthru = true;
					return ret2[0];
				}
			}
			
			return null;
		}
	};
	
	behinstProto.tick = function ()
	{
	};

	behinstProto.posttick = function ()
	{
		var dt = this.runtime.getDt(this.inst);
		var mx, my, obstacle, mag, allover, i, len, j, oldx, oldy;
		
		// The "jumped" flag needs resetting whenever the jump key is not simulated for custom controls
		// This musn't conflict with default controls so make sure neither the jump key nor simulate jump is on
		if (!this.jumpkey && !this.simjump)
			this.jumped = false;
			
		var left = this.leftkey || this.simleft;
		var right = this.rightkey || this.simright;
		var jumpkey = (this.jumpkey || this.simjump);
		var jump = jumpkey && !this.jumped;
		this.simleft = false;
		this.simright = false;
		this.simjump = false;
		
		if (!this.enabled)
			return;
		
		// Ignoring input: ignore all keys
		if (this.ignoreInput)
		{
			left = false;
			right = false;
			jumpkey = false;
			jump = false;
		}
		
		if (!jumpkey)
			this.sustainTime = 0;
		
		var lastFloor = this.lastFloorObject;
		var floor_moved = false;
		
		// On first tick, push up out the floor with sub-pixel precision.  This resolves 1px float issues
		// with objects placed starting exactly on the floor.
		if (this.firstTick)
		{
			if (this.runtime.testOverlapSolid(this.inst) || this.runtime.testOverlapJumpThru(this.inst))
			{
				this.runtime.pushOutSolid(this.inst, -this.downx, -this.downy, 4, true);
			}
			
			this.firstTick = false;
		}
		
		// Track moving platforms
		if (lastFloor && this.dy === 0 && (lastFloor.y !== this.lastFloorY || lastFloor.x !== this.lastFloorX))
		{
			mx = (lastFloor.x - this.lastFloorX);
			my = (lastFloor.y - this.lastFloorY);
			this.inst.x += mx;
			this.inst.y += my;
			this.inst.set_bbox_changed();
			this.lastFloorX = lastFloor.x;
			this.lastFloorY = lastFloor.y;
			floor_moved = true;
			
			// Platform moved player in to a solid: push out of the solid again
			if (this.runtime.testOverlapSolid(this.inst))
			{
				this.runtime.pushOutSolid(this.inst, -mx, -my, Math.sqrt(mx * mx + my * my) * 2.5);
			}
		}
		
		// Test if on floor
		var floor_ = this.isOnFloor();
		
		// Push out nearest here to prevent moving objects crushing/trapping the player.
		// Skip this when input predicted by the multiplayer object, since it just conflicts horribly and
		// makes the player wobble all over the place.
		var collobj = this.runtime.testOverlapSolid(this.inst);
		if (collobj)
		{
			if (this.inst.extra["inputPredicted"])
			{
				this.runtime.pushOutSolid(this.inst, -this.downx, -this.downy, 10, false);
			}
			else if (this.runtime.pushOutSolidNearest(this.inst, Math.max(this.inst.width, this.inst.height) / 2))
			{
				this.runtime.registerCollision(this.inst, collobj);
			}
			// If can't push out, must be stuck, give up
			else
				return;
		}
		
		if (floor_)
		{
			this.doubleJumped = false;		// reset double jump flags for next jump
			this.canDoubleJump = false;
			
			if (this.dy > 0)
			{
				// By chance we may have fallen perfectly to 1 pixel above the floor, which might make
				// isOnFloor return true before we've had a pushOutSolid from the floor to make us sit
				// tightly on it.  So we might actually be hovering 1 pixel in the air.  To resolve this,
				// if this is the first landing issue another pushInFractional.
				if (!this.wasOnFloor)
				{
					this.runtime.pushInFractional(this.inst, -this.downx, -this.downy, floor_, 16);
					this.wasOnFloor = true;
				}
					
				this.dy = 0;
			}

			// First landing on the floor or floor changed
			if (lastFloor != floor_)
			{
				this.lastFloorObject = floor_;
				this.lastFloorX = floor_.x;
				this.lastFloorY = floor_.y;
				this.runtime.registerCollision(this.inst, floor_);
			}
			// If the floor has moved, check for moving in to a solid
			else if (floor_moved)
			{
				collobj = this.runtime.testOverlapSolid(this.inst);
				if (collobj)
				{
					this.runtime.registerCollision(this.inst, collobj);
					
					// Push out horizontally then up
					if (mx !== 0)
					{
						if (mx > 0)
							this.runtime.pushOutSolid(this.inst, -this.rightx, -this.righty);
						else
							this.runtime.pushOutSolid(this.inst, this.rightx, this.righty);
					}

					this.runtime.pushOutSolid(this.inst, -this.downx, -this.downy);
				}
			}
		}
		// not on floor
		else
		{
			// If in mid-air and not holding jump key, set flag ready for double jump
			if (!jumpkey)
				this.canDoubleJump = true;
		}
		
		// If jumping from floor or double-jumping in mid-air
		if ((floor_ && jump) || (!floor_ && this.enableDoubleJump && jumpkey && this.canDoubleJump && !this.doubleJumped))
		{			
			// Check we can move up 1px else assume jump is blocked.
			oldx = this.inst.x;
			oldy = this.inst.y;
			this.inst.x -= this.downx;
			this.inst.y -= this.downy;
			this.inst.set_bbox_changed();
			
			if (!this.runtime.testOverlapSolid(this.inst))
			{
				// Reset sustain time
				this.sustainTime = this.jumpSustain;
				
				// Trigger On Jump
				this.runtime.trigger(cr.behaviors.Platform.prototype.cnds.OnJump, this.inst);
				this.animMode = ANIMMODE_JUMPING;
				this.dy = -this.jumpStrength;
				jump = true;		// set in case is double jump
				
				// Prevent bunnyhopping: dont allow another jump until key up
				if (floor_)
					this.jumped = true;
				else
					this.doubleJumped = true;
			}
			else
				jump = false;
				
			this.inst.x = oldx;
			this.inst.y = oldy;
			this.inst.set_bbox_changed();
		}

		// Not on floor: apply gravity
		if (!floor_)
		{
			// Holding jump sustain
			if (jumpkey && this.sustainTime > 0)
			{
				this.dy = -this.jumpStrength;
				this.sustainTime -= dt;
			}
			// Otherwise apply gravity
			else
			{
				this.lastFloorObject = null;
				
				this.dy += this.g * dt;
				
				// Cap to max fall speed
				if (this.dy > this.maxFall)
					this.dy = this.maxFall;
			}
			
			// Still set the jumped flag to prevent double tap bunnyhop
			if (jump)
				this.jumped = true;
		}
		
		this.wasOnFloor = !!floor_;
		
		// Apply horizontal deceleration when no arrow key pressed
		if (left == right)	// both up or both down
		{
			if (this.dx < 0)
			{
				this.dx += this.dec * dt;
				
				if (this.dx > 0)
					this.dx = 0;
			}
			else if (this.dx > 0)
			{
				this.dx -= this.dec * dt;
				
				if (this.dx < 0)
					this.dx = 0;
			}
		}
		
		// Apply acceleration
		if (left && !right)
		{
			// Moving in opposite direction to current motion: add deceleration
			if (this.dx > 0)
				this.dx -= (this.acc + this.dec) * dt;
			else
				this.dx -= this.acc * dt;
		}
		
		if (right && !left)
		{
			if (this.dx < 0)
				this.dx += (this.acc + this.dec) * dt;
			else
				this.dx += this.acc * dt;
		}
		
		// Cap to max speed
		if (this.dx > this.maxspeed)
			this.dx = this.maxspeed;
		else if (this.dx < -this.maxspeed)
			this.dx = -this.maxspeed;
		
		var landed = false;
		
		if (this.dx !== 0)
		{		
			// Attempt X movement
			oldx = this.inst.x;
			oldy = this.inst.y;
			mx = this.dx * dt * this.rightx;
			my = this.dx * dt * this.righty;
			
			// Check that 1 px across and 1 px up is free.  Otherwise the slope is too steep to
			// try climbing.
			this.inst.x += this.rightx * (this.dx > 1 ? 1 : -1) - this.downx;
			this.inst.y += this.righty * (this.dx > 1 ? 1 : -1) - this.downy;
			this.inst.set_bbox_changed();
			
			var is_jumpthru = false;
			
			var slope_too_steep = this.runtime.testOverlapSolid(this.inst);
			
			/*
			if (!slope_too_steep && floor_)
			{
				slope_too_steep = this.runtime.testOverlapJumpThru(this.inst);
				is_jumpthru = true;
				
				// Check not also overlapping jumpthru from original position, in which
				// case ignore it as a bit of background.
				if (slope_too_steep)
				{
					this.inst.x = oldx;
					this.inst.y = oldy;
					this.inst.set_bbox_changed();
					
					if (this.runtime.testOverlap(this.inst, slope_too_steep))
					{
						slope_too_steep = null;
						is_jumpthru = false;
					}
				}
			}
			*/

			// Move back and move the real amount
			this.inst.x = oldx + mx;
			this.inst.y = oldy + my;
			this.inst.set_bbox_changed();
			
			// Test for overlap to side.
			obstacle = this.runtime.testOverlapSolid(this.inst);

			if (!obstacle && floor_)
			{
				obstacle = this.runtime.testOverlapJumpThru(this.inst);
				
				// Check not also overlapping jumpthru from original position, in which
				// case ignore it as a bit of background.
				if (obstacle)
				{
					this.inst.x = oldx;
					this.inst.y = oldy;
					this.inst.set_bbox_changed();
					
					if (this.runtime.testOverlap(this.inst, obstacle))
					{
						obstacle = null;
						is_jumpthru = false;
					}
					else
						is_jumpthru = true;
						
					this.inst.x = oldx + mx;
					this.inst.y = oldy + my;
					this.inst.set_bbox_changed();
				}
			}
			
			if (obstacle)
			{
				// First try pushing out up the same distance that was moved horizontally.
				// If this works it's an acceptable slope.
				var push_dist = Math.abs(this.dx * dt) + 2;
				
				if (slope_too_steep || !this.runtime.pushOutSolid(this.inst, -this.downx, -this.downy, push_dist, is_jumpthru, obstacle))
				{
					// Failed to push up out of slope.  Must be a wall - push back horizontally.
					// Push either 2.5x the horizontal distance moved this tick, or at least 30px.
					this.runtime.registerCollision(this.inst, obstacle);
					push_dist = Math.max(Math.abs(this.dx * dt * 2.5), 30);
					
					// Push out of solid: push left if moving right, or push right if moving left
					if (!this.runtime.pushOutSolid(this.inst, this.rightx * (this.dx < 0 ? 1 : -1), this.righty * (this.dx < 0 ? 1 : -1), push_dist, false))
					{
						// Failed to push out of solid.  Restore old position.
						this.inst.x = oldx;
						this.inst.y = oldy;
						this.inst.set_bbox_changed();
					}
					else if (floor_ && !is_jumpthru && !this.floorIsJumpthru)
					{
						// Push out wall horizontally succeeded. The player might be on a slope, in which case they might
						// now be hovering in the air slightly. So push 1px in to the floor and push out again.
						oldx = this.inst.x;
						oldy = this.inst.y;
						this.inst.x += this.downx;
						this.inst.y += this.downy;
						
						if (this.runtime.testOverlapSolid(this.inst))
						{
							if (!this.runtime.pushOutSolid(this.inst, -this.downx, -this.downy, 3, false))
							{
								// Failed to push out of solid.  Restore old position.
								this.inst.x = oldx;
								this.inst.y = oldy;
								this.inst.set_bbox_changed();
							}
						}
						else
						{
							// Not over a solid. Put it back.
							this.inst.x = oldx;
							this.inst.y = oldy;
							this.inst.set_bbox_changed();
						}
					}
					
					if (!is_jumpthru)
						this.dx = 0;	// stop
				}
				else if (!slope_too_steep && !jump && (Math.abs(this.dy) < Math.abs(this.jumpStrength / 4)))
				{
					// Must have pushed up out of slope.  Set dy to 0 to handle rare edge case when
					// jumping on to a platform from the side triggers slope detection upon landing.
					this.dy = 0;
					
					// On this rare occasion, if the player was not on the floor, they may have landed without
					// ever having been falling.  This will mean 'On landed' doesn't trigger, so trigger it now.
					if (!floor_)
						landed = true;
				}
			}
			else
			{
				// Was on floor but now isn't
				var newfloor = this.isOnFloor();
				if (floor_ && !newfloor)
				{
					// Moved horizontally but not overlapping anything.  Push down
					// to keep feet on downwards slopes (to an extent).
					mag = Math.ceil(Math.abs(this.dx * dt)) + 2;
					oldx = this.inst.x;
					oldy = this.inst.y;
					this.inst.x += this.downx * mag;
					this.inst.y += this.downy * mag;
					this.inst.set_bbox_changed();
					
					if (this.runtime.testOverlapSolid(this.inst) || this.runtime.testOverlapJumpThru(this.inst))
						this.runtime.pushOutSolid(this.inst, -this.downx, -this.downy, mag + 2, true);
					else
					{
						this.inst.x = oldx;
						this.inst.y = oldy;
						this.inst.set_bbox_changed();
					}
				}
				else if (newfloor && this.dy === 0)
				{
					// Push in to the floor fractionally to ensure player stays tightly on ground
					this.runtime.pushInFractional(this.inst, -this.downx, -this.downy, newfloor, 16);
				}
			}
		}
		
		if (this.dy !== 0)
		{
			// Attempt Y movement
			oldx = this.inst.x;
			oldy = this.inst.y;
			this.inst.x += this.dy * dt * this.downx;
			this.inst.y += this.dy * dt * this.downy;
			var newx = this.inst.x;
			var newy = this.inst.y;
			this.inst.set_bbox_changed();
			
			collobj = this.runtime.testOverlapSolid(this.inst);
			
			var fell_on_jumpthru = false;
			
			if (!collobj && (this.dy > 0) && !floor_)
			{
				// Get all jump-thrus currently overlapping
				allover = this.fallthrough > 0 ? null : this.runtime.testOverlapJumpThru(this.inst, true);
				
				// Filter out all objects it is not overlapping in its old position
				if (allover && allover.length)
				{
					// Special case to support vertically moving jumpthrus.
					if (this.wasOverJumpthru)
					{
						this.inst.x = oldx;
						this.inst.y = oldy;
						this.inst.set_bbox_changed();
						
						for (i = 0, j = 0, len = allover.length; i < len; i++)
						{
							allover[j] = allover[i];
							
							if (!this.runtime.testOverlap(this.inst, allover[i]))
								j++;
						}
						
						allover.length = j;
							
						this.inst.x = newx;
						this.inst.y = newy;
						this.inst.set_bbox_changed();
					}
					
					if (allover.length >= 1)
						collobj = allover[0];
				}
				
				fell_on_jumpthru = !!collobj;
			}
			
			if (collobj)
			{
				this.runtime.registerCollision(this.inst, collobj);
				this.sustainTime = 0;
				
				// Push either 2.5x the vertical distance (+10px) moved this tick, or at least 30px. Don't clamp to 30px when falling on a jumpthru.
				var push_dist = (fell_on_jumpthru ? Math.abs(this.dy * dt * 2.5 + 10) : Math.max(Math.abs(this.dy * dt * 2.5 + 10), 30));
				
				// Push out of solid: push down if moving up, or push up if moving down
				if (!this.runtime.pushOutSolid(this.inst, this.downx * (this.dy < 0 ? 1 : -1), this.downy * (this.dy < 0 ? 1 : -1), push_dist, fell_on_jumpthru, collobj))
				{
					// Failed to push out of solid.  Restore old position.
					this.inst.x = oldx;
					this.inst.y = oldy;
					this.inst.set_bbox_changed();
					this.wasOnFloor = true;		// prevent adjustment for unexpected floor landings
					
					// If shearing through a jump-thru while falling, we fail the push out, but
					// want to let the player keep falling, so don't stop them.
					if (!fell_on_jumpthru)
						this.dy = 0;	// stop
				}
				else
				{
					this.lastFloorObject = collobj;
					this.lastFloorX = collobj.x;
					this.lastFloorY = collobj.y;
					this.floorIsJumpthru = fell_on_jumpthru;
					
					// Make sure 'On landed' triggers for landing on a jumpthru
					if (fell_on_jumpthru)
						landed = true;
						
					this.dy = 0;	// stop
				}
			}
		}
		
		// Run animation triggers
		
		// Has started falling?
		if (this.animMode !== ANIMMODE_FALLING && this.dy > 0 && !floor_)
		{
			this.runtime.trigger(cr.behaviors.Platform.prototype.cnds.OnFall, this.inst);
			this.animMode = ANIMMODE_FALLING;
		}
		
		// Is on floor?
		if (floor_ || landed)
		{
			// Was falling? (i.e. has just landed) or has jumped, but jump was blocked
			if (this.animMode === ANIMMODE_FALLING || landed || (jump && this.dy === 0))
			{
				this.runtime.trigger(cr.behaviors.Platform.prototype.cnds.OnLand, this.inst);
				
				if (this.dx === 0 && this.dy === 0)
					this.animMode = ANIMMODE_STOPPED;
				else
					this.animMode = ANIMMODE_MOVING;
			}
			// Has not just landed: handle normal moving/stopped triggers
			else
			{
				if (this.animMode !== ANIMMODE_STOPPED && this.dx === 0 && this.dy === 0)
				{
					this.runtime.trigger(cr.behaviors.Platform.prototype.cnds.OnStop, this.inst);
					this.animMode = ANIMMODE_STOPPED;
				}
				
				// Has started moving and is on floor?
				if (this.animMode !== ANIMMODE_MOVING && (this.dx !== 0 || this.dy !== 0) && !jump)
				{
					this.runtime.trigger(cr.behaviors.Platform.prototype.cnds.OnMove, this.inst);
					this.animMode = ANIMMODE_MOVING;
				}
			}
		}
		
		if (this.fallthrough > 0)
			this.fallthrough--;
			
		this.wasOverJumpthru = this.runtime.testOverlapJumpThru(this.inst);
	};
	
	/**BEGIN-PREVIEWONLY**/
	var animmodes = ["stopped", "moving", "jumping", "falling"];
	behinstProto.getDebuggerValues = function (propsections)
	{
		propsections.push({
			"title": this.type.name,
			"properties": [
				{"name": "Vector X", "value": this.dx},
				{"name": "Vector Y", "value": this.dy},
				{"name": "Max speed", "value": this.maxspeed},
				{"name": "Acceleration", "value": this.acc},
				{"name": "Deceleration", "value": this.dec},
				{"name": "Jump strength", "value": this.jumpStrength},
				{"name": "Gravity", "value": this.g},
				{"name": "Gravity angle", "value": cr.to_degrees(this.ga)},
				{"name": "Max fall speed", "value": this.maxFall},
				{"name": "Animation mode", "value": animmodes[this.animMode], "readonly": true},
				{"name": "Enabled", "value": this.enabled}
			]
		});
	};
	
	behinstProto.onDebugValueEdited = function (header, name, value)
	{
		switch (name) {
		case "Vector X":					this.dx = value;					break;
		case "Vector Y":					this.dy = value;					break;
		case "Max speed":					this.maxspeed = value;				break;
		case "Acceleration":				this.acc = value;					break;
		case "Deceleration":				this.dec = value;					break;
		case "Jump strength":				this.jumpStrength = value;			break;
		case "Gravity":						this.g = value;						break;
		case "Gravity angle":				this.ga = cr.to_radians(value);		break;
		case "Max fall speed":				this.maxFall = value;				break;
		case "Enabled":						this.enabled = value;				break;
		}
		
		this.updateGravity();
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	Cnds.prototype.IsMoving = function ()
	{
		return this.dx !== 0 || this.dy !== 0;
	};
	
	Cnds.prototype.CompareSpeed = function (cmp, s)
	{
		var speed = Math.sqrt(this.dx * this.dx + this.dy * this.dy);
		
		return cr.do_cmp(speed, cmp, s);
	};
	
	Cnds.prototype.IsOnFloor = function ()
	{
		if (this.dy !== 0)
			return false;
			
		var ret = null;
		var ret2 = null;
		var i, len, j;
		
		// Move object one pixel down
		var oldx = this.inst.x;
		var oldy = this.inst.y;
		this.inst.x += this.downx;
		this.inst.y += this.downy;
		this.inst.set_bbox_changed();
		
		ret = this.runtime.testOverlapSolid(this.inst);
		
		if (!ret && this.fallthrough === 0)
			ret2 = this.runtime.testOverlapJumpThru(this.inst, true);
		
		// Put the object back
		this.inst.x = oldx;
		this.inst.y = oldy;
		this.inst.set_bbox_changed();
		
		if (ret)		// was overlapping solid
		{
			// If the object is still overlapping the solid one pixel up, it
			// must be stuck inside something.  So don't count it as floor.
			return !this.runtime.testOverlap(this.inst, ret);
		}
		
		// Is overlapping one or more jumpthrus
		if (ret2 && ret2.length)
		{
			// Filter out jumpthrus it is still overlapping one pixel up
			for (i = 0, j = 0, len = ret2.length; i < len; i++)
			{
				ret2[j] = ret2[i];
				
				if (!this.runtime.testOverlap(this.inst, ret2[i]))
					j++;
			}
			
			// All jumpthrus it is only overlapping one pixel down are floor pieces/tiles.
			// Return first in list.
			if (j >= 1)
				return true;
		}
		
		return false;
	};
	
	Cnds.prototype.IsByWall = function (side)
	{
		// Move 1px up to side and make sure not overlapping anything
		var ret = false;
		var oldx = this.inst.x;
		var oldy = this.inst.y;
		
		this.inst.x -= this.downx * 3;
		this.inst.y -= this.downy * 3;
		
		// Is overlapping solid above: must be hitting head on ceiling, don't count as wall
		this.inst.set_bbox_changed();
		if (this.runtime.testOverlapSolid(this.inst))
		{
			this.inst.x = oldx;
			this.inst.y = oldy;
			this.inst.set_bbox_changed();
			return false;
		}
		
		// otherwise move to side
		if (side === 0)		// left
		{
			this.inst.x -= this.rightx * 2;
			this.inst.y -= this.righty * 2;
		}
		else
		{
			this.inst.x += this.rightx * 2;
			this.inst.y += this.righty * 2;
		}
		
		this.inst.set_bbox_changed();
		
		// Is touching solid to side
		ret = this.runtime.testOverlapSolid(this.inst);
		
		this.inst.x = oldx;
		this.inst.y = oldy;
		this.inst.set_bbox_changed();
		
		return ret;
	};
	
	Cnds.prototype.IsJumping = function ()
	{
		return this.dy < 0;
	};
	
	Cnds.prototype.IsFalling = function ()
	{
		return this.dy > 0;
	};
	
	Cnds.prototype.OnJump = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnFall = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnStop = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnMove = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnLand = function ()
	{
		return true;
	};
	
	Cnds.prototype.IsDoubleJumpEnabled = function ()
	{
		return this.enableDoubleJump;
	};
	
	behaviorProto.cnds = new Cnds();

	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.SetIgnoreInput = function (ignoring)
	{
		this.ignoreInput = ignoring;
	};
	
	Acts.prototype.SetMaxSpeed = function (maxspeed)
	{
		this.maxspeed = maxspeed;
		
		if (this.maxspeed < 0)
			this.maxspeed = 0;
	};
	
	Acts.prototype.SetAcceleration = function (acc)
	{
		this.acc = acc;
		
		if (this.acc < 0)
			this.acc = 0;
	};
	
	Acts.prototype.SetDeceleration = function (dec)
	{
		this.dec = dec;
		
		if (this.dec < 0)
			this.dec = 0;
	};
	
	Acts.prototype.SetJumpStrength = function (js)
	{
		this.jumpStrength = js;
		
		if (this.jumpStrength < 0)
			this.jumpStrength = 0;
	};
	
	Acts.prototype.SetGravity = function (grav)
	{
		if (this.g1 === grav)
			return;		// no change
		
		this.g = grav;
		this.updateGravity();
		
		// Push up to 10px out any current solid to prevent glitches
		if (this.runtime.testOverlapSolid(this.inst))
		{
			this.runtime.pushOutSolid(this.inst, this.downx, this.downy, 10);
			
			// Bodge to workaround 1px float causing pushOutSolidNearest
			this.inst.x += this.downx * 2;
			this.inst.y += this.downy * 2;
			this.inst.set_bbox_changed();
		}
		
		// Allow to fall off current floor in case direction of gravity changed
		this.lastFloorObject = null;
	};
	
	Acts.prototype.SetMaxFallSpeed = function (mfs)
	{
		this.maxFall = mfs;
		
		if (this.maxFall < 0)
			this.maxFall = 0;
	};
	
	Acts.prototype.SimulateControl = function (ctrl)
	{
		// 0=left, 1=right, 2=jump
		switch (ctrl) {
		case 0:		this.simleft = true;	break;
		case 1:		this.simright = true;	break;
		case 2:		this.simjump = true;	break;
		}
	};
	
	Acts.prototype.SetVectorX = function (vx)
	{
		this.dx = vx;
	};
	
	Acts.prototype.SetVectorY = function (vy)
	{
		this.dy = vy;
	};
	
	Acts.prototype.SetGravityAngle = function (a)
	{
		a = cr.to_radians(a);
		a = cr.clamp_angle(a);
		
		if (this.ga === a)
			return;		// no change
			
		this.ga = a;
		this.updateGravity();
		
		// Allow to fall off current floor in case direction of gravity changed
		this.lastFloorObject = null;
	};
	
	Acts.prototype.SetEnabled = function (en)
	{
		if (this.enabled !== (en === 1))
		{
			this.enabled = (en === 1);
			
			// when disabling, drop the last floor object, otherwise resets to the moving platform when enabled again
			if (!this.enabled)
				this.lastFloorObject = null;
		}
	};
	
	Acts.prototype.FallThrough = function ()
	{
		// Test is standing on jumpthru 1px down
		var oldx = this.inst.x;
		var oldy = this.inst.y;
		this.inst.x += this.downx;
		this.inst.y += this.downy;
		this.inst.set_bbox_changed();
		
		var overlaps = this.runtime.testOverlapJumpThru(this.inst, false);
		
		this.inst.x = oldx;
		this.inst.y = oldy;
		this.inst.set_bbox_changed();
		
		if (!overlaps)
			return;
			
		this.fallthrough = 3;			// disable jumpthrus for 3 ticks (1 doesn't do it, 2 does, 3 to be on safe side)
		this.lastFloorObject = null;
	};
	
	Acts.prototype.SetDoubleJumpEnabled = function (e)
	{
		this.enableDoubleJump = (e !== 0);
	};
	
	Acts.prototype.SetJumpSustain = function (s)
	{
		this.jumpSustain = s / 1000;		// convert to ms
	};
	
	behaviorProto.acts = new Acts();

	//////////////////////////////////////
	// Expressions
	function Exps() {};

	Exps.prototype.Speed = function (ret)
	{
		ret.set_float(Math.sqrt(this.dx * this.dx + this.dy * this.dy));
	};
	
	Exps.prototype.MaxSpeed = function (ret)
	{
		ret.set_float(this.maxspeed);
	};
	
	Exps.prototype.Acceleration = function (ret)
	{
		ret.set_float(this.acc);
	};
	
	Exps.prototype.Deceleration = function (ret)
	{
		ret.set_float(this.dec);
	};
	
	Exps.prototype.JumpStrength = function (ret)
	{
		ret.set_float(this.jumpStrength);
	};
	
	Exps.prototype.Gravity = function (ret)
	{
		ret.set_float(this.g);
	};
	
	Exps.prototype.GravityAngle = function (ret)
	{
		ret.set_float(cr.to_degrees(this.ga));
	};
	
	Exps.prototype.MaxFallSpeed = function (ret)
	{
		ret.set_float(this.maxFall);
	};
	
	Exps.prototype.MovingAngle = function (ret)
	{
		ret.set_float(cr.to_degrees(Math.atan2(this.dy, this.dx)));
	};
	
	Exps.prototype.VectorX = function (ret)
	{
		ret.set_float(this.dx);
	};
	
	Exps.prototype.VectorY = function (ret)
	{
		ret.set_float(this.dy);
	};
	
	Exps.prototype.JumpSustain = function (ret)
	{
		ret.set_float(this.jumpSustain * 1000);		// convert back to ms
	};
	
	behaviorProto.exps = new Exps();
	
}());