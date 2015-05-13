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

#include "spriteanimationcontrol.h"
#include <QtGui>
#include <QGraphicsSceneHoverEvent>
#include <QApplication>
#include <math.h>
#include "spritescene.h"
#include "commondefine.h"
#include "imagehelper.h"
#include "spritesequence.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

SpriteAnimationControl::SpriteAnimationControl( QGraphicsItem * parent) :QGraphicsProxyWidget(parent, Qt::Widget){

	setWidget(new AnimationControlWindow);

	// 设置深度值
	setZValue(200);

	// 设置标志
	setFlag(QGraphicsItem::ItemIsMovable, false);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

	// 接受鼠标经过事件
	setAcceptHoverEvents(true);

	// 不接受拖放
	setAcceptDrops(false);
	setVisible(false);

}

SpriteAnimationControl::~SpriteAnimationControl(){
}

QRectF SpriteAnimationControl::boundingRect() const{
	return QGraphicsProxyWidget::boundingRect();
}

void SpriteAnimationControl::paintWindowFrame(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	QGraphicsProxyWidget::paintWindowFrame(painter, option, widget);
}


void SpriteAnimationControl::hoverEnterEvent(QGraphicsSceneHoverEvent *event){
	setCursor(Qt::ArrowCursor);
	QGraphicsProxyWidget::hoverEnterEvent(event);
	scene()->setActiveWindow(this);
}

void SpriteAnimationControl::hoverLeaveEvent(QGraphicsSceneHoverEvent *event){
	QGraphicsProxyWidget::hoverLeaveEvent(event);
}

void SpriteAnimationControl::bind(SpriteSequence* seq ){
	
	if (parentItem() != seq){
		setParentItem(seq);
	}

	((AnimationControlWindow*)widget())->setHost(seq);
}
