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
#include "tilemapwidget.h"
#include <QtGui>
#include "commondefine.h"


TileMapWidget::TileMapWidget(QWidget *parent) : QWidget(parent){
	currentRect_.setRect(0, 0, 0, 0);
	selectMode_ = 0;
	zoom_ = 0;
	currentIndex_ = -1;
}

TileMapWidget::~TileMapWidget(){

}

void TileMapWidget::init(const QImage& img, const QSize& grid, const QByteArray& collison){

	image_ = img;

	// 获取网格大小
	gridsize_ = grid;

	// 获取碰撞网格
	QDataStream stream(collison);
	stream >> collision_;

	updateBlock();
}


void TileMapWidget::updateBlock(){

	if (image_.width() == 0 || image_.height() == 0){
		return;
	}

	// 如果图片不能填满网格，则认为该TileBlock无效
	validBlock_.setWidth( image_.width() / gridsize_.width() );
	validBlock_.setHeight( image_.height() / gridsize_.height());

	setMinimumWidth(validBlock_.width() * gridsize_.width());
	setMinimumHeight(validBlock_.height() * gridsize_.height());

	currentRect_.setRect(0, 0, 0, 0);
}


void TileMapWidget::paintEvent(QPaintEvent * event){
	QPainter painter( this );

	qreal zoom = 1/qPow(2, zoom_);

	QTransform trans;
	trans.scale(zoom, zoom);
	painter.setTransform(trans,true);

	// 绘制纹理( 全部 )
	painter.drawImage(QPoint(0,0), image_ );

	painter.setPen(Qt::red);
	painter.fillRect(currentRect_, QColor(50, 50, 255, 100));
	painter.drawRect(currentRect_);

	// 绘制碰撞网格（需要优化）
	QRectF r = currentRect_.intersected(QRect(0, 0, minimumWidth(), minimumHeight()));
	if (!r.isNull()){
		// 计算行列上下限
		int col1 = r.left() / gridsize_.width();
		int col2 = col1 + r.width() / gridsize_.width() - 1;
		int row1 = r.top() / gridsize_.height();
		int row2 = row1 + r.height() / gridsize_.height() - 1;

		int index = -1;

		for (int i = row1; i <= row2; i++){
			for (int j = col1; j <= col2; j++){
				index = i*validBlock_.width() + j;  // 索引
				if (collision_.contains(index)){
					QList<QPointF> colrect = collision_[index];
					QPainterPath path;
					path.moveTo(j*gridsize_.width() + colrect[0].x() * gridsize_.width(), i *gridsize_.height() + colrect[0].y() * gridsize_.height());
					for (int k = 1; k < colrect.size(); k++){
						path.lineTo(j*gridsize_.width() + colrect[k].x() * gridsize_.width(), i *gridsize_.height() + colrect[k].y() * gridsize_.height());
					}
					// 闭合
					path.closeSubpath();
					painter.fillPath(path, QColor(0, 0, 0, 150));
				}
			}
		}

	}


	painter.drawRect(currentRect_);

}

void TileMapWidget::mouseDoubleClickEvent(QMouseEvent * event){
	if (event->button() == Qt::LeftButton){
		// 双击打开碰撞编辑窗口
	}
}

QRectF TileMapWidget::activeBlock(const QPoint& pos){
	qreal zoom = 1 / qPow(2, zoom_);

	QRectF rect;
	rect.setLeft((qRound(pos.x() / zoom)/ gridsize_.width())* gridsize_.width() );
	rect.setTop((qRound(pos.y() / zoom) / gridsize_.height())* gridsize_.height());

	if (rect.left() >= minimumWidth() || rect.top() >= minimumHeight()){
		rect.setRect(0, 0, 0, 0);
	}
	else{
		rect.setSize(gridsize_);
	}

	return rect;
}

void TileMapWidget::mousePressEvent(QMouseEvent * event){
	// 计算当前相对坐标，考察滚动
	if (event->button() == Qt::LeftButton){
		// 计算激活矩形
		currentRect_ = activeBlock(event->pos());
		startPos_ = event->pos();
		update();
	}
}

void TileMapWidget::mouseReleaseEvent(QMouseEvent * event){
	// 提交更新刷子
	if (event->button() == Qt::LeftButton){
		// 计算当前左上索引
		QRect r = currentRect_.toRect();
		if (r.width() == 0 || r.height() == 0){
			currentIndex_ = -1;
		}
		else{
			int col = currentRect_.left() / gridsize_.width();
			int row = currentRect_.top() / gridsize_.height();
			currentIndex_ = row * validBlock_.width() + col;
		}
		emit changeBrush( currentRect_.toRect());
	}
}

void TileMapWidget::mouseMoveEvent(QMouseEvent * event){
	if (event->buttons() &  Qt::LeftButton){

		if (selectMode_ == 1){
			qreal zoom = 1 / qPow(2, zoom_);

			QRect r(event->pos(), startPos_);
			r.setLeft(startPos_.x() / zoom);
			r.setTop(startPos_.y() / zoom);
			r.setRight(event->pos().x() / zoom);
			r.setBottom(event->pos().y() / zoom);
			r = r.normalized();

			r = r.intersected(QRect(0, 0, minimumWidth(), minimumHeight()));
			if (!r.isNull()){
				// 计算行列上下限
				int col1 = r.left() / gridsize_.width();
				int col2 = r.right() / gridsize_.width();
				int row1 = r.top() / gridsize_.height();
				int row2 = r.bottom() / gridsize_.height();


				currentRect_ = QRectF();

				for (int i = row1; i <= row2; i++){
					for (int j = col1; j <= col2; j++){
						QRectF sub = QRectF(j* gridsize_.width(), i* gridsize_.height(), gridsize_.width(), gridsize_.height());
						if (currentRect_.isNull()){
							currentRect_ = sub;
						}
						else{
							currentRect_ = currentRect_.united(sub);
						}
					}
				}
			}
			else{
				currentRect_ = QRectF();
			}
		}
		else{
			// 替换
			currentRect_ = activeBlock(event->pos());
		}
		
		update();
	}
}

void TileMapWidget::setSelectMode(int mode){
	selectMode_ = mode;
	if (selectMode_ == 0 && !currentRect_.isNull() ){
		currentRect_.setSize(gridsize_);
		update();
	}
}


void TileMapWidget::zoomOut(){
	if (zoom_ < 4){
		zoom_++;
		update();
	}
}

void TileMapWidget::zoomIn(){
	if (zoom_ > 0){
		zoom_--;
		update();
	}
}

void TileMapWidget::clear(){
	image_ = QImage();
	selectMode_ = 0;
	currentRect_ = QRectF(0,0,0,0);
	currentIndex_ = -1;
}

void TileMapWidget::updateCollision(const QByteArray& collison){
	// 获取碰撞网格
	QDataStream stream(collison);
	stream >> collision_;
	update();
}