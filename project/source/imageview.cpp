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

#include <QtGui>
#include "imageview.h"
#include <QtOpenGL>
#include "commondefine.h"
#include "imagescene.h"

const int PREVIEW_ZOOM = 2;

ImageView::ImageView(QWidget *parent) : QGraphicsView(parent) {

	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

	setAlignment(Qt::AlignCenter);
	setRenderHint(QPainter::Antialiasing, true);
	setRenderHint(QPainter::SmoothPixmapTransform, true);
	setRenderHint(QPainter::TextAntialiasing, true);
	setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
	setViewport( new QWidget() ); 
	setDragMode(QGraphicsView::ScrollHandDrag);
	setMinimumSize(QSize(STRUCTURE_SIZE, STRUCTURE_SIZE));
	setAcceptDrops( true );

	setCursor( Qt::ArrowCursor );
	zoomStep_ = 0;
}

ImageView::~ImageView(){

}


void ImageView::mouseMoveEvent(QMouseEvent *mouseEvent){
	setFocus();
	QGraphicsView::mouseMoveEvent( mouseEvent );

}

void ImageView::wheelEvent(QWheelEvent * event){

	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;

	double zoom = 1.0;
	if (numSteps > 0) { // 放大
		numSteps = qMin(zoomStep_ + PREVIEW_ZOOM, numSteps);
	}
	else if (numSteps < 0) { // 缩小
		numSteps = qMax(zoomStep_ - PREVIEW_ZOOM, numSteps);
	}

	if(  zoomStep_ == numSteps ){  // 恢复正常大小
		// 恢复原始比例
		zoomStep_ = 0;
		QTransform mat = transform();
		mat.reset();
		setTransform( mat );
		return;
	}
	else{
		if ( numSteps > 0 ){
			zoom = pow( 2, numSteps );
			zoomStep_ -= numSteps;
		}
		else if( numSteps < 0 ){
			zoom = pow( 0.5, -numSteps );    
			zoomStep_ -= numSteps;
		}
	}

	setMatrix( matrix().scale(zoom, zoom) );	

}