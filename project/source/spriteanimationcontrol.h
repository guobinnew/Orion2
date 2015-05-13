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

#ifndef  ORION_SPRITEANIMATIONCONTROL_H
#define ORION_SPRITEANIMATIONCONTROL_H

#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QFont>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "editordefine.h"
#include "spritedefine.h"
#include "animationcontrolwindow.h"


#include "resourceobject.h"
using namespace ORION;


class SpriteAnimationControl : public QGraphicsProxyWidget{
public:
	// 实体类型
	ORION_TYPE_DECLARE(SPRITE_ANIMATINCONTROL);

	SpriteAnimationControl(QGraphicsItem * parent = 0);
	virtual ~SpriteAnimationControl();
	//  重载函数
	QRectF boundingRect() const;
	void paintWindowFrame(QPainter *painter, const QStyleOptionGraphicsItem *option,
		QWidget *widget);

	// 绑定
	void bind(SpriteSequence* seq = NULL);

protected:
	void	hoverEnterEvent(QGraphicsSceneHoverEvent * event);
	void	hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
};

typedef boost::shared_ptr<SpriteAnimationControl> SpriteAnimationControlPtr;

#endif
