/************************************************************************
**
**  Copyright (C) 2014  guobin  <guobin.gb@alibaba-inc.com>
**
**  This file is part of OSSSnake.
**
** OSSSnake is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation.
**
**  OSSSnake is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with OSSSnake.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#include "lockwidget.h"
#include "commondefine.h"
#include <qmath.h>

const int TEXT_MARGIN = 8;
const int BUTTON_YPOS = 8;
const int BUTTON_SIZE = 64;
const int TOUCH_RANGE = 4;
const int BUTTON_ROW = 3;
const int BUTTON_COL = 3;

LockWidget::LockWidget(QWidget *parent) 
	: QWidget(parent), lockMode_(false), disableMode_(false){
	bgColor_ = QColor(50,50,50,255);
	color_ = QColor(Qt::lightGray);
	highlightColor_ = QColor(63, 72, 204, 255);
	adjust();
}

LockWidget::~LockWidget(){

}

void LockWidget::paintEvent(QPaintEvent * event){
	QPainter painter( this );
	painter.setRenderHint( QPainter::SmoothPixmapTransform, true );
	painter.setRenderHint(QPainter::Antialiasing);

	// 填充背景
	QRect box = rect();
	painter.fillRect( box, bgColor_ );

	drawButtons(painter);
	drawLines(painter);

}

void LockWidget::adjust(){
	QSize s = rect().size();
	s.setHeight(s.height() - BUTTON_YPOS);

	buttonBounds_.clear();

	if (s.width() < BUTTON_SIZE*BUTTON_COL || s.height() < BUTTON_SIZE * BUTTON_ROW){
	}
	else{

		int colWidth = s.width() / BUTTON_COL;
		int rowHeight = s.height() / BUTTON_ROW;

		QRect bound;
		// 计算按钮包围盒
		for (int i = 0; i < 9; i++){
			int row = i / 3;
			int col = i % 3;

			bound.setLeft(col*colWidth + (colWidth - BUTTON_SIZE) / 2);
			bound.setRight(col*colWidth + (colWidth + BUTTON_SIZE) / 2);

			bound.setTop(BUTTON_YPOS + row*rowHeight + (rowHeight - BUTTON_SIZE) / 2);
			bound.setBottom(BUTTON_YPOS + row*rowHeight + (rowHeight + BUTTON_SIZE) / 2);

			buttonBounds_.append(bound);
		}

	}
}

void	LockWidget::resizeEvent(QResizeEvent * event){
	adjust();
	QWidget::resizeEvent(event);
}

//  绘制按钮
void LockWidget::drawButtons(QPainter& painter){
	QPen pen;
	pen.setColor(Qt::white);
	pen.setWidthF(1.5);
	painter.setPen(pen);

	QPainterPath pathBound;
	QPainterPath pathCenter;
	QPainterPath highlightBound;

	for (int i = 0; i < buttonBounds_.size(); i++){
		QRect bound = buttonBounds_.at(i);
		pathBound.addEllipse(bound);
		pathCenter.addEllipse(bound.adjusted(26, 26, -26, -26));

		if (lockActions_.contains(i)){
			highlightBound.addEllipse(bound);
		}
	}

	painter.fillPath(highlightBound, highlightColor_);
	painter.drawPath(pathBound);
	painter.fillPath(pathCenter, QBrush(color_));

}


// 绘制连线
void LockWidget::drawLines(QPainter& painter){

	if (lockActions_.size() <= 0)
		return;

	QPen pen;
	pen.setColor(Qt::cyan);
	pen.setWidthF(6);
	painter.setPen(pen);

	QPainterPath path;
	for (int i = 0; i < lockActions_.size(); i++){
		if (i == 0){
			path.moveTo(buttonBounds_.at(lockActions_[i]).center());
		}
		else{
			path.lineTo(buttonBounds_.at(lockActions_[i]).center());
		}
	}

	if ( lockMode_ ){
		path.lineTo(currentMousePos_);
	}
	
	painter.drawPath(path);
}


void	LockWidget::mouseMoveEvent(QMouseEvent * event){
	if (lockMode_){
		currentMousePos_ = event->pos();

		// 判断是否与按钮相交
		for (int i = 0; i < buttonBounds_.size();i++){
			if (lockActions_.contains(i)){
				continue;
			}

			QRect bound = buttonBounds_.at(i);
			QPoint pt = currentMousePos_ - bound.center();
			qreal dist = qSqrt(pt.x() * pt.x() + pt.y() * pt.y());
			
			if (dist < BUTTON_SIZE / 2 + TOUCH_RANGE){
				lockActions_.append(i);
				break;
			}	
		}
		update();
	}

	QWidget::mouseMoveEvent(event);
}

void	LockWidget::mousePressEvent(QMouseEvent * event){

	if (!disableMode_){
		currentMousePos_ = event->pos();
		lockMode_ = true;
		lockActions_.clear();
		update();
	}

	QWidget::mousePressEvent(event);
}

void	LockWidget::mouseReleaseEvent(QMouseEvent * event){
	if (!disableMode_){
		lockMode_ = false;
		emit inputPwd(toSignature());
		update();
	}

	QWidget::mouseReleaseEvent(event);
}

// 将动作转变为签名
QByteArray LockWidget::toSignature(){

	if (lockActions_.size() < 3){  // 长度不能小于3
		return QByteArray();
	}

	QString str;
	foreach(int i, lockActions_){
		str.append(QString::number(i));
	}

	QByteArray data = QCryptographicHash::hash(str.toLatin1(), QCryptographicHash::Md5);
	return data.toBase64();
}

// 重置
void LockWidget::setLock(bool flag){
	disableMode_ = !flag;
	lockMode_ = false;
	lockActions_.clear();
	update();
}
