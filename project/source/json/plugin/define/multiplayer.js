function GetPluginSettings()
{
	return {
		"name":			"Multiplayer",			// as appears in 'insert object' dialog, can be changed as long as "id" stays the same
		"id":			"Multiplayer",			// this is used to identify this plugin and is saved to the project; never change it
		"version":		"1.0",					// (float in x.y format) Plugin version - C2 shows compatibility warnings based on this
		"description":	"Create real-time multiplayer online games using WebRTC DataChannels.",
		"author":		"Scirra",
		"help url":		"http://www.scirra.com/manual/174/multiplayer",
		"category":		"Web",				// Prefer to re-use existing categories, but you can set anything here
		"type":			"object",				// either "world" (appears in layout and is drawn), else "object"
		"rotatable":	false,					// only used when "type" is "world".  Enables an angle property on the object.
		"dependency":	"c2mp-net.js;c2mp-peer.js;c2mp.js;waker.js",
		"flags":		0						// uncomment lines to enable flags...
						| pf_singleglobal		// exists project-wide, e.g. mouse, keyboard.  "type" must be "object".
					//	| pf_texture			// object has a single texture (e.g. tiled background)
					//	| pf_position_aces		// compare/set/get x, y...
					//	| pf_size_aces			// compare/set/get width, height...
					//	| pf_angle_aces			// compare/set/get angle (recommended that "rotatable" be set to true)
					//	| pf_appearance_aces	// compare/set/get visible, opacity...
					//	| pf_tiling				// adjusts image editor features to better suit tiled images (e.g. tiled background)
					//	| pf_animations			// enables the animations system.  See 'Sprite' for usage
					//	| pf_zorder_aces		// move to top, bottom, layer...
					//  | pf_nosize				// prevent resizing in the editor
					//	| pf_effects			// allow WebGL shader effects to be added
					//  | pf_predraw			// set for any plugin which draws and is not a sprite (i.e. does not simply draw
												// a single non-tiling image the size of the object) - required for effects to work properly
	};
};

////////////////////////////////////////
// Parameter types:
// AddNumberParam(label, description [, initial_string = "0"])			// a number
// AddStringParam(label, description [, initial_string = "\"\""])		// a string
// AddAnyTypeParam(label, description [, initial_string = "0"])			// accepts either a number or string
// AddCmpParam(label, description)										// combo with equal, not equal, less, etc.
// AddComboParamOption(text)											// (repeat before "AddComboParam" to add combo items)
// AddComboParam(label, description [, initial_selection = 0])			// a dropdown list parameter
// AddObjectParam(label, description)									// a button to click and pick an object type
// AddLayerParam(label, description)									// accepts either a layer number or name (string)
// AddLayoutParam(label, description)									// a dropdown list with all project layouts
// AddKeybParam(label, description)										// a button to click and press a key (returns a VK)
// AddAnimationParam(label, description)								// a string intended to specify an animation name
// AddAudioFileParam(label, description)								// a dropdown list with all imported project audio files

////////////////////////////////////////
// Conditions

// AddCondition(id,					// any positive integer to uniquely identify this condition
//				flags,				// (see docs) cf_none, cf_trigger, cf_fake_trigger, cf_static, cf_not_invertible,
//									// cf_deprecated, cf_incompatible_with_triggers, cf_looping
//				list_name,			// appears in event wizard list
//				category,			// category in event wizard list
//				display_str,		// as appears in event sheet - use {0}, {1} for parameters and also <b></b>, <i></i>
//				description,		// appears in event wizard dialog when selected
//				script_name);		// corresponding runtime function name
						
AddCondition(0, cf_trigger | cf_deprecated, "On server list received", "Signalling", "On server list received", "Triggered after 'Request server list' successfully completes.", "OnServerList");

AddCondition(1, cf_trigger, "On error", "Signalling", "On signalling error", "Triggered if an error occurs with the server list or signalling server.", "OnSignallingError");

AddCondition(2, cf_trigger, "On connected", "Signalling", "On signalling connected", "Triggered when successfully connected to the signalling server.", "OnSignallingConnected");

AddCondition(3, cf_trigger, "On disconnected", "Signalling", "On signalling disconnected", "Triggered when disconnected from the signalling server.", "OnSignallingDisconnected");

AddCondition(4, cf_trigger, "On logged in", "Signalling", "On signalling logged in", "Triggered when successfully logged in to the signalling server.", "OnSignallingLoggedIn");

AddCondition(5, cf_trigger, "On joined room", "Signalling", "On signalling joined room", "Triggered upon successfully joining a room.", "OnSignallingJoinedRoom");

AddCondition(6, cf_trigger, "On left room", "Signalling", "On signalling left room", "Triggered upon successfully leaving a room.", "OnSignallingLeftRoom");

AddCondition(7, cf_none, "Is connected", "Signalling", "Is connected to signalling server", "True if currently connected to a signalling server.", "SignallingIsConnected");

AddCondition(8, cf_none, "Is logged in", "Signalling", "Is logged in to signalling server", "True if currently logged in to a signalling server.", "SignallingIsLoggedIn");

AddCondition(9, cf_none, "Is in room", "Signalling", "Is in room", "True if currently in a room on the a signalling server.", "SignallingIsInRoom");

AddCondition(10, cf_none, "Is host", "Room", "Is host", "True if host of the current room.", "IsHost");

AddCondition(11, cf_trigger, "On peer connected", "Room", "On peer connected", "Triggered when connected to a peer (either to host or a peer joining the host).", "OnPeerConnected");

AddCondition(12, cf_none, "Supports multiplayer", "Signalling", "Supports multiplayer", "True if the current browser supports multiplayer games.", "IsSupported");

AddCondition(13, cf_trigger, "On peer disconnected", "Room", "On peer disconnected", "Triggered when disconnected from a peer (either from host or a peer leaving the host).", "OnPeerDisconnected");

AddCondition(14, cf_trigger, "On kicked", "Room", "On kicked", "Triggered when kicked from a room.", "OnSignallingKicked");

AddStringParam("Tag", "The tag to identify the message type.");
AddCondition(15, cf_trigger, "On peer message", "Room", "On peer message <i>{0}</i>", "Triggered when received a message with a specific tag from a peer.", "OnPeerMessage");

AddCondition(16, cf_trigger, "On any peer message", "Room", "On any peer message", "Triggered when received any message from a peer.", "OnAnyPeerMessage");

AddCondition(17, cf_trigger, "On client update", "Room: Peer", "On client update", "Triggered when sending the new client state to the host.", "OnClientUpdate");

AddCondition(18, cf_trigger, "On game instance list", "Signalling", "On game instance list received", "Triggered after 'Request game instance list' when the list has been received from the server.", "OnGameInstanceList");

AddCondition(19, cf_none, "Is ready for input", "Room: Peer", "Is ready for input", "True when ready to send input to the host. Don't move the local player until this is true.", "IsReadyForInput");

AddCmpParam("Comparison", "How to compare the peer count.");
AddNumberParam("Count", "Number of peers to compare to.");
AddCondition(20, cf_none, "Compare peer count", "Room", "Peer count {0} {1}", "Compare the number of peers currently in the room.", "ComparePeerCount");

AddCondition(21, cf_trigger, "On room list", "Signalling", "On room list received", "Triggered after 'Request room list' when the list has been received from the server.", "OnRoomList");

////////////////////////////////////////
// Actions

// AddAction(id,				// any positive integer to uniquely identify this action
//			 flags,				// (see docs) af_none, af_deprecated
//			 list_name,			// appears in event wizard list
//			 category,			// category in event wizard list
//			 display_str,		// as appears in event sheet - use {0}, {1} for parameters and also <b></b>, <i></i>
//			 description,		// appears in event wizard dialog when selected
//			 script_name);		// corresponding runtime function name

AddStringParam("List URL", "A URL of a server list to retrieve.", "\"http://www.scirra.com/multiplayer/serverlist.json\"");
AddAction(0, af_none | af_deprecated, "Request server list", "Signalling", "Request server list from <b>{0}</b>", "Request a list of signalling servers that can be joined.", "RequestServerList");

AddStringParam("Server", "The signalling server URL to connect to.", "\"wss://multiplayer.scirra.com\"");
AddAction(1, af_none, "Connect", "Signalling", "Connect to signalling server <b>{0}</b>", "Connect to a signalling server to be able to join rooms.", "SignallingConnect");

AddAction(2, af_none, "Disconnect", "Signalling", "Disconnect from signalling server", "Disconnect from the current signalling server.", "SignallingDisconnect");

AddStringParam("ICE server", "A custom ICE server, e.g. STUN or TURN, to help establish connectivity.", "\"stun:\"");
AddStringParam("Username (optional)", "If a TURN server, then the username to use with that server.");
AddStringParam("Credential (optional)", "If a TURN server, then the credential to use with that server.");
AddAction(3, af_none, "Add ICE server", "Signalling", "Add ICE server <b>{0}</b> (username <i>{1}</i>, credential <i>{2}</i>", "Add another ICE server to help establish connectivity. Several defaults are built-in.", "AddICEServer");

AddStringParam("Alias", "The desired alias to use on the server. The server may assign you a different alias if it is taken.");
AddAction(4, af_none, "Log in", "Signalling", "Log in with alias <b>{0}</b>", "Once connected, log in to the signalling server with an alias.", "SignallingLogin");

AddStringParam("Game", "A string uniquely identifying this game on the server. To help ensure uniqueness, include you or your company's name, e.g. \"MyStudio-Asteroids\" instead of just \"Asteroids\".", "\"MyCompany-MyGame\"");
AddStringParam("Instance", "A string identifying an instance of the game, in case you wish to run separate instances.", "\"default\"");
AddStringParam("Room", "The name of the room to request joining.");
AddNumberParam("Max peers", "The maximum number of peers that can join this room. Only the host's value is used. Leave 0 for unlimited.");
AddAction(5, af_none, "Join room", "Signalling", "Join room <b>{2}</b> for game <i>{0}</i> instance <i>{1}</i> (max peers: <i>{3}</i>)", "Once logged in, join a room to meet other peers.", "SignallingJoinRoom");

AddAction(6, af_none, "Leave room", "Signalling", "Leave room", "Request to leave the current room on the signalling server. Peer connections are not affected.", "SignallingLeaveRoom");

AddAction(7, af_none, "Disconnect", "Room", "Disconnect from room", "Disconnect from peers in the current room, and also leave the room on the signalling server.", "DisconnectRoom");

AddStringParam("Peer ID", "The ID of the peer to send the message to. Leave empty to send to host.");
AddStringParam("Tag", "A tag to identify this kind of message.");
AddStringParam("Message", "The message data to send.");
AddComboParamOption("Reliable ordered");
AddComboParamOption("Reliable unordered");
AddComboParamOption("Unreliable");
AddComboParam("Mode", "The transmission mode to send this message with.");
AddAction(8, af_none, "Send message", "Room", "Send to peer <i>{0}</i> tag <i>{1}</i> message <b>{2}</b> (mode <i>{3}</i>)", "Send a message to a specific peer.", "SendPeerMessage");

AddStringParam("From ID", "Peer ID message originates from, if relaying a peer message. Leave blank to indicate from host.");
AddStringParam("Tag", "A tag to identify this kind of message.");
AddStringParam("Message", "The message data to send.");
AddComboParamOption("Reliable ordered");
AddComboParamOption("Reliable unordered");
AddComboParamOption("Unreliable");
AddComboParam("Mode", "The transmission mode to send this message with.");
AddAction(9, af_none, "Broadcast message", "Room: host", "Broadcast tag <i>{1}</i> message <b>{2}</b> (from <i>{0}</i>, mode <i>{3}</i>)", "Send a message from the host to every peer in the room.", "HostBroadcastMessage");

AddNumberParam("Latency", "Enter a latency in seconds to simulate. Sent and received messages will be artificially delayed.", "0.1");
AddNumberParam("PDV", "Enter a packet delay variance in seconds to simulate. Sent and received messages will be artificially delayed.", "0.03");
AddNumberParam("Packet loss", "Enter a packet loss percentage to simulate, from 0 to 1 (e.g. 0.05 for 5%). Sent and received messages will be artificially ignored or further delayed.", "0.05");
AddAction(10, af_none, "Simulate latency", "Room", "Simulate latency <i>{0}</i> seconds with variance <i>{1}</i> and packet loss <i>{2}</i>", "Simulate a delay and packet loss in sending/receiving messages. Useful for testing lag performance locally.", "SimulateLatency");

AddObjectParam("Object", "An object type to sync over the network.");
AddComboParamOption("None");
AddComboParamOption("Position only");
AddComboParamOption("Angle only");
AddComboParamOption("Position and angle");
AddComboParam("Data", "Choose whether to sync the position and/or angle of the object.");
AddComboParamOption("High (double, 8 bytes)");
AddComboParamOption("Normal (float, 4 bytes)");
AddComboParamOption("Low (int16, 2 bytes)");
AddComboParam("Precision", "Choose the precision of the above data to sync. Higher precision requires more bandwidth.", 1);
AddComboParamOption("Normal bandwidth (unpredictable)");
AddComboParamOption("Low bandwidth (highly predictable)");
AddComboParamOption("Very low bandwidth (essentially static)");
AddComboParam("Bandwidth", "Choose the bandwidth setting for this object. Only choose 'Low' for highly predictable movement (e.g. linear constant speed).");
AddAction(11, af_none, "Sync object", "Room", "Sync {0} (with <b>{1}</b>, precision <i>{2}</i> at <i>{3}</i>)", "Automatically synchronise an object type over the network.", "SyncObject");

AddObjectParam("Object", "An object type already being synced. The chosen instance variable will also be synced.");
AddObjectInstanceVarParam("Instance variable", "Choose the instance variable in the above object to sync.");
AddComboParamOption("High (double, 8 bytes)");
AddComboParamOption("Normal (float, 4 bytes)");
AddComboParamOption("Low (int16, 2 bytes)");
AddComboParamOption("Very low (uint8, 1 byte)");
AddComboParam("Precision", "Choose the precision of the value to sync.", 1);
AddComboParamOption("None");
AddComboParamOption("Linear");
AddComboParamOption("Angular");
AddComboParam("Interpolation", "Choose the interpolation for clients to use between received values.");
AddStringParam("Client value tag", "If this value corresponds to a client input value, the tag for the value. This allows the host to sync the value between peers with less latency.");
AddAction(12, af_none, "Sync instance variable", "Room", "Sync {0} variable <b>{1}</b> (precision <i>{2}</i>, interpolation: <i>{3}</i>, client value tag: <i>{4}</i>)", "Add a synced object's instance variable to also sync over the network.", "SyncObjectInstanceVar");

AddObjectParam("Object", "An object to associate with a peer. There must be exactly one of this object per connected peer.");
AddStringParam("Peer ID", "The ID of the peer to associate the chosen object with.");
AddAction(13, af_none, "Associate object with peer", "Room", "Associate {0} with peer <i>{1}</i>", "Indicate an object which represents a connected peer.", "AssociateObjectWithPeer");

AddStringParam("Tag", "A tag to identify the client input value to set. It must have been added previously.");
AddNumberParam("Value", "A number representing the client input state.");
AddAction(14, af_none, "Set client state", "Room: peer", "Set client input state <i>{0}</i> to value <b>{1}</b>", "Set the client input state for a peer to indicate to the host their controls.", "SetClientState");

AddStringParam("Tag", "A tag to identify this client input value.");
AddComboParamOption("High (double, 8 bytes)");
AddComboParamOption("Normal (float, 4 bytes)");
AddComboParamOption("Low (int16, 2 bytes)");
AddComboParamOption("Very low (uint8, 1 byte)");
AddComboParam("Precision", "Choose the precision of the client input value.", 1);
AddComboParamOption("None");
AddComboParamOption("Linear");
AddComboParamOption("Angular");
AddComboParam("Interpolation", "Choose the interpolation for the host to use between values.");
AddAction(15, af_none, "Add client input value", "Room", "Add client input value tag <b>{0}</b>, precision <i>{1}</i>, interpolation <i>{2}</i>", "Add a value to send to the host representing the client input state.", "AddClientInputValue");

AddObjectParam("Object", "An object representing the local player to enable local input prediction for.");
AddAction(16, af_none, "Enable local input prediction", "Room: peer", "Enable local input prediction for {0}", "Enable local input prediction for an object representing the local player.", "InputPredictObject");

AddStringParam("Game", "The game on the server to list instances for.");
AddAction(17, af_none, "Request game instance list", "Signalling", "Request game <b>{0}</b> instance list", "Request the list of game instances currently on the server.", "SignallingRequestGameInstanceList");

AddComboParamOption("Internet (30 Hz updates, 80ms buffer)");
AddComboParamOption("LAN (60 Hz updates, 40ms buffer)");
AddComboParam("Profile", "The bandwidth profile to use. Do not use LAN mode over the Internet. However Internet mode is still suitable for LANs.");
AddAction(18, af_none, "Set bandwidth profile", "Room", "Set bandwidth profile to <b>{0}</b>", "Before joining a room, set Internet or LAN mode to adapt the use of the network.", "SetBandwidthProfile");

AddStringParam("Game", "A string uniquely identifying this game on the server. To help ensure uniqueness, include you or your company's name, e.g. \"MyStudio-Asteroids\" instead of just \"Asteroids\".", "\"MyCompany-MyGame\"");
AddStringParam("Instance", "A string identifying an instance of the game, in case you wish to run separate instances.", "\"default\"");
AddStringParam("First room", "The name of the first room to request joining. If the room is full, subsequent rooms will be checked (\"room\", \"room2\", \"room3\"...).");
AddNumberParam("Max peers", "The number of peers per room. Once full, later peers will be sent to the next room.", "2");
AddComboParamOption("lock when full");
AddComboParamOption("don't lock");
AddComboParam("Locking", "Locking the room prevents other peers joining again even after other peers leave.");
AddAction(19, af_none, "Auto-join room", "Signalling", "Auto-join from room <b>{2}</b> for game <i>{0}</i> instance <i>{1}</i> (max peers: <i>{3}</i>, {4})", "Join the first room which is not full.", "SignallingAutoJoinRoom");

AddStringParam("Game", "The game on the server to list rooms from.");
AddStringParam("Instance", "The game instance on the server to list rooms for.");
AddComboParamOption("all");
AddComboParamOption("unlocked");
AddComboParamOption("available");
AddComboParam("Which", "Optionally filter the returned rooms by their state.");
AddAction(20, af_none, "Request room list", "Signalling", "Request game <b>{0}</b> instance <b>{1}</b> room list (<i>{2}</i>)", "Request the list of rooms in a game instance currently on the server.", "SignallingRequestRoomList");

AddStringParam("Peer ID", "The ID of the peer to forcibly remove. Only works when host.");
AddStringParam("Reason", "A string of the reason to give for kicking (optional).");
AddAction(21, af_none, "Kick peer", "Room: host", "Kick peer <i>{0}</i> (reason: <i>{1}</i>)", "Forcibly remove a peer from the room when host.", "KickPeer");

////////////////////////////////////////
// Expressions

// AddExpression(id,			// any positive integer to uniquely identify this expression
//				 flags,			// (see docs) ef_none, ef_deprecated, ef_return_number, ef_return_string,
//								// ef_return_any, ef_variadic_parameters (one return flag must be specified)
//				 list_name,		// currently ignored, but set as if appeared in event wizard
//				 category,		// category in expressions panel
//				 exp_name,		// the expression name after the dot, e.g. "foo" for "myobject.foo" - also the runtime function name
//				 description);	// description in expressions panel

AddExpression(0, ef_return_number | ef_deprecated, "", "Signalling", "ServerListCount", "The number of servers in a received server list.");

AddNumberParam("Index", "Index of server in list");
AddExpression(1, ef_return_string | ef_deprecated, "", "Signalling", "ServerListURLAt", "The URL of the Nth received signalling server.");

AddNumberParam("Index", "Index of server in list");
AddExpression(2, ef_return_string | ef_deprecated, "", "Signalling", "ServerListNameAt", "The name of the Nth received signalling server.");

AddNumberParam("Index", "Index of server in list");
AddExpression(3, ef_return_string | ef_deprecated, "", "Signalling", "ServerListOperatorAt", "The operator of the Nth received signalling server.");

AddNumberParam("Index", "Index of server in list");
AddExpression(4, ef_return_string | ef_deprecated, "", "Signalling", "ServerListWebsiteAt", "The operator's website for the Nth received signalling server.");

AddExpression(5, ef_return_string, "", "Signalling", "SignallingURL", "The URL of the current signalling server.");
AddExpression(6, ef_return_string, "", "Signalling", "SignallingVersion", "The version of the current signalling server.");
AddExpression(7, ef_return_string, "", "Signalling", "SignallingName", "The name of the current signalling server.");
AddExpression(8, ef_return_string, "", "Signalling", "SignallingOperator", "The operator of the current signalling server.");
AddExpression(9, ef_return_string, "", "Signalling", "SignallingMOTD", "The 'message of the day' for the current signalling server.");

AddExpression(10, ef_return_string, "", "Signalling", "MyAlias", "The alias for the current user.");
AddExpression(11, ef_return_string, "", "Signalling", "CurrentGame", "The current game name joined.");
AddExpression(12, ef_return_string, "", "Signalling", "CurrentInstance", "The current game instance joined.");
AddExpression(13, ef_return_string, "", "Signalling", "CurrentRoom", "The current room joined.");

AddExpression(14, ef_return_string, "", "Signalling", "ErrorMessage", "In 'on signalling error', the error message.");

AddExpression(15, ef_return_string, "", "Signalling", "MyID", "The ID for the current user.");
AddExpression(16, ef_return_string, "", "Room", "PeerID", "The ID of relevant peer in a trigger.");
AddExpression(17, ef_return_string, "", "Room", "PeerAlias", "The alias of relevant peer in a trigger.");
AddExpression(18, ef_return_string, "", "Room", "HostID", "The ID of the room host.");
AddExpression(19, ef_return_string, "", "Room", "HostAlias", "The alias of the room host.");

AddExpression(20, ef_return_string, "", "Room", "Message", "The message received in a message trigger.");
AddExpression(21, ef_return_string, "", "Room", "Tag", "The tag of the message received in a message trigger.");
AddExpression(22, ef_return_string, "", "Room", "FromID", "The ID of the user the message is from in a message trigger.");
AddExpression(23, ef_return_string, "", "Room", "FromAlias", "The alias of the user the message is from in a message trigger.");

AddStringParam("id", "Peer ID");
AddExpression(24, ef_return_string, "", "Room", "PeerAliasFromID", "Get a peer's alias for a given peer ID.");

AddStringParam("id", "Peer ID");
AddExpression(25, ef_return_number, "", "Room", "PeerLatency", "Get the latency (ping time) for a peer.");

AddStringParam("id", "Peer ID");
AddExpression(26, ef_return_number, "", "Room", "PeerPDV", "Get the packet delay variation for a peer.");

AddExpression(27, ef_return_number, "", "Statistics", "StatOutboundCount", "Number of outbound messages per second.");
AddExpression(28, ef_return_number, "", "Statistics", "StatOutboundBandwidth", "Uploaded payload bytes per second.");
AddExpression(29, ef_return_number, "", "Statistics", "StatInboundCount", "Number of inbound messages per second.");
AddExpression(30, ef_return_number, "", "Statistics", "StatInboundBandwidth", "Downloaded payload bytes per second.");

AddStringParam("id", "Peer ID");
AddStringParam("tag", "Client value tag");
AddExpression(31, ef_return_number, "", "Room: Host", "PeerState", "When host, get the client input state of a peer.");

AddExpression(33, ef_return_number, "", "Statistics", "ClientXError", "Client-side input prediction X error from host, for debugging.");
AddExpression(34, ef_return_number, "", "Statistics", "ClientYError", "Client-side input prediction Y error from host, for debugging.");

AddExpression(35, ef_return_number, "", "Statistics", "HostX", "Host X position for debugging input prediction.");
AddExpression(36, ef_return_number, "", "Statistics", "HostY", "Host Y position for debugging input prediction.");

AddExpression(37, ef_return_number, "", "Room", "PeerCount", "The number of connected peers in the current room.");

AddExpression(38, ef_return_number, "", "Listings", "ListInstanceCount", "The number of game instances available.");

AddNumberParam("Index", "Index");
AddExpression(39, ef_return_string, "", "Listings", "ListInstanceName", "The name of a game instance at an index.");

AddNumberParam("Index", "Index");
AddExpression(40, ef_return_number, "", "Listings", "ListInstancePeerCount", "The total number of peers in a game instance at an index.");

AddExpression(41, ef_return_string, "", "Room", "LeaveReason", "In 'On peer disconnected', the leave reason if known.");

AddStringParam("movingPeerID", "Peer ID to move");
AddStringParam("fromPeerID", "Peer ID to compensate for");
AddExpression(42, ef_return_number, "", "Room: Host", "LagCompensateX", "The X position of a moving peer as it appears to another peer.");

AddStringParam("movingPeerID", "Peer ID to move");
AddStringParam("fromPeerID", "Peer ID to compensate for");
AddExpression(43, ef_return_number, "", "Room: Host", "LagCompensateY", "The Y position of a moving peer as it appears to another peer.");

AddStringParam("movingPeerID", "Peer ID to move");
AddStringParam("fromPeerID", "Peer ID to compensate for");
AddExpression(44, ef_return_number, "", "Room: Host", "LagCompensateAngle", "The angle of a moving peer as it appears to another peer.");

AddNumberParam("Index", "Peer index");
AddExpression(45, ef_return_string, "", "Room", "PeerIDAt", "The ID of the nth connected peer in the room.");

AddNumberParam("Index", "Peer index");
AddExpression(46, ef_return_string, "", "Room", "PeerAliasAt", "The alias of the nth connected peer in the room.");

AddExpression(47, ef_return_number, "", "Listings", "ListRoomCount", "The number of rooms returned.");

AddNumberParam("Index", "Index");
AddExpression(48, ef_return_string, "", "Listings", "ListRoomName", "The name of a game instance at an index.");

AddNumberParam("Index", "Index");
AddExpression(49, ef_return_number, "", "Listings", "ListRoomPeerCount", "The total number of peers in a room at an index.");

AddNumberParam("Index", "Index");
AddExpression(50, ef_return_number, "", "Listings", "ListRoomMaxPeerCount", "The maximum number of peers for a room at an index.");

AddNumberParam("Index", "Index");
AddExpression(51, ef_return_string, "", "Listings", "ListRoomState", "The room state at an index (\"available\", \"locked\" or \"full\").");

////////////////////////////////////////
ACESDone();

////////////////////////////////////////
// Array of property grid properties for this plugin
// new cr.Property(ept_integer,		name,	initial_value,	description)		// an integer value
// new cr.Property(ept_float,		name,	initial_value,	description)		// a float value
// new cr.Property(ept_text,		name,	initial_value,	description)		// a string
// new cr.Property(ept_color,		name,	initial_value,	description)		// a color dropdown
// new cr.Property(ept_font,		name,	"Arial,-16", 	description)		// a font with the given face name and size
// new cr.Property(ept_combo,		name,	"Item 1",		description, "Item 1|Item 2|Item 3")	// a dropdown list (initial_value is string of initially selected item)
// new cr.Property(ept_link,		name,	link_text,		description, "firstonly")		// has no associated value; simply calls "OnPropertyChanged" on click

var property_list = [
	//new cr.Property(ept_integer, 	"My property",		77,		"An example property.")
	];
	
// Called by IDE when a new object type is to be created
function CreateIDEObjectType()
{
	return new IDEObjectType();
}

// Class representing an object type in the IDE
function IDEObjectType()
{
	assert2(this instanceof arguments.callee, "Constructor called as a function");
}

// Called by IDE when a new object instance of this type is to be created
IDEObjectType.prototype.CreateInstance = function(instance)
{
	return new IDEInstance(instance);
}

// Class representing an individual instance of an object in the IDE
function IDEInstance(instance, type)
{
	assert2(this instanceof arguments.callee, "Constructor called as a function");
	
	// Save the constructor parameters
	this.instance = instance;
	this.type = type;
	
	// Set the default property values from the property table
	this.properties = {};
	
	for (var i = 0; i < property_list.length; i++)
		this.properties[property_list[i].name] = property_list[i].initial_value;
		
	// Plugin-specific variables
	// this.myValue = 0...
}

// Called when inserted via Insert Object Dialog for the first time
IDEInstance.prototype.OnInserted = function()
{
}

// Called when double clicked in layout
IDEInstance.prototype.OnDoubleClicked = function()
{
}

// Called after a property has been changed in the properties bar
IDEInstance.prototype.OnPropertyChanged = function(property_name)
{
}

// For rendered objects to load fonts or textures
IDEInstance.prototype.OnRendererInit = function(renderer)
{
}

// Called to draw self in the editor if a layout object
IDEInstance.prototype.Draw = function(renderer)
{
}

// For rendered objects to release fonts or textures
IDEInstance.prototype.OnRendererReleased = function(renderer)
{
}