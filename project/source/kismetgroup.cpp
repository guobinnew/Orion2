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


#include "kismetgroup.h"
#include <QtGui>
#include <math.h>
#include "kismetview.h"
#include "kismetscene.h"
#include "kismetgroupeditwindow.h"

const int GROUP_CORNER_LAP = 3;
const int GROUP_CORNER_SIZE = 10;
const int GROUP_MINW = 300;
const int GROUP_MINH =200;


BlueprintGroupText::BlueprintGroupText(const QString& text, QPointF pos, QGraphicsItem *parent) : QGraphicsSimpleTextItem(parent){

	setZValue( KISMET_DEPTH_GROUPTEXT );
	setFlags( ItemIgnoresTransformations );
	setText( text );
	setPos( pos );
	setBrush( Qt::black );
	setFont(BlueprintGroup::titleFont());

}

//////////////////////////////////////////////////////////////////////////

BlueprintGroup::BlueprintGroup(const QString& title, QGraphicsItem * parent) :
BlueprintObject(KISMET_DEPTH_GROUP, parent), sizingFlag_(false), fillColor_(Qt::gray), activeOnStart_(true){

	boundingBox_.setRect(0, 0, GROUP_MINW, GROUP_MINH);
	text_ = new BlueprintGroupText(title, QPointF(0, boundingBox_.height() + 2), this);

}

BlueprintGroup::~BlueprintGroup(){
	removeAllSubObject();
}

// 编码
void BlueprintGroup::encodeData(QDataStream& stream){
	BlueprintObject::encodeData(stream);

	stream<<text_->text();
	stream<<boundingBox_.size();
	stream<<fillColor_;

	// 组内列表
	stream << subObjects_.size();
	foreach(BlueprintObject* obj, subObjects_){
		stream << obj->iid();
	}

	stream << activeOnStart_;
}

void BlueprintGroup::decodeData(QDataStream& stream){
	BlueprintObject::decodeData(stream);

	QString text;
	stream >> text;
	text_->setText(text);

	QSizeF size;
	stream >> size;
	resize(size);

	stream >> fillColor_;

	// 设置子对象
	subIds_.clear();
	int num;
	stream >> num;
	ResourceHash iid;
	for (int i = 0; i < num; i++){
		stream >> iid;
		subIds_.append(iid);
	}

	stream >> activeOnStart_;
}

void BlueprintGroup::postInit(){
	// 设置子对象
	removeAllSubObject();
	KismetScene* parent = (KismetScene*)scene();
	ResourceHash iid;
	foreach( ResourceHash iid, subIds_){ 
		BlueprintObject* obj = parent->getObjectItem(iid);
		if (obj != NULL)
			obj->addGroup(this);
	}
}

void BlueprintGroup::resize(const QSizeF& newSize){
	prepareGeometryChange();
	boundingBox_.setSize( newSize );
	text_ ->setPos( QPointF(0, boundingBox_.height() + 2 ));
}

QPainterPath BlueprintGroup::shape() const{
	QPainterPath path;
	path.addRect( boundingBox_ );

	QPainterPathStroker stroker;
	stroker.setWidth( 8 );
	return stroker.createStroke( path );

}

const int INDEX_CIRCLE = 16;
const QColor INDEX_COLOR = QColor(203, 109, 23);

void BlueprintGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	Q_UNUSED(widget);

	QPen borderPen;
	if( isSelected()){
		// 绘制选择框颜色
		borderPen.setColor( SELECT_COLOR );
	}
	else{
		borderPen.setColor( Qt::black  );
	}
	painter->setPen( borderPen );

	// 绘制矩形
	painter->drawRect( boundingBox_.toRect() );

	if( isSelected()){
		painter->drawLine(boundingBox_.right() - GROUP_CORNER_LAP * 3, boundingBox_.bottom(), boundingBox_.right(), boundingBox_.bottom() - GROUP_CORNER_LAP * 3);
		painter->drawLine(boundingBox_.right() - GROUP_CORNER_LAP * 2, boundingBox_.bottom(), boundingBox_.right(), boundingBox_.bottom() - GROUP_CORNER_LAP * 2);
		painter->drawLine(boundingBox_.right() - GROUP_CORNER_LAP, boundingBox_.bottom(), boundingBox_.right(), boundingBox_.bottom() - GROUP_CORNER_LAP);
	}

	// 绘制矩形
	int inter = GROUP_CORNER_LAP * 2;
	QRectF interBox = boundingBox_.adjusted(inter, inter, 0-inter, 0-inter);

	if( fillColor_.alpha() > 0 ){
		QBrush brush( fillColor_ );
		painter->fillRect( interBox.toRect(), brush );
	}

	painter->setPen( Qt::gray );
	painter->drawRect( interBox.toRect() );

	// 绘制次序索引
	QRectF sortRect = QRectF(boundingBox_.left() - INDEX_CIRCLE, boundingBox_.top() - INDEX_CIRCLE, INDEX_CIRCLE * 2, INDEX_CIRCLE * 2);
	QPainterPath path;
	painter->setPen(Qt::black);
	path.addEllipse(sortRect.adjusted(2,2,-2,-2));
	painter->fillPath(path, INDEX_COLOR );
	painter->drawEllipse(sortRect);

	//  写文字
	int i = ((KismetScene*)scene())->groupIndex(this);
	painter->setFont(nameFont());
	painter->drawText(sortRect, Qt::AlignCenter, QString::number(i));
}

void BlueprintGroup::hoverMoveEvent(QGraphicsSceneHoverEvent * event){

	// 如果在右下角
	QRectF cornerRect(boundingBox_.width() - GROUP_CORNER_SIZE,
		boundingBox_.height() - GROUP_CORNER_SIZE,
		GROUP_CORNER_SIZE,
		GROUP_CORNER_SIZE);
	if( cornerRect.contains(event->pos() ) ){
		setCursor(Qt::SizeFDiagCursor);
	}
	else{
		setCursor( Qt::ArrowCursor );
	}

	QGraphicsItem::hoverMoveEvent( event );
}


void BlueprintGroup::mouseMoveEvent(QGraphicsSceneMouseEvent * event){

	// 改变尺寸
	if( sizingFlag_ ){
		// 保障最小尺寸
		prepareGeometryChange ();
		boundingBox_.setBottomRight( event->pos() );
		if (boundingBox_.width() < GROUP_MINW)
			boundingBox_.setWidth(GROUP_MINW);

		if (boundingBox_.height() < GROUP_MINH)
			boundingBox_.setHeight(GROUP_MINH);

		text_ ->setPos( QPointF(0, boundingBox_.height() + 2 ));

		update();
		return;
	}
	QGraphicsItem::mouseMoveEvent( event );

}

void BlueprintGroup::mousePressEvent(QGraphicsSceneMouseEvent * event){
	// 如果在右下角
	QRectF cornerRect(boundingBox_.width() - GROUP_CORNER_SIZE,
		boundingBox_.height() - GROUP_CORNER_SIZE,
		GROUP_CORNER_SIZE,
		GROUP_CORNER_SIZE);

	QPointF t = event->pos();
	if( event->button() == Qt::LeftButton && cornerRect.contains( event->pos() ) ){
		// 进入改变尺寸状态
		sizingFlag_ = true;
	}

	QGraphicsItem::mousePressEvent( event );

}

void BlueprintGroup::mouseReleaseEvent(QGraphicsSceneMouseEvent * event){

	if( event->button() == Qt::LeftButton && sizingFlag_ ){
		sizingFlag_ = false;
		// 更新子对象
		updateSubObjects();
		// 通知改变大小

	}

	QGraphicsItem::mouseReleaseEvent( event );

}

void	BlueprintGroup::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event){

	backupState();

	KismetGroupEditWindow w(this);
	w.exec();
}

void BlueprintGroup::setTitle(const QString& title){
	if( text_)
		text_->setText( title );
}

void BlueprintGroup::updateSubObjects(){

	// 移除不在组范围内的子对象
	QSet<BlueprintObject*> oldchildren = subObjects_;
	subObjects_.clear();

	// 添加新的子对象
	KismetView* view = (KismetView*)scene()->views().at(0);
	BlueprintGroup* newGroup = NULL;
	BlueprintObject* obj = NULL;
	QList<QGraphicsItem *>  hititems = scene()->items(sceneBoundingRect(), Qt::ContainsItemBoundingRect, Qt::DescendingOrder, view->transform());
	foreach(QGraphicsItem* item, hititems){
		if (!(item->type() == KISMET_GROUP ||
			item->type() == KISMET_LINK ||
			item->type() == KISMET_LINK_SINGLE ||
			item->type() == KISMET_LINK_ARRAY ||
			item->type() == KISMET_MODEL_PORT ) && parentItem() == NULL ){

			obj = (BlueprintObject*)item;
			if (obj->eventGroup() == NULL){
				obj->addGroup(this);
			}
			else if( obj->eventGroup() == this ){  // 添加进去原来的
				oldchildren.remove(obj);
				subObjects_.insert(obj);
			}
		}
	}

	foreach(BlueprintObject* obj, oldchildren){  // 需要移除的
		obj->addGroup(NULL);
	}

}


QVariant BlueprintGroup::itemChange(GraphicsItemChange change, const QVariant & value){
	
	if (change == ItemPositionChange && scene()){
		oldPos_ = pos();
	}
	else if (change == ItemPositionHasChanged  && scene()){
		//
		QPointF dist = value.toPointF() - oldPos_;
		// 通知所有端口更新连线
		foreach(BlueprintObject *obj, subObjects_){
			obj->setPos( obj->pos() + dist );
		}
		// 添加新的子对象
		updateSubObjects();
	}
	else if (change == QGraphicsItem::ItemSelectedHasChanged && scene()){
		if (isSelected()){
			subIds_.clear();
			foreach(BlueprintObject* child, subObjects_){
				subIds_.append(child->iid());
			}
		}
	}
	return BlueprintObject::itemChange(change, value);
}

void BlueprintGroup::addSubObject(BlueprintObject* obj){
	if (obj){
		subObjects_.insert(obj);
		obj->addGroup(this);
	}
}

void BlueprintGroup::removeSubObject(BlueprintObject* obj){
	if (obj){
		subObjects_.remove(obj);
	}
}

void BlueprintGroup::removeAllSubObject(){
	foreach(BlueprintObject* obj, subObjects_){
		obj->setEventGroup(NULL);
	}
	subObjects_.clear();
}

void BlueprintGroup::copyState(QDataStream& stream, int type ){
	BlueprintObject::copyState(stream, type);
	if (type == BST_MOVE || type == BST_RESIZE ){
		stream << subIds_;
	}
	else if (type == BST_DISPLAY){
		stream << oldText_ << oldFillColor_ << oldActive_;
	}
}

void BlueprintGroup::backupState(){
	BlueprintObject::backupState();
	oldText_ = title();
	oldFillColor_ = fillColor_;
	oldActive_ = activeOnStart_;
}

bool BlueprintGroup::isStateChanged(int type){
	bool result = BlueprintObject::isStateChanged(type);

	if (result){
		return true;
	}

	if (type == BST_DISPLAY){
		return (oldText_ != title()) || (oldFillColor_ != fillColor_) || (oldActive_ != activeOnStart_);
	}

	return false;
}

void BlueprintGroup::setActiveOnStart(bool flag){
	activeOnStart_ = flag;
	text_->setBrush(flag ? Qt::black : Qt::gray);
}


QJsonValue BlueprintGroup::expNode(int dt){
	static const QString  grpActiveH = QString::number(hashString(UDQ_T("cr.system_object.prototype.cnds.IsGroupActive")));

	QJsonArray code;
	code.append((int)0);  // 事件块

	QJsonArray arr;
	arr.append(isActiveOnStart());
	arr.append(title());
	code.append(arr);

	code.append(false); // 是否为OR事件块
	code.append(QJsonValue());  // 保留， NULL
	code.append((qint64)iid());

	// 条件
	QJsonArray cnds;
	QJsonArray cnd;
	// [-1, 2, null, 0, false, false, false, sid, false, [[1, [2, "Title"]]
	cnd.append((int)-1);
	cnd.append(grpActiveH);  // 导出时需要替换
	cnd.append(QJsonValue());  // 保留， NULL
	cnd.append((int)0);
	cnd.append(false);
	cnd.append(false);
	cnd.append(false);
	cnd.append((qint64)iid());
	cnd.append(false);

	// 参数列表
	QJsonArray param;
	param.append((int)1);  // 字符串类型

	QJsonArray exp;
	exp.append((int)2); // 字符串常量
	exp.append(title());
	param.append(exp);

	QJsonArray paramList;
	paramList.append(param);
	cnd.append(paramList);

	cnds.append(cnd);
	code.append(cnds);
	// 动作( 空)
	code.append(QJsonArray());

	// 子事件数组
	return code;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

QFont BlueprintGroup::titleFont(){
	QFont font;
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPixelSize(18);
	font.setBold(true);
	font.setFamily(UDQ_L8("微软雅黑"));
	return font;
}
