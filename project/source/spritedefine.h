/************************************************************************
**
**  Copyright (C) 2015  guobin  <guobin.gb@alibaba-inc.com>
**
**  This file is part of Orion2.
**
**  Orion2 is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation.
**
**  Orion is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#ifndef ORION_SPRITE_DEFINE_H
#define ORION_SPRITE_DEFINE_H

#include "commondefine.h"
#include "editordefine.h"

namespace ORION{

	enum{
		SPRITE_UNKNOWN = EDITOR_SPRITE_BEGIN,

		SPRITE_OBJECT,
		SPRITE_SEQUENCE,
		SPRITE_SEQUENCEFRAME,

		SPRITE_ANIMATINCONTROL,


		SPRITE_IMAGE,
		SPRITE_ANCHOR,
		SPRITE_COLLISION,
		SPRITE_COLLISION_JOINT,
	};

	const int STRUCTURE_SIZE = 512;
	const int STRUCTURE_MAXSIZE= 2048;

	const int SPRITE_MINW = 150;
	const int SPRITE_MINH = 100;

	const int SEQUENCE_MINW = 300;
	const int SEQUENCE_MINH = 200;
	const int SEQUENCE_TITLEHEIGHT = 24;
	const int SEQUENCE_IMAGESIZE = 64;

	const int SEQUENCE_MARGIN = 16;

	const int SEQUENCEFRAME_MINW = 64;
	const int SEQUENCEFRAME_MINH = 64;


	const int ANCHOR_WIDTH = 32;
	const int ANCHOR_CUSTEM = 255;

	const int DEPTH_SPRTIE_IMAGE = 0;
	const int DEPTH_SPRTIE_COLLISION = 50;
	const int DEPTH_SPRTIE_COLLISIONJOINT = 60;
	const int DEPTH_SPRTIE_ANCHOR = 100;

	////////////////////////////////////////////////////////////////////////////
	// °æ±¾
	//////////////////////////////////////////////////////////////////////////

#define SPRITE_DATA_VERSON_BASIC                2100        // »ù´¡°æ±¾

#define SPRITE_DATA_VERSON        SPRITE_DATA_VERSON_BASIC 

	//////////////////////////////////////////////////////////////////////////

#define MIME_ORION_TILE UDQ_T("orion/tile")
#define MIME_ORION_FRAME UDQ_T("orion/frame")

}

#endif
