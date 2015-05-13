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
#ifndef  ORION_SPRITEOBJECT_H
#define ORION_SPRITEOBJECT_H

#include <QGraphicsObject>
#include <QGraphicsSimpleTextItem>
#include <QFont>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "editordefine.h"
#include "spritedefine.h"
using namespace ORION;


class SpriteObject : public QGraphicsObject{
	Q_OBJECT
public:
	// 实体类型
	ORION_TYPE_DECLARE(SPRITE_OBJECT);

	SpriteObject(double depth = 0, QGraphicsItem * parent = 0);
	virtual ~SpriteObject();

	// 相对场景的中心
	virtual QPointF sceneCenter( void );

	//  获取标题名
	virtual QString title(){
		return QString();
	}

	// 类型标示
	virtual QString typeName(){
		return QString();
	}

	//  重载函数
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

	static QFont nameFont();
	static QFont textFont();

protected:
	virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
	virtual QVariant itemChange ( GraphicsItemChange change, const QVariant & value );
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );	

protected:
	QRectF boundingBox_;

};


#endif
