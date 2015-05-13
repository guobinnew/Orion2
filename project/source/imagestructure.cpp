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

#include "imagestructure.h"
#include <QtGui>
#include <math.h>
#include "imagescene.h"
#include "commondefine.h"
#include "configmanager.h"


ImageStructure::ImageStructure(double width, double height, double depth){
	offset_ = QPointF(0, 0);
	boundingBox_.setRect( 0, 0, width, height );
	// 设置深度值
	setZValue( depth );
	// 不接受鼠标事件
	setAcceptedMouseButtons( Qt::NoButton );
	// 无任何标志
	setFlags( 0 ); 
	// 接受鼠标经过事件
	setAcceptHoverEvents( true );
	// 接受拖放
	setAcceptDrops( true );

}

ImageStructure::~ImageStructure(){
}


void ImageStructure::setBoundingRect(const QRectF& newRect){
	prepareGeometryChange();
	boundingBox_ = newRect;
}

QRectF ImageStructure::boundingRect() const{
	return boundingBox_;
}

QPainterPath ImageStructure::shape() const{
	QPainterPath path;
	path.addRect(boundingBox_);
	return path;
}

void ImageStructure::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	Q_UNUSED(widget);

	if( tile_.isNull() )
		return;

	if (buffer_.isNull()){
		bufferImage();
	}

	painter->drawImage(offset_, buffer_);

}

QSize ImageStructure::imageSize(){

	if (state_.size() == 0){
		state_.fill(false, 8);
	}
	QSize isize = tile_.size();
	if (state_[6]){
		isize.transpose();
	}

	return isize;
}

void ImageStructure::setImage(const QImage& img){
	tile_ = img;
	buffer_ = QImage();
	adjust();
}

void ImageStructure::bufferImage(){
	if (state_.size() == 0){
		buffer_ = tile_;
	}
	else{
		buffer_ = tile_.mirrored(state_[4], state_[5]);
		// 对角翻转
		if (state_[6]){
			QTransform trans;
			trans.rotate(90);
			buffer_ = buffer_.transformed(trans);
			buffer_ = buffer_.mirrored(false, true);
		}
	}
}

QImage ImageStructure::getImage(bool trans){
	if (!trans){
		return tile_;
	}

	if (buffer_.isNull()){
		bufferImage();
	}
	return buffer_;
}

void ImageStructure::adjust(){
	// 修改offset
	QRectF bound = boundingRect();
	QSize isize = tile_.size();

	if (state_.size() == 0){
		state_.fill(false, 8);
	}

	if (state_[6]){
		isize.transpose();
	}

	offset_.setX((bound.width() - isize.width()) / 2.0);
	offset_.setY((bound.height() - isize.height()) / 2.0);
}

void ImageStructure::updateState(const QBitArray& state){
	state_ = state;
	adjust();
	bufferImage();
	update();
}