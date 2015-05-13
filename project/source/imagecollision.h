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

#ifndef ORION_IMAGECOLLISION_H
#define ORION_IMAGECOLLISION_H

#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include <QFont>
#include "spritedefine.h"
using namespace ORION;

class ImageCollisionJoint;

// 选中状态下，显示锚点，可选中后删除或移动，锚点根据与中心点的方位顺时针排序
// 按alt，点击鼠标左键可添加新锚点
class ImageCollision : public 	QGraphicsPathItem{
public:
	ORION_TYPE_DECLARE(SPRITE_COLLISION)

	ImageCollision(const QList<QPointF>& pts,  QGraphicsItem * parent = 0);
	virtual ~ImageCollision();

	//  重载函数
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

	// 生成初始四边形
	void initPolygon(const QList<QPointF>& pts );

	// 清理
	virtual void clear();

	// 更新路径
	void updatePath();

	// 修改路径点( 位置为本地坐标 )
	void changePointAt(int index, const QPointF& newPoint);

	QList<QPointF>  points_;  //  坐标
	QList<ImageCollisionJoint*> joints_;  // 锚点列表

	void initJoint();

	QPointF updateJoint(ImageCollisionJoint* joint);
	void removeJoint(ImageCollisionJoint* joint);
	void clearJoint();
	void adjust();
	void showJoint(bool visible);
protected:
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

private:
	QColor fillColor_;
	bool editing_;

};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

class ImageCollisionJoint : public QGraphicsItem{
public:
	// 实体类型
	ORION_TYPE_DECLARE(SPRITE_COLLISION_JOINT)

	ImageCollisionJoint(ImageCollision* host, int index, QGraphicsItem* parent = NULL);
	virtual ~ImageCollisionJoint();

	int  index_;   // 索引
	ImageCollision   *host_;   // 宿主
	bool ready_;  // 是否处于就绪状态

	//  重载函数
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

	// 改变位置，但不发送通知
	void changePos(const QPointF& newPos);

protected:
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);
	void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
	void keyReleaseEvent(QKeyEvent * event);

private:
	QRectF  boundingBox_;     // 像素坐标		
	bool underMouse_; // 是否在鼠标下方

};



#endif // ORION_IMAGEANCHOR_H
