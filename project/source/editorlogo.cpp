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

#include "editorlogo.h"
#include <QtGui>
#include <math.h>
#include "commondefine.h"
#include "editordefine.h"

EditorLogo::EditorLogo( const QString& img,  double depth,  QGraphicsItem * parent  ):QGraphicsItem(parent){
	logoImage_ = QImage( img ).scaledToWidth(128);
	boundingBox_.setRect( 0, 0, logoImage_.width(), logoImage_.height() );
	
	// 设置深度值
	setZValue( depth );

	// 设置标志
	setFlag(QGraphicsItem::ItemIsMovable, false);
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

	// 接受鼠标经过事件
	setAcceptHoverEvents(false);
	// 不接受拖放
	setAcceptDrops( false );

	setVisible( true );

}

EditorLogo::~EditorLogo(){
}


QRectF EditorLogo::boundingRect() const{
	return boundingBox_;
}

QPainterPath EditorLogo::shape() const{
	QPainterPath path;
	path.addRect( boundingBox_ );
	return path;
}

void EditorLogo::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	Q_UNUSED(widget);
	painter->drawImage( boundingBox_, logoImage_ );

}

