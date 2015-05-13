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

#ifndef ORION_MACDOCKPANEL_ITEM_H
#define ORION_MACDOCKPANEL_ITEM_H

#include <QGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include "macdockpanel.h"

#include "editordefine.h"
using namespace ORION;

class MacDockPanelItem : public QGraphicsItem {
public:
	// 实体类型
	ORION_TYPE_DECLARE(EDITOR_MACDOCKPANEL)

    MacDockPanelItem( const QSize& size, 
	const QColor& color =QColor(125,125,255,150),  
	double depth = EDITOR_DEPTH_MACPANEL,
	QGraphicsItem * parent = 0 );
	virtual ~MacDockPanelItem();

	void setSize(int width, int height = MAC_PANEL_HEIGHT );
	//  重载函数
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
	MacDockPanel& panel(){
		return macPanel_;
	}

	void setDropType( const QString& type ){
		dropType_ = type;
	}

protected:
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent* event );
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent* event );
    virtual void hoverMoveEvent ( QGraphicsSceneHoverEvent* event );
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent* event );
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent* event );

private:
	QColor panelColor_;  // 面板颜色
	QRectF  boundingBox_;     // 像素坐标
	MacDockPanel macPanel_;  // MAC浮动条
	QString dropType_;
	QPointF dragStartPosition_;

};

#endif
