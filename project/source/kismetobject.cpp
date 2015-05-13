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

#include "kismetobject.h"
#include <QtGui>
#include <QUuid>
#include <math.h>
#include <kismetview.h>
#include "kismetscene.h"
#include "kismetlink.h"
#include "kismetgroup.h"
#include "commondefine.h"


BlueprintObject::BlueprintObject(double depth, QGraphicsItem * parent) :
QGraphicsItem(parent), evtgroup_(NULL), objectflags_(0),isError_(false){
	iid_ = makeUniqueId();

	boundingBox_.setRect( -75, -50, 150, 100 );

	// 设置深度值
	setZValue( depth );

	// 设置标志
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	// 接受鼠标经过事件
	setAcceptHoverEvents( true );

	// 不接受拖放
	setAcceptDrops( false );
	setVisible( true );

}

BlueprintObject::~BlueprintObject(){

}

//  获取缩略图（128,128）
QImage BlueprintObject::thumbnail(){
	return QImage( UDQ_T(":/images/defaultseq.png"));
}

QRectF BlueprintObject::boundingRect() const{
	return boundingBox_;
}

QPainterPath BlueprintObject::shape() const{
	QPainterPath path;
	path.addRect( boundingBox_.adjusted(-4,-4,4,4) );
	return path;
}

void BlueprintObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	Q_UNUSED(widget);

	// 绘制矩形
	painter->drawRect( boundingBox_.toRect() );
}

void BlueprintObject::addLink(BlueprintLink *line){
	lineList_ << line;
}

// 删除连线
void BlueprintObject::removeLink(BlueprintLink *line){
	lineList_.removeAll( line );
}

// 清除连线
void BlueprintObject::clearLink(){

	QList<BlueprintLink *> listcopy = lineList_;
	foreach(BlueprintLink *line, listcopy){
		line->unlink();
		line->clearAnchor();
		scene()->removeItem( line );
		delete line;
	}

	Q_ASSERT( lineList_.isEmpty() );
}

QVariant BlueprintObject::itemChange(GraphicsItemChange change, const QVariant & value){

	if( change == ItemPositionChange && scene() ){

		QRectF sRect =  scene()->sceneRect();
		QPointF ltpos = mapToScene(boundingBox_.topLeft() + (value.toPointF() - pos()) );
		QSizeF size = boundingRect().size();

		// 调整位置在范围内
		if (ltpos.rx() <= 0){
			ltpos.setX(0);
		}
		else if (ltpos.rx() >= sRect.width() - size.width()){
			ltpos.setX(sRect.width() - size.width());
		}

		if (ltpos.ry() <= 0){
			ltpos.setY(0);
		}
		else if (ltpos.ry() >= sRect.height() - size.height()){
			ltpos.setY(sRect.height() - size.height());
		}
		
		return ltpos;
	}
	else if( change == ItemPositionHasChanged && scene() ){
		// 更新组
		if (type() != KISMET_GROUP){
			KismetView* view = (KismetView*)scene()->views().at(0);
			BlueprintGroup* newGroup = NULL;
			bool selfGroup = false;

			QList<QGraphicsItem *>  hititems = scene()->items(value.toPointF(), Qt::ContainsItemBoundingRect, Qt::DescendingOrder, view->transform());
			foreach(QGraphicsItem* item, hititems){
				if (item->type() == KISMET_GROUP){
					if (newGroup == NULL){
						newGroup = (BlueprintGroup*)item;
					}

					if (item == evtgroup_){
						selfGroup = true;
					}
				}
			}

			if ( !selfGroup && newGroup != evtgroup_){  // 如果还是原来组则不处理
				addGroup(newGroup);
			}

		}


		foreach(BlueprintLink *line, lineList_){
			line->adjust();
		}
	}
	else if (change == QGraphicsItem::ItemSelectedHasChanged && scene()){
		if (isSelected()){
			backupState();
		}
	}
	
	return QGraphicsItem::itemChange( change, value );
}

// 相对场景的中心
QPointF BlueprintObject::sceneCenter(void){
	QPointF point = boundingRect().center();
	return mapToScene( point );
}

void BlueprintObject::hoverEnterEvent(QGraphicsSceneHoverEvent * event){
	setCursor( Qt::ArrowCursor );
	QGraphicsItem::hoverEnterEvent( event );

}

void BlueprintObject::hoverLeaveEvent(QGraphicsSceneHoverEvent * event){
	QGraphicsItem::hoverLeaveEvent( event );
}

void BlueprintObject::mousePressEvent(QGraphicsSceneMouseEvent * event){
	if( event->button() == Qt::RightButton ){
		scene()->clearSelection();
		// 选择当前实体
		setSelected( true );
	}
	else if (event->button() == Qt::LeftButton){
		if (isSelected()){
			backupState();
		}
	}
	QGraphicsItem::mousePressEvent( event );
}

void BlueprintObject::addGroup(BlueprintGroup* grp){
	if (grp == evtgroup_)
		return;

	if (evtgroup_){
		evtgroup_->removeSubObject(this);
	}

	evtgroup_ = grp;
	if (evtgroup_){
		evtgroup_->addSubObject(this);
	}

}

// 输出编码
QByteArray BlueprintObject::encode(){
	QByteArray arr;
	{
		QDataStream stream(&arr, QIODevice::WriteOnly);
		encodeData(stream);
	}
	return arr;
}

void BlueprintObject::decode(QByteArray& data){
	QDataStream stream(data);
	decodeData(stream);
}

// 编码
void BlueprintObject::encodeData(QDataStream& stream){
	stream << type();
	unsigned int ver =  KISMET_DATA_VERSON;
	stream << ver;
	stream << iid_;
	stream << objectflags_;
	stream << pos();

}

void BlueprintObject::decodeData(QDataStream& stream){
	int t;
	stream >> t;
	Q_ASSERT(t == type());

	stream >> version_;

	stream >> iid_;
	stream >> objectflags_;

	QPointF pos;
	stream >> pos;
	setPos(pos);
}

ResourceHash BlueprintObject::makeUniqueId(){
	QString uid = QUuid::createUuid().toString();
	return hashString(uid);
}

bool BlueprintObject::canSimpleDraw(){
	QPolygonF poly = mapToScene(boundingBox_).boundingRect();
	QRect scrBox = scene()->views().at(0)->mapFromScene(poly).boundingRect();
	return  (boundingBox_.width() / scrBox.width()) >= 3.0;  // 小于2倍则简化绘制
}

void BlueprintObject::copyState(QDataStream& stream , int type ){
	if (type == BST_MOVE){
		stream << oldPos_ << oldGroup_;
	}
	else if (type == BST_RESIZE){
		stream << oldSize_;
	}
}

bool BlueprintObject::isStateChanged(int type){
	if (type == BST_MOVE){
		ResourceHash curGrp = eventGroup() ? eventGroup()->iid() : 0;
		QPointF curPos = pos();
		return (oldPos_ != curPos) || (oldGroup_ != curGrp);
	}
	else if (type == BST_RESIZE){
		QSizeF curSize = boundingBox_.size();
		return (curSize != oldSize_);
	}
	return false;
}

void BlueprintObject::backupState(){
	oldPos_ = pos();
	oldGroup_ = evtgroup_ ? evtgroup_->iid() : 0;
	oldSize_ = boundingBox_.size();

	// 通知父对象
	BlueprintObject* parent = (BlueprintObject*)parentItem();
	if (parent){
		parent->backupState();
	}
}

//////////////////////////////////////////////////////////////////////////

QFont BlueprintObject::nameFont(){
	QFont font;
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPixelSize(12);
	font.setFamily(UDQ_L8("微软雅黑"));
	return font;
}


QFont BlueprintObject::textFont(){
	QFont font;
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPixelSize(10);
	font.setFamily(UDQ_L8("微软雅黑"));
	return font;
}



