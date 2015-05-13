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
#include "spriteview.h"
#include <QtOpenGL>
#include "commondefine.h"
#include "levelscene.h"

const int DEVS_ZOOM = 12;

SpriteView::SpriteView(QWidget *parent) : QGraphicsView(parent) {

	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setRenderHint(QPainter::Antialiasing, true);
	setRenderHint(QPainter::SmoothPixmapTransform, true);
	setRenderHint(QPainter::TextAntialiasing, true);
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	setViewport( new QWidget()/* new QGLWidget(QGLFormat())*/); 
	setDragMode( QGraphicsView::ScrollHandDrag );
	setMinimumSize( QSize(800, 600 ) );
	setAcceptDrops( true );
    zoomStep_ = 0;

}

SpriteView::~SpriteView(){

}


void SpriteView::mouseMoveEvent(QMouseEvent *mouseEvent){
	setFocus();
	QGraphicsView::mouseMoveEvent( mouseEvent );
}


void SpriteView::wheelEvent(QWheelEvent * event){

	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;

	if( numSteps > 0 && zoomStep_ >= 0 ) { // 放大
		zoomUP(numSteps);
	}
	else if( numSteps < 0 ) { // 缩小
		zoomDown(-numSteps);
	}
}

void SpriteView::zoomUP(int step ){
	double zoom = 1.0;
	if (zoomStep_ >0) { // 放大
		if (zoomStep_  < step ){
			// 恢复原始比例
			zoomStep_ = 0;
			QTransform mat = transform();
			mat.reset();
			setTransform(mat);
			return;
		}
		else{
			zoom = pow(1.25, step);
			zoomStep_ -= step;
		}
		setTransform(transform().scale(zoom, zoom));
	}
}
void SpriteView::zoomDown(int step){

	double zoom = 1.0;
	if (zoomStep_ < DEVS_ZOOM){
		step = qMin(DEVS_ZOOM - zoomStep_, step);
		zoom = pow(0.8, step);
		zoomStep_ += step;
	}

	setTransform(transform().scale(zoom, zoom));
}


void SpriteView::keyPressEvent(QKeyEvent * keyEvent){

	if (keyEvent->key() == Qt::Key_Plus || keyEvent->key() == Qt::Key_Equal){
		zoomUP();
		return;
	}
	else if (keyEvent->key() == Qt::Key_Minus){
		zoomDown();
		return;
	}

	QGraphicsView::keyPressEvent(keyEvent);
}
