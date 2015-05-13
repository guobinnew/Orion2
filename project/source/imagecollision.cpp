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

#include "imagecollision.h"
#include <QtGui>
#include <QGraphicsSceneMouseEvent>
#include <math.h>
#include "imagescene.h"
#include "commondefine.h"
#include "configmanager.h"


ImageCollision::ImageCollision(const QList<QPointF>& pts, QGraphicsItem * parent){
	fillColor_ = QColor(255, 0, 255, 50);
	editing_ = false;
	// 设置标志
	setFlag(ItemIsMovable, true);
	setFlag(ItemIsSelectable, false);
	setFlag(ItemIsFocusable, true);
	setFlag(ItemSendsGeometryChanges, true);

	// 接受鼠标经过事件
	setAcceptHoverEvents(true);

	// 不接受拖放
	setAcceptDrops(false);

	initPolygon(pts);
	initJoint();
	adjust();
}

ImageCollision::~ImageCollision(){
	clear();
}

//  重载函数
void ImageCollision::clear(){
	clearJoint();
}

// 生成初始四边形
void ImageCollision::initPolygon(const QList<QPointF>& pts){
	// 按方位角排序（顺时针）
	points_ = pts;
	updatePath();
}


void ImageCollision::showJoint(bool visible){
	initJoint();
	foreach(ImageCollisionJoint* joint, joints_){
		joint->setVisible(visible);
		if (visible){
			scene()->addItem(joint);
		}
		else{
			scene()->removeItem(joint);
		}
	}
	adjust();
}


void ImageCollision::clearJoint(){

	foreach(ImageCollisionJoint* joint, joints_){
		delete joint;
	}
	joints_.clear();
}

void  ImageCollision::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget){
	// 填充
	painter->fillPath(path(), fillColor_);

	QPen borderPen;
	// 绘制 边框颜色
	borderPen.setColor(Qt::red);
	borderPen.setStyle(Qt::DashDotLine);
	borderPen.setWidth(0);
	painter->setPen(borderPen);

	// 绘制矩形
	painter->drawPath(path());
}

QVariant ImageCollision::itemChange(GraphicsItemChange change, const QVariant & value){
	static QPointF oldPos;
	// 位置变化时，调整控制点位置
	if (change == QGraphicsItem::ItemPositionChange && scene()){
		oldPos = pos();
	}
	else if (change == QGraphicsItem::ItemPositionHasChanged && scene()){
		if (!editing_){
			// 移动点位置
			QPointF delta = pos() - oldPos;
			for (int i = 0; i < points_.size(); i++){
				points_[i] += delta;
			}
			initJoint();
			adjust();
		}
	}
	return QGraphicsItem::itemChange(change, value);
}

void ImageCollision::initJoint(){

	if (joints_.size() == 0){
		int index = 0;
		foreach(QPointF pt, points_){
			ImageCollisionJoint *joint = new ImageCollisionJoint(this, index++);
			joints_.append(joint);
			joint->ready_ = true;
		}
	}
	else{

		while (joints_.size() > points_.size()){
			ImageCollisionJoint* joint = joints_.last();
			joints_.pop_back();
			delete joint;
		}

		while (joints_.size() < points_.size()){
			ImageCollisionJoint *joint = new ImageCollisionJoint(this, joints_.size());
			joints_.append(joint);
			joint->ready_ = true;
		}

	}

}


// 更新控制点
QPointF ImageCollision::updateJoint(ImageCollisionJoint* joint){
	// 修改控制点数据
	points_.replace(joint->index_, joint->scenePos());
	updatePath();
	update();
	return joint->scenePos();
}


// 角度排序
bool jointSort(const QVector3D &p1, const QVector3D &p2){
	return p1.z() < p2.z();
}

void ImageCollision::adjust(){
	//  更新anchor位置
	for (int i = 0; i < joints_.size(); i++) {
		joints_[i]->changePos(points_.at(joints_[i]->index_));  // 放入场景中
	}

}


void ImageCollision::removeJoint(ImageCollisionJoint* joint){

	if (points_.size() <= 3)  // 不能少于3个点
		return;

	int index = joint->index_;
	points_.removeAt(joint->index_);

	if (joints_.size() > index){
		joints_.removeAt(index);
		//  更新后续节点索引
		for (int i = index; i < joints_.size(); i++){
			joints_[i]->index_ = i;
		}
	}

	delete joint;
	updatePath();
	adjust();
	update();
}

bool pointOnSegment(const QLineF& line, const QPointF& pt){

	qreal  a = QLineF(pt, line.p1()).length();
	qreal  b = QLineF(pt, line.p2()).length();

	if (qAbs((a + b) - line.length()) < 4){
		return true;
	}
	return false;
}

void ImageCollision::mousePressEvent(QGraphicsSceneMouseEvent * event){

	// 如果（ctrl+左键）点击在多边形边上，在那条边的中点添加一个新锚点
	if ((event->modifiers() & Qt::ControlModifier) && event->button() == Qt::LeftButton){
		bool valid = true;

		// 碰撞检测
		for (int i = 0; i < points_.size(); i++){
			// 如果是点到顶点，则不处理
			QVector2D vec(event->scenePos() - points_.at(i));
			if (vec.length() < 4){
				valid = false;
				break;
			}
		}

		if (valid){

			// 碰撞检测
			for (int i = 0; i < points_.size(); i++){
				int end = i < points_.size() - 1 ? i + 1 : 0;
				QLineF line(points_.at(i), points_.at(end));
				if (pointOnSegment(line, event->scenePos())){  // 像素距离
					// 则认为点击到直线上, 直线中点上添加新锚点
					QPointF newPoint = line.pointAt(0.5);
					if (end != 0){
						points_.insert(end, newPoint);
					}
					else{
						points_.append(newPoint);
					}

					showJoint(true);
					break;
				}
			}
		}
	}

	QGraphicsItem::mousePressEvent(event);
}

// 更新路径
void ImageCollision::updatePath(){
	editing_ = true;

	QPainterPath path;
	bool start = true;
	QPointF begin = points_[0];
	foreach(QPointF pt, points_){

		if (start){
			path.moveTo(QPointF(0,0));
			start = false;
		}
		path.lineTo(pt - begin);
	}
	path.closeSubpath();
	setPath(path);
	setPos(begin);
	editing_ = false;
}


void ImageCollision::hoverEnterEvent(QGraphicsSceneHoverEvent * event){
	setCursor(Qt::ArrowCursor);
	QGraphicsItem::hoverEnterEvent(event);
}

void ImageCollision::hoverLeaveEvent(QGraphicsSceneHoverEvent * event){

	QGraphicsItem::hoverLeaveEvent(event);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

ImageCollisionJoint::ImageCollisionJoint(ImageCollision* host, int index, QGraphicsItem* parent)
	: QGraphicsItem(parent), host_(host), index_(index)
{

	setZValue(DEPTH_SPRTIE_COLLISIONJOINT);
	setFlags(ItemIsSelectable | ItemIsMovable | ItemIsFocusable);
	setFlag(ItemSendsGeometryChanges, true);
	setFlag(ItemIgnoresTransformations, true);
	setAcceptHoverEvents(true);

	boundingBox_ = QRect(-4, -4, 8, 8);
	underMouse_ = false;
	ready_ = false;
}

QRectF ImageCollisionJoint::boundingRect() const{
	return boundingBox_;
}

QPainterPath ImageCollisionJoint::shape() const{
	QPainterPath path;
	path.addRect(boundingBox_);
	return path;
}

ImageCollisionJoint::~ImageCollisionJoint(){

}

void ImageCollisionJoint::hoverEnterEvent(QGraphicsSceneHoverEvent * event){
	underMouse_ = true;
	setCursor(Qt::ArrowCursor);
	QGraphicsItem::hoverEnterEvent(event);
	update();
}

void ImageCollisionJoint::hoverLeaveEvent(QGraphicsSceneHoverEvent * event){
	underMouse_ = false;
	QGraphicsItem::hoverLeaveEvent(event);
	update();
}

void ImageCollisionJoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget){

	QPen borderPen;
	if (isSelected() || underMouse_){
		// 绘制选择框颜色
		painter->fillRect(boundingBox_, Qt::yellow);
	}
	else{
		painter->fillRect(boundingBox_, Qt::blue);
	}

	painter->setPen(Qt::white);
	// 填充色
	painter->drawRect(boundingBox_);
}

QVariant ImageCollisionJoint::itemChange(GraphicsItemChange change, const QVariant & value){

	if (change == ItemPositionHasChanged && scene()){
		// 如果位置发生改变，修改轨迹
		Q_ASSERT(host_ != NULL);
		if (ready_){
			QPointF newPos = host_->updateJoint(this);
			return newPos;
		}
	}

	return QGraphicsItem::itemChange(change, value);
}

void ImageCollisionJoint::changePos(const QPointF& newPos){
	ready_ = false;
	setPos(newPos);
	ready_ = true;
}

void ImageCollisionJoint::keyReleaseEvent(QKeyEvent * event){

	if (isSelected() && isPressDelete(event->key())){
		// 删除控制点	
		host_->removeJoint(this);
		return;
	}

	QGraphicsItem::keyPressEvent(event);
}