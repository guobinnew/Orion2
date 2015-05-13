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

#include <QtGui>
#include <QGraphicsView>
#include <QApplication>
#include "macdockpanelitem.h"
#include <math.h>
#include "commondefine.h"


MacDockPanelItem::MacDockPanelItem(  const QSize& size, const QColor& color , double depth,  QGraphicsItem * parent  ):QGraphicsItem(parent), panelColor_(color){

	boundingBox_.setRect(0, 0, size.width(), size.height() );

	// 设置深度值
	setZValue( depth );

	// 设置标志
	setFlag(QGraphicsItem::ItemIsMovable, false);
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

	// 接受鼠标经过事件
    setAcceptHoverEvents( true );

	// 不接受拖放
	setAcceptDrops( false );
	setVisible( false );

	macPanel_.setTextUpper( true );

}

MacDockPanelItem::~MacDockPanelItem(){
}

void MacDockPanelItem::setSize(int width, int height  ){
	boundingBox_.setRect( 0, 0, width, height );
}


QRectF MacDockPanelItem::boundingRect() const{
	return boundingBox_;
}

QPainterPath MacDockPanelItem::shape() const{
	QPainterPath path;
	path.addRect( boundingBox_ );
	return path;
}

void MacDockPanelItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	Q_UNUSED(widget);

	QRectF newRect = boundingBox_;
	newRect.setTop( newRect.bottom() - MAC_PANEL_WIDTH );
	macPanel_.setCenterPoint( newRect.center() );
	// 绘制MacDock条
	macPanel_.drawPanel( painter, newRect, panelColor_ );
}

void MacDockPanelItem::hoverEnterEvent ( QGraphicsSceneHoverEvent* event ){
}

void MacDockPanelItem::hoverLeaveEvent ( QGraphicsSceneHoverEvent* event ){
	macPanel_.setHoverIndex( -1 );
	macPanel_.setHoverFlag( false );
	update();
}

void MacDockPanelItem::hoverMoveEvent ( QGraphicsSceneHoverEvent* event ){
	macPanel_.setLastMousePos( event->pos()  );
	macPanel_.setHoverFlag( true );
	macPanel_.mouseMoveAt( event->pos() );
	update();
}

void MacDockPanelItem::mousePressEvent ( QGraphicsSceneMouseEvent* event ){

	if (event->button() == Qt::LeftButton){
		// 判断是否点击功能按钮
		if (macPanel_.mousePressAt(event->pos())){
			update();
		}
		else{
			dragStartPosition_ = event->pos();
		}
	}
}

void MacDockPanelItem::mouseMoveEvent ( QGraphicsSceneMouseEvent* event ){
	if ( !(event->buttons() & Qt::LeftButton) )
		return;

    if ((event->pos() - dragStartPosition_).manhattanLength() < QApplication::startDragDistance())
		return;

	// 检查是否选择到了ICON
	if( macPanel_.hoverFlag() && macPanel_.hoverIndex() >= 0 ){

		QGraphicsView::DragMode oldMode = scene()->views().at(0)->dragMode();
		scene()->views().at(0)->setDragMode( QGraphicsView::NoDrag );
		DOCKICONITEM* item = macPanel_.getIcon( macPanel_.hoverIndex() );
		QByteArray itemData;
		QDataStream dataStream(&itemData, QIODevice::WriteOnly);
		dataStream << macPanel_.hoverIndex();
		QMimeData *mimeData = new QMimeData;
		mimeData->setData(dropType_, itemData);
		QDrag *drag = new QDrag(scene()->views().at(0));
		drag->setMimeData(mimeData);
		drag->setPixmap( QPixmap::fromImage( item->image ).scaledToWidth(48, Qt::SmoothTransformation) );
		drag->setHotSpot( QPoint(drag->pixmap().width()/2, drag->pixmap().height()/2) );

		macPanel_.setHoverIndex( -1 );
		macPanel_.setHoverFlag( false );
		update();
		drag->exec( Qt::MoveAction);
		scene()->views().at(0)->setDragMode( oldMode );
	}
	else{
		macPanel_.mousePressAt( event->pos() );
		update();
	}
}

