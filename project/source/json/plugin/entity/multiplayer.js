// ECMAScript 5 strict mode
"use strict";

assert2(cr, "cr namespace not created");
assert2(cr.plugins_, "cr.plugins_ not created");

/////////////////////////////////////
// Plugin class
cr.plugins_.Multiplayer = function(runtime)
{
	this.runtime = runtime;
};

(function ()
{
	/////////////////////////////////////
	var pluginProto = cr.plugins_.Multiplayer.prototype;
		
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

	/////////////////////////////////////
	// Instance class
	pluginProto.Instance = function(type)
	{
		this.type = type;
		this.runtime = type.runtime;
	};
	
	var instanceProto = pluginProto.Instance.prototype;
	
	var isSupported = false;
	var serverlist = [];

	// called whenever an instance is created
	instanceProto.onCreate = function()
	{
		this.mp = null;
		
		isSupported = window["C2Multiplayer_IsSupported"]();
		
		if (isSupported && typeof window["C2Multiplayer"] !== "undefined")
			this.mp = new window["C2Multiplayer"]();
		
		this.signallingUrl = "";
		this.errorMsg = "";
		this.peerID = "";
		this.peerAlias = "";
		this.leaveReason = "";
		
		this.msgContent = "";
		this.msgTag = "";
		this.msgFromId = "";
		this.msgFromAlias = "";
		
		this.typeToRo = {};
		this.instToPeer = {};
		this.peerToInst = {};
		
		this.gameInstanceList = null;
		this.roomList = null;
		this.wakerWorker = null;
		
		// Local objects to enable local input prediction for
		this.inputPredictObjects = [];
		this.trackObjects = [];				// objects to keep a history for
		this.objectHistories = {};
		
		var self = this;
		
		if (isSupported)
		{
			this.mp["onserverlist"] = function (servers) {
				serverlist = servers;
				self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnServerList, self);
			};
			
			//this.mp["onsignallingopen"] = function () {
			//	self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnServerList);
			//};
			
			this.mp["onsignallingerror"] = function (e) {
				self.setError(e);
				self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnSignallingError, self);
			};
			
			this.mp["onsignallingclose"] = function () {
				self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnSignallingDisconnected, self);
				self.signallingUrl = "";
			};
			
			this.mp["onsignallingwelcome"] = function () {
				// note we only fire this when the welcome message arrives, so server details are immediately available
				self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnSignallingConnected, self);
			};
			
			this.mp["onsignallinglogin"] = function () {
				self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnSignallingLoggedIn, self);
			};
			
			this.mp["onsignallingjoin"] = function (became_host) {
				self.instToPeer = {};
				self.peerToInst = {};
				self.trackObjects.length = 0;
				self.inputPredictObjects.length = 0;
				self.objectHistories = {};
				self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnSignallingJoinedRoom, self);
				
				// If became the host and the tab has already gone in to the background, the
				// waker worker won't be running, so start it running now.
				if (self.runtime.isSuspended && became_host)
				{
					self.wakerWorker.postMessage("start");
				}
			};
			
			this.mp["onsignallingleave"] = function () {
				self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnSignallingLeftRoom, self);
			};
			
			this.mp["onsignallingkicked"] = function () {
				self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnSignallingKicked, self);
			};
			
			this.mp["onbeforeclientupdate"] = function () {
				self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnClientUpdate, self);
			};
			
			this.mp["onpeeropen"] = function (peer) {
				self.peerID = peer["id"];
				self.peerAlias = peer["alias"];
				self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnPeerConnected, self);
			};
			
			this.mp["onpeerclose"] = function (peer, reason) {
				self.peerID = peer["id"];
				self.peerAlias = peer["alias"];
				self.leaveReason = reason || "unknown";
				
				// Destroy associated object if any
				var inst = self.getAssociatedInstanceForPeer(peer);
				
				if (inst)
				{
					var ro = self.typeToRo[inst.type];
					
					if (ro)
						ro["removeObjectNid"](peer["nid"]);
					
					self.runtime.DestroyInstance(inst);
				}
				
				if (self.peerToInst.hasOwnProperty(peer["id"]))
					delete self.peerToInst[peer["id"]];
				
				self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnPeerDisconnected, self);
			};
			
			this.mp["onpeermessage"] = function (peer, o) {
				// message payload
				self.msgTag = o["t"];
				
				// allow "f" field to override sender
				if (o["f"])
					self.msgFromId = o["f"];
				else
					self.msgFromId = peer["id"];
				
				self.msgFromAlias = self.mp["getAliasFromId"](self.msgFromId);
				self.msgContent = o["m"];
				
				self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnAnyPeerMessage, self);
				self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnPeerMessage, self);
			};
			
			this.mp["onsignallinginstancelist"] = function (list) {
				self.gameInstanceList = list;
				self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnGameInstanceList, self);
			};
			
			this.mp["onsignallingroomlist"] = function (list) {
				self.roomList = list;
				self.runtime.trigger(cr.plugins_.Multiplayer.prototype.cnds.OnRoomList, self);
			};
			
			this.mp["ongetobjectcount"] = function (obj) {
				return obj.instances.length;
			};
			
			this.mp["ongetobjectvalue"] = function (obj, index, nv) {
				// no netvalue specified: return unique id to indentify which object this is
				if (!nv)
					return obj.instances[index].uid;
				
				// otherwise return requested netvalue
				var tag = nv["tag"];
				var inst = obj.instances[index];
				var value, peer;
				
				switch (tag) {
				case "x":
					return inst.x;
				case "y":
					return inst.y;
				case "a":
					return inst.angle;
				case "iv":
					// If a client value tag is available, try and look up the latest non-delayed non-interpolated
					// value to forward to other peers as quickly as possible
					if (nv["clientvaluetag"])
					{
						peer = self.instToPeer[inst.uid];
						
						// Don't do this for self peer when host though
						if (peer && self.mp["me"] !== peer && !peer["wasRemoved"] && peer["hasClientState"](nv["clientvaluetag"]))
						{
							return peer["getInterpClientState"](nv["clientvaluetag"]);
						}
					}
					
					// Otherwise just forward the current value
					value = inst.instance_vars[nv["userdata"]];
					
					if (typeof value === "number")
						return value;
					else
						return 0;
				default:
					return 0;
				}
			};
			
			this.mp["oninstancedestroyed"] = function (ro, nid, timestamp)
			{
				var userdata = ro["userdata"];
				var instmap = userdata.instmap;		// map NID to C2 instance
				
				// Log the time that this NID was destroyed, so we avoid creating it if late incoming
				// updates arrive that were originally sent before it was destroyed
				if (!userdata.deadmap)
					userdata.deadmap = {};
				
				var deadmap = userdata.deadmap;
				deadmap[nid] = timestamp;
				
				if (!instmap)
				{
					return;
				}
				
				// Lookup instance and destroy if we find it
				if (instmap.hasOwnProperty(nid))
				{
					var inst = instmap[nid];
					
					if (inst)
					{
						self.runtime.DestroyInstance(inst);
					}
					
					delete instmap[nid];
				}
			};
		
			this.runtime.addDestroyCallback(function (inst)
			{
				var p;
				var uid = inst.uid;
				
				if (self.instToPeer.hasOwnProperty(uid))
					delete self.instToPeer[uid];
				
				for (p in self.peerToInst)
				{
					if (self.peerToInst.hasOwnProperty(p))
					{
						if (self.peerToInst[p] === inst)
						{
							delete self.peerToInst[p];
							break;
						}
					}
				}
				
				if (self.objectHistories.hasOwnProperty(uid))
					delete self.objectHistories[uid];
				
				var i = self.inputPredictObjects.indexOf(inst);
				
				if (i > -1)
					self.inputPredictObjects.splice(i, 1);
				
				i = self.trackObjects.indexOf(inst);
				
				if (i > -1)
					self.trackObjects.splice(i, 1);
				
				self.mp["removeObjectId"](uid);
			});

			this.wakerWorker = new Worker("waker.js");
			
			this.wakerWorker.addEventListener("message", function (e) {
				if (e.data === "tick" && self.runtime.isSuspended)
				{
					self.runtime.tick(true);
				}
			}, false);
			
			this.wakerWorker.postMessage("");
		}
		
		this.runtime.addSuspendCallback(function (s) {
			if (!isSupported || !self.mp["isHost"]())
				return;
			
			// Suspending and is currently host: use a web worker to keep the game alive
			if (s)
			{
				self.wakerWorker.postMessage("start");
			}
			// Resuming and is currently host: stop using web worker to keep running, will revert to rAF
			else
			{
				self.wakerWorker.postMessage("stop");
			}
		});
		
		this.runtime.pretickMe(this);
	};
	
	instanceProto.getAssociatedInstanceForPeer = function (peer)
	{
		var peer_id = peer["id"];
		
		if (this.peerToInst.hasOwnProperty(peer_id))
			return this.peerToInst[peer_id];
		else
			return null;
	};
	
	instanceProto.isInputPredicting = function (inst)
	{
		return this.inputPredictObjects.indexOf(inst) >= 0;
	};
	
	instanceProto.pretick = function ()
	{
		if (!isSupported)
			return;
		
		this.mp["tick"](this.runtime.dt1);
		
		// keep history for all tracked objects, so we can tell where they were after a delay
		this.trackObjectHistories();
		
		// client: update registered objects
		var i, len, ro;
		
		if (this.mp["isInRoom"]() && !this.mp["isHost"]())
		{
			for (i = 0, len = this.mp["registeredObjects"].length; i < len; ++i)
			{
				ro = this.mp["registeredObjects"][i];
				
				this.updateRegisteredObject(ro);
			}
		}
	};
	
	var netInstToRemove = [];
	
	instanceProto.updateRegisteredObject = function (ro)
	{
		ro["tick"]();
		
		var type = ro["obj"];
		var count = ro["getCount"]();
		var netvalues = ro["netvalues"];
		var userdata = ro["userdata"];
		if (!userdata.instmap)
			userdata.instmap = {};
		var instmap = userdata.instmap;		// map NID to C2 instance
		var deadmap = userdata.deadmap;
		var simTime = ro["simTime"];
		
		var i, netinst, nid, inst, peer, nv, iv, value, j, p, created, lenj = netvalues.length;
		
		// Clear any destroyed instances from instmap
		var p;
		for (p in instmap)
		{
			if (instmap.hasOwnProperty(p))
			{
				inst = instmap[p];
				
				if (!this.runtime.getObjectByUID(inst.uid))
					delete instmap[p];
			}
		}
		
		// Clear any records older than 3s from the deadmap
		if (deadmap)
		{
			for (p in deadmap)
			{
				if (deadmap.hasOwnProperty(p))
				{
					if (deadmap[p] < simTime - 3000)
						delete deadmap[p];
				}
			}
		}
		
		// For each network instance
		for (i = 0; i < count; ++i)
		{
			netinst = ro["getNetInstAt"](i);
			nid = netinst["nid"];
			
			// If this NID is in the deadmap, we received a destroy notification <3s ago.
			// In this case assume this update is late and ignore it.
			if (deadmap && deadmap[nid] >= simTime - 3000)
			{
				netInstToRemove.push(netinst);
				
				if (instmap.hasOwnProperty(nid))
				{
					this.runtime.DestroyInstance(instmap[nid]);
					delete instmap[nid];
				}
				
				continue;
			}
			
			// Look up C2 instance for this NID, or create it if it's new
			if (instmap.hasOwnProperty(nid))
			{
				inst = instmap[nid];
				created = false;
			}
			else
			{
				// We want the client to know the correct peer ID and alias for the 'on created' event for this object,
				// if this is an object type associated with peers. When it's an associated object its NID will match
				// the NID of the peer who it represents. So look up the peer by the object NID and set their ID and
				// alias for the trigger.
				peer = this.mp["getPeerByNid"](nid);
				
				if (peer)
				{
					this.peerID = peer["id"];
					this.peerAlias = peer["alias"];
				}
				else
				{
					// We don't know which peer this netinst corresponds to.
					// If the registered object indicates that NIDs are corresponding
					// to peer NIDs, then we can't create the object until we know about
					// that peer, so in this case skip until we receive info about the peer.
					if (ro["hasOverriddenNids"])
					{
						continue;
					}
					
					this.peerID = "";
					this.peerAlias = "";
				}
				
				inst = this.runtime.createInstance(type, this.runtime.running_layout.layers[type.default_layerindex], -1000, -1000);
				instmap[nid] = inst;
				created = true;
			}
			
			if (netinst["isTimedOut"](simTime))
			{
				// Destroy this instance, haven't had any updates for it for too long
				netInstToRemove.push(netinst);
				
				if (instmap.hasOwnProperty(nid))
					delete instmap[nid];
				
				this.runtime.DestroyInstance(inst);
				continue;
			}
			
			if (this.isInputPredicting(inst))
			{
				this.correctInputPrediction(inst, netinst, netvalues, simTime);
			}
			// Not input predicting: ordinary value update
			else
			{
				// Update each net value
				for (j = 0; j < lenj; ++j)
				{
					value = netinst["getInterp"](simTime, j);				
					nv = netvalues[j];
					
					switch (nv["tag"]) {
					case "x":
						inst.x = value;
						inst.set_bbox_changed();
						break;
					case "y":
						inst.y = value;
						inst.set_bbox_changed();
						break;
					case "a":
						inst.angle = value;
						inst.set_bbox_changed();
						break;
					case "iv":
						iv = nv["userdata"];
						if (iv > inst.instance_vars.length || typeof inst.instance_vars[iv] !== "number")
							break;
						
						inst.instance_vars[iv] = value;
						break;
					}
				}
			}
			
			if (created)
			{
				// Fire 'On created'
				this.runtime.trigger(Object.getPrototypeOf(type.plugin).cnds.OnCreated, inst);
			}
		}
		
		for (i = 0, count = netInstToRemove.length; i < count; ++i)
		{
			ro["removeNetInstance"](netInstToRemove[i]);
		}
		
		netInstToRemove.length = 0;
	};
	
	instanceProto.trackObjectHistories = function ()
	{
		var hosttime = this.mp["getHostInputArrivalTime"]();
		
		var i, len;
		for (i = 0, len = this.trackObjects.length; i < len; ++i)
		{
			this.trackObjectHistory(this.trackObjects[i], hosttime);
		}
	};
	
	function ObjectHistory(inst_)
	{
		this.inst = inst_;
		
		this.history = [];
	};
	
	ObjectHistory.prototype.getLastDelta = function (tag, i)
	{
		if (this.history.length < 2)
			return 0;		// not yet enough data
		
		var from = this.history[this.history.length - 2];
		var to = this.history[this.history.length - 1];
		
		switch (tag) {
		case "x":
			return to.x - from.x;
		case "y":
			return to.y - from.y;
		case "a":
			return to.angle - from.angle;
		case "iv":
			return to.ivs[i] - from.ivs[i];
		}
		
		return 0;
	};
	
	ObjectHistory.prototype.getInterp = function (tag, index, time, interp)
	{
		// Search through history for newest entry older than 'time'
		var i, len, h;
		var prev = null;
		var next = null;
		
		for (i = 0, len = this.history.length; i < len; ++i)
		{
			h = this.history[i];
			
			if (h.timestamp < time)
				prev = h;
			else
			{
				if (i + 1 < this.history.length)
					next = this.history[i + 1];
				
				break;
			}
		}
		
		// No data: return undefined
		if (!prev)
			return (void 0);
		
		var prev_value = 0;
		
		switch (tag) {
		case "x":
			prev_value = prev.x;
			break;
		case "y":
			prev_value = prev.y;
			break;
		case "a":
			prev_value = prev.angle;
			break;
		case "iv":
			prev_value = prev.ivs[index];
			break;
		}
		
		if (!next)
			return prev_value;
		
		var next_value = prev_value;
		
		switch (tag) {
		case "x":
			next_value = next.x;
			break;
		case "y":
			next_value = next.y;
			break;
		case "a":
			next_value = next.angle;
			break;
		case "iv":
			next_value = next.ivs[index];
			break;
		}
		
		var x = cr.unlerp(prev.timestamp, next.timestamp, time);
		
		return window["interpNetValue"](interp, prev_value, next_value, x, false);
	};
	
	ObjectHistory.prototype.applyCorrection = function (tag, index, correction)
	{
		var i, len, h;
		for (i = 0, len = this.history.length; i < len; ++i)
		{
			h = this.history[i];
			
			switch (tag) {
			case "x":
				h.x += correction;
				break;
			case "y":
				h.y += correction;
				break;
			case "a":
				h.angle += correction;
				break;
			case "iv":
				h.ivs[index] += correction;
				break;
			}
		};
	};
	
	function ObjectHistoryEntry(oh_)
	{
		this.oh = oh_;
		
		this.timestamp = 0;
		this.x = 0;
		this.y = 0;
		this.angle = 0;
		this.ivs = [];
	};
	
	var history_cache = [];
	
	function allocHistoryEntry(oh_)
	{
		var ret;
		
		if (history_cache.length)
		{
			ret = history_cache.pop();
			ret.oh = oh_;
			return ret;
		}
		else
			return new ObjectHistoryEntry(oh_);
	};
	
	function freeHistoryEntry(he)
	{
		he.ivs.length = 0;
		
		if (history_cache.length < 1000)
			history_cache.push(he);
	};
	
	instanceProto.trackObjectHistory = function (inst, hosttime)
	{
		// Create object history if none yet exists
		if (!this.objectHistories.hasOwnProperty(inst.uid))
			this.objectHistories[inst.uid] = new ObjectHistory(inst);
		
		var oh = this.objectHistories[inst.uid];
		
		// Add latest history entry
		var he = allocHistoryEntry(oh);
		he.timestamp = hosttime;
		he.x = inst.x;
		he.y = inst.y;
		he.angle = inst.angle;
		cr.shallowAssignArray(he.ivs, inst.instance_vars);
		oh.history.push(he);
		
		// Expire entries older than 2s
		while (oh.history.length > 0 && oh.history[0].timestamp <= (hosttime - 2000))
			freeHistoryEntry(oh.history.shift());
	};
	
	var clientXerror = 0;
	var clientYerror = 0;
	var hostX = 0;
	var hostY = 0;
	
	function linearCorrect(current, target, delta, position, isPlatformBehavior)
	{
		var diff = target - current;
		
		// No error
		if (diff === 0)
			return 0;
		
		var abs_diff = cr.abs(diff);
		
		// If the error is less than 10% of the current change, or is simply
		// very small, simply bring up to the correct value.
		// Alternatively if there is a discontinuity and we're over 1000px off,
		// jump back in to place.
		if (abs_diff <= cr.abs(delta / 10) || (position && (abs_diff < 1 || abs_diff >= 1000)))
			return diff;
		
		// Minimum correction is 2%, or at least 1px for the Platform behavior
		var minimum_correction = abs_diff / 50;
		
		if (isPlatformBehavior)
			minimum_correction = cr.max(minimum_correction, 1);
		
		// If delta indicates this value is changing, allow the correction
		// to be up to 20% of the change
		var delta_correction = cr.abs(delta / 5);
		
		// Except if a position which is more than 10px out, allow bigger delta correction
		if (position && abs_diff >= 10)
		{
			minimum_correction = abs_diff / 10;
			delta_correction = cr.abs(delta / 2);
		}
		
		// Apply whichever correction is larger
		var abs_correction = cr.max(minimum_correction, delta_correction);
		
		// Don't allow the correction to exceed the difference (would cause overshoot)
		if (abs_correction > abs_diff)
			abs_correction = abs_diff;
		
		// Apply the correction, but don't overshoot the target
		if (diff > 0)
			return abs_correction;
		else
			return -abs_correction;
	};
	
	instanceProto.correctInputPrediction = function (inst, netinst, netvalues, simTime)
	{
		if (!this.objectHistories.hasOwnProperty(inst.uid))
			return;		// no data yet, leave as is
		
		var oh = this.objectHistories[inst.uid];
		
		var j, latestupdate, host_value, my_value, nv, diff, iv, tag, interp, userdata, correction;
		var lenj = netvalues.length;
		
		var position_delta = cr.distanceTo(0, 0, oh.getLastDelta("x"), oh.getLastDelta("y"));
		var isPlatformBehavior = !!inst.extra["isPlatformBehavior"];
		
		// Update each net value
		for (j = 0; j < lenj; ++j)
		{
			nv = netvalues[j];
			tag = nv["tag"];
			userdata = nv["userdata"];
			interp = nv["interp"];
			
			latestupdate = netinst["getLatestUpdate"]();
			
			if (!latestupdate)
				continue;		// no data from server
			
			host_value = latestupdate.data[j];
			my_value = oh.getInterp(tag, userdata, latestupdate.timestamp, interp);
			
			if (typeof my_value === "undefined")
				continue;		// no local history yet
			
			correction = 0;
			
			// For positions, if the object is more than 1px out, move it half way to
			// the destination. For angles do the same if more than 1 degree out. For
			// instance variables just always move half way to the host value.
			switch (tag) {
			case "x":				
				correction = linearCorrect(my_value, host_value, position_delta, true, isPlatformBehavior);
				
				if (correction !== 0)
				{
					inst.x += correction;
					inst.set_bbox_changed();
				}
				
				clientXerror = host_value - my_value + correction;
				hostX = host_value;
				break;
			case "y":
				correction = linearCorrect(my_value, host_value, position_delta, true, isPlatformBehavior);
				
				if (correction !== 0)
				{
					inst.y += correction;
					inst.set_bbox_changed();
				}
				
				clientYerror = host_value - my_value + correction;
				hostY = host_value;
				break;
			case "a":
				correction = cr.anglelerp(my_value, host_value, 0.5) - my_value;
				
				if (correction !== 0)
				{
					inst.angle += correction;
					inst.set_bbox_changed();
				}
				break;
			case "iv":
				iv = userdata;
				if (iv > inst.instance_vars.length || typeof inst.instance_vars[iv] !== "number")
					break;
				
				// Is not input value: update from host
				if (!nv["clientvaluetag"])
					inst.instance_vars[iv] = netinst["getInterp"](simTime, j);
				
				break;
			}
			
			if (correction !== 0)
				oh.applyCorrection(tag, userdata, correction);
		}
	};
	
	instanceProto.setError = function (e)
	{
		if (!e)
			this.errorMsg = "unknown error";
		else if (typeof e === "string")
			this.errorMsg = e;
		else if (typeof e.message === "string")
			this.errorMsg = e;
		else if (typeof e.details === "string")
			this.errorMsg = e;
		else if (typeof e.data === "string")
			this.errorMsg = e;
		else
			this.errorMsg = e.toString();
	};
	
	// called whenever an instance is destroyed
	// note the runtime may keep the object after this call for recycling; be sure
	// to release/recycle/reset any references to other objects in this function.
	instanceProto.onDestroy = function ()
	{
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

	Cnds.prototype.OnServerList = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnSignallingError = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnSignallingConnected = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnSignallingDisconnected = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnSignallingLoggedIn = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnSignallingJoinedRoom = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnSignallingLeftRoom = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnSignallingKicked = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnPeerConnected = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnPeerDisconnected = function ()
	{
		return true;
	};
	
	Cnds.prototype.SignallingIsConnected = function ()
	{
		return isSupported && this.mp["isConnected"]();
	};
	
	Cnds.prototype.SignallingIsLoggedIn = function ()
	{
		return isSupported && this.mp["isLoggedIn"]();
	};
	
	Cnds.prototype.SignallingIsInRoom = function ()
	{
		return isSupported && this.mp["isInRoom"]();
	};
	
	Cnds.prototype.IsHost = function ()
	{
		return isSupported && this.mp["isHost"]();
	};
	
	Cnds.prototype.IsSupported = function ()
	{
		return isSupported;
	};
	
	Cnds.prototype.OnPeerMessage = function (tag_)
	{
		return this.msgTag === tag_;
	};
	
	Cnds.prototype.OnAnyPeerMessage = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnClientUpdate = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnGameInstanceList = function ()
	{
		return true;
	};
	
	Cnds.prototype.OnRoomList = function ()
	{
		return true;
	};
	
	Cnds.prototype.IsReadyForInput = function ()
	{
		if (!isSupported)
			return false;
		
		return this.mp["isReadyForInput"]();
	};
	
	Cnds.prototype.ComparePeerCount = function (cmp_, x_)
	{
		var peercount = 0;
		
		if (isSupported)
			peercount = this.mp["getPeerCount"]();
		
		return cr.do_cmp(peercount, cmp_, x_);
	};
	
	pluginProto.cnds = new Cnds();
	
	//////////////////////////////////////
	// Actions
	function Acts() {};

	Acts.prototype.RequestServerList = function (url_)
	{
		if (!isSupported)
			return;
		
		this.mp["requestServerList"](url_);
	};
	
	Acts.prototype.SignallingConnect = function (url_)
	{
		if (!isSupported || this.mp["isConnected"]())
			return;
		
		this.signallingUrl = url_;
		this.mp["signallingConnect"](url_);
	};
	
	Acts.prototype.SignallingDisconnect = function ()
	{
		if (!isSupported || !this.mp["isConnected"]())
			return;
		
		this.mp["signallingDisconnect"]();
	};
	
	Acts.prototype.AddICEServer = function (url_, username_, credential_)
	{
		if (!isSupported)
			return;
		
		var o = {
			"urls": url_
		};
		
		if (username_)
			o["username"] = username_;
		
		if (credential_)
			o["credential"] = credential_;
		
		this.mp["mergeIceServerList"]([o]);
	};
	
	Acts.prototype.SignallingLogin = function (alias_)
	{
		if (!isSupported || !this.mp["isConnected"] || this.mp["isLoggedIn"]())
			return;
		
		this.mp["signallingLogin"](alias_);
	};
	
	Acts.prototype.SignallingJoinRoom = function (game_, instance_, room_, max_clients_)
	{
		if (!isSupported || !this.mp["isLoggedIn"]() || this.mp["isInRoom"]())
			return;
		
		this.mp["signallingJoinGameRoom"](game_, instance_, room_, max_clients_);
	};
	
	Acts.prototype.SignallingAutoJoinRoom = function (game_, instance_, room_, max_clients_, locking_)
	{
		if (!isSupported || !this.mp["isLoggedIn"]() || this.mp["isInRoom"]())
			return;
		
		// Locking mode 0 = lock when full
		this.mp["signallingAutoJoinGameRoom"](game_, instance_, room_, max_clients_, (locking_ === 0));
	};
	
	Acts.prototype.SignallingLeaveRoom = function ()
	{
		if (!isSupported || !this.mp["isInRoom"]())
			return;
		
		this.mp["signallingLeaveRoom"]();
	};
	
	Acts.prototype.DisconnectRoom = function ()
	{
		if (!isSupported)
			return;
		
		this.mp["disconnectRoom"](true);
	};
	
	function modeToDCType(mode_)
	{
		switch (mode_) {
		case 0:		// reliable ordered
			return "o";
		case 1:		// reliable unordered
			return "r";
		case 2:		// unreliable
			return "u";
		default:
			return "o";
		}
	};
	
	Acts.prototype.SendPeerMessage = function (peerid_, tag_, message_, mode_)
	{
		if (!isSupported)
			return;
		
		if (!peerid_)
			peerid_ = this.mp["getHostID"]();
		
		var peer = this.mp["getPeerById"](peerid_);
		
		if (!peer)
			return;
		
		peer["send"](modeToDCType(mode_), JSON.stringify({
			"c": "m",			// command: message
			"t": tag_,			// tag
			"m": message_		// content
		}));
	};
	
	Acts.prototype.HostBroadcastMessage = function (from_id_, tag_, message_, mode_)
	{
		if (!isSupported)
			return;
		
		var skip = this.mp["getPeerById"](from_id_);
		
		var o = {
			"c": "m",			// command: message
			"t": tag_,			// tag
			"f": (from_id_ || this.mp["getHostID"]()),
			"m": message_		// content
		};
		
		this.mp["hostBroadcast"](modeToDCType(mode_), JSON.stringify(o), skip);
	};
	
	Acts.prototype.SimulateLatency = function (latency_, pdv_, loss_)
	{
		if (!isSupported)
			return;
		
		this.mp["setLatencySimulation"](latency_ * 1000, pdv_ * 1000, loss_);
	};
	
	instanceProto.doSyncObject = function (type_, data_, precision_, bandwidth_)
	{
		if (!isSupported)
			return;
		
		var ro = this.mp["registerObject"](type_, type_.sid, bandwidth_);
		
		if (data_ === 1)		// position only
		{
			ro["addValue"](this.mp["INTERP_LINEAR"], precision_, "x");		// for x
			ro["addValue"](this.mp["INTERP_LINEAR"], precision_, "y");		// for y
		}
		else if (data_ === 2)	// angle only
		{
			ro["addValue"](this.mp["INTERP_ANGULAR"], precision_, "a");		// for angle
		}
		else if (data_ === 3)	// position and angle
		{
			ro["addValue"](this.mp["INTERP_LINEAR"], precision_, "x");		// for x
			ro["addValue"](this.mp["INTERP_LINEAR"], precision_, "y");		// for y
			ro["addValue"](this.mp["INTERP_ANGULAR"], precision_, "a");		// for angle
		}
		
		this.typeToRo[type_] = ro;
	};
	
	Acts.prototype.SyncObject = function (type_, data_, precision_, bandwidth_)
	{
		if (!isSupported)
			return;
		
		var i, len;
		
		// If syncing a family, treat as if using a separate 'Sync object' for each type in the family
		if (type_.is_family)
		{
			for (i = 0, len = type_.members.length; i < len; ++i)
			{
				this.doSyncObject(type_.members[i], data_, precision_, bandwidth_);
			}
		}
		else
		{
			this.doSyncObject(type_, data_, precision_, bandwidth_);
		}
	};
	
	var prompted_bad_sync = false;
	
	instanceProto.doSyncObjectInstanceVar = function (type_, var_, precision_, interp_, clientvaluetag_)
	{
		if (!isSupported)
			return;
		
		// Get existing registered object (if any)
		var ro = this.typeToRo[type_];
		
		if (!ro)
		{
/**BEGIN-PREVIEWONLY**/
			// This seems to be a common mistake so throw up an alert (once only)
			if (!prompted_bad_sync)
			{
				prompted_bad_sync = true;
				alert("Multiplayer object: incorrect use of 'Sync instance variable' for object type '" + type_.name + "'. You must use 'Sync object' before you can use 'Sync instance variable'. Please refer to the documentation for more information.");
			}
/**END-PREVIEWONLY**/
			
			return;
		}
		
		var ip = this.mp["INTERP_NONE"];
		
		if (interp_ === 1)		// linear interpolation
			ip = this.mp["INTERP_LINEAR"];
		else if (interp_ === 2)	// angular interpolation
			ip = this.mp["INTERP_ANGULAR"];
		
		ro["addValue"](ip, precision_, "iv", var_, clientvaluetag_);
	};
	
	Acts.prototype.SyncObjectInstanceVar = function (type_, var_, precision_, interp_, clientvaluetag_)
	{
		if (!isSupported)
			return;
		
		var i, len, t;
		
		if (type_.is_family)
		{
			for (i = 0, len = type_.members.length; i < len; ++i)
			{
				t = type_.members[i];
				this.doSyncObjectInstanceVar(t, var_ + t.family_var_map[type_.family_index], precision_, interp_, clientvaluetag_);
			}
		}
		else
		{
			this.doSyncObjectInstanceVar(type_, var_, precision_, interp_, clientvaluetag_);
		}
	};
	
	Acts.prototype.AssociateObjectWithPeer = function (type_, peerid_)
	{
		if (!isSupported)
			return;
		
		var inst = type_.getFirstPicked();
		
		if (!inst)
			return;
		
		// Get existing registered object (if any)
		var ro = this.typeToRo[type_];
		
		if (!ro)
			return;
		
		var peer = this.mp["getPeerById"](peerid_);
		
		if (!peer)
			return;
		
		if (this.mp["isHost"]())
		{
			ro["overrideNid"](inst.uid, peer["nid"]);
			
			// Host needs to track all objects for lag compensation
			if (this.trackObjects.indexOf(inst) === -1)		// only add if not already tracked
				this.trackObjects.push(inst);
		}
		
		this.instToPeer[inst.uid] = peer;
		this.peerToInst[peer["id"]] = inst;
	};
	
	Acts.prototype.SetClientState = function (tag, x)
	{
		if (!isSupported)
			return;
		
		this.mp["setClientState"](tag, x);
	};
	
	Acts.prototype.AddClientInputValue = function (tag, precision, interpolation)
	{
		if (!isSupported)
			return;
		
		this.mp["addClientInputValue"](tag, precision, interpolation);
	};
	
	Acts.prototype.InputPredictObject = function (obj)
	{
		if (!isSupported || !obj)
			return;
		
		if (this.mp["isHost"]())
			return;		// host mustn't input predict anything
		
		var inst = obj.getFirstPicked();
		
		if (!inst)
			return;
		
		if (this.inputPredictObjects.indexOf(inst) >= 0)
			return;		// already input predicted
		
		// Mark as input predicting - platform behavior uses this to turn off some anti-jamming code
		inst.extra["inputPredicted"] = true;
		
		this.trackObjects.push(inst);
		this.inputPredictObjects.push(inst);
	};
	
	Acts.prototype.SignallingRequestGameInstanceList = function (game_)
	{
		if (!isSupported)
			return;
		
		this.mp["signallingRequestGameInstanceList"](game_);
	};
	
	Acts.prototype.SignallingRequestRoomList = function (game_, instance_, which_)
	{
		if (!isSupported)
			return;
		
		var whichstr = "all";
		
		if (which_ === 1)
			whichstr = "unlocked";
		else if (which_ === 2)
			whichstr = "available";
		
		this.mp["signallingRequestRoomList"](game_, instance_, whichstr);
	};
	
	Acts.prototype.SetBandwidthProfile = function (profile)
	{
		if (!isSupported)
			return;
		
		var updateRate, delay;
		
		if (profile === 0)		// Internet
		{
			updateRate = 30;	// 30 Hz updates
			delay = 80;			// 80 ms buffer
		}
		else
		{
			updateRate = 60;	// 60 Hz updates
			delay = 40;			// 40ms buffer
		}
		
		this.mp["setBandwidthSettings"](updateRate, delay);
	};
	
	Acts.prototype.KickPeer = function (peerid_, reason_)
	{
		if (!isSupported)
			return;
		
		if (!this.mp["isHost"]())
			return;
		
		if (peerid_ === this.mp["getMyID"]())
			return;		// can't kick self
		
		var peer = this.mp["getPeerById"](peerid_);
		
		if (!peer)
			return;
		
		peer["remove"](reason_);
	};
	
	pluginProto.acts = new Acts();
	
	//////////////////////////////////////
	// Expressions
	function Exps() {};
	
	Exps.prototype.ServerListCount = function (ret)
	{
		ret.set_int(serverlist.length);
	};
	
	Exps.prototype.ServerListURLAt = function (ret, i)
	{
		i = Math.floor(i);
		
		if (i < 0 || i >= serverlist.length)
			ret.set_string("");
		else
			ret.set_string(serverlist[i]["url"]);
	};
	
	Exps.prototype.ServerListNameAt = function (ret, i)
	{
		i = Math.floor(i);
		
		if (i < 0 || i >= serverlist.length)
			ret.set_string("");
		else
			ret.set_string(serverlist[i]["name"]);
	};
	
	Exps.prototype.ServerListOperatorAt = function (ret, i)
	{
		i = Math.floor(i);
		
		if (i < 0 || i >= serverlist.length)
			ret.set_string("");
		else
			ret.set_string(serverlist[i]["operator"]);
	};
	
	Exps.prototype.ServerListWebsiteAt = function (ret, i)
	{
		i = Math.floor(i);
		
		if (i < 0 || i >= serverlist.length)
			ret.set_string("");
		else
			ret.set_string(serverlist[i]["operator_website"]);
	};
	
	Exps.prototype.SignallingURL = function (ret)
	{
		ret.set_string(this.signallingUrl);
	};
	
	Exps.prototype.SignallingVersion = function (ret)
	{
		ret.set_string(isSupported ? this.mp["sigserv_version"] : "");
	};
	
	Exps.prototype.SignallingName = function (ret)
	{
		ret.set_string(isSupported ? this.mp["sigserv_name"] : "");
	};
	
	Exps.prototype.SignallingOperator = function (ret)
	{
		ret.set_string(isSupported ? this.mp["sigserv_operator"] : "");
	};
	
	Exps.prototype.SignallingMOTD = function (ret)
	{
		ret.set_string(isSupported ? this.mp["sigserv_motd"] : "");
	};
	
	Exps.prototype.MyAlias = function (ret)
	{
		ret.set_string(isSupported ? this.mp["getMyAlias"]() : "");
	};
	
	Exps.prototype.CurrentGame = function (ret)
	{
		ret.set_string(isSupported ? this.mp["getCurrentGame"]() : "");
	};
	
	Exps.prototype.CurrentInstance = function (ret)
	{
		ret.set_string(isSupported ? this.mp["getCurrentGameInstance"]() : "");
	};
	
	Exps.prototype.CurrentRoom = function (ret)
	{
		ret.set_string(isSupported ? this.mp["getCurrentRoom"]() : "");
	};
	
	Exps.prototype.ErrorMessage = function (ret)
	{
		ret.set_string(this.errorMsg);
	};
	
	Exps.prototype.MyID = function (ret)
	{
		ret.set_string(isSupported ? this.mp["getMyID"]() : "");
	};
	
	Exps.prototype.PeerID = function (ret)
	{
		ret.set_string(this.peerID);
	};
	
	Exps.prototype.PeerAlias = function (ret)
	{
		ret.set_string(this.peerAlias);
	};
	
	Exps.prototype.HostID = function (ret)
	{
		ret.set_string(isSupported ? this.mp["getHostID"]() : "");
	};
	
	Exps.prototype.HostAlias = function (ret)
	{
		ret.set_string(isSupported ? this.mp["getHostAlias"]() : "");
	};
	
	Exps.prototype.Message = function (ret)
	{
		ret.set_string(this.msgContent);
	};
	
	Exps.prototype.Tag = function (ret)
	{
		ret.set_string(this.msgTag);
	};
	
	Exps.prototype.FromID = function (ret)
	{
		ret.set_string(this.msgFromId);
	};
	
	Exps.prototype.FromAlias = function (ret)
	{
		ret.set_string(this.msgFromAlias);
	};
	
	Exps.prototype.PeerAliasFromID = function (ret, id_)
	{
		ret.set_string(isSupported ? this.mp["getAliasFromId"](id_) : "");
	};
	
	Exps.prototype.PeerLatency = function (ret, id_)
	{
		if (!isSupported)
		{
			ret.set_float(0);
			return;
		}
		
		var peer = this.mp["getPeerById"](id_);
		ret.set_float(peer ? peer["latency"] : 0);
	};
	
	Exps.prototype.PeerPDV = function (ret, id_)
	{
		if (!isSupported)
		{
			ret.set_float(0);
			return;
		}
		
		var peer = this.mp["getPeerById"](id_);
		ret.set_float(peer ? peer["pdv"] : 0);
	};
	
	Exps.prototype.StatOutboundCount = function (ret)
	{
		ret.set_int(isSupported ? this.mp["stats"]["outboundPerSec"] : 0);
	};
	
	Exps.prototype.StatOutboundBandwidth = function (ret)
	{
		ret.set_int(isSupported ? this.mp["stats"]["outboundBandwidthPerSec"] : 0);
	};
	
	Exps.prototype.StatInboundCount = function (ret)
	{
		ret.set_int(isSupported ? this.mp["stats"]["inboundPerSec"] : 0);
	};
	
	Exps.prototype.StatInboundBandwidth = function (ret)
	{
		ret.set_int(isSupported ? this.mp["stats"]["inboundBandwidthPerSec"] : 0);
	};
	
	Exps.prototype.PeerState = function (ret, id_, tag_)
	{
		if (!isSupported)
		{
			ret.set_float(0);
			return;
		}
		
		var peer = this.mp["getPeerById"](id_);
		ret.set_float(peer ? peer["getInterpClientState"](tag_) : 0);
	};
	
	Exps.prototype.ClientXError = function (ret)
	{
		ret.set_float(clientXerror);
	};
	
	Exps.prototype.ClientYError = function (ret)
	{
		ret.set_float(clientYerror);
	};
	
	Exps.prototype.HostX = function (ret)
	{
		ret.set_float(hostX);
	};
	
	Exps.prototype.HostY = function (ret)
	{
		ret.set_float(hostY);
	};
	
	Exps.prototype.PeerCount = function (ret)
	{
		ret.set_int(isSupported ? this.mp["getPeerCount"]() : 0);
	};
	
	Exps.prototype.ListInstanceCount = function (ret)
	{
		if (this.gameInstanceList)
		{
			ret.set_int(this.gameInstanceList.length);
		}
		else
			ret.set_int(0);
	};
	
	Exps.prototype.ListInstanceName = function (ret, index)
	{
		index = Math.floor(index);
		
		if (this.gameInstanceList)
		{
			if (index < 0 || index >= this.gameInstanceList.length)
			{
				ret.set_string("");
			}
			else
				ret.set_string(this.gameInstanceList[index]["name"] || "");
		}
		else
			ret.set_string("");
	};
	
	Exps.prototype.ListInstancePeerCount = function (ret, index)
	{
		index = Math.floor(index);
		
		if (this.gameInstanceList)
		{
			if (index < 0 || index >= this.gameInstanceList.length)
			{
				ret.set_int(0);
			}
			else
				ret.set_int(this.gameInstanceList[index]["peercount"] || 0);
		}
		else
			ret.set_int(0);
	};
	
	Exps.prototype.ListRoomCount = function (ret)
	{
		ret.set_int(this.roomList ? this.roomList.length : 0);
	};
	
	function getListRoomAt(roomList, i)
	{
		if (!roomList)
			return null;
		
		i = Math.floor(i);
		
		if (i < 0 || i >= roomList.length)
			return null;
		
		return roomList[i];
	};
	
	Exps.prototype.ListRoomName = function (ret, index)
	{
		var r = getListRoomAt(this.roomList, index);
		
		ret.set_string(r ? r["name"] : "");
	};
	
	Exps.prototype.ListRoomPeerCount = function (ret, index)
	{
		var r = getListRoomAt(this.roomList, index);
		
		ret.set_int(r ? r["peercount"] : 0);
	};
	
	Exps.prototype.ListRoomMaxPeerCount = function (ret, index)
	{
		var r = getListRoomAt(this.roomList, index);
		
		ret.set_int(r ? r["maxpeercount"] : 0);
	};
	
	Exps.prototype.ListRoomState = function (ret, index)
	{
		var r = getListRoomAt(this.roomList, index);
		
		ret.set_string(r ? r["state"] : "");
	};
	
	Exps.prototype.LeaveReason = function (ret)
	{
		ret.set_string(this.leaveReason);
	};
	
	instanceProto.lagCompensate = function (movingPeerID, fromPeerID, tag, interp)
	{
		if (!isSupported)
			return 0;
		
		// Peers who are not host should not implement lag compensation
		if (!this.mp["isHost"]())
			return 0;
		
		// Look up the peer to move
		var movePeer = this.mp["getPeerById"](movingPeerID);
		
		if (!movePeer)
			return 0;
		
		// Get the object for this peer
		var moveInst = this.getAssociatedInstanceForPeer(movePeer);
		
		if (!moveInst)
			return 0;
		
		// Default the return value to the object's current values
		var ret = 0;
		
		switch (tag) {
		case "x":	ret = moveInst.x;		break;
		case "y":	ret = moveInst.y;		break;
		case "a":	ret = moveInst.angle;	break;
		}
		
		// Determine latency of the from-peer (the peer whose point of view we are using)
		var fromPeer = this.mp["getPeerById"](fromPeerID);
		
		if (!fromPeer || fromPeer === movePeer)		// cannot find that peer or is same peer
			return ret;
		
		// Host sees authoritative game; does not need to lag compensate
		if (this.mp["me"] === fromPeer)
			return ret;
		
		// The total delay is the measured peer latency plus the client delay
		// that they artificially add as well, multiplied by two. This is because the data travels
		// both ways:
		// Host sends position of a peer to player (+latency), peer displays that on delay (+clientDelay)
		// Peer sends input back to host (+latency), host displays that on delay (+clientDelay)
		var delay = (fromPeer["latency"] + this.mp["clientDelay"]) * 2;
		
		var moveUid = moveInst.uid;
		
		if (!this.objectHistories.hasOwnProperty(moveUid))
			return ret;		// no object history data yet
		
		var oh = this.objectHistories[moveUid];
		
		// return data from the past according to 'delay'
		// getInterp can return undefined; return 'ret' in that case
		var interp = oh.getInterp(tag, 0, cr.performance_now() - delay, interp);
		
		if (typeof interp === "undefined")
			return ret;
		else
			return interp;
	};
	
	Exps.prototype.LagCompensateX = function (ret, movingPeerID, fromPeerID)
	{
		ret.set_float(this.lagCompensate(movingPeerID, fromPeerID, "x", this.mp["INTERP_LINEAR"]));
	};
	
	Exps.prototype.LagCompensateY = function (ret, movingPeerID, fromPeerID)
	{
		ret.set_float(this.lagCompensate(movingPeerID, fromPeerID, "y", this.mp["INTERP_LINEAR"]));
	};
	
	Exps.prototype.LagCompensateAngle = function (ret, movingPeerID, fromPeerID)
	{
		ret.set_float(cr.to_degrees(this.lagCompensate(movingPeerID, fromPeerID, "a", this.mp["INTERP_ANGULAR"])));
	};
	
	Exps.prototype.PeerIDAt = function (ret, i)
	{
		if (!isSupported)
		{
			ret.set_string("");
			return;
		}
		
		i = Math.floor(i);
		
		var peer = this.mp["getPeerAt"](i);
		ret.set_string(peer ? peer["id"] : "");
	};
	
	Exps.prototype.PeerAliasAt = function (ret, i)
	{
		if (!isSupported)
		{
			ret.set_string("");
			return;
		}
		
		i = Math.floor(i);
		
		var peer = this.mp["getPeerAt"](i);
		ret.set_string(peer ? peer["alias"] : "");
	};
	
	pluginProto.exps = new Exps();

}());