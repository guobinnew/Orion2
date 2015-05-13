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

#include "kismetlink.h"
#include <QtGui>
#include <math.h>
#include "kismetscene.h"
#include "kismetmodel.h"
#include "kismetmodelport.h"


BlueprintLink::BlueprintLink(BlueprintObject *sourceNode, BlueprintObject *destNode, bool temp)
	:QGraphicsPathItem(), source_(0), dest_(0), isError_(false){

	setZValue( KISMET_DEPTH_LINK  );

	tempLink_ = temp;
	editCtrl_ = false;
	underMouse_ = false;

	// 设置标志
	setFlag( QGraphicsItem::ItemIsFocusable,true);
	setFlag( QGraphicsItem::ItemIsSelectable, true);
	setFlag( QGraphicsItem::ItemSendsGeometryChanges, true);

	// 接受鼠标经过事件
	setAcceptHoverEvents( true );

	setSourceNode( sourceNode );
	setDestNode( destNode );

	adjust();
}

BlueprintLink::~BlueprintLink(){

}

void BlueprintLink::unlink(){

	if( source_ ){
		source_->removeLink( this );
	}

	if( dest_ ){
		dest_->removeLink( this );
	}
}


BlueprintObject *BlueprintLink::sourceNode() const{
	return source_;
}

void BlueprintLink::setSourceNode(BlueprintObject *node){
	if( source_  && !tempLink_ ){
		source_->removeLink(this );
	}

	source_ = node;

	if( source_ ){
		if( !tempLink_ ){
			source_->addLink( this );
		}
	}
	
}

BlueprintObject *BlueprintLink::destNode() const{
	return dest_;
}

void BlueprintLink::setDestNode(BlueprintObject *node){
	if( dest_ && !tempLink_ ){
		dest_->removeLink(this );
	}

	dest_ = node;
	if( dest_ ){
		if( !tempLink_ ){
			dest_->addLink( this );
		}
	}

}

// 更新端口
void BlueprintLink::updatePort(BlueprintObject* node){
	 adjust();
	 update();
}


void BlueprintLink::hoverEnterEvent(QGraphicsSceneHoverEvent * event){
	setCursor( Qt::ArrowCursor );
	QGraphicsPathItem::hoverEnterEvent(event);
}

void BlueprintLink::hoverLeaveEvent(QGraphicsSceneHoverEvent * event){
	QGraphicsPathItem::hoverLeaveEvent(event);
}

// 更新临时线
void BlueprintLink::updateTempLink(const QPointF& pos){

	if( !source_ ){
		sourcePoint_ = pos;
	}
	else{
		destPoint_ = pos;
	}

	QPainterPath path;
	path.moveTo( sourcePoint_ );
	path.lineTo( destPoint_ );
	setPath( path );
}


// 升级曲线
void BlueprintLink::upgradeLink(BlueprintObject *node){

	tempLink_ = false;
	if( !source_ ){
		source_ = node;
	}
	else{
		dest_ = node;
	}

	dest_->addLink(this);
	source_->addLink(this);

	adjust();
}

// 更新锚点
QPointF BlueprintLink::updateAnchor(BlueprintLinkAnchor* anchor){
	return anchor->scenePos();
}

void BlueprintLink::removeAnchor(BlueprintLinkAnchor* anchor){

}

QByteArray BlueprintLink::encodeEnd(BlueprintObject* obj, bool tmpl){
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);

	BlueprintModelPort* port = dynamic_cast<BlueprintModelPort*>(obj);
	Q_ASSERT( port != NULL );
	if( port->type() == KISMET_MODEL_PORT  ){
		BlueprintModel* model = dynamic_cast<BlueprintModel*>(obj->parentItem());
		Q_ASSERT( model != NULL );
		stream << (int)1;  // 子模型
		stream << model->iid();   // 实例ID
		stream << port->name();
		stream << port->orient();
	}
	else{
		stream << (int)0;  // 父模型
		stream << port->iid();   // 实例ID
	}

	return arr;
}

// 编码
void BlueprintLink::encode(QDataStream& stream){
	// 连接线端点
	QByteArray endarr;
	endarr = encodeEnd( source_ );
	stream<<endarr;
	endarr =encodeEnd( dest_ );
	stream<<endarr;
}

// 计算端口锚点
QPointF BlueprintLink::portAnchorPoint(BlueprintObject* node){
	if( node->type() == KISMET_MODEL_PORT ){
		return  ((BlueprintModelPort*)node)->endPoint();
	}
	return node->sceneCenter();
}


void BlueprintLink::adjust(){

	if( source_ ){
		sourcePoint_ = portAnchorPoint( source_ );
	}
	else{
		sourcePoint_ = QPointF(0,0);
	}

	if( dest_ ){
		destPoint_ = portAnchorPoint( dest_ );
	}
	else{
		destPoint_ = QPointF(0,0);
	}

	if (!source_ || !dest_)
		return;

	QPainterPath path;
	path.moveTo( sourcePoint_ );
	path.lineTo(  destPoint_ );
	setPath( path );
}


void BlueprintLink::drawLine(QPainter *painter, const QStyleOptionGraphicsItem *option){

	painter->setRenderHint( QPainter::Antialiasing, true );

	QPen borderPen;
	borderPen.setWidth(1);

	if ( isSelected() || underMouse_ || editCtrl_ ) {
		borderPen.setColor( Qt::yellow );
	}
	else{
		if (isError_){
			borderPen.setColor(Qt::red);
		}
		else{
			borderPen.setColor(source_ ? source_->linkColor() : dest_->linkColor());
		}
	}
	painter->setPen( borderPen );
	painter->drawPath( path() );

}

void BlueprintLink::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *){

	if (!tempLink_  && ( !source_ || !dest_ ) )
		return;

	drawLine( painter, option );
} 

void BlueprintLink::showAnchor(bool visible){
	editCtrl_ = visible;
}

void BlueprintLink::copyState(QDataStream& stream){
	stream << oldCtrlPoints_;
}


void BlueprintLink::toggleAnchor(){
	editCtrl_ = !editCtrl_;
	if (editCtrl_){
		// 记录数据
		oldCtrlPoints_ = controlPoints();
	}
}

void BlueprintLink::clearAnchor(){

}

QPainterPath BlueprintLink::shape() const{

	QPainterPath path;
	path.moveTo( sourcePoint_ );
	path.lineTo( destPoint_ );

	QPainterPathStroker stroker;
	stroker.setWidth( 4 );
	return stroker.createStroke( path );

}


void BlueprintLink::keyPressEvent(QKeyEvent * event){

	if( isSelected() && event->key() == Qt::Key_Space){
		// 切换编辑状态
		toggleAnchor();
		update();
	}

	QGraphicsItem::keyPressEvent( event );
}

// 端口类型检查
bool BlueprintLink::validate(QStringList& msg){
	static QMultiMap<QString, QString> castWarnings;
	//////////////////////////////////////////////////////////////////////////
	// 类型转换
	if (castWarnings.size() == 0){
		castWarnings.insert(UDQ_T("int"), UDQ_T("string"));
		castWarnings.insert(UDQ_T("int"), UDQ_T("float"));
		castWarnings.insert(UDQ_T("int"), UDQ_T("bool"));
		castWarnings.insert(UDQ_T("int"), UDQ_T("any"));
		castWarnings.insert(UDQ_T("int"), UDQ_T("event"));
		castWarnings.insert(UDQ_T("int"), UDQ_T("color"));
		castWarnings.insert(UDQ_T("int"), UDQ_T("instance"));
		castWarnings.insert(UDQ_T("float"), UDQ_T("int"));
		castWarnings.insert(UDQ_T("float"), UDQ_T("string"));
		castWarnings.insert(UDQ_T("float"), UDQ_T("any"));
		castWarnings.insert(UDQ_T("float"), UDQ_T("event"));
		castWarnings.insert(UDQ_T("float"), UDQ_T("instance"));
		castWarnings.insert(UDQ_T("bool"), UDQ_T("int"));
		castWarnings.insert(UDQ_T("bool"), UDQ_T("string"));
		castWarnings.insert(UDQ_T("bool"), UDQ_T("any"));
		castWarnings.insert(UDQ_T("bool"), UDQ_T("event"));
		castWarnings.insert(UDQ_T("bool"), UDQ_T("instance"));
		castWarnings.insert(UDQ_T("string"), UDQ_T("any"));
		castWarnings.insert(UDQ_T("string"), UDQ_T("event"));
		castWarnings.insert(UDQ_T("string"), UDQ_T("instance"));
		castWarnings.insert(UDQ_T("event"), UDQ_T("bool"));
		castWarnings.insert(UDQ_T("event"), UDQ_T("int"));
		castWarnings.insert(UDQ_T("event"), UDQ_T("float"));
		castWarnings.insert(UDQ_T("event"), UDQ_T("string"));
		castWarnings.insert(UDQ_T("event"), UDQ_T("any"));
		castWarnings.insert(UDQ_T("instance"), UDQ_T("int"));
		castWarnings.insert(UDQ_T("instance"), UDQ_T("float"));
		castWarnings.insert(UDQ_T("instance"), UDQ_T("bool"));
		castWarnings.insert(UDQ_T("instance"), UDQ_T("string"));
		castWarnings.insert(UDQ_T("instance"), UDQ_T("any"));
		castWarnings.insert(UDQ_T("color"), UDQ_T("int"));
		castWarnings.insert(UDQ_T("any"), UDQ_T("int"));
		castWarnings.insert(UDQ_T("any"), UDQ_T("float"));
		castWarnings.insert(UDQ_T("any"), UDQ_T("string"));
		castWarnings.insert(UDQ_T("any"), UDQ_T("bool"));
		castWarnings.insert(UDQ_T("any"), UDQ_T("event"));
		castWarnings.insert(UDQ_T("any"), UDQ_T("instance"));
	}

	Q_ASSERT(dest_ != NULL && source_ != NULL);
	QString type1 = ((BlueprintModelPort*)source_)->dataType();
	QString type2 = ((BlueprintModelPort*)dest_)->dataType();

	isError_ = false;
	if (type1 != type2){
		if (castWarnings.values(type1).contains(type2)){

			if (type1 == UDQ_T("any") || type2 == UDQ_T("any")){
			}
			else{
				//  警告
				msg.append(WARNING_MSG.arg(UDQ_TR("源端口<%1>类型<%2>与目标端口<%3>类型<%4>不一致（需要类型转换）!")
					.arg(source_->title()).arg(type1)
					.arg(dest_->title()).arg(type2)));
			}
		}
		else{
			// 错误
			isError_ = true;
			msg.append(ERROR_MSG.arg(UDQ_TR("源端口<%1>类型<%2>与目标端口<%3>类型<%4>不匹配，无法进行类型转换!")
				.arg(source_->title()).arg(type1)
				.arg(dest_->title()).arg(type2)));
		}
	}
	update();
	return !isError_;
}


QVariant BlueprintLink::itemChange(GraphicsItemChange change, const QVariant & value){

	return QGraphicsPathItem::itemChange(change, value);
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

BlueprintLinkAnchor::BlueprintLinkAnchor(BlueprintLink *host, QGraphicsItem* parent) : QGraphicsItem(parent), host_(host){

	setZValue( KISMET_DEPTH_LINKANCHOR );
	setFlags( ItemIsSelectable | ItemIsMovable | ItemIgnoresTransformations );
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	setFlag( QGraphicsItem::ItemIsFocusable,true);

	setAcceptHoverEvents( true );
	boundingBox_ = QRect( -4, -4, 8, 8 );
	underMouse_ = false;
}

QRectF BlueprintLinkAnchor::boundingRect() const{
	return boundingBox_;
}

QPainterPath BlueprintLinkAnchor::shape() const{
	QPainterPath path;
	path.addRect( boundingBox_ );
	return path;
}


BlueprintLinkAnchor::~BlueprintLinkAnchor(){

}

void BlueprintLinkAnchor::hoverEnterEvent(QGraphicsSceneHoverEvent * event){
	underMouse_ = true;
	setCursor( Qt::ArrowCursor );
	QGraphicsItem::hoverEnterEvent(event);
	update();
}

void BlueprintLinkAnchor::hoverLeaveEvent(QGraphicsSceneHoverEvent * event){
	underMouse_ = false;
	QGraphicsItem::hoverLeaveEvent(event);
	update();
}

void BlueprintLinkAnchor::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget){

	QPen borderPen;
	if( isSelected()  || underMouse_ ){
		// 绘制选择框颜色
		borderPen.setColor( Qt::yellow );
	}
	else{
		borderPen.setColor( Qt::black  );
	}
	painter->setPen( borderPen );

	// 填充色
	painter->fillRect(boundingBox_, Qt::black);
	painter->drawRect(boundingBox_);

}


void BlueprintLinkAnchor::keyPressEvent(QKeyEvent * event){

	if( isSelected() && event->key() == Qt::Key_Space){
		// 退出编辑状态
		setSelected( false );
		host_->showAnchor( false );
		// 退出编辑状态
		KismetScene* parent = (KismetScene*)scene();
		if (parent){
			parent->notifyLinkChanged(host_);
		}
		return;
	}
	else if( isSelected() && event->key() == Qt::Key_Delete){
		// 删除控制点	
		host_->removeAnchor( this );
		return;
	}
	QGraphicsItem::keyPressEvent( event );
}

QVariant BlueprintLinkAnchor::itemChange(GraphicsItemChange change, const QVariant & value){

	if(  change == ItemPositionHasChanged && scene()){
		// 如果位置发生改变，修改轨迹
		Q_ASSERT( host_ != NULL );
		QPointF newPos = host_->updateAnchor( this );
		return newPos;
	}
	return QGraphicsItem::itemChange( change, value );
}

