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

#include "pathwidget.h"
#include "commondefine.h"
#include "configmanager.h"
#include <qmath.h>

const int TEXT_MARGIN = 8;
const int BUTTON_SPACE = 4;
const int BUTTON_HEAD = 16;


PathWidget::PathWidget(QWidget *parent)
	: QWidget(parent), disableMode_(false){
	bgColor_ = QColor(50,50,50,255);
	hoverColor_ = QColor(Qt::darkGray);
	color_ = QColor(Qt::white);
	setMouseTracking(true);
	adjust();
}

PathWidget::~PathWidget(){

}

void PathWidget::paintEvent(QPaintEvent * event){
	QPainter painter( this );
	painter.setRenderHint( QPainter::SmoothPixmapTransform, true );
	painter.setRenderHint(QPainter::Antialiasing);

	// 填充背景
	QRect box = rect();
	painter.fillRect( box, bgColor_ );

	drawButtons(painter);
}

void PathWidget::adjust(){

	QSize s = rect().size();
	s.setHeight(s.height() - TEXT_MARGIN);

	rootButton_.rect.setTop(TEXT_MARGIN / 2);
	rootButton_.rect.setLeft(TEXT_MARGIN / 2);
	if (rootButton_.title.isEmpty()){
		rootButton_.rect.setWidth(24);
	}
	else{
		QFontMetrics metric(textFont());
		QSize size = metric.size(Qt::TextSingleLine, rootButton_.title);
		rootButton_.rect.setWidth(size.width() + TEXT_MARGIN);
	}
	rootButton_.rect.setHeight(s.height());

	//
	int xpos = rootButton_.rect.right() + TEXT_MARGIN;
	for (int i = 0; i < buttonBounds_.size(); i++){
		buttonBounds_[i].rect.moveLeft(xpos);

		if (buttonBounds_[i].rect.height() == 0){  // 没有调整过
			if (i == 0){
				xpos += buttonBounds_[i].rect.width();
				buttonBounds_[i].rect.setWidth(buttonBounds_[i].rect.width() + BUTTON_HEAD);
			}
			else{
				xpos += buttonBounds_[i].rect.width() + BUTTON_HEAD;
				buttonBounds_[i].rect.setWidth(buttonBounds_[i].rect.width() + BUTTON_HEAD * 2);
			}
			buttonBounds_[i].rect.setHeight(s.height());
		}
		else{
			xpos = buttonBounds_[i].rect.right() - BUTTON_HEAD;
		}

	}

}

void	PathWidget::resizeEvent(QResizeEvent * event){
	adjust();
	QWidget::resizeEvent(event);
}

void	PathWidget::mouseMoveEvent(QMouseEvent * event){

	if (!disableMode_){

		currentMousePos_ = event->pos();

		if (rootButton_.rect.contains(currentMousePos_)){
			rootButton_.state = 1;
		}
		else{
			rootButton_.state = 0;
		}

		//判断是否与按钮相交
		bool found = false;
		for (int i = 0; i < buttonBounds_.size(); i++){
			QRect rect = buttonBounds_[i].rect;
			if (rect.contains(currentMousePos_) && !found){
				buttonBounds_[i].state = 1;
				found = true;
			}
			else{
				buttonBounds_[i].state = 0;
			}
		}
	
		update();
	}

	QWidget::mouseMoveEvent(event);
}

void	PathWidget::mousePressEvent(QMouseEvent * event){

	if (!disableMode_){
		currentMousePos_ = event->pos();

		if (rootButton_.state == 1){
			rootButton_.state = 2;

			buttonBounds_.clear();
			emit changePath(currentPath());
		}
		else{
			for (int i = 0; i < buttonBounds_.size(); i++){
				if (buttonBounds_[i].state == 1){
					buttonBounds_[i].state = 2;

					while (buttonBounds_.size() > i + 1){  // 移除后面的按钮
						buttonBounds_.removeLast();
					}

					emit changePath(currentPath());

					break;
				}
			}
		}
		update();
	}

	QWidget::mousePressEvent(event);
}

void PathWidget::mouseReleaseEvent(QMouseEvent * event){
	if (!disableMode_){

		if (rootButton_.state == 2){
			rootButton_.state = 1;
		}
		else{
			for (int i = 0; i < buttonBounds_.size(); i++){
				if (buttonBounds_[i].state == 2){
					buttonBounds_[i].state = 1;
					break;
				}
			}
		}
		update();
	}

	QWidget::mouseReleaseEvent(event);
}

void PathWidget::setRoot(const QString& root, const QString& loc){
	buttonBounds_.clear();
	rootButton_.text = loc;
	rootButton_.title = root;
	adjust();
}

void PathWidget::enterPath(const QString& subPath){
	PathButton btn;
	setButton(subPath, btn);
	buttonBounds_.append(btn);
	adjust();
}

void PathWidget::setButton(const QString& text, PathButton& btn){

	btn.text = text;
	btn.state = 0;

	if (btn.text.length() > 24){
		btn.title.append(text.left(5));
		btn.title.append(UDQ_T("..."));
		btn.title.append(text.right(5));
	}
	else{
		btn.title = text;
	}

	QFontMetrics metric(textFont());
	QSize size = metric.size(Qt::TextSingleLine, btn.title);
	btn.rect = QRect(0, TEXT_MARGIN / 2, size.width() + TEXT_MARGIN, 0);
}


void PathWidget::drawButtons(QPainter& painter){
	painter.setFont(textFont());

	// 绘制拷贝路径按钮
	drawButton(painter, rootButton_);

	// 绘制路径按钮
	for (int i = buttonBounds_.size() -1; i >= 0; i-- ){
		drawPathButton(painter, buttonBounds_[i], i == 0);
	}

}

void PathWidget::drawButton(QPainter& painter, PathButton& button){
	QColor fillColor = bgColor_;
	if (button.state == 1){
		fillColor = hoverColor_;
	}
	else if (button.state == 2){
		fillColor = color_;
	}
	painter.fillRect(button.rect, fillColor);

	// 绘制拷贝路径按钮
	painter.setPen(Qt::gray);
	painter.drawRect(button.rect);

	// 绘制文字
	if (!button.title.isEmpty()){
		painter.setPen(button.state == 2 ? Qt::black : Qt::white);
		painter.drawText(button.rect, Qt::AlignCenter, button.title);
	}

}

void PathWidget::drawPathButton(QPainter& painter, PathButton& button, bool root){

	QPainterPath path;
	path.moveTo(button.rect.topLeft());
	path.lineTo(button.rect.right() - BUTTON_HEAD, button.rect.top());
	path.lineTo(button.rect.right(), button.rect.center().y());
	path.lineTo(button.rect.right() - BUTTON_HEAD, button.rect.bottom());
	path.lineTo(button.rect.bottomLeft());
	path.closeSubpath();

	QColor fillColor = bgColor_;
	if (button.state == 1){
		fillColor = hoverColor_;
	}
	else if (button.state == 2){
		fillColor = color_;
	}
	painter.fillPath(path, fillColor);

	// 绘制拷贝路径按钮
	painter.setPen(Qt::gray);
	painter.drawPath(path);

	// 绘制文字
	if (!button.text.isEmpty()){
		painter.setPen(button.state == 2 ? Qt::black : Qt::white);
		QRect textRect = button.rect.adjusted(BUTTON_HEAD, 0, -BUTTON_HEAD, 0);
		if (root){
			textRect.adjust(-BUTTON_HEAD, 0, 0, 0);
		}
		painter.drawText(textRect, Qt::AlignCenter, button.title);
	}
}

void PathWidget::leaveEvent(QEvent * event){
	rootButton_.state = 0;
	for (int i = 0; i < buttonBounds_.size(); i++){
		buttonBounds_[i].state = 0;
	}

	update();
	QWidget::leaveEvent(event);
}

// 获取当前路径
QString PathWidget::currentPath(){
	QString path;
	for (int i = 0; i < buttonBounds_.size(); i++){
		path.append(buttonBounds_[i].text + UDQ_T("/"));
	}

	if (path.size() > 0){
		path.chop(1); // 去掉末尾的"/”
	}

	return path;
}

QString PathWidget::currentRoot(){
	return rootButton_.title;
}
QString PathWidget::currentLocation(){
	return rootButton_.text;
}


void PathWidget::setEnable(bool flag ){
	disableMode_ = !flag;

	if (disableMode_){
		rootButton_.state = 0;
		for (int i = 0; i < buttonBounds_.size(); i++){
			buttonBounds_[i].state = 0;
		}
	}
}


QFont PathWidget::textFont(void){
	QFont font;
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPixelSize(14);
	font.setFamily(gConfigManager->baseFont());
	font.setBold(true);
	return font;
}

