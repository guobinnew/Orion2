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
#ifndef  ORION_EDITORSHADOWLIGHT_H
#define ORION_EDITORSHADOWLIGHT_H

#include <QGraphicsItem>
#include <QFont>
#include <QVector3D>
#include <QMap>
#include <QDataStream>
#include <QByteArray>
#include <QPainterPath>
#include "leveldefine.h"
#include "editorregion.h"
#include "resourceobject.h"
using namespace ORION;

class EditorShadowLight : public EditorRegion{
public:
	// 实体类型
	ORION_TYPE_DECLARE(LEVEL_SHADOWLIGHT)
	EditorShadowLight(ObjectTypeResourceObject* prototype, QGraphicsItem * parent = 0);
	virtual ~EditorShadowLight();

	// 绘制触发区域形状
	virtual void drawShape(QPainter *painter);

private:
	QImage icon_;  // 灯光图片
};


#endif // ORION_EDITORIMAGE_H
