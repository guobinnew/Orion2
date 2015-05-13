// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.shadowlight = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	/////////////////////////////////////
	var pluginProto = cr.plugins_.shadowlight.prototype;
		
	/////////////////////////////////////
	// Object type class
	pluginProto.Type = function(plugin)
	{
		this.plugin = plugin;
		this.runtime = plugin.runtime;
	};

	var typeProto = pluginProto.Type.prototype;
	
	var loaded_penumbra_img = false;
	var penumbra_img = null;
	var penumbra_data = null;

	// called on startup for each object type
	typeProto.onCreate = function()
	{
		if (this.is_family)
			return;
		
		if (!loaded_penumbra_img)
		{
			loaded_penumbra_img = true;
			penumbra_img = new Image();
			penumbra_img.cr_filesize = 15000;
			this.runtime.waitForImageLoad(penumbra_img, "penumbra.png");
		}
	};
	
	typeProto.onLostWebGLContext = function ()
	{
		if (this.is_family)
			return;
			
		this.webGL_texture = null;
		this.penumbra_texture = null;
		
		var i, len, inst;
		for (i = 0, len = this.instances.length; i < len; ++i)
		{
			inst = this.instances[i];
			inst.webGL_texture = null;
			inst.penumbra_texture = null;
		}
	};
	
	typeProto.onRestoreWebGLContext = function ()
	{
		if (this.is_family)
			return;
		
		var i, len;
		for (i = 0, len = this.instances.length; i < len; ++i)
		{
			this.instances[i].createTextures();
		}
	};

	/////////////////////////////////////
	// Instance class
	pluginProto.Instance = function(type)
	{
		this.type = type;
		this.runtime = type.runtime;
		
		// any other properties you need, e.g...
		// this.myValue = 0;
	};
	
	var instanceProto = pluginProto.Instance.prototype;

	// called whenever an instance is created
	instanceProto.onCreate = function()
	{
		// note the object is sealed after this call; ensure any properties you'll ever need are set on the object
		// e.g...
		// this.myValue = 0;
		
		maybeGetPenumbraData();		// get the pixel data for the penumbra image
		
		this.lightX = this.x;
		this.lightY = this.y;
		this.lightZ = this.properties[0];
		this.max_extrude = 1000;
		
		this.lightRadius = this.properties[1];
		
		this.castFrom = this.properties[2];		// 0 = all, 1 = same tag, 2 = different tag
		this.tag = this.properties[3];
		
		this.lightR = 0;
		this.lightG = 0;
		this.lightB = 0;
		
		this.lastKnownX = this.x;
		this.lastKnownY = this.y;
		
		this.webGL_texture = null;
		this.penumbra_texture = null;
		
		if (this.runtime.glwrap)
		{
			this.createTextures();
		}
		else
			this.lightRadius = 0;		// cannot use light radius in canvas2d mode
		
		this.runtime.tick2Me(this);
	};
	
	instanceProto.castsFrom = function (othertag)
	{
		if (this.castFrom === 1)		// same tag
			return cr.equals_nocase(this.tag, othertag);
		else if (this.castFrom === 2)	// different tag
			return !cr.equals_nocase(this.tag, othertag);
		else							// all
			return true;
	};
	
	instanceProto.tick2 = function ()
	{
		// If position has changed, update light position
		if (this.lastKnownX !== this.x || this.lastKnownY !== this.y)
		{
			this.lightX = this.x;
			this.lightY = this.y;
		}
		
		// Position object to fill viewport exactly, so shadows can be rendered anywhere
		var layer = this.layer;
		var newx = (layer.viewLeft + layer.viewRight) / 2;
		var newy = (layer.viewTop + layer.viewBottom) / 2;
		var neww = layer.viewRight - layer.viewLeft;
		var newh = layer.viewBottom - layer.viewTop;
		
		if (newx !== this.x || newy !== this.y || neww !== this.width || newh !== this.height)
		{
			this.x = newx;
			this.y = newy;
			this.width = neww;
			this.height = newh;
			this.set_bbox_changed();
		}
		
		this.lastKnownX = this.x;
		this.lastKnownY = this.y;
		
		this.max_extrude = cr.distanceTo(layer.viewLeft, layer.viewTop, layer.viewRight, layer.viewBottom) * 1.5;
	};
	
	// called whenever an instance is destroyed
	// note the runtime may keep the object after this call for recycling; be sure
	// to release/recycle/reset any references to other objects in this function.
	instanceProto.onDestroy = function ()
	{
		if (!this.runtime.glwrap)
			return;		// not WebGL mode
		
		if (this.webGL_texture)
		{
			this.runtime.glwrap.deleteTexture(this.webGL_texture);
			this.webGL_texture = null;
		}
		
		if (this.penumbra_texture)
		{
			this.runtime.glwrap.deleteTexture(this.penumbra_texture);
			this.penumbra_texture = null;
		}
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
	
	var casters = [];
	var collrect_candidates = [];
	
	instanceProto.getShadowCasterCandidates = function ()
	{
		if (!this.runtime.shadowcasterBehavior)
			return;
		
		// Look in collision cells to quickly find possible shadow caster instances
		this.runtime.getTypesCollisionCandidates(this.layer, this.runtime.shadowcasterBehavior.myTypes, this.bbox, casters);
		
		// Collision candidates can return duplicates. For correct rendering, dump in to a set and then
		// pull out an array again to eliminate duplicates.
		cr.removeArrayDuplicates(casters);
	};
	
	// Current poly points
	var poly_pts = [];
	var poly_len = 0;
	
	// Calculated backfaces. Array of booleans corresponding to collision poly points.
	var back_faces1 = [];
	var back_faces2 = [];
	var cw_edge1 = -1;		// index of clockwise edge face
	var cw_edge2 = -1;
	var acw_edge1 = -1;		// index of anticlockwise edge face
	var acw_edge2 = -1;
	var midx = 0;
	var midy = 0;
	
	// Calculated points of current shadow
	var umbra_pts = [];
	
	var cw_umbra_rootx = 0;
	var cw_umbra_rooty = 0;
	var cw_umbra_projx = 0;
	var cw_umbra_projy = 0;
	
	var acw_umbra_rootx = 0;
	var acw_umbra_rooty = 0;
	var acw_umbra_projx = 0;
	var acw_umbra_projy = 0;
	
	var temp_poly = new cr.CollisionPoly([]);
	
	// draw to a canvas 2D context
	instanceProto.draw = function (ctx)
	{
		this.getShadowCasterCandidates();
		ctx.save();
		ctx.fillStyle = "rgba(" + this.lightR + "," + this.lightG + "," + this.lightB + "," + this.opacity + ")";
		
		var i, len, inst, j, lenj, k, lenk, h, poly, tmx, tmy, offx, offy, c, tilerc;
		for (i = 0, len = casters.length; i < len; ++i)
		{
			inst = casters[i];
			
			if (!inst.extra["shadowcasterEnabled"] || !this.castsFrom(inst.extra["shadowcasterTag"]))
				continue;
			
			inst.update_bbox();
			h = inst.extra["shadowcasterHeight"];
			
			// Render against tilemap
			if (inst.tilemap_exists)
			{
				inst.getCollisionRectCandidates(this.bbox, collrect_candidates);
				
				tmx = inst.x;
				tmy = inst.y;
				
				for (j = 0, lenj = collrect_candidates.length; j < lenj; ++j)
				{
					c = collrect_candidates[j];
					tilerc = c.rc;
					
					poly = null;
				
					// Get either collision poly or bounding box as poly
					if (c.poly)
					{
						poly = c.poly;
						offx = tilerc.left;
						offy = tilerc.top;
					}
					else
					{
						temp_poly.set_from_rect(tilerc, 0, 0);
						poly = temp_poly;
						offx = 0;
						offy = 0;
					}
					
					this.calcShadow(poly, tmx + offx, tmy + offy, h);
					
					// Fill shadow points with a canvas2d path
					ctx.beginPath();
					ctx.moveTo(umbra_pts[0], umbra_pts[1]);
					
					for (k = 2, lenk = umbra_pts.length; k < lenk; k += 2)
					{
						ctx.lineTo(umbra_pts[k], umbra_pts[k + 1]);
					}
					
					ctx.closePath();
					ctx.fill();
					
					back_faces1.length = 0;
					umbra_pts.length = 0;
				}
				
				collrect_candidates.length = 0;
			}
			// Render against ordinary object
			else
			{
				poly = null;
				
				// Get either collision poly or bounding box as poly
				if (inst.collision_poly && !inst.collision_poly.is_empty())
				{
					inst.collision_poly.cache_poly(inst.width, inst.height, inst.angle);
					poly = inst.collision_poly;
				}
				else
				{
					temp_poly.set_from_quad(inst.bquad, inst.x, inst.y, inst.width, inst.height);
					poly = temp_poly;
				}
				
				this.calcShadow(poly, inst.x, inst.y, h);
				
				// Fill shadow points with a canvas2d path
				ctx.beginPath();
				ctx.moveTo(umbra_pts[0], umbra_pts[1]);
				
				for (j = 2, lenj = umbra_pts.length; j < lenj; j += 2)
				{
					ctx.lineTo(umbra_pts[j], umbra_pts[j + 1]);
				}
				
				ctx.closePath();
				ctx.fill();
				
				back_faces1.length = 0;
				umbra_pts.length = 0;
			}
		}
		
		ctx.restore();
		
		casters.length = 0;
	};
	
	instanceProto.getPolyPoints = function (poly, instx, insty)
	{
		poly_len = poly.pts_count;
		poly_pts.length = 0;
		var x, y;
		var pts = poly.pts_cache;
		midx = 0;
		midy = 0;
		
		var i, i2;
		for (i = 0; i < poly_len; ++i)
		{
			i2 = i * 2;
			x = pts[i2] + instx;
			y = pts[i2+1] + insty;
			poly_pts.push(x);
			poly_pts.push(y);
			midx += x;
			midy += y;
		}
		
		midx /= poly_len;
		midy /= poly_len;
	};
	
	instanceProto.calcShadow = function (poly, instx, insty, h)
	{
		this.getPolyPoints(poly, instx, insty);
			
		var i, i2;
		var polyx, polyy;
		
		back_faces1.length = 0;
		back_faces2.length = 0;
		umbra_pts.length = 0;
		cw_edge1 = -1;
		acw_edge1 = -1;
		cw_edge2 = -1;
		acw_edge2 = -1;
		
		var Lx = this.lightX;
		var Ly = this.lightY;
		
		// Light radius rendering
		if (this.lightRadius > 0)
		{
			// Find right-hand edges and place results in #2 storage
			this.calcBackFaces(-this.lightRadius);
			findEdges();
			
			cr.shallowAssignArray(back_faces2, back_faces1);
			cw_edge2 = cw_edge1;
			acw_edge2 = acw_edge1;
			
			// Find left hand edges (results go in #1 storage)
			back_faces1.length = 0;
			cw_edge1 = -1;
			acw_edge1 = -1;
			
			this.calcBackFaces(this.lightRadius);
			findEdges();
			
			// Any edges missing: probably overlapping light, skip rendering
			if (cw_edge1 === -1 || acw_edge1 === -1 || cw_edge2 === -1 || acw_edge2 === -1)
				return;
			
			// Determine umbra for the given light radius
			this.calcUmbraWithRadius(h);
		}
		// Point source rendering
		else
		{
			this.calcBackFaces(0);
			findEdges();
			
			// Calculate basic shadow edges
			for (i = 0; i < poly_len; ++i)
			{
				i2 = i*2;
				polyx = poly_pts[i2];
				polyy = poly_pts[i2+1];
				
				this.calcShadowSegment(i, Lx, Ly, polyx, polyy, h);
			}
		}
	};
	
	instanceProto.calcShadowSegment = function (i, Lx, Ly, px, py, h)
	{
		var ap, dp, e, x, y, Lx, Ly;
		
		var isBackFace = back_faces1[i];
		var lastBackFace = back_faces1[wrapBackFace(i - 1)];
		var isClockwiseEdge = (i === cw_edge1);
		var isAnticlockwiseEdge = (i === acw_edge1);
		
		// If this is at the start or end of a back face, extrude out P.
		if (isBackFace || lastBackFace)
		{			
			ap = cr.angleTo(Lx, Ly, px, py);
			dp = cr.distanceTo(Lx, Ly, px, py);
			e = this.calculateExtrusion(Lx, Ly, px, py, h);
			x = Lx + Math.cos(ap) * (dp + e);
			y = Ly + Math.sin(ap) * (dp + e);
		}
		// If this is a front face, we simply line to the next point to fill the
		// collision poly shape itself
		else
		{
			x = px;
			y = py;
		}
	
		// Moving from front face to back face: put in extra point
		// at face position before extruding
		if (isAnticlockwiseEdge)
		{
			umbra_pts.push(px);
			umbra_pts.push(py);
		}
		
		umbra_pts.push(x);
		umbra_pts.push(y);
		
		// Moving from back face to front face: put in extra point
		// at face position after extruding
		if (isClockwiseEdge)
		{
			umbra_pts.push(px);
			umbra_pts.push(py);
		}
	};
	
	function isBackFace(Lx, Ly, px, py, qx, qy)
	{
		// Determine normal of poly at this point
		var n = cr.angleTo(px, py, qx, qy) - Math.PI / 2;
		
		// Angle of face to light
		var a = cr.angleTo(px, py, Lx, Ly);
		
		// Is back face if angle to light is >= 90 degrees
		return (cr.angleDiff(a, n) >= Math.PI / 2);
	};
	
	function wrapBackFace(index)
	{
		index %= back_faces1.length;
		
		if (index < 0)
			index += back_faces1.length;
		
		return index;
	};
	
	instanceProto.calcBackFaces = function (r)
	{
		var OLx = this.lightX;
		var OLy = this.lightY;
		var Lx = OLx;
		var Ly = OLy;
		
		var i, i2, imod, polyx, polyy, nextx, nexty, a;
		
		for (i = 0; i < poly_len; ++i)
		{
			i2 = i*2;
			imod = i+1;
			imod = (imod === poly_len ? 0 : imod * 2);
			
			polyx = poly_pts[i2];
			polyy = poly_pts[i2+1];
			nextx = poly_pts[imod];
			nexty = poly_pts[imod+1];
			
			if (r !== 0)
			{
				// Move light perpendicular to the poly by the radius
				a = cr.angleTo(OLx, OLy, midx, midy) - Math.PI / 2;
				Lx = OLx + Math.cos(a) * r;
				Ly = OLy + Math.sin(a) * r;
			}
			
			back_faces1.push(isBackFace(Lx, Ly, polyx, polyy, nextx, nexty));
		}
	};
	
	function findEdges()
	{
		var i, len, isBackFace, lastBackFace;
		for (i = 0, len = back_faces1.length; i < len; ++i)
		{
			isBackFace = back_faces1[i];
			lastBackFace = back_faces1[wrapBackFace(i - 1)];
			
			// Clockwise boundary point is when a back face transitions to a front face
			if (lastBackFace && !isBackFace)
				cw_edge1 = i;
			
			// Likewise, the anticlockwise boundary point is when a front face transitions to a back face
			if (!lastBackFace && isBackFace)
				acw_edge1 = i;
		}
	};
	
	var intersect_x = 0;
	var intersect_y = 0;
	
	function segment_intersection_at(p0_x, p0_y, p1_x, p1_y, p2_x, p2_y, p3_x, p3_y)
	{
		var s1_x, s1_y, s2_x, s2_y, s, t, div;
		
		s1_x = p1_x - p0_x;    
		s1_y = p1_y - p0_y;
		s2_x = p3_x - p2_x;
		s2_y = p3_y - p2_y;
		
		div = (-s2_x * s1_y + s1_x * s2_y);
		
		if (div === 0)
			return false;
		
		s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / div;
		
		div = (-s2_x * s1_y + s1_x * s2_y);
		
		if (div === 0)
			return false;
		
		t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / div;

		if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
		{
			intersect_x = p0_x + (t * s1_x);
			intersect_y = p0_y + (t * s1_y);
			return true;
		}

		return false;
	}
	
	instanceProto.calcUmbraWithRadius = function (h)
	{
		// Fill front face of poly clockwise from L2 clockwise edge
		// round to L1 anticlockwise edge
		var i = cw_edge2;
		var i2, polyx, polyy;
		
		for ( ; ; ++i)
		{
			i = i % poly_len;
			
			i2 = i * 2;
			polyx = poly_pts[i2];
			polyy = poly_pts[i2+1];
			
			umbra_pts.push(polyx);
			umbra_pts.push(polyy);
			
			if (i === acw_edge1)
				break;
		}
		
		// Determine L1 light position to anticlockwise edge (left stored in polyx, polyy)
		var r = this.lightRadius;
		
		var acw_x = polyx;
		var acw_y = polyy;
		var a = cr.angleTo(this.lightX, this.lightY, midx, midy);
		var L1x = this.lightX + Math.cos(a - Math.PI / 2) * r;
		var L1y = this.lightY + Math.sin(a - Math.PI / 2) * r;
		
		// Project umbra shadow from L1 to anticlockwise edge
		var ap = cr.angleTo(L1x, L1y, acw_x, acw_y);
		var ad = cr.distanceTo(L1x, L1y, acw_x, acw_y);
		var u1x = L1x + Math.cos(ap) * (ad + this.max_extrude);
		var u1y = L1y + Math.sin(ap) * (ad + this.max_extrude);
		
		// Determine L2 light position to clockwise edge
		i2 = cw_edge2 * 2;
		var cw_x = poly_pts[i2];
		var cw_y = poly_pts[i2+1];
		
		a = cr.angleTo(this.lightX, this.lightY, midx, midy);
		var L2x = this.lightX + Math.cos(a + Math.PI / 2) * r;
		var L2y = this.lightY + Math.sin(a + Math.PI / 2) * r;
		
		// Project umbra shadow from L2 to clockwise edge
		ap = cr.angleTo(L2x, L2y, cw_x, cw_y);
		ad = cr.distanceTo(L2x, L2y, cw_x, cw_y);
		var u2x = L2x + Math.cos(ap) * (ad + this.max_extrude);
		var u2y = L2y + Math.sin(ap) * (ad + this.max_extrude);
		
		acw_umbra_rootx = acw_x;
		acw_umbra_rooty = acw_y;
		cw_umbra_rootx = cw_x;
		cw_umbra_rooty = cw_y;
		
		// Detect umbra intersection. If segment acw -> u1 intersects cw -> u2, replace with a
		// single point at the point of intersection.
		if (segment_intersection_at(acw_x, acw_y, u1x, u1y, cw_x, cw_y, u2x, u2y))
		{
			// push one point at intersection
			umbra_pts.push(intersect_x);
			umbra_pts.push(intersect_y);
			
			acw_umbra_projx = intersect_x;
			acw_umbra_projy = intersect_y;
			cw_umbra_projx = intersect_x;
			cw_umbra_projy = intersect_y;
		}
		else
		{
			// no intersection, OK to push both points
			umbra_pts.push(u1x);
			umbra_pts.push(u1y);
			umbra_pts.push(u2x);
			umbra_pts.push(u2y);
			
			acw_umbra_projx = u1x;
			acw_umbra_projy = u1y;
			cw_umbra_projx = u2x;
			cw_umbra_projy = u2y;
		}
	};
	
	instanceProto.calculateExtrusion = function (Lx, Ly, x, y, h)
	{
		// Object taller than light: effectively infinite shadow
		if (h >= this.lightZ)
			return this.max_extrude;
		if (h <= 0)
			return 0;
		
		var dist = cr.distanceTo(Lx, Ly, x, y);
		var a = Math.atan(dist / (this.lightZ - h));
		var e = h * Math.tan(a);
		
		if (e >= this.max_extrude)
			e = this.max_extrude;
		if (e < 0)
			e = 0;
		
		return e;
	};
	
	// only called if a layout object in WebGL mode - draw to the WebGL context
	// 'glw' is not a WebGL context, it's a wrapper - you can find its methods in GLWrap.js in the install
	// directory or just copy what other plugins do.
	instanceProto.drawGL = function (glw)
	{
		glw.setOpacity(this.opacity);
		
		this.getShadowCasterCandidates();
		
		var i, len, j, lenj, inst, h, poly, tmx, tmy, c, tilerc, offx, offy;
		for (i = 0, len = casters.length; i < len; ++i)
		{
			inst = casters[i];
			
			if (!inst.extra["shadowcasterEnabled"] || !this.castsFrom(inst.extra["shadowcasterTag"]))
				continue;
			
			inst.update_bbox();
			h = inst.extra["shadowcasterHeight"];
			
			// Render against tilemap
			if (inst.tilemap_exists)
			{
				inst.getCollisionRectCandidates(this.bbox, collrect_candidates);
				
				tmx = inst.x;
				tmy = inst.y;
				
				for (j = 0, lenj = collrect_candidates.length; j < lenj; ++j)
				{
					c = collrect_candidates[j];
					tilerc = c.rc;
					
					poly = null;
				
					// Get either collision poly or bounding box as poly
					if (c.poly)
					{
						poly = c.poly;
						offx = tilerc.left;
						offy = tilerc.top;
					}
					else
					{
						temp_poly.set_from_rect(tilerc, 0, 0);
						poly = temp_poly;
						offx = 0;
						offy = 0;
					}
					
					this.calcShadow(poly, tmx + offx, tmy + offy, h);
					
					if (umbra_pts.length < 6)
						continue;		// could not get at least an umbra triangle
					
					// Fill poly
					glw.setTexture(this.webGL_texture);
					glw.convexPoly(umbra_pts);
					
					// Fill penumbras
					if (cw_edge1 > -1 && cw_edge2 > -1)
					{
						glw.setTexture(this.penumbra_texture);
						this.drawPenumbraGL(glw, true);
					}
					
					if (acw_edge1 > -1 && acw_edge2 > -1)
					{
						glw.setTexture(this.penumbra_texture);
						this.drawPenumbraGL(glw, false);
					}
					
					back_faces1.length = 0;
					umbra_pts.length = 0;
				}
				
				collrect_candidates.length = 0;
			}
			else
			{
				poly = null;
				
				// Get either collision poly or bounding box as poly
				if (inst.collision_poly && !inst.collision_poly.is_empty())
				{
					inst.collision_poly.cache_poly(inst.width, inst.height, inst.angle);
					poly = inst.collision_poly;
				}
				else
				{
					temp_poly.set_from_quad(inst.bquad, inst.x, inst.y, inst.width, inst.height);
					poly = temp_poly;
				}
				
				this.calcShadow(poly, inst.x, inst.y, h);
				
				if (umbra_pts.length < 6)
					continue;		// could not get at least an umbra triangle
				
				// Fill poly
				glw.setTexture(this.webGL_texture);
				glw.convexPoly(umbra_pts);
				
				// Fill penumbras
				if (cw_edge1 > -1 && cw_edge2 > -1)
				{
					glw.setTexture(this.penumbra_texture);
					this.drawPenumbraGL(glw, true);
				}
				
				if (acw_edge1 > -1 && acw_edge2 > -1)
				{
					glw.setTexture(this.penumbra_texture);
					this.drawPenumbraGL(glw, false);
				}
				
				back_faces1.length = 0;
				umbra_pts.length = 0;
			}
		}
		
		casters.length = 0;
	};
	
	instanceProto.drawPenumbraGL = function (glw, clockwise)
	{
		var Lx = this.lightX;
		var Ly = this.lightY;
		var secx, secy;
		
		// Get edge position
		var rootx, rooty, innerx, innery;
		
		if (clockwise)
		{
			rootx = cw_umbra_rootx;
			rooty = cw_umbra_rooty;
			innerx = cw_umbra_projx;
			innery = cw_umbra_projy;
		}
		else
		{
			rootx = acw_umbra_rootx;
			rooty = acw_umbra_rooty;
			innerx = acw_umbra_projx;
			innery = acw_umbra_projy;
		}
		
		// Get L1 position to the clockwise root or L2 position to anticlockwise root
		var r = this.lightRadius;
		var a = cr.angleTo(Lx, Ly, midx, midy);
		var oa = Math.PI / 2;
		if (clockwise)
			oa = -oa;
		Lx += Math.cos(a + oa) * r;
		Ly += Math.sin(a + oa) * r;
		
		var twopart = false;
		
		// Check if both sides of the light radius agree on which edge this is. If not,
		// a two-part umbra will be necessary.
		if (clockwise)
		{
			twopart = (cw_edge1 !== cw_edge2);
			
			if (twopart)
			{
				// get secondary position according to L1
				secx = poly_pts[cw_edge1*2];
				secy = poly_pts[cw_edge1*2+1];
			}
		}
		else
		{
			twopart = (acw_edge1 !== acw_edge2);
			
			if (twopart)
			{
				// get secondary position according to L2
				secx = poly_pts[acw_edge2*2];
				secy = poly_pts[acw_edge2*2+1];
			}
		}
		
		// Extrude primary umbra. If two-part, primary umbra must be clamped from the angle of
		// the edge of the collision shape. Otherwise it can project freely.		
		if (twopart)
		{
			a = cr.angleTo(secx, secy, rootx, rooty);
		}
		else
		{
			a = cr.angleTo(Lx, Ly, rootx, rooty);
		}
		var Ux = rootx + Math.cos(a) * this.max_extrude;
		var Uy = rooty + Math.sin(a) * this.max_extrude;
		var U2x, U2y, a3, a2, a1;
		var umbra_factor = 0;
		var rootu = 0;
		var rootv = 1;
		
		if (twopart)
		{
			// Project out from secondary position
			a3 = cr.angleTo(Lx, Ly, secx, secy);
			var U2x = secx + Math.cos(a3) * this.max_extrude;
			var U2y = secy + Math.sin(a3) * this.max_extrude;
			
			// a3: angle of outer edge
			a2 = a;		// angle of middle edge
			// angle of inner edge
			a1 = cr.angleTo(rootx, rooty, innerx, innery);
			
			// determine umbra factor: proportion of penumbra that is the outer section
			umbra_factor = (cr.angleDiff(a2, a3) / cr.angleDiff(a1, a3));
			
			// determine root factor: how far along the outer umbra's edge the inner umbra is rooted
			var side_dist = cr.distanceTo(secx, secy, rootx, rooty);
			var inner_root_factor = (side_dist / (side_dist + this.max_extrude));
			
			// calculate the rootu and rootv based on this
			var sega = cr.angleTo(0, 1, umbra_factor, 0);
			var segd = cr.distanceTo(0, 1, umbra_factor, 0);
			rootu += Math.cos(sega) * inner_root_factor * segd;
			rootv += Math.sin(sega) * inner_root_factor * segd;
		}
		
		// Fill the primary umbra triangle (root, inner, umbra)
		glw.quadTexUV(rootx, rooty, innerx, innery, Ux, Uy, Ux, Uy, rootu, rootv, 1, 0, umbra_factor, 0, umbra_factor, 0);
		
		// Fill secondary umbra if two-part
		if (twopart)
		{			
			// Fill secondary umbra triangle (sec, umbra, umbra2)
			glw.quadTexUV(secx, secy, Ux, Uy, U2x, U2y, U2x, U2y, 0, 1, umbra_factor, 0, 0, 0, 0, 0);
		}
	};
	
	function maybeGetPenumbraData()
	{
		if (penumbra_data)
			return;		// already got data
		
		var w = penumbra_img.width;
		var h = penumbra_img.height;
		
		var canvas = document.createElement("canvas");
		canvas.width = w;
		canvas.height = h;
		var ctx = canvas.getContext("2d");
		ctx.clearRect(0, 0, w, h);
		ctx.drawImage(penumbra_img, 0, 0, w, h);
		penumbra_data = ctx.getImageData(0, 0, w, h).data;
	};
	
	instanceProto.createTextures = function ()
	{
		if (!this.runtime.glwrap)
			return;		// not WebGL mode
		
		if (this.webGL_texture)
		{
			this.runtime.glwrap.deleteTexture(this.webGL_texture);
			this.webGL_texture = null;
		}
		
		if (this.penumbra_texture)
		{
			this.runtime.glwrap.deleteTexture(this.penumbra_texture);
			this.penumbra_texture = null;
		}
		
		this.createFillTexture();
		this.createPenumbraTexture();
	};
	
	instanceProto.createFillTexture = function ()
	{
		// create 16x16 texture filled with shadow color
		var canvas = document.createElement("canvas");
		canvas.width = 16;
		canvas.height = 16;
		var ctx = canvas.getContext("2d");
		ctx.fillStyle = "rgb(" + this.lightR + "," + this.lightG + "," + this.lightB + ")";
		ctx.fillRect(0, 0, 16, 16);
		
		this.webGL_texture = this.runtime.glwrap.createEmptyTexture(16, 16, false, false, true);
		this.runtime.glwrap.videoToTexture(canvas, this.webGL_texture);
	};
	
	instanceProto.createPenumbraTexture = function ()
	{
		// create 256x256 penumbra texture with given color
		var w = penumbra_img.width;
		var h = penumbra_img.height;
		var r = this.lightR, g = this.lightG, b = this.lightB;
		var src_data = penumbra_data;
		
		var canvas = document.createElement("canvas");
		canvas.width = w;
		canvas.height = h;
		var ctx = canvas.getContext("2d");
		var imgdata = ctx.createImageData(w, h);
		var arr = imgdata.data;
		
		// For every 4 bytes in arr
		var i, len;
		for (i = 0, len = arr.length; i < len; i += 4)
		{
			arr[i] = r;
			arr[i+1] = g;
			arr[i+2] = b;
			arr[i+3] = src_data[i+3];		// copy alpha from original
		}
		
		// Copy back to canvas then create texture
		ctx.putImageData(imgdata, 0, 0);
		this.penumbra_texture = this.runtime.glwrap.createEmptyTexture(w, h, this.runtime.linearSampling, false, false);
		this.runtime.glwrap.videoToTexture(canvas, this.penumbra_texture);
		
		//this.penumbra_texture = this.runtime.glwrap.loadTexture(penumbra_img, false, this.runtime.linearSampling, 0, null, true);
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
		/*
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
		*/
	};
	
	instanceProto.onDebugValueEdited = function (header, name, value)
	{
		// Called when a non-readonly property has been edited in the debugger. Usually you only
		// will need 'name' (the property name) and 'value', but you can also use 'header' (the
		// header title for the section) to distinguish properties with the same name.
		//if (name === "My property")
		//	this.myProperty = value;
	};
	/**END-PREVIEWONLY**/

	//////////////////////////////////////
	// Conditions
	function Cnds() {};

	/*
	Cnds.prototype.MyCondition = function (myparam)
	{
		return myparam >= 0;
	};
	*/
	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};
	
	Acts.prototype.SetLightHeight = function (z)
	{
		if (this.lightZ !== z)
		{
			this.lightZ = z;
			this.runtime.redraw = true;
		}
	};
	
	Acts.prototype.SetLightColor = function (rgb)
	{
		var r = cr.GetRValue(rgb);
		var g = cr.GetGValue(rgb);
		var b = cr.GetBValue(rgb);
		
		if (this.lightR !== r || this.lightG !== g || this.lightB !== b)
		{
			this.lightR = r;
			this.lightG = g;
			this.lightB = b;
			this.createTextures();
			this.runtime.redraw = true;
		}
	};
	
	Acts.prototype.SetTag = function (tag_)
	{
		if (this.tag !== tag_)
		{
			this.tag = tag_;
			this.runtime.redraw = true;
		}
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.LightX = function (ret)
	{
		ret.set_float(this.lightX);
	};
	
	Exps.prototype.LightY = function (ret)
	{
		ret.set_float(this.lightY);
	};
	
	Exps.prototype.Tag = function (ret)
	{
		ret.set_string(this.tag);
	};
	
	pluginProto.exps = new Exps();

}());