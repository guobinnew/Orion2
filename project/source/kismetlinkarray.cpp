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

#include "kismetlinkarray.h"
#include <QtGui>
#include <math.h>
#include "kismetscene.h"
#include "kismetmodel.h"
#include "kismetmodelport.h"

BlueprintLinkArray::BlueprintLinkArray(BlueprintObject *sourceNode, BlueprintObject *destNode, const QList<QPointF>& ctrlPts, bool temp)
	:BlueprintLink(sourceNode,destNode,temp ){

	// 控制点
	ctrlPoints_ = ctrlPts;
	directline_ = false;
	adjust();
}

BlueprintLinkArray::~BlueprintLinkArray(){

}

// 添加控制点
void BlueprintLinkArray::addControlPoint(const QPointF& pt){

	if( !source_ ){
		ctrlPoints_.push_front( pt );
	}
	else{
		ctrlPoints_.push_back( pt );
	}
}


// 更新临时线
void BlueprintLinkArray::updateTempLink(const QPointF& pos){

	if( !source_ ){
		sourcePoint_ = pos;
	}
	else{
		destPoint_ = pos;
	}

	QPainterPath path;
	path.moveTo( sourcePoint_ );

	// 绘制控制点
	foreach( QPointF pt, ctrlPoints_ ){
		path.lineTo( pt );
	}
	path.lineTo( destPoint_ );
	setPath( path );
}


// 升级曲线
void BlueprintLinkArray::upgradeLink(BlueprintObject *node){

	tempLink_ = false;
	if( !source_ ){
		source_ = node;
	}
	else{
		dest_ = node;
	}

	ctrlPoints_.push_front( source_->controlPoint() );
	ctrlPoints_.push_back( dest_->controlPoint() );

	dest_->addLink(this);
	source_->addLink(this);

	adjust();
}


// 编码
void BlueprintLinkArray::encode(QDataStream& stream){

	BlueprintLink::encode( stream );
	stream<<ctrlPoints_;
}

// 更新端口
void BlueprintLinkArray::updatePort(BlueprintObject* node){

	QPointF deltaPt = QPointF(0,0);
	int startIndex = 0;
	int endIndex = ctrlPoints_.size();
	int ctrlNum = ctrlPoints_.size();

	// 控制点分为2部分

	if( source_ == node ){
		QPointF oldPt = sourcePoint_;
		sourcePoint_ =  portAnchorPoint( source_ );
		deltaPt =sourcePoint_ - oldPt;
		endIndex = ctrlNum / 2 + ctrlNum % 2;
	}

	if( dest_ == node ){
		QPointF oldPt = destPoint_;
		destPoint_ = portAnchorPoint( dest_ );
		deltaPt =destPoint_ - oldPt;
		startIndex = ctrlNum / 2 + ctrlNum % 2;
	}

	QPointF tempPt;
	for( int i=startIndex; i<endIndex; ++i){
		tempPt = ctrlPoints_.at(i) + deltaPt;
		ctrlPoints_.replace(i, tempPt );
	}

	adjust();
	update();
}


void BlueprintLinkArray::adjust(){
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

	//  更新anchor位置
	foreach( BlueprintLinkArrayAnchor* ar, ctrlAnchors_){
		ar->setPos( ctrlPoints_.at(ar->anchorIndex()));
	}

	// 计算新的曲线
	QPainterPath path;
	path.moveTo( sourcePoint_ );

	if( directline_ ){
		// 绘制控制点
		foreach( QPointF pt, ctrlPoints_ ){
			path.lineTo( pt );
		}
		path.lineTo( destPoint_ );
	}
	else{
		for( int i=0; i<ctrlPoints_.size() - 1; ++i ){
			QPointF endPt = ( ctrlPoints_.at(i) + ctrlPoints_.at(i+1) ) /2;
			path.quadTo( ctrlPoints_.at(i), endPt );
		}
		path.quadTo( ctrlPoints_.at( ctrlPoints_.size() - 1), destPoint_ );
	}

	setPath( path );

}


void BlueprintLinkArray::drawLine(QPainter *painter, const QStyleOptionGraphicsItem *option){

	painter->setRenderHint( QPainter::Antialiasing, true );

	QPen borderPen;
	borderPen.setWidth(1);

	if ( isSelected() || underMouse_ || editCtrl_ ) {
		borderPen.setColor( Qt::yellow );
	}
	else{
		borderPen.setColor(source_ ? source_->linkColor() : dest_->linkColor());
	}
	painter->setPen( borderPen );
	painter->drawPath( path() );

	// 绘制控制线
	if( editCtrl_ ){
		QPen ctrlPen;  
		ctrlPen.setStyle(Qt::DashLine);
		ctrlPen.setWidth(2);
		ctrlPen.setColor(Qt::red);
		painter->setPen( ctrlPen  );

		Q_ASSERT( ctrlPoints_.size() >= 2 );

		painter->drawLine(sourcePoint_,ctrlPoints_.at(0));
		for( int i=0; i<ctrlPoints_.size() - 1; ++i ){
			painter->drawLine(ctrlPoints_.at(i),ctrlPoints_.at(i+1));
		}
		painter->drawLine(ctrlPoints_.at(ctrlPoints_.size() - 1 ), destPoint_);
	}

}


void BlueprintLinkArray::initAnchor(){

	if( ctrlAnchors_.size() == 0 ){
		int index = 0;
		foreach( QPointF pt, ctrlPoints_){
			BlueprintLinkArrayAnchor *anchor = new BlueprintLinkArrayAnchor(this, index++);
			scene()->addItem(anchor);
			ctrlAnchors_.append( anchor );
			anchor->setPos( pt );
		}
	}

	foreach(BlueprintLinkArrayAnchor* ar, ctrlAnchors_){
		ar->setPos( ctrlPoints_.at(ar->anchorIndex()));
	}
}

void BlueprintLinkArray::showAnchor(bool visible){
	BlueprintLink::showAnchor(visible);

	initAnchor();
	foreach(BlueprintLinkArrayAnchor* ar, ctrlAnchors_){
		ar->setVisible( visible );
	}
}

void BlueprintLinkArray::toggleAnchor(){
	BlueprintLink::toggleAnchor();

	initAnchor();
	foreach(BlueprintLinkArrayAnchor* ar, ctrlAnchors_){
		ar->setVisible( editCtrl_ );
	}
}

void BlueprintLinkArray::clearAnchor(){

	foreach(BlueprintLinkArrayAnchor* ar, ctrlAnchors_){
		delete ar;
	}
	ctrlAnchors_.clear();
}

// 更新控制点
QPointF BlueprintLinkArray::updateAnchor(BlueprintLinkAnchor* anchor){
	Q_ASSERT( anchor->type() == KISMET_LINK_ANCHOR_ARRAY );
	BlueprintLinkArrayAnchor* arr = static_cast<BlueprintLinkArrayAnchor*>(anchor);

	// 修改控制点数据
	ctrlPoints_.replace( arr->anchorIndex(), arr->scenePos() );
	adjust();
	update();

	return BlueprintLink::updateAnchor( anchor );
}

void BlueprintLinkArray::removeAnchor(BlueprintLinkAnchor* anchor){

	if( ctrlPoints_.size() <= 2 )
		return;

	Q_ASSERT( anchor->type() == KISMET_LINK_ANCHOR_ARRAY );
	BlueprintLinkArrayAnchor* arr = static_cast<BlueprintLinkArrayAnchor*>(anchor);

	int index = arr->anchorIndex();
	ctrlPoints_.removeAt(index);

	if( ctrlAnchors_.size() > index ){
		ctrlAnchors_.removeAt(index );

		for( int i= index; i<ctrlAnchors_.size(); i++ ){
			ctrlAnchors_.at(i)->setAnchorIndex(i);
		}
	}

	if( editCtrl_ ){
		scene()->removeItem(anchor);
	}

	delete arr;
	adjust();
	update();

}

QPainterPath BlueprintLinkArray::shape() const{

	QPainterPath path;
	path.moveTo( sourcePoint_ );

	if( ctrlPoints_.size() > 0 ){
		for( int i=0; i<ctrlPoints_.size() - 1; ++i ){
			QPointF endPt = ( ctrlPoints_.at(i) + ctrlPoints_.at(i+1) ) /2;
			path.quadTo( ctrlPoints_.at(i), endPt );
		}
		path.quadTo( ctrlPoints_.at( ctrlPoints_.size() - 1), destPoint_ );
	}
	else{
		path.lineTo( destPoint_ );
	}

	QPainterPathStroker stroker;
	stroker.setWidth( 4 );
	return stroker.createStroke( path );

}

void BlueprintLinkArray::changeControlPoints(const QList<QPointF>& ctrlPts){
	setControlPoints(ctrlPts);
	clearAnchor();
	initAnchor();
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

BlueprintLinkArrayAnchor::BlueprintLinkArrayAnchor(BlueprintLink* host, int index, QGraphicsItem* parent) : BlueprintLinkAnchor(host, parent), anchorIndex_(index){
	setVisible(false);
}

BlueprintLinkArrayAnchor::~BlueprintLinkArrayAnchor(){

}

void BlueprintLinkArrayAnchor::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget){

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
	QPainterPath path;
	path.addEllipse( boundingBox_ );

	painter->fillPath( path, Qt::black );
	painter->drawEllipse(boundingBox_ .adjusted(-2,-2,2,2) );

}

