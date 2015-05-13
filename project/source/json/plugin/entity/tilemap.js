// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.Tilemap = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	/////////////////////////////////////
	var pluginProto = cr.plugins_.Tilemap.prototype;
		
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
		var i, len, p;
		
		if (this.is_family)
			return;
		
		// Create the texture
		this.texture_img = new Image();
		this.texture_img.cr_filesize = this.texture_filesize;
		
		// Tell runtime to wait for this to load
		this.runtime.waitForImageLoad(this.texture_img, this.texture_file);
		
		// Array of tile id to either a pattern (canvas2d) or texture (webgl)
		this.cut_tiles = [];
		this.cut_tiles_valid = false;
		
		// Create tile collision polys from the poly data
		this.tile_polys = [];
		this.tile_polys_cached = false;		// first instance will cache polys with the tile width/height
		
		if (this.tile_poly_data && this.tile_poly_data.length)
		{
			for (i = 0, len = this.tile_poly_data.length; i < len; ++i)
			{
				p = this.tile_poly_data[i];
				
				if (p)
				{
					// For rotated and flipped tiles, store a flipmap like the canvas2d tiles
					this.tile_polys.push({
						poly: p,
						flipmap: [[[null, null], [null, null]], [[null, null], [null, null]]]
					});
				}
				else
					this.tile_polys.push(null);
			}
		}
	};
	
	typeProto.cacheTilePoly = function (tileid, tilewidth, tileheight, fliph, flipv, flipd)
	{
		if (tileid < 0 || tileid >= this.tile_polys.length)
			return;
		
		if (!this.tile_polys[tileid])
			return;		// no poly for this tile
		
		var poly = this.tile_polys[tileid].poly;
		var flipmap = this.tile_polys[tileid].flipmap;
		
		var cached_poly = new cr.CollisionPoly(poly);
		cached_poly.cache_poly(tilewidth, tileheight, 0);
		
		if (flipd)
			cached_poly.diag();
		
		if (fliph)
			cached_poly.mirror(tilewidth / 2);
		
		if (flipv)
			cached_poly.flip(tileheight / 2);
		
		flipmap[fliph?1:0][flipv?1:0][flipd?1:0] = cached_poly;
	};
	
	typeProto.getTilePoly = function (id)
	{
		if (id === -1)
			return null;
		
		var tileid = (id & TILE_ID_MASK);
		
		if (tileid < 0 || tileid >= this.tile_polys.length)
			return null;		// out of range
		
		if (!this.tile_polys[tileid])
			return null;		// no poly for this tile
		
		var fliph = (id & TILE_FLIPPED_HORIZONTAL) ? 1 : 0;
		var flipv = (id & TILE_FLIPPED_VERTICAL) ? 1 : 0;
		var flipd = (id & TILE_FLIPPED_DIAGONAL) ? 1 : 0;
		return this.tile_polys[tileid].flipmap[fliph][flipv][flipd];
	};
	
	typeProto.freeCutTiles = function ()
	{
		var i, len;
		
		// In WebGL mode, need to delete textures
		var glwrap = this.runtime.glwrap;
		
		if (glwrap)
		{
			for (i = 0, len = this.cut_tiles.length; i < len; ++i)
				glwrap.deleteTexture(this.cut_tiles[i]);
		}
		
		this.cut_tiles.length = 0;
		this.cut_tiles_valid = false;
	}
	
	typeProto.maybeCutTiles = function (tw, th, offx, offy, sepx, sepy, seamless)
	{
		if (this.cut_tiles_valid)
			return;		// no changed
			
		if (tw <= 0 || th <= 0)
			return;
		
		this.freeCutTiles();
		
		var img_width = this.texture_img.width;
		var img_height = this.texture_img.height;
		
		var x, y;
		
		for (y = offy; y + th <= img_height; y += (th + sepy))
		{
			for (x = offx; x + tw <= img_width; x += (tw + sepx))
			{
				this.cut_tiles.push(this.CutTileImage(x, y, tw, th, seamless));
			}
		}
		
		this.cut_tiles_valid = true;
	};
	
	typeProto.CutTileImage = function(x, y, w, h, seamless)
	{
		// WebGL mode: just cut the texture without any flipping; at draw time we can rotate
		// and flip it accordingly using just texture co-ordinates
		if (this.runtime.glwrap)
		{
			return this.DoCutTileImage(x, y, w, h, false, false, false, seamless);
		}
		// Canvas2D mode: the 2D context doesn't give us convenient tools to do texture
		// co-ordinate flipping, especially given that we render quads of tiles at a time.
		// So we pre-process tiles with flipped images which can simply be drawn as if they
		// were a different type of tile. This can use up to 8x more memory, so we lazy create
		// all the flipped variants. This could jank, but it's probably better than wasting
		// memory in case large tilemaps are used.
		else
		{
			// Bit of an array nightmare but allows us to index a flipped tile via:
			// ret[horiz_flip ? 1 : 0][vert_flip ? 1 : 0][diag_flip ? 1 : 0]
			var flipmap = [[[null, null], [null, null]], [[null, null], [null, null]]];
			flipmap[0][0][0] = this.DoCutTileImage(x, y, w, h, false, false, false, seamless);
			
			return {
				flipmap: flipmap,
				x: x,
				y: y,
				w: w,
				h: h
			};
		}
	};
	
	typeProto.GetFlippedTileImage = function (tileid, fliph, flipv, flipd, seamless)
	{
		// Canvas2D only. Access the given pre-flipped tile, lazy creating it if missing.
		if (tileid < 0 || tileid >= this.cut_tiles.length)
			return null;
			
		var tile = this.cut_tiles[tileid];
		var flipmap = tile.flipmap;
		
		var hi = (fliph ? 1 : 0);
		var vi = (flipv ? 1 : 0);
		var di = (flipd ? 1 : 0);
		
		var ret = flipmap[hi][vi][di];
		
		if (ret)
		{
			return ret;
		}
		else
		{
			// Lazy create and return
			ret = this.DoCutTileImage(tile.x, tile.y, tile.w, tile.h, hi!==0, vi!==0, di!==0, seamless);
			flipmap[hi][vi][di] = ret;
			return ret;
		}
	};
	
	typeProto.DoCutTileImage = function(x, y, w, h, fliph, flipv, flipd, seamless)
	{
		var dw = w;
		var dh = h;
		
		// For WebGL: must size up to next power of two for tiling, unless "seamless" mode (which does not tile anything)
		if (this.runtime.glwrap && !seamless)
		{
			if (!cr.isPOT(dw))
				dw = cr.nextHighestPowerOfTwo(dw);
			
			if (!cr.isPOT(dh))
				dh = cr.nextHighestPowerOfTwo(dh);
		}
		
		var tmpcanvas = document.createElement("canvas");
		tmpcanvas.width = dw;
		tmpcanvas.height = dh;
		var tmpctx = tmpcanvas.getContext("2d");
			
		// Canvas2D mode: create a pattern from the source image
		if (this.runtime.ctx)
		{
			if (fliph)
			{
				if (flipv)
				{
					if (flipd)
					{
						// Flip diagonal, horizontal and vertical (aka rotate 90 clockwise, flip vertical)
						tmpctx.rotate(Math.PI / 2);
						tmpctx.scale(-1, 1);
						tmpctx.translate(-dw, -dh);
					}
					else
					{
						// Flip horizontal and vertical
						tmpctx.scale(-1, -1);
						tmpctx.translate(-dw, -dh);
					}
				}
				else
				{
					if (flipd)
					{
						// Flip horizontal and diagonal (aka rotate 90 clockwise)
						tmpctx.rotate(Math.PI / 2);
						tmpctx.translate(0, -dh);
					}
					else
					{
						// Flip horizontal only
						tmpctx.scale(-1, 1);
						tmpctx.translate(-dw, 0);
					}
				}
			}
			else
			{
				if (flipv)
				{
					if (flipd)
					{
						// Flip diagonal and vertical (aka rotate 90 anticlockwise)
						tmpctx.rotate(-Math.PI / 2);
						tmpctx.translate(-dw, 0);
					}
					else
					{
						// Flip vertical only
						tmpctx.scale(1, -1);
						tmpctx.translate(0, -dh);
					}
				}
				else
				{
					if (flipd)
					{
						// Only flip diagonal (aka rotate 90 anticlockwise, flip vertical)
						tmpctx.scale(-1, 1);
						tmpctx.rotate(Math.PI / 2);
					}
					//else no flipping, draw normally
				}
			}
			
			tmpctx.drawImage(this.texture_img, x, y, w, h, 0, 0, dw, dh);
			
			// Seamless mode directly returns the canvas to be drawn; non-seamless mode returns a repeating pattern.
			if (seamless)
				return tmpcanvas;
			else
				return this.runtime.ctx.createPattern(tmpcanvas, "repeat");
		}
		// WebGL mode: create a texture from the source image
		else
		{
			assert2(this.runtime.glwrap, "If ctx not supplied, must supply glwrap");
			
			tmpctx.drawImage(this.texture_img, x, y, w, h, 0, 0, dw, dh);
			
			// Don't set tiling in seamless mode, each tile is drawn individually.
			var tex = this.runtime.glwrap.createEmptyTexture(dw, dh, this.runtime.linearSampling, false, !seamless);
			this.runtime.glwrap.videoToTexture(tmpcanvas, tex);
			return tex;
		}
	};
	
	typeProto.onLostWebGLContext = function ()
	{
		if (this.is_family)
			return;
			
		this.freeCutTiles();
	};
	
	typeProto.onRestoreWebGLContext = function ()
	{
		// Do nothing, next draw will create on demand
	};
	
	typeProto.loadTextures = function ()
	{
		// Do nothing, next draw will create on demand
	};
	
	typeProto.unloadTextures = function ()
	{
		// Don't release textures if any instances still exist, they are probably using them
		if (this.is_family || this.instances.length)
			return;
		
		this.freeCutTiles();
	};
	
	typeProto.preloadCanvas2D = function (ctx)
	{
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
	
	// Flip flags matching Tiled's format
	var TILE_FLIPPED_HORIZONTAL = -0x80000000		// note: pretend is a signed int, so negate
	var TILE_FLIPPED_VERTICAL = 0x40000000
	var TILE_FLIPPED_DIAGONAL = 0x20000000
	var TILE_FLAGS_MASK = 0xE0000000
	var TILE_ID_MASK = 0x1FFFFFFF
	
	function TileQuad()
	{
		this.id = -1;
		this.tileid = -1;
		this.horiz_flip = false;
		this.vert_flip = false;
		this.diag_flip = false;
		this.any_flip = false;
		this.rc = new cr.rect(0, 0, 0, 0);
	};
	
	var tilequad_cache = [];
	
	function allocTileQuad()
	{
		if (tilequad_cache.length)
			return tilequad_cache.pop();
		else
			return new TileQuad();
	};
	
	function freeTileQuad(tq)
	{
		if (tilequad_cache.length < 10000)
			tilequad_cache.push(tq);
	};
	
	function TileCollisionRect()
	{
		this.id = -1;
		this.rc = new cr.rect(0, 0, 0, 0);
		this.poly = null;
	}
	
	var collrect_cache = [];
	
	function allocCollRect()
	{
		if (collrect_cache.length)
			return collrect_cache.pop();
		else
			return new TileCollisionRect();
	};
	
	function freeCollRect(r)
	{
		if (collrect_cache.length < 10000)
			collrect_cache.push(r);
	};
	
	var tile_cell_cache = [];
	
	function allocTileCell(inst_, x_, y_)
	{
		var ret;
		
		if (tile_cell_cache.length)
		{
			ret = tile_cell_cache.pop();
			ret.inst = inst_;
			ret.x = x_;
			ret.y = y_;
			ret.left = ret.x * ret.inst.cellwidth * ret.inst.tilewidth;
			ret.top = ret.y * ret.inst.cellheight * ret.inst.tileheight;
			ret.clear();
			ret.quadmap_valid = false;
			return ret;
		}
		else
			return new TileCell(inst_, x_, y_);
	};
	
	function freeTileCell(tc)
	{
		var i, len;
		
		// Recycle quads and collision rects
		for (i = 0, len = tc.quads.length; i < len; ++i)
			freeTileQuad(tc.quads[i]);
			
		tc.quads.length = 0;
		
		for (i = 0, len = tc.collision_rects.length; i < len; ++i)
			freeCollRect(tc.collision_rects[i]);
		
		tc.collision_rects.length = 0;
		
		if (tile_cell_cache.length < 1000)
			tile_cell_cache.push(tc);
	};
	
	function TileCell(inst_, x_, y_)
	{
		this.inst = inst_;
		this.x = x_;
		this.y = y_;
		this.left = this.x * this.inst.cellwidth * this.inst.tilewidth;
		this.top = this.y * this.inst.cellheight * this.inst.tileheight;
		
		this.tiles = [];
		this.quads = [];
		this.collision_rects = [];
		this.quadmap_valid = false;
		
		// Reserve enough tiles to fill the cell
		var i, len, j, lenj, arr;
		for (i = 0, len = this.inst.cellheight; i < len; ++i)
		{
			arr = [];
			
			for (j = 0, lenj = this.inst.cellwidth; j < lenj; ++j)
				arr.push(-1);
			
			this.tiles.push(arr);
		}
	};
	
	TileCell.prototype.clear = function ()
	{
		var i, len, j, lenj, arr;
		
		this.tiles.length = this.inst.cellheight;
		
		for (i = 0, len = this.tiles.length; i < len; ++i)
		{
			arr = this.tiles[i];
			
			if (!arr)
			{
				arr = [];
				this.tiles[i] = arr;
			}
			
			arr.length = this.inst.cellwidth;
			
			for (j = 0, lenj = arr.length; j < lenj; ++j)
				arr[j] = -1;
		}
	};
	
	TileCell.prototype.maybeBuildQuadMap = function ()
	{
		if (this.quadmap_valid)
			return;		// not changed
		
		var tilewidth = this.inst.tilewidth;
		var tileheight = this.inst.tileheight;
		
		if (tilewidth <= 0 || tileheight <= 0)
			return;
		
		var i, j, len, y, leny, x, lenx, arr, t, p, q;
		
		// Recycle all existing quads and collision rects
		for (i = 0, len = this.quads.length; i < len; ++i)
			freeTileQuad(this.quads[i]);
		
		for (i = 0, len = this.collision_rects.length; i < len; ++i)
			freeCollRect(this.collision_rects[i]);
		
		this.quads.length = 0;
		this.collision_rects.length = 0;
		
		// Only calculate to the edge of the visible area of the object
		var extentwidth = Math.floor(this.inst.width / tilewidth);
		var extentheight = Math.floor(this.inst.height / tileheight);
		
		extentwidth -= this.left / tilewidth;
		extentheight -= this.top / tileheight;
		
		if (extentwidth > this.inst.cellwidth)
			extentwidth = this.inst.cellwidth;
		if (extentheight > this.inst.cellheight)
			extentheight = this.inst.cellheight;
		
		var seamless = this.inst.seamless;
		
		// Normalize tiles horizontally
		var cur_quad = null;
		for (y = 0, leny = extentheight; y < leny; ++y)
		{
			arr = this.tiles[y];
			
			for (x = 0, lenx = extentwidth; x < lenx; ++x)
			{
				t = arr[x];
				
				// Skip empty tiles
				if (t === -1)
				{
					if (cur_quad)
					{
						this.quads.push(cur_quad);
						cur_quad = null;
					}
						
					continue;
				}
				
				// First tile, or reached a tile of different id.
				// In seamless mode break up every tile individually.
				if (seamless || !cur_quad || t !== cur_quad.id)
				{
					if (cur_quad)
						this.quads.push(cur_quad);
					
					cur_quad = allocTileQuad();
					cur_quad.id = t;
					cur_quad.tileid = (t & TILE_ID_MASK);
					cur_quad.horiz_flip = (t & TILE_FLIPPED_HORIZONTAL) !== 0;
					cur_quad.vert_flip = (t & TILE_FLIPPED_VERTICAL) !== 0;
					cur_quad.diag_flip = (t & TILE_FLIPPED_DIAGONAL) !== 0;
					cur_quad.any_flip = (cur_quad.horiz_flip || cur_quad.vert_flip || cur_quad.diag_flip);
					cur_quad.rc.left = x * tilewidth + this.left;
					cur_quad.rc.top = y * tileheight + this.top;
					cur_quad.rc.right = cur_quad.rc.left + tilewidth;
					cur_quad.rc.bottom = cur_quad.rc.top + tileheight;
				}
				// Otherwise another tile of the same type. Extend existing quad
				else
				{
					cur_quad.rc.right += tilewidth;
				}
			}
			
			// End of row: flush current quad
			if (cur_quad)
			{
				this.quads.push(cur_quad);
				cur_quad = null;
			}
		}
		
		// Now do more or less the same thing but for collision rects, where any tile >= 0 makes
		// a collision rect. Use tile polys when set, and note every tile with a poly must be split
		// off in to its own rect
		var cur_rect = null;
		var tileid, tilepoly;
		var cur_has_poly = false;
		var rc;
		for (y = 0, leny = extentheight; y < leny; ++y)
		{
			arr = this.tiles[y];
			
			for (x = 0, lenx = extentwidth; x < lenx; ++x)
			{
				t = arr[x];
				
				// Skip empty tiles
				if (t === -1)
				{
					if (cur_rect)
					{
						this.collision_rects.push(cur_rect);
						cur_rect = null;
						cur_has_poly = false;
					}
						
					continue;
				}
				
				tileid = (t & TILE_ID_MASK);
				tilepoly = this.inst.type.getTilePoly(t);
				
				// Got a new tile or new tile uses a collision poly: start making a new collision rect
				if (!cur_rect || tilepoly || cur_has_poly)
				{
					if (cur_rect)
					{
						this.collision_rects.push(cur_rect);
						cur_rect = null;
					}
					
					assert2(t !== -1, "Creating collision rect for empty tile");
					cur_rect = allocCollRect();
					cur_rect.id = t;
					cur_rect.poly = tilepoly ? tilepoly : null;
					rc = cur_rect.rc;
					
					rc.left = x * tilewidth + this.left;
					rc.top = y * tileheight + this.top;
					rc.right = rc.left + tilewidth;
					rc.bottom = rc.top + tileheight;
					
					cur_has_poly = !!tilepoly;
				}
				// Otherwise another tile. Extend existing collision rect
				else
				{
					cur_rect.rc.right += tilewidth;
				}
			}
			
			// End of row: flush current rect
			if (cur_rect)
			{
				this.collision_rects.push(cur_rect);
				cur_rect = null;
				cur_has_poly = false;
			}
		}
		
		// Now normalize quads vertically. Loop each quad and look for another quad on the row below it
		// which is the same width and id. If there is one, extend the current quad down instead.
		// In seamless mode skip this entirely.
		if (!seamless)
		{
			len = this.quads.length;
			
			for (i = 0; i < len; ++i)
			{
				q = this.quads[i];
				
				for (j = i + 1; j < len; ++j)
				{
					p = this.quads[j];
					
					// Not yet on to next row
					if (p.rc.top < q.rc.bottom)
						continue;
					// Gone too far (on next row down)
					if (p.rc.top > q.rc.bottom)
						break;
					// Is on the row below: check if gone too far to the right
					if (p.rc.right > q.rc.right || p.rc.left > q.rc.left)
						break;
					
					// Sits exactly beneath, is same width and same ID: extend q down and delete p
					if (p.id === q.id && p.rc.left === q.rc.left && p.rc.right === q.rc.right)
					{
						freeTileQuad(this.quads[j]);
						this.quads.splice(j, 1);
						--len;
						q.rc.bottom += tileheight;
						--j;		// look at same j index again
					}
				}
			}
		}
		
		// Again, do the same thing for collision rects.
		len = this.collision_rects.length;
		var prc, qrc;
		
		for (i = 0; i < len; ++i)
		{
			q = this.collision_rects[i];
			
			// Tiles with collision polys cannot be extended
			if (q.poly)
				continue;
			
			qrc = q.rc;
			
			for (j = i + 1; j < len; ++j)
			{
				p = this.collision_rects[j];
				prc = p.rc;
				
				// Not yet on to next row
				if (prc.top < qrc.bottom)
					continue;
				// Gone too far (on next row down)
				if (prc.top > qrc.bottom)
					break;
				// Is on the row below: check if gone too far to the right
				if (prc.right > qrc.right || prc.left > qrc.left)
					break;
				// Tiles with collision polys cannot be extended
				if (p.poly)
					continue;
				
				// Sits exactly beneath: extend q down and delete p
				if (prc.left === qrc.left && prc.right === qrc.right)
				{
					freeCollRect(this.collision_rects[j]);
					this.collision_rects.splice(j, 1);
					--len;
					qrc.bottom += tileheight;
					--j;		// look at same j index again
				}
			}
		}
		
		this.quadmap_valid = true;
	};
	
	TileCell.prototype.setTileAt = function (x_, y_, t_)
	{
		if (this.tiles[y_][x_] !== t_)
		{
			this.tiles[y_][x_] = t_;
			this.quadmap_valid = false;
			this.inst.any_quadmap_changed = true;
			this.inst.physics_changed = true;
			this.inst.runtime.redraw = true;
		}
	};
	
	// called whenever an instance is created
	instanceProto.onCreate = function()
	{
		assert2(this.tilemap_exists, "No tilemap exists for tilemap object");
		var i, len, p;
		
		this.visible = (this.properties[0] === 0);
		this.tilewidth = this.properties[1];
		this.tileheight = this.properties[2];
		this.tilexoffset = this.properties[3];
		this.tileyoffset = this.properties[4];
		this.tilexspacing = this.properties[5];
		this.tileyspacing = this.properties[6];
		this.seamless = (this.properties[7] !== 0);
		
		this.mapwidth = this.tilemap_width;
		this.mapheight = this.tilemap_height;
		
		this.lastwidth = this.width;
		this.lastheight = this.height;
		
		var tw = this.tilewidth;
		var th = this.tileheight;
		
		if (tw === 0)
			tw = 1;
		if (th === 0)
			th = 1;
		
		this.cellwidth = Math.ceil(this.runtime.original_width / tw);
		this.cellheight = Math.ceil(this.runtime.original_height / th);
		
		// Cache tile collision polys if first instance
		if (!this.type.tile_polys_cached)
		{
			this.type.tile_polys_cached = true;
			
			for (i = 0, len = this.type.tile_polys.length; i < len; ++i)
			{
				p = this.type.tile_polys[i];
				
				if (!p)
					continue;
				
				this.type.cacheTilePoly(i, tw, th, false, false, false);
				this.type.cacheTilePoly(i, tw, th, false, false, true);
				this.type.cacheTilePoly(i, tw, th, false, true, false);
				this.type.cacheTilePoly(i, tw, th, false, true, true);
				this.type.cacheTilePoly(i, tw, th, true, false, false);
				this.type.cacheTilePoly(i, tw, th, true, false, true);
				this.type.cacheTilePoly(i, tw, th, true, true, false);
				this.type.cacheTilePoly(i, tw, th, true, true, true);
			}
		}
		
		if (!this.recycled)
			this.tilecells = [];
		
		this.maybeResizeTilemap(true);
		this.setTilesFromRLECSV(this.tilemap_data);
		
		this.type.maybeCutTiles(this.tilewidth, this.tileheight, this.tilexoffset, this.tileyoffset, this.tilexspacing, this.tileyspacing, this.seamless);
		
		this.physics_changed = false;		// to indicate to physics behavior to recreate body
		this.any_quadmap_changed = true;
		this.maybeBuildAllQuadMap();
	};
	
	instanceProto.maybeBuildAllQuadMap = function ()
	{
		if (!this.any_quadmap_changed)
			return;		// no change
			
		var i, len, j, lenj, arr;
		
		for (i = 0, len = this.tilecells.length; i < len; ++i)
		{
			arr = this.tilecells[i];
			
			for (j = 0, lenj = arr.length; j < lenj; ++j)
			{
				arr[j].maybeBuildQuadMap();
			}
		}
		
		this.any_quadmap_changed = false;
	};
	
	instanceProto.setAllQuadMapChanged = function ()
	{
		var i, len, j, lenj, arr;
		
		for (i = 0, len = this.tilecells.length; i < len; ++i)
		{
			arr = this.tilecells[i];
			
			for (j = 0, lenj = arr.length; j < lenj; ++j)
			{
				arr[j].quadmap_valid = false;
			}
		}
		
		this.any_quadmap_changed = true;
	};
	
	function RunLengthDecode(str)
	{
		var ret = [];
		
		var parts = str.split(",");
		var i, len, p, x, n, t, part;
		
		for (i = 0, len = parts.length; i < len; ++i)
		{
			p = parts[i];
			
			// Contains an 'x': treat as n x t format for repeating a number
			x = p.indexOf("x");
			
			if (x > -1)
			{
				n = parseInt(p.substring(0, x), 10);
				part = p.substring(x + 1);
				t = parseInt(part, 10);
				
				// Apply flip flags if found in the number string
				if (part.indexOf("h") > -1)
					t = t | TILE_FLIPPED_HORIZONTAL;
				if (part.indexOf("v") > -1)
					t = t | TILE_FLIPPED_VERTICAL;
				if (part.indexOf("d") > -1)
					t = t | TILE_FLIPPED_DIAGONAL;
				
				for ( ; n > 0; --n)
					ret.push(t);
			}
			else
			{
				// No 'x': treat as single number to add
				t = parseInt(p, 10);
				
				if (p.indexOf("h") > -1)
					t = t | TILE_FLIPPED_HORIZONTAL;
				if (p.indexOf("v") > -1)
					t = t | TILE_FLIPPED_VERTICAL;
				if (p.indexOf("d") > -1)
					t = t | TILE_FLIPPED_DIAGONAL;
				
				ret.push(t);
			}
		}
		
		return ret;
	};
	
	instanceProto.maybeResizeTilemap = function (force)
	{
		var curwidth = cr.floor(this.width / this.tilewidth);
		var curheight = cr.floor(this.height / this.tileheight);
		
		// Same size or smaller: no need to do anything
		if (curwidth <= this.mapwidth && curheight <= this.mapheight && !force)
			return;
		
		var vcells, hcells;
		
		if (force)
		{
			vcells = Math.ceil(this.mapheight / this.cellheight);
			hcells = Math.ceil(this.mapwidth / this.cellwidth);
		}
		else
		{
			vcells = this.tilecells.length;
			hcells = Math.ceil(this.mapwidth / this.cellwidth);
			
			// Otherwise map is now bigger: extend the tile cells if necessary
			if (curheight > this.mapheight)
			{
				this.mapheight = curheight;
				vcells = Math.ceil(this.mapheight / this.cellheight);
			}
			
			if (curwidth > this.mapwidth)
			{
				this.mapwidth = curwidth;
				hcells = Math.ceil(this.mapwidth / this.cellwidth);
			}
			
			this.setAllQuadMapChanged();
			this.physics_changed = true;
			this.runtime.redraw = true;
		}
		
		// Update tiles cells array to be the new size
		var y, x, arr;
		for (y = 0; y < vcells; ++y)
		{
			arr = this.tilecells[y];
			
			if (!arr)
			{
				// new row: insert a row of empty cells
				arr = [];
				for (x = 0; x < hcells; ++x)
					arr.push(allocTileCell(this, x, y));
				
				this.tilecells[y] = arr;
			}
			else
			{
				// row has gotten wider: insert empty tiles
				for (x = arr.length; x < hcells; ++x)
					arr.push(allocTileCell(this, x, y));
			}
		}
	};
	
	instanceProto.cellAt = function (tx, ty)
	{
		if (tx < 0 || ty < 0)
			return null;
		
		var cy = cr.floor(ty / this.cellheight);
		
		if (cy >= this.tilecells.length)
			return null;
		
		var row = this.tilecells[cy];
		
		var cx = cr.floor(tx / this.cellwidth);
		
		if (cx >= row.length)
			return null;
		
		return row[cx];
	};
	
	instanceProto.cellAtIndex = function (cx, cy)
	{
		if (cx < 0 || cy < 0 || cy >= this.tilecells.length)
			return null;
		
		var row = this.tilecells[cy];
		
		if (cx >= row.length)
			return null;
		
		return row[cx];
	};
	
	// RLECSV = run-length encoded comma separated values
	instanceProto.setTilesFromRLECSV = function (str)
	{
		// Run-length decode the tilemap in to a long number sequence
		var tilestream = RunLengthDecode(str);
		
		// Now fill tilemap arrays from the stream of tile data.
		// Note: this uses y by x format, editor stores in x by y
		var next = 0;
		var y, x, arr, tile, cell;
		
		for (y = 0; y < this.mapheight; ++y)
		{
			for (x = 0; x < this.mapwidth; ++x)
			{
				tile = tilestream[next++];
				
				cell = this.cellAt(x, y);
				
				if (cell)
					cell.setTileAt(x % this.cellwidth, y % this.cellheight, tile);
			}
		}
	};
	
	instanceProto.getTilesAsRLECSV = function ()
	{
		var ret = "";
		
		if (this.mapwidth <= 0 || this.mapheight <= 0)
			return ret;
			
		var run_count = 1;
		var run_number = this.getTileAt(0, 0);
		
		var y, leny = this.mapheight, x, lenx = this.mapwidth, t;
		var tileid, horiz_flip, vert_flip, diag_flip;
		
		for (y = 0; y < leny; ++y)
		{
			// First tile is pre-loaded in to run count, so start x from 1 on first iteration
			for (x = (y === 0 ? 1 : 0) ; x < lenx; ++x)
			{
				t = this.getTileAt(x, y);
				
				// Another of the same number: increase run count
				if (t === run_number)
					++run_count;
				else
				{
					if (run_number === -1)
					{
						tileid = -1;
						horiz_flip = false;
						vert_flip = false;
						diag_flip = false;
					}
					else
					{
						tileid = (run_number & TILE_ID_MASK);
						horiz_flip = (run_number & TILE_FLIPPED_HORIZONTAL) !== 0;
						vert_flip = (run_number & TILE_FLIPPED_VERTICAL) !== 0;
						diag_flip = (run_number & TILE_FLIPPED_DIAGONAL) !== 0;
					}
				
					// A different number: add current run then start new run
					if (run_count === 1)
						ret += "" + tileid;
					else
						ret += "" + run_count + "x" + tileid;
					
					if (horiz_flip)
						ret += "h";
					if (vert_flip)
						ret += "v";
					if (diag_flip)
						ret += "d";
					
					ret += ",";
					run_count = 1;
					run_number = t;
				}
			}
		}
		
		// Add last run
		if (run_number === -1)
		{
			tileid = -1;
			horiz_flip = false;
			vert_flip = false;
			diag_flip = false;
		}
		else
		{
			tileid = (run_number & TILE_ID_MASK);
			horiz_flip = (run_number & TILE_FLIPPED_HORIZONTAL) !== 0;
			vert_flip = (run_number & TILE_FLIPPED_VERTICAL) !== 0;
			diag_flip = (run_number & TILE_FLIPPED_DIAGONAL) !== 0;
		}
		
		// Add last run
		if (run_count === 1)
			ret += "" + tileid;
		else
			ret += "" + run_count + "x" + tileid;
		
		if (horiz_flip)
			ret += "h";
		if (vert_flip)
			ret += "v";
		if (diag_flip)
			ret += "d";
	
		return ret;
	};
	
	instanceProto.getTileAt = function (x_, y_)
	{
		x_ = Math.floor(x_);
		y_ = Math.floor(y_);
		
		if (x_ < 0 || y_ < 0 || x_ >= this.mapwidth || y_ >= this.mapheight)
			return -1;
		
		var cell = this.cellAt(x_, y_);
		
		if (!cell)
			return -1;
		
		return cell.tiles[y_ % this.cellheight][x_ % this.cellwidth];
	};
	
	instanceProto.setTileAt = function (x_, y_, t_)
	{
		x_ = Math.floor(x_);
		y_ = Math.floor(y_);
		
		if (x_ < 0 || y_ < 0 || x_ >= this.mapwidth || y_ >= this.mapheight)
			return -1;
			
		var cell = this.cellAt(x_, y_);
		
		if (!cell)
			return -1;
		
		cell.setTileAt(x_ % this.cellwidth, y_ % this.cellheight, t_);
	};
	
	instanceProto.worldToCellX = function (x)
	{
		return Math.floor((x - this.x) / (this.cellwidth * this.tilewidth));
	};
	
	instanceProto.worldToCellY = function (y)
	{
		return Math.floor((y - this.y) / (this.cellheight * this.tileheight));
	};
	
	instanceProto.getCollisionRectCandidates = function (bbox, candidates)
	{
		var firstCellX = this.worldToCellX(bbox.left);
		var firstCellY = this.worldToCellY(bbox.top);
		var lastCellX = this.worldToCellX(bbox.right);
		var lastCellY = this.worldToCellY(bbox.bottom);
		
		var cx, cy, cell;
		
		for (cx = firstCellX; cx <= lastCellX; ++cx)
		{
			for (cy = firstCellY; cy <= lastCellY; ++cy)
			{
				cell = this.cellAtIndex(cx, cy);
				
				if (!cell)
					continue;
				
				cell.maybeBuildQuadMap();
				cr.appendArray(candidates, cell.collision_rects);
			}
		}
	};
	
	instanceProto.getAllCollisionRects = function (candidates)
	{
		var i, len, j, lenj, row, cell;
		
		for (i = 0, len = this.tilecells.length; i < len; ++i)
		{
			row = this.tilecells[i];
			
			for (j = 0, lenj = row.length; j < lenj; ++j)
			{
				cell = row[j];
				cell.maybeBuildQuadMap();
				cr.appendArray(candidates, cell.collision_rects);
			}
		}
	};
	
	// called whenever an instance is destroyed
	// note the runtime may keep the object after this call for recycling; be sure
	// to release/recycle/reset any references to other objects in this function.
	instanceProto.onDestroy = function ()
	{
		// Recycle all existing tile cells
		var i, len, j, lenj, arr;
		
		for (i = 0, len = this.tilecells.length; i < len; ++i)
		{
			arr = this.tilecells[i];
			
			for (j = 0, lenj = arr.length; j < lenj; ++j)
			{
				freeTileCell(arr[j]);
			}
			
			arr.length = 0;
		}
		
		this.tilecells.length = 0;
	};
	
	// called when saving the full state of the game
	instanceProto.saveToJSON = function ()
	{
		// return a Javascript object containing information about your object's state
		// note you MUST use double-quote syntax (e.g. "property": value) to prevent
		// Closure Compiler renaming and breaking the save format
		return {
			"w": this.mapwidth,
			"h": this.mapheight,
			"d": this.getTilesAsRLECSV()
		};
	};
	
	// called when loading the full state of the game
	instanceProto.loadFromJSON = function (o)
	{
		this.mapwidth = o["w"];
		this.mapheight = o["h"];
		this.setTilesFromRLECSV(o["d"]);
		
		this.physics_changed = true;
		this.setAllQuadMapChanged();
	};
	
	// only called if a layout object - draw to a canvas 2D context
	instanceProto.draw = function(ctx)
	{
		if (this.tilewidth <= 0 || this.tileheight <= 0)
			return;
		
		this.type.maybeCutTiles(this.tilewidth, this.tileheight, this.tilexoffset, this.tileyoffset, this.tilexspacing, this.tileyspacing, this.seamless);
		
		// If size changed, rebuild the map so it only extends as far as the object bounds
		if (this.width !== this.lastwidth || this.height !== this.lastheight)
		{
			this.physics_changed = true;
			this.setAllQuadMapChanged();
			this.maybeBuildAllQuadMap();
			this.lastwidth = this.width;
			this.lastheight = this.height;
		}

		ctx.globalAlpha = this.opacity;
		//ctx.strokeStyle = "red";
		
		var layer = this.layer;
		var viewLeft = layer.viewLeft;
		var viewTop = layer.viewTop;
		var viewRight = layer.viewRight;
		var viewBottom = layer.viewBottom;
		
		var myx = this.x;
		var myy = this.y;
		var seamless = this.seamless;
		var qrc;
		
		if (this.runtime.pixel_rounding)
		{
			myx = Math.round(myx);
			myy = Math.round(myy);
		}
		
		var cellWidthPx = this.cellwidth * this.tilewidth;
		var cellHeightPx = this.cellheight * this.tileheight;
		
		var firstCellX = Math.floor((viewLeft - myx) / cellWidthPx);
		var lastCellX = Math.floor((viewRight - myx) / cellWidthPx);
		var firstCellY = Math.floor((viewTop - myy) / cellHeightPx);
		var lastCellY = Math.floor((viewBottom - myy) / cellHeightPx);
		
		// The canvas pattern API is complete nonsense. Fudge the transform so it draws in the
		// right darn place.
		var offx = myx % this.tilewidth;
		var offy = myy % this.tileheight;
		
		if (this.seamless)
		{
			offx = 0;
			offy = 0;
		}
		
		if (offx !== 0 || offy !== 0)
		{
			ctx.save();
			ctx.translate(offx, offy);
			myx -= offx;
			myy -= offy;
			viewLeft -= offx;
			viewTop -= offy;
			viewRight -= offx;
			viewBottom -= offy;
		}
		
		var cx, cy, cell, i, len, q, qleft, qtop, qright, qbottom, img;
		for (cx = firstCellX; cx <= lastCellX; ++cx)
		{
			for (cy = firstCellY; cy <= lastCellY; ++cy)
			{
				cell = this.cellAtIndex(cx, cy);
				
				if (!cell)
					continue;
				
				cell.maybeBuildQuadMap();
				
				for (i = 0, len = cell.quads.length; i < len; ++i)
				{
					q = cell.quads[i];
				
					if (q.id === -1)
						continue;
					
					qrc = q.rc;
					qleft = qrc.left + myx;
					qtop = qrc.top + myy;
					qright = qrc.right + myx;
					qbottom = qrc.bottom + myy;
					
					if (qleft > viewRight || qright < viewLeft || qtop > viewBottom || qbottom < viewTop)
						continue;
					
					img = this.type.GetFlippedTileImage(q.tileid, q.horiz_flip, q.vert_flip, q.diag_flip, this.seamless);
					
					// Seamless mode returns a canvas and drawImage's it; otherwise fill the repeating pattern
					if (seamless)
					{
						ctx.drawImage(img, qleft, qtop);
					}
					else
					{
						ctx.fillStyle = this.type.GetFlippedTileImage(q.tileid, q.horiz_flip, q.vert_flip, q.diag_flip, this.seamless);
						ctx.fillRect(qleft, qtop, qright - qleft, qbottom - qtop);
					}
					
					// Outline quads for debug
					//ctx.strokeRect(qleft, qtop, qright - qleft, qbottom - qtop);
				}
				
				/*
				// Highlight collision rects for debug
				for (i = 0, len = cell.collision_rects.length; i < len; ++i)
				{
					qrc = cell.collision_rects[i].rc;
					qleft = qrc.left + myx;
					qtop = qrc.top + myy;
					qright = qrc.right + myx;
					qbottom = qrc.bottom + myy;
					ctx.strokeRect(qleft, qtop, qright - qleft, qbottom - qtop);
				}
				*/
			}
		}
		
		if (offx !== 0 || offy !== 0)
			ctx.restore();
	};
	
	// only called if a layout object in WebGL mode - draw to the WebGL context
	// 'glw' is not a WebGL context, it's a wrapper - you can find its methods in GLWrap.js in the install
	// directory or just copy what other plugins do.
	var tmp_rect = new cr.rect(0, 0, 1, 1);
	
	instanceProto.drawGL = function (glw)
	{
		if (this.tilewidth <= 0 || this.tileheight <= 0)
			return;
		
		this.type.maybeCutTiles(this.tilewidth, this.tileheight, this.tilexoffset, this.tileyoffset, this.tilexspacing, this.tileyspacing, this.seamless);
		
		// If size changed, rebuild the map so it only extends as far as the object bounds
		if (this.width !== this.lastwidth || this.height !== this.lastheight)
		{
			this.physics_changed = true;
			this.setAllQuadMapChanged();
			this.maybeBuildAllQuadMap();
			this.lastwidth = this.width;
			this.lastheight = this.height;
		}
		
		glw.setOpacity(this.opacity);
		
		var cut_tiles = this.type.cut_tiles;
		
		var layer = this.layer;
		var viewLeft = layer.viewLeft;
		var viewTop = layer.viewTop;
		var viewRight = layer.viewRight;
		var viewBottom = layer.viewBottom;
		
		var myx = this.x;
		var myy = this.y;
		var qrc;
		
		if (this.runtime.pixel_rounding)
		{
			myx = Math.round(myx);
			myy = Math.round(myy);
		}
		
		var cellWidthPx = this.cellwidth * this.tilewidth;
		var cellHeightPx = this.cellheight * this.tileheight;
		
		var firstCellX = Math.floor((viewLeft - myx) / cellWidthPx);
		var lastCellX = Math.floor((viewRight - myx) / cellWidthPx);
		var firstCellY = Math.floor((viewTop - myy) / cellHeightPx);
		var lastCellY = Math.floor((viewBottom - myy) / cellHeightPx);
		
		var i, len, q, qleft, qtop, qright, qbottom;
		var qtlx, qtly, qtrx, qtry, qbrx, qbry, qblx, qbly, temp;
		var cx, cy, cell;
		
		for (cx = firstCellX; cx <= lastCellX; ++cx)
		{
			for (cy = firstCellY; cy <= lastCellY; ++cy)
			{
				cell = this.cellAtIndex(cx, cy);
				
				if (!cell)
					continue;
				
				cell.maybeBuildQuadMap();
		
				for (i = 0, len = cell.quads.length; i < len; ++i)
				{
					q = cell.quads[i];
					
					if (q.id === -1)
						continue;
					
					qrc = q.rc;
					qleft = qrc.left + myx;
					qtop = qrc.top + myy;
					qright = qrc.right + myx;
					qbottom = qrc.bottom + myy;
					
					if (qleft > viewRight || qright < viewLeft || qtop > viewBottom || qbottom < viewTop)
						continue;
					
					glw.setTexture(cut_tiles[q.tileid]);
					tmp_rect.right = (qright - qleft) / this.tilewidth;
					tmp_rect.bottom = (qbottom - qtop) / this.tileheight;
					
					if (q.any_flip)
					{
						if (q.diag_flip)
						{
							temp = tmp_rect.right;
							tmp_rect.right = tmp_rect.bottom;
							tmp_rect.bottom = temp;
						}
						
						qtlx = 0;
						qtly = 0;
						qtrx = tmp_rect.right;
						qtry = 0;
						qbrx = tmp_rect.right;
						qbry = tmp_rect.bottom;
						qblx = 0;
						qbly = tmp_rect.bottom;
						
						if (q.diag_flip)
						{
							// Diagonal flip: swap topright and bottomleft points
							temp = qblx;		qblx = qtrx;		qtrx = temp;
							temp = qbly;		qbly = qtry;		qtry = temp;
						}
						
						if (q.horiz_flip)
						{
							// Horizontal flip: swap topleft and topright; bottomleft and bottomright points
							temp = qtlx;		qtlx = qtrx;		qtrx = temp;
							temp = qtly;		qtly = qtry;		qtry = temp;
							
							temp = qblx;		qblx = qbrx;		qbrx = temp;
							temp = qbly;		qbly = qbry;		qbry = temp;
						}
						
						if (q.vert_flip)
						{
							// Vertical flip: swap topleft and bottomleft; topright and bottomright points
							temp = qtlx;		qtlx = qblx;		qblx = temp;
							temp = qtly;		qtly = qbly;		qbly = temp;
							
							temp = qtrx;		qtrx = qbrx;		qbrx = temp;
							temp = qtry;		qtry = qbry;		qbry = temp;
						}
						
						glw.quadTexUV(qleft, qtop, qright, qtop, qright, qbottom, qleft, qbottom, qtlx, qtly, qtrx, qtry, qbrx, qbry, qblx, qbly);
					}
					else
					{
						// Normal render without any flip
						glw.quadTex(qleft, qtop, qright, qtop, qright, qbottom, qleft, qbottom, tmp_rect);
					}
				}
			}
		}
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
		
		var drawrects = 0;
		var collrects = 0;
		
		var i, len, row, j, lenj, c;
		for (i = 0, len = this.tilecells.length; i < len; ++i)
		{
			row = this.tilecells[i];
			
			for (j = 0, lenj = row.length; j < lenj; ++j)
			{
				c = row[j];
				
				drawrects += c.quads.length;
				collrects += c.collision_rects.length;
			}
		}

		propsections.push({
			"title": "Tilemap",
			"properties": [
				// Each property entry can use the following values:
				// "name" (required): name of the property (must be unique within this section)
				// "value" (required): a boolean, number or string for the value
				// "html" (optional, default false): set to true to interpret the name and value
				//									 as HTML strings rather than simple plain text
				// "readonly" (optional, default false): set to true to disable editing the property
				 {"name": "Tile width", "value": this.tilewidth, "readonly": true},
				 {"name": "Tile height", "value": this.tileheight, "readonly": true},
				 {"name": "Draw rects", "value": drawrects, "readonly": true},
				 {"name": "Collision rects", "value": collrects, "readonly": true}
			]
		});

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

	Cnds.prototype.CompareTileAt = function (tx, ty, cmp, t)
	{
		var tile = this.getTileAt(tx, ty);
		
		if (tile !== -1)
			tile = (tile & TILE_ID_MASK);
		
		return cr.do_cmp(tile, cmp, t);
	};

	function StateComboToFlags(state)
	{
		switch (state) {
		case 0:		// normal
			return 0;
		case 1:		// flipped horizontal
			return TILE_FLIPPED_HORIZONTAL;
		case 2:		// flipped vertical
			return TILE_FLIPPED_VERTICAL;
		case 3:		// rotated 90
			return TILE_FLIPPED_HORIZONTAL | TILE_FLIPPED_DIAGONAL;
		case 4:		// rotated 180
			return TILE_FLIPPED_HORIZONTAL | TILE_FLIPPED_VERTICAL;
		case 5:		// rotated 270
			return TILE_FLIPPED_VERTICAL | TILE_FLIPPED_DIAGONAL;
		case 6:		// rotated 90, flipped vertical
			return TILE_FLIPPED_HORIZONTAL | TILE_FLIPPED_VERTICAL | TILE_FLIPPED_DIAGONAL;
		case 7:		// rotated 270, flipped vertical
			return TILE_FLIPPED_DIAGONAL;
		default:
			return 0;
		}
	};
	
	Cnds.prototype.CompareTileStateAt = function (tx, ty, state)
	{
		var tile = this.getTileAt(tx, ty);
		var flags = 0;
		
		if (tile !== -1)
			flags = (tile & TILE_FLAGS_MASK);
		
		return flags === StateComboToFlags(state);
	};
	
	Cnds.prototype.OnURLLoaded = function ()
	{
		return true;
	};
	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.EraseTile = function (tx, ty)
	{
		this.maybeResizeTilemap();
		this.setTileAt(tx, ty, -1);
	};
	
	Acts.prototype.SetTile = function (tx, ty, t, state)
	{
		this.maybeResizeTilemap();
		this.setTileAt(tx, ty, (t & TILE_ID_MASK) | StateComboToFlags(state));
	};
	
	Acts.prototype.SetTileState = function (tx, ty, state)
	{
		var t = this.getTileAt(tx, ty);
		if (t !== -1)
		{
			this.maybeResizeTilemap();
			this.setTileAt(tx, ty, (t & TILE_ID_MASK) | StateComboToFlags(state));
		}
	};
	
	Acts.prototype.EraseTileRange = function (tx, ty, tw, th)
	{
		var fromx = Math.floor(cr.max(tx, 0));
		var fromy = Math.floor(cr.max(ty, 0));
		var tox = Math.floor(cr.min(tx + tw, this.mapwidth));
		var toy = Math.floor(cr.min(ty + th, this.mapheight));
		
		var x, y;
		for (y = fromy; y < toy; ++y)
		{
			for (x = fromx; x < tox; ++x)
			{
				this.setTileAt(x, y, -1);
			}
		}
	};
	
	Acts.prototype.SetTileRange = function (tx, ty, tw, th, t, state)
	{
		this.maybeResizeTilemap();
		
		var fromx = Math.floor(cr.max(tx, 0));
		var fromy = Math.floor(cr.max(ty, 0));
		var tox = Math.floor(cr.min(tx + tw, this.mapwidth));
		var toy = Math.floor(cr.min(ty + th, this.mapheight));
		
		var settile = (t & TILE_ID_MASK) | StateComboToFlags(state);
		
		var x, y;
		for (y = fromy; y < toy; ++y)
		{
			for (x = fromx; x < tox; ++x)
			{
				this.setTileAt(x, y, settile);
			}
		}
	};
	
	Acts.prototype.SetTileStateRange = function (tx, ty, tw, th, state)
	{
		this.maybeResizeTilemap();
		
		var fromx = Math.floor(cr.max(tx, 0));
		var fromy = Math.floor(cr.max(ty, 0));
		var tox = Math.floor(cr.min(tx + tw, this.mapwidth));
		var toy = Math.floor(cr.min(ty + th, this.mapheight));
		
		var setstate = StateComboToFlags(state);
		
		var x, y, t;
		for (y = fromy; y < toy; ++y)
		{
			for (x = fromx; x < tox; ++x)
			{
				t = this.getTileAt(x, y);
				
				if (t !== -1)
					this.setTileAt(x, y, (t & TILE_ID_MASK) | setstate);
			}
		}
	};
	
	Acts.prototype.LoadFromJSON = function (str)
	{
		var o;
		
		try {
			o = JSON.parse(str);
		}
		catch (e) {
			return;
		}
		
		if (!o["c2tilemap"])
			return;		// not a known tilemap data format
		
		this.mapwidth = o["width"];
		this.mapheight = o["height"];
		this.setTilesFromRLECSV(o["data"]);
		this.setAllQuadMapChanged();
		this.physics_changed = true;
	};
	
	Acts.prototype.JSONDownload = function (filename)
	{
		var a = document.createElement("a");
		
		var o = {
			"c2tilemap": true,
			"width": this.mapwidth,
			"height": this.mapheight,
			"data": this.getTilesAsRLECSV()
		};
		
		if (typeof a.download === "undefined")
		{
			var str = 'data:text/html,' + encodeURIComponent("<p><a download='data.json' href=\"data:application/json,"
				+ encodeURIComponent(JSON.stringify(o))
				+ "\">Download link</a></p>");
			window.open(str);
		}
		else
		{
			// auto download
			var body = document.getElementsByTagName("body")[0];
			a.textContent = filename;
			a.href = "data:application/json," + encodeURIComponent(JSON.stringify(o));
			a.download = filename;
			body.appendChild(a);
			var clickEvent = document.createEvent("MouseEvent");
			clickEvent.initMouseEvent("click", true, true, window, 0, 0, 0, 0, 0, false, false, false, false, 0, null);
			a.dispatchEvent(clickEvent);
			body.removeChild(a);
		}
	};
	
	Acts.prototype.LoadURL = function (url_)
	{
		var img = new Image();
		var self = this;
		
		img.onload = function ()
		{
			var type = self.type;
			type.freeCutTiles();
			type.texture_img = img;
			self.runtime.redraw = true;
			self.runtime.trigger(cr.plugins_.Tilemap.prototype.cnds.OnURLLoaded, self);
		};
		
		if (url_.substr(0, 5) !== "data:")
			img.crossOrigin = "anonymous";
		
		img.src = url_;
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};

	Exps.prototype.TileAt = function (ret, tx, ty)
	{
		var tile = this.getTileAt(tx, ty);
		ret.set_int(tile === -1 ? -1 : (tile & TILE_ID_MASK));
	};
	
	Exps.prototype.PositionToTileX = function (ret, x_)
	{
		ret.set_float(Math.floor((x_ - this.x) / this.tilewidth));
	};
	
	Exps.prototype.PositionToTileY = function (ret, y_)
	{
		ret.set_float(Math.floor((y_ - this.y) / this.tileheight));
	};
	
	Exps.prototype.TileToPositionX = function (ret, x_)
	{
		ret.set_float((x_ * this.tilewidth) + this.x + (this.tilewidth / 2));
	};
	
	Exps.prototype.TileToPositionY = function (ret, y_)
	{
		ret.set_float((y_ * this.tileheight) + this.y + (this.tileheight / 2));
	};
	
	Exps.prototype.SnapX = function (ret, x_)
	{
		ret.set_float((Math.floor((x_ - this.x) / this.tilewidth) * this.tilewidth) + this.x + (this.tilewidth / 2));
	};
	
	Exps.prototype.SnapY = function (ret, y_)
	{
		ret.set_float((Math.floor((y_ - this.y) / this.tileheight) * this.tileheight) + this.y + (this.tileheight / 2));
	};
	
	Exps.prototype.TilesJSON = function (ret)
	{
		ret.set_string(JSON.stringify({
			"c2tilemap": true,
			"width": this.mapwidth,
			"height": this.mapheight,
			"data": this.getTilesAsRLECSV()
		}));
	};
	
	pluginProto.exps = new Exps();

}());