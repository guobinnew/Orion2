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
**  Orion2 is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion2.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/
#include "kismetcollection.h"
#include <QtGui>
#include <math.h>
#include "kismetscene.h"
#include "kismetmodel.h"
#include "kismetlinkarray.h"

const int  COLLPORT_SIZE =  64;
const int  COLLPORT_NAMEW = 120;
const int  COLLPORT_NAMEH = 20;

BlueprintCollection::BlueprintCollection(ResourceHash key) : BlueprintModelPort(EVT_DATA_OUT, UDQ_T("object")){
	setZValue(KISMET_DEPTH_INSTANCE);
	boundingBox_.setRect(0, 0, COLLPORT_SIZE, COLLPORT_SIZE);
	// 设置标志
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

	setFamily(key);
	portcolor_ = FAMILY_COLOR;
}

BlueprintCollection::~BlueprintCollection(){

}

void BlueprintCollection::setFamily(ResourceHash key){
	prototype_ = (FamilyResourceObject*)ResourceObject::findResource(key);
	if (prototype_){
		name_ = prototype_->description();
	}
	else{
		name_.clear();
	}

}

QPointF BlueprintCollection::controlPoint(int orient){

	QPointF point = sceneCenter();
	point.setX(point.x() + boundingBox_.width() /2 + 50 );
	return point;
}

void BlueprintCollection::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget){
	Q_UNUSED(widget);

	painter->setRenderHint( QPainter::Antialiasing, true );
	QPen borderPen;
	if( isSelected() || isUnderMouse() ){
		// 绘制选择框颜色
		borderPen.setColor( Qt::yellow );
	}
	else{
		borderPen.setColor( Qt::black  );
	}
	borderPen.setWidth(1);
	painter->setPen( borderPen );

	QRectF circleR = boundingBox_.adjusted(4,4,-4,-4).normalized();
	QPainterPath path;
	path.addEllipse( circleR );
	painter->fillPath(path, portcolor_);

	painter->drawEllipse(boundingBox_);

	bool simpleDraw = canSimpleDraw();
	if (!simpleDraw){
		painter->setFont(nameFont());
		painter->setPen(Qt::white);
		// 计算绘制位置
		QString typeStr;
		typeStr = UDQ_T("family");
		painter->drawText(computeTextPos(circleR, typeStr, nameFont()), typeStr);

		// 绘制名称
		painter->setPen(borderPen);
		QSize nameSize = computeText(name_, nameFont());
		painter->drawText(boundingBox_.left() + (boundingBox_.width() - nameSize.width()) / 2.0, boundingBox_.bottom() + COLLPORT_NAMEH, name_);
	}

}

void BlueprintCollection::mousePressEvent(QGraphicsSceneMouseEvent * event){
	// 连线状态

	if( isUnderMouse()  && isSelected() ){
		// 如果同时按下ALt
		if( event->modifiers() & Qt::AltModifier ){
			clearLink();
		}
		return;
	}

	QGraphicsItem::mousePressEvent( event );

}


QVariant BlueprintCollection::itemChange(GraphicsItemChange change, const QVariant & value){

	if (change == ItemPositionHasChanged && scene()){
		foreach(BlueprintLink *line, lineList_){
			line->updatePort(this);
		}
		return BlueprintObject::itemChange(change, value);
	}

	return BlueprintModelPort::itemChange(change, value);
}

// 编码
void BlueprintCollection::encodeData(QDataStream& stream){
	BlueprintObject::encodeData(stream);
	stream << prototype_->hashKey();
}

void BlueprintCollection::decodeData(QDataStream& stream){
	BlueprintObject::decodeData(stream);

	ResourceHash key;
	stream >> key;
	setFamily(key);

}

QJsonValue BlueprintCollection::expNode(int dt){
	QJsonArray arr;
	arr.append((int)20);
	arr.append(QString::number(prototype_->hashKey()));  // 导出时需要替换为索引
	return arr;
}

bool BlueprintCollection::validate(QStringList& msg){
	if (!hasLink()){
		// 警告： 没有连接
		msg.append(WARNING_MSG.arg(UDQ_TR("集合<%1>没有连接!").arg(collectionName())));
	}
	return !isError_;
}