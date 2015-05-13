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

#ifndef ORION_EDITOR_DEFINE_H
#define ORION_EDITOR_DEFINE_H

#include "commondefine.h"
#include <QGraphicsItem>

namespace ORION{

#define EDITOR_IMAGE_FORMAT  "PNG"
// 实体列表图标大小
#define EDITOR_ICON_WIDTH  32
#define EDITOR_ICON_HEIGHT 32

//  锚点位置掩码
	enum{
		ANCHOR_LT = 0,
		ANCHOR_RT,
		ANCHOR_LB,
		ANCHOR_RB,
		ANCHOR_LM,
		ANCHOR_RM,
		ANCHOR_TM,
		ANCHOR_BM,
		ANCHOR_CE,   // 中心
		ANCHOR_RO,   // 旋转
		ANCHOR_NUM
	};

enum{
	ANCHORFLAG_NONE = 0,

	ANCHORFLAG_SCALERESIZE_LT = 1,
	ANCHORFLAG_SCALERESIZE_RT = 2,
	ANCHORFLAG_SCALERESIZE_LB = 4,
	ANCHORFLAG_SCALERESIZE_RB = 8,

	ANCHORFLAG_EDGERESIZE_L = 16,
	ANCHORFLAG_EDGERESIZE_R = 32,
	ANCHORFLAG_EDGERESIZE_T = 64,
	ANCHORFLAG_EDGERESIZE_B = 128,

	ANCHORFLAG_EDGERESIZE_CE = 256,
	ANCHORFLAG_EDGERESIZE_RO = 512,

	ANCHORFLAG_RESIZE_R = ANCHORFLAG_EDGERESIZE_R | ANCHORFLAG_EDGERESIZE_B | ANCHORFLAG_SCALERESIZE_RB,
	ANCHORFLAG_SCALERESIZE = ANCHORFLAG_SCALERESIZE_LT | ANCHORFLAG_SCALERESIZE_RT | ANCHORFLAG_SCALERESIZE_LB | ANCHORFLAG_SCALERESIZE_RB,
	ANCHORFLAG_EDGERESIZE = ANCHORFLAG_EDGERESIZE_L | ANCHORFLAG_EDGERESIZE_R | ANCHORFLAG_EDGERESIZE_T | ANCHORFLAG_EDGERESIZE_B,
	ANCHORFLAG_RESIZE = ANCHORFLAG_SCALERESIZE | ANCHORFLAG_EDGERESIZE,
	ANCHORFLAG_ROTATE = ANCHORFLAG_EDGERESIZE_RO,
	ANCHORFLAG_RESIZE_ALL = ANCHORFLAG_EDGERESIZE_CE | ANCHORFLAG_RESIZE | ANCHORFLAG_ROTATE,
};

enum{
	EDITOR_UNKNOWN = QGraphicsItem::UserType,

	EDITOR_MACDOCKPANEL,
	EDITOR_LOGO, 

	EDITOR_ITEM,
	EDITOR_ANCHOR,
	EDITOR_REGION,

	EDITOR_LEVEL_BEGIN = QGraphicsItem::UserType + 100,
	EDITOR_SPRITE_BEGIN= QGraphicsItem::UserType + 200,
	EDITOR_BLUEPRINT_BEGIN = QGraphicsItem::UserType + 300,

};


#define ORION_TYPE_DECLARE( T )\
    enum { Type = T };\
    int type() const { return Type ;}\

//////////////////////////////////////////////////////////////////////////

const int EDITOR_DEPTH_MACPANEL = 2000;  // MACPANEL
const int EDITOR_DEPTH_LOGO = 2000;  // LOGO

const int EDITOR_DEPTH_ANCHOR = 1800;  //
const int EDITOR_DEPTH_COMMENT = 1500;   // 注释说明( 非文本块 )

const int EDITOR_DEPTH_DEFAULT = 100;  // 初始深度
const int EDITOR_DEPTH_FRONT = 1000;   // 最前层深度值

const int EDITOR_DEPTH_BACK = 10;   // 最低层深度值

const int EDITOR_DEPTH_ITEM = 0;  //
//////////////////////////////////////////////////////////////////////////
//
const int ITEM_MINW = 8;
const int ITEM_MINH = 8;

const QColor BORDER_COLOR = QColor(255, 50, 50, 255);
const QColor ANCHOR_COLOR = QColor(0, 122, 182, 255);

#define MIME_ORION_IMAGE UDQ_T("orion/image")
#define MIME_ORION_GALLERY UDQ_T("orion/gallery")

}

#endif
