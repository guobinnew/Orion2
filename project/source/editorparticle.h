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
#ifndef  ORION_EDITORPARTICLE_H
#define ORION_EDITORPARTICLE_H

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

class EditorParticle : public EditorRegion{
public:
	// 实体类型
	ORION_TYPE_DECLARE(LEVEL_PARTICLE)
	EditorParticle(ObjectTypeResourceObject* prototype, QGraphicsItem * parent = 0);
	virtual ~EditorParticle();

	// 绘制触发区域形状
	virtual void drawShape(QPainter *painter);
	// 刷新
	void postinit(bool resize = true);

	QImage tileImage_;  // tile图像
	qreal cone_;  // 锥角
protected:
	virtual void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
	virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
	virtual void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
	virtual void dropEvent(QGraphicsSceneDragDropEvent * event);

};


#endif // ORION_EDITORIMAGE_H
