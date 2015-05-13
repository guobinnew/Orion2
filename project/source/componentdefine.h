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

#ifndef ORION_COMPONENT_DEFINE_H
#define ORION_COMPONENT_DEFINE_H

#include "commondefine.h"
#include <QGraphicsItem>

namespace ORION{

// 定义组件ID
#define DEFINE_COMP(x) \
	  const unsigned int COMP_##x = hashStringLower(UDQ_T(#x)); \

	DEFINE_COMP(NINEPATCH);
	DEFINE_COMP(FUNCTION);
	DEFINE_COMP(SPRITE);
	DEFINE_COMP(SPRITEFONT);
	DEFINE_COMP(PARTICLES);
	DEFINE_COMP(TILEDBG);
	DEFINE_COMP(TILEMAP);
	DEFINE_COMP(SHADOWLIGHT);
	DEFINE_COMP(AMAP);
	DEFINE_COMP(BUTTON);
	DEFINE_COMP(TEXTBOX);
	DEFINE_COMP(LIST);
	DEFINE_COMP(PROGRESSBAR);
	DEFINE_COMP(SLIDERBAR);
	DEFINE_COMP(FILECHOOSER);

#undef  DEFINE_COMP

	const unsigned int COMP_TEXT = hashStringLower(UDQ_T("TEXT"));

	const unsigned int DATATYPE_BOOL = hashStringLower(UDQ_T("bool"));
	const unsigned int DATATYPE_INT = hashStringLower(UDQ_T("int"));
	const unsigned int DATATYPE_DOUBLE = hashStringLower(UDQ_T("float"));
	const unsigned int DATATYPE_STRING = hashStringLower(UDQ_T("string"));

#define DEFINE_DATATYPE(x) \
	 const  unsigned int DATATYPE_##x = hashStringLower(UDQ_T(#x)); \

	DEFINE_DATATYPE(ENUM);
	DEFINE_DATATYPE(OBJECT);
	DEFINE_DATATYPE(LAYOUT);
	DEFINE_DATATYPE(LAYER);
	DEFINE_DATATYPE(ANIMATION);

#undef  DEFINE_DATATYPE


#define MIME_ORION_ENTITY UDQ_T("orion/comp_entity")
#define MIME_ORION_BEHAVIOR UDQ_T("orion/comp_behavior")
#define MIME_ORION_EFFECT UDQ_T("orion/comp_effect")

}

#endif
