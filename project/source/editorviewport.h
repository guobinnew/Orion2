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

#ifndef ORION_EDITORVIEWPORT_H
#define ORION_EDITORVIEWPORT_H

#include <QGraphicsItem>
#include <QFont>
#include <QList>

#include "leveldefine.h"

using namespace ORION;

class EditorViewportText : public QGraphicsSimpleTextItem{

public:
	ORION_TYPE_DECLARE(LEVEL_VIEWPORT_TEXT);
	EditorViewportText(const QString& text, QPointF pos, QGraphicsItem *parent = 0);

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
};

class EditorViewport : public QGraphicsItem{
public:
	// 实体类型
	ORION_TYPE_DECLARE(LEVEL_VIEWPORT)

	EditorViewport( const QString& device = QString(), qreal zoom = 1.0f, qreal rot = 0.0f, QGraphicsItem * parent = 0);
	virtual ~EditorViewport();

	//  重载函数
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

	bool isMinimized(){
		return minimized_;
	}

	// 进入最小化状态
	void setMinimized(bool enable);

protected:
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
	QRectF  boundingBox_;  // 像素坐标
	QImage image_;
	EditorViewportText* text_;  // 文本

	QPointF oldPos_;

	QString device_;  // 设备名
	QSize baseSize_;  // 基础大小
	qreal zoom_; // 缩放比例
	qreal rotation_;  // 旋转角度

	bool minimized_;  // 最小化状态

	// 获取当前字符串
	QString currentTitleString();

	// 调整文本位置
	void adjustText();

};

#endif // ORION_EDITORCANVAS_H
