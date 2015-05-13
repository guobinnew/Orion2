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

#include "kismetlinksingle.h"
#include <QtGui>
#include <math.h>
#include "kismetscene.h"
#include "kismetmodel.h"
#include "kismetmodelport.h"

BlueprintLinkSingle::BlueprintLinkSingle(BlueprintObject *sourceNode, BlueprintObject *destNode, const QPointF& srcPt, const QPointF& desPt, bool temp)
	:BlueprintLink(sourceNode, destNode, temp ),sourceCtrlPoint_(srcPt),destCtrlPoint_(desPt){
}

BlueprintLinkSingle::~BlueprintLinkSingle(){

}

// 升级曲线
void BlueprintLinkSingle::upgradeLink(BlueprintObject *node){

	tempLink_ = false;
	if( !source_ ){
		source_ = node;
		sourceCtrlPoint_ = node->controlPoint();
	}
	else{
		dest_ = node;
		destCtrlPoint_ = node->controlPoint();
	}

	dest_->addLink(this);
	source_->addLink(this);
	adjust();
}

// 编码
void BlueprintLinkSingle::encode(QDataStream& stream){
	BlueprintLink::encode(stream);

	// 转换为相对坐标

	 QList<QPointF> ctrlPts;
	 ctrlPts.append(sourceCtrlPoint_ );
	 ctrlPts.append( destCtrlPoint_ );
	stream<<ctrlPts;
}


void BlueprintLinkSingle::adjust(){

	if( source_ ){
		sourcePoint_ = portAnchorPoint( source_ );
	}
	else{
		sourcePoint_ = QPointF(0,0);
		sourceCtrlPoint_ = QPointF(0,0);
	}

	if( dest_ ){
		destPoint_ = portAnchorPoint( dest_ );
	}
	else{
		destPoint_ = QPointF(0,0);
		destCtrlPoint_= QPointF(0,0);
	}

	if (!source_ || !dest_)
		return;

	// 根据偏移位置更新控制点显示
	if( sourceAnchor_ ){
		sourceAnchor_->setPos(sourceCtrlPoint_);
	}
	if( destAnchor_ ){
		destAnchor_->setPos(destCtrlPoint_);
	}

	QPainterPath path;
	path.moveTo( sourcePoint_ );
	path.cubicTo( sourceCtrlPoint_, destCtrlPoint_, destPoint_ );
	setPath( path );
}


void BlueprintLinkSingle::drawLine(QPainter *painter, const QStyleOptionGraphicsItem *option){

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
		painter->drawLine(sourcePoint_,sourceCtrlPoint_);
		painter->drawLine(destPoint_,destCtrlPoint_);
	}

}

void BlueprintLinkSingle::initAnchor(){

	if( sourceAnchor_ == NULL ){
		sourceAnchor_ = new BlueprintLinkSingleAnchor( this, END_SOURCE );
		scene()->addItem(sourceAnchor_);
	}
	sourceAnchor_->setPos(sourceCtrlPoint_);

	if( destAnchor_ == NULL ){
		destAnchor_ = new BlueprintLinkSingleAnchor(this, END_DEST);
		scene()->addItem( destAnchor_ );
	}
	destAnchor_->setPos(destCtrlPoint_);

}

void BlueprintLinkSingle::showAnchor(bool visible){
	BlueprintLink::showAnchor(visible);

	initAnchor();
	sourceAnchor_->setVisible( visible );
	destAnchor_->setVisible( visible );
}

void BlueprintLinkSingle::toggleAnchor(){
	BlueprintLink::toggleAnchor();

	initAnchor();
	sourceAnchor_->setVisible( editCtrl_ );
	destAnchor_->setVisible( editCtrl_ );
}

void BlueprintLinkSingle::clearAnchor(){
	if( sourceAnchor_ ){
		scene()->removeItem( sourceAnchor_);
		delete sourceAnchor_;
		sourceAnchor_ = NULL;
	}

	if( destAnchor_ ){
		scene()->removeItem( destAnchor_);
		delete destAnchor_;
		destAnchor_ = NULL;
	}
}

// 更新控制点
QPointF BlueprintLinkSingle::updateAnchor(BlueprintLinkAnchor* anchor){
	Q_ASSERT( anchor->type() == KISMET_LINK_ANCHOR_SINGLE );
	BlueprintLinkSingleAnchor* single = static_cast<BlueprintLinkSingleAnchor*>(anchor);

	if( single->anchorType() == END_SOURCE ){
		sourceCtrlPoint_ = anchor->scenePos();
	}
	else if( single->anchorType() == END_DEST ){
		destCtrlPoint_ = anchor->scenePos();
	}

	adjust();
	update();

	return anchor->scenePos();
}


// 更新端口
void BlueprintLinkSingle::updatePort(BlueprintObject* node){

	if( source_ == node ){
		QPointF oldPt = sourcePoint_;
		sourcePoint_ = portAnchorPoint( source_ );
		sourceCtrlPoint_ += ( sourcePoint_ - oldPt );
	}

	if( dest_ == node ){
		QPointF oldPt = destPoint_;
		destPoint_ = portAnchorPoint( dest_ );
		destCtrlPoint_ += ( destPoint_ - oldPt );
	}

	adjust();
	update();
}


QPainterPath BlueprintLinkSingle::shape() const{

	QPainterPath path;
	path.moveTo( sourcePoint_ );
	path.cubicTo( sourceCtrlPoint_, destCtrlPoint_, destPoint_ );

	QPainterPathStroker stroker;
	stroker.setWidth( 4 );
	return stroker.createStroke( path );

}

// 控制点
QList<QPointF> BlueprintLinkSingle::controlPoints(){
	QList<QPointF> pts;
	pts << sourceCtrlPoint_ << destCtrlPoint_;
	return pts;
}


void BlueprintLinkSingle::changeControlPoints(const QList<QPointF>& ctrlPts){
	Q_ASSERT(ctrlPts.size() >= 2);
	sourceCtrlPoint_ = ctrlPts[0];
	destCtrlPoint_ = ctrlPts[1];
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

BlueprintLinkSingleAnchor::BlueprintLinkSingleAnchor(BlueprintLink* host, int type, QGraphicsItem* parent) : BlueprintLinkAnchor(host, parent), anchorType_(type){
}

BlueprintLinkSingleAnchor::~BlueprintLinkSingleAnchor(){

}

