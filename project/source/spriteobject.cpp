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

#include "spriteobject.h"
#include <QtGui>
#include <QGraphicsSceneHoverEvent>
#include <math.h>
#include "spritescene.h"
#include "commondefine.h"


SpriteObject::SpriteObject(double depth, QGraphicsItem * parent) :
QGraphicsObject(parent){

	boundingBox_.setRect(0, 0, SPRITE_MINW, SPRITE_MINH);

	// 设置深度值
	setZValue( depth );

	// 设置标志
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	// 接受鼠标经过事件
	setAcceptHoverEvents( true );

	setVisible( true );

}

SpriteObject::~SpriteObject(){

}

QRectF SpriteObject::boundingRect() const{
	return boundingBox_;
}

QPainterPath SpriteObject::shape() const{
	QPainterPath path;
	path.addRect( boundingBox_.adjusted(-4,-4,4,4) );
	return path;
}

void SpriteObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	Q_UNUSED(widget);

	// 绘制矩形
	painter->drawRect( boundingBox_.toRect() );
}

QVariant SpriteObject::itemChange(GraphicsItemChange change, const QVariant & value){

	if( change == ItemPositionChange && scene() && parentItem() == NULL ){  // 仅针对根对象

		QRectF sRect =  scene()->sceneRect();
		QPointF pos = value.toPointF();  // 新位置
		QSizeF size = boundingRect().size();

		// 调整位置在范围内
		if (pos.rx() < 0){
			pos.setX(0);
		}
		else if( pos.rx() > sRect.width() - size.width()  ){
			pos.setX(sRect.width() - size.width() );
		}

		if( pos.ry() < 0  ){
			pos.setY( 0 );
		}
		else if( pos.ry() > sRect.height() - size.height() ){
			pos.setY(sRect.height() - size.height()  );
		}
		
		return pos;
	}

	return QGraphicsItem::itemChange( change, value );
}

// 相对场景的中心
QPointF SpriteObject::sceneCenter(void){
	QPointF point = boundingRect().center();
	return mapToScene( point );
}

void SpriteObject::hoverEnterEvent(QGraphicsSceneHoverEvent * event){
	setCursor( Qt::ArrowCursor );
	QGraphicsItem::hoverEnterEvent( event );

}

void SpriteObject::hoverLeaveEvent(QGraphicsSceneHoverEvent * event){
	QGraphicsItem::hoverLeaveEvent( event );
}

void SpriteObject::mousePressEvent(QGraphicsSceneMouseEvent * event){
	QGraphicsItem::mousePressEvent( event );
}

//////////////////////////////////////////////////////////////////////////

QFont SpriteObject::nameFont(){
	QFont font;
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPixelSize(12);
	return font;
}


QFont SpriteObject::textFont(){
	QFont font;
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPixelSize(10);
	return font;
}
