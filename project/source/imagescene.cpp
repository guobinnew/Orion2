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
#include "imagescene.h"
#include <math.h>
#include "imageview.h"
#include "commondefine.h"
#include "imagehelper.h"
using namespace ORION;

ImageScene::ImageScene(QObject *parent) : QGraphicsScene(parent){
	setSceneRect(0, 0, STRUCTURE_MAXSIZE, STRUCTURE_MAXSIZE);
	mapMode_ = POM_POINTER;
	gridImage_.load(UDQ_T(":/images/grid2.png"));
	init();
}

ImageScene::~ImageScene(){

}

// 设置模式
void ImageScene::setMode(PREVIEW_MODE mode){

	mapMode_  = mode;
	if( mapMode_ == POM_POINTER ){
		clearSelection();
	}
}

// 获取可见区域（场景坐标系）
QRectF ImageScene::viewRegionRect(void){

	// 获取当前缩放比例
	QSize viewRect = views().at(0)->maximumViewportSize();

	QPointF viewLTPos = views().at(0)->mapToScene( 0, 0 );
	if( viewLTPos.rx() < 0 )
		viewLTPos.rx() = 0;
	if( viewLTPos.ry() < 0 )
		viewLTPos.ry() = 0;

	QRectF sRect = sceneRect();

	QPointF viewRBPos = views().at(0)->mapToScene( viewRect.width(), viewRect.height() );
	if( viewRBPos.rx() > sRect.width() )
		viewRBPos.rx() = sRect.width();
	if( viewRBPos.ry() > sRect.height() )
		viewRBPos.ry() = sRect.height();

	return QRectF(viewLTPos, viewRBPos );
}


void ImageScene::drawBackground(QPainter * painter, const QRectF & rect){
	// 绘制底色
	painter->fillRect( rect, gridImage_ );
	// 确定尺寸和位置
	QSize viewRect = views().at(0)->maximumViewportSize();

}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent){

	QGraphicsScene::mousePressEvent( mouseEvent );
}


void ImageScene::keyPressEvent(QKeyEvent * keyEvent){
	
	if( keyEvent->key() == Qt::Key_Delete ){
	}

	QGraphicsScene::keyPressEvent( keyEvent );
}

void ImageScene::setAnchor(const QPointF& pos){
	
	QSize imgSize = structure_->imageSize();
	QPointF anchorPos;
	anchorPos.setX(structure_->offset_.x() + pos.x() * imgSize.width());
	anchorPos.setY(structure_->offset_.y() + pos.y() * imgSize.height());
	anchor_->setPos(anchorPos);
	setAnchorVisible(mapMode_ == POM_ANCHOR);
}

void ImageScene::setAnchorVisible(bool flag){
	anchor_->setVisible(flag);
}

void ImageScene::setCollisionVisible(bool flag){
	collision_->setVisible(flag);
	collision_->showJoint(flag);
}

void ImageScene::clearImage(){
	setAnchorVisible(false);
	setCollisionVisible(false);
}

void ImageScene::setImage(const QImage& img, const QBitArray& state ){
	structure_->setImage(img);
	structure_->updateState(state);
	structure_->update();
}

void ImageScene::setCollision(const QList<QPointF>& pts){
	QList<QPointF> scenePts;
	QSize imgSize = structure_->imageSize();

	foreach(QPointF p, pts){
		scenePts.append(QPointF(structure_->offset_.x() + p.x() * imgSize.width(), structure_->offset_.y() + p.y() * imgSize.height()));
	}

	collision_->initPolygon(scenePts);
	setCollisionVisible(mapMode_ == POM_COLLISION);
}


void ImageScene::save(SequenceFrame* frame, const QString& anchor){
	ResourceHash key = hashString(anchor);
	QSize imgSize = structure_->imageSize();
	QPointF pt;

	// 保存state
	frame->state = getStructure()->imageState();
	// 更新图像
	frame->buffer = getStructure()->getImage(true);

	if (mapMode_ == POM_ANCHOR || mapMode_ == POM_POINTER ){
		pt.setX((anchor_->pos().x() - structure_->offset_.x()) / imgSize.width());
		pt.setY((anchor_->pos().y() - structure_->offset_.y()) / imgSize.height());
		frame->anchors.insert(key, pt);
	}

	if (mapMode_ == POM_COLLISION || mapMode_ == POM_POINTER ){
		// 保存碰撞网格
		frame->collisionRegion.clear();
		foreach(QPointF p, collision_->points_){
			pt.setX((p.x() - structure_->offset_.x()) / imgSize.width());
			pt.setY((p.y() - structure_->offset_.y()) / imgSize.height());
			frame->collisionRegion.append(pt);
		}
	}

}

void ImageScene::init(){
	structure_ = new ImageStructure(STRUCTURE_MAXSIZE, STRUCTURE_MAXSIZE);
	structure_->setPos(0, 0);
	addItem( structure_);

	anchor_ = new ImageAnchor();
	anchor_->setPos(0, 0);
	anchor_->setVisible(false);
	addItem(anchor_);

	QList<QPointF> pts;
	pts.append(QPointF(0, 0));
	pts.append(QPointF(STRUCTURE_MAXSIZE, 0));
	pts.append(QPointF(STRUCTURE_MAXSIZE, STRUCTURE_MAXSIZE));
	pts.append(QPointF(0, STRUCTURE_MAXSIZE));

	collision_ = new ImageCollision(pts);
	collision_->setPos(0, 0);
	collision_->setVisible(false);
	addItem(collision_);
}


// 获取碰撞网格
QList<QPointF> ImageScene::collisions(){
	QList<QPointF> pts;
	// 保存碰撞网格
	QPointF pt;
	QSize imgSize = structure_->imageSize();
	foreach(QPointF p, collision_->points_){
		pt.setX((p.x() - structure_->offset_.x()) / imgSize.width());
		pt.setY((p.y() - structure_->offset_.y()) / imgSize.height());
		pts.append(pt);
	}
	return pts;
}

void ImageScene::setDefaultCollision(){
	QList<QPointF> pts;
	pts.append(QPointF(0, 0));
	pts.append(QPointF(1, 0));
	pts.append(QPointF(1, 1));
	pts.append(QPointF(0, 1));

	QList<QPointF> scenePts;
	QSize imgSize = structure_->imageSize();

	foreach(QPointF p, pts){
		scenePts.append(QPointF(structure_->offset_.x() + p.x() * imgSize.width(), structure_->offset_.y() + p.y() * imgSize.height()));
	}

	collision_->initPolygon(scenePts);
	setCollisionVisible(mapMode_ == POM_COLLISION);
}