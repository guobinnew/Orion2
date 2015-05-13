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

#ifndef  ORION_LEVEL_DEFINE_H
#define ORION_LEVEL_DEFINE_H

#include "commondefine.h"
#include "editordefine.h"


namespace ORION{


#define VIEWPORT_FILLCOLOR   QColor(255, 255, 50, 100)
#define VIEWPORT_MINI_FILLCOLOR   QColor(200, 200, 200, 200)

#define IMAGE_FILLCOLOR   QColor(255, 255, 0, 125)    // 缺省图层填充色
#define LEVEL_FILLCOLOR   QColor(255, 255, 255, 255)    // 缺省图层填充色
#define LEVELMARGIN_FILLCOLOR   QColor(255, 0, 255, 50)    // 缺省图层填充色

#define SPRITE_FILLCOLOR   QColor(50, 50, 255, 100)    // 缺省精灵填充色


enum{
	LEVEL_UNKNOWN = EDITOR_LEVEL_BEGIN,

	LEVEL_PROFILE,   // 场景Profile
	LEVEL_PROFILE_TEXT,   // 场景Profile

	LEVEL_VIEWPORT,  // 视口
	LEVEL_VIEWPORT_TEXT,  // 视口文字

	LEVEL_LAYER,

	LEVEL_IMAGE,  // 图片
	LEVEL_TEXT,
	LEVEL_COMMENT,
	LEVEL_SPRITE,
	LEVEL_TILEBG,
	LEVEL_TILEMAP,
	LEVEL_9PATCH,
	LEVEL_SPRITEFONT,
	LEVEL_SHADOWLIGHT,
	LEVEL_PARTICLE,

	LEVEL_TILEBRUSH,
	LEVEL_OBJECT,

	LEVEL_MAP,   // 地图
	LEVEL_INPUT,   // 输入控件
	LEVEL_INPUT_BUTTON,   // 按钮控件
	LEVEL_INPUT_TEXTBOX,   // 文本框控件
	LEVEL_INPUT_LIST,   // 列表控件
	LEVEL_INPUT_FILECHOOSER,   // 文件控件
	LEVEL_INPUT_PROGRESSBAR,   // 进度条控件
	LEVEL_INPUT_SLIDERBAR,   // 滑动条控件

};


//////////////////////////////////////////////////////////////////////////

const int LEVEL_DEPTH_PROFILE = 20000;  // Profile
const int LEVEL_DEPTH_VIEWPORT = 18000;   // 视点


////////////////////////////////////////////////////////////////////////////
// 版本
//////////////////////////////////////////////////////////////////////////

#define LEVEL_DATA_VERSON_BASIC                                     1100        // 基础版本
#define LEVEL_DATA_ADD_IID                                                 1101        // 加入IID
#define LEVEL_DATA_ADD_TEXTFLAG                                      1102       // 加入文本的绘制标志

#define LEVEL_DATA_VERSON        LEVEL_DATA_ADD_TEXTFLAG 

//////////////////////////////////////////////////////////////////////////


#define MIME_ORION_LAYER UDQ_T("orion/layer")
#define MIME_ORION_LEVEL UDQ_T("orion/level")

#define MIME_ORION_OBJECTTYPE UDQ_T("orion/objecttype")
#define MIME_ORION_FAMILY UDQ_T("orion/family")

#define MIME_ORION_VARIABLE UDQ_T("orion/variable")

}

#endif
