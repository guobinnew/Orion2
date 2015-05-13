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

#include "kismetcomment.h"
#include <QtGui>
#include <math.h>
#include "commondefine.h"
#include "kismetcommenteditwindow.h"


const int COMMENT_MINWIDTH =  120;
const int COMMENT_MINHEIGHT = 20;

BlueprintComment::BlueprintComment(const QString& text, QGraphicsItem * parent) :
BlueprintObject(KISMET_DEPTH_COMMENT, parent),comment_(text), textColor_(Qt::black){
	adjust();
	setVisible(true);
}

BlueprintComment::~BlueprintComment(){

}

QPainterPath BlueprintComment::shape() const{
	QPainterPath path;
	path.addRect( boundingBox_ );
	return path;
}

void BlueprintComment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	Q_UNUSED(widget);

	if( isSelected()){
		QPen borderPen;
		// 绘制选择框颜色
		borderPen.setColor( Qt::yellow );
		borderPen.setStyle(Qt::DashLine);
		painter->setPen( borderPen );
		// 绘制矩形
		painter->drawRect( boundingBox_.toRect() );
	}

	// 绘制文本
	QFont txtFont = nameFont();
	painter->setFont( txtFont);
	painter->setPen( textColor_ );

	// 多行显示
	QStringList lines = comment_.split("\n");
	if (lines.size() > 1){
		int ypos = (boundingBox_.height() - txtFont.pixelSize() * lines.size()) / 2;
		foreach(QString txt, lines){
			if (!txt.isEmpty()){
				painter->drawText(boundingBox_.left(), boundingBox_.top() + ypos + txtFont.pixelSize(), txt);
			}
			ypos += txtFont.pixelSize();
		}
	}
	else{  // 单行显示
		painter->drawText(boundingBox_.left(), boundingBox_.top() + (boundingBox_.height() + txtFont.pixelSize())/2, comment_);
	}
	
}

// 编码
void BlueprintComment::encodeData(QDataStream& stream){
	BlueprintObject::encodeData(stream);

	stream<<comment_;
	stream<<textColor_;
}

void BlueprintComment::decodeData(QDataStream& stream){
	BlueprintObject::decodeData(stream);

	stream >> comment_;
	stream >> textColor_;
	adjust();
}

// 更改文本
void BlueprintComment::changeText(const QString& text){
	comment_ = text;
	adjust();
}

// 追加文本
void BlueprintComment::appendText(const QString& text){
	comment_.append( text );
	adjust();
}

// 调整尺寸
void BlueprintComment::adjust(){
	prepareGeometryChange();
	boundingBox_ = QRectF(0,0,COMMENT_MINWIDTH, COMMENT_MINHEIGHT);

	if( !comment_.isEmpty() ){
		// 计算文字区域
		QFontMetrics metric(  nameFont() );
		QSize size = metric.size(Qt::TextExpandTabs, comment_ );

		if( size.width() < COMMENT_MINWIDTH )
			size.setWidth( COMMENT_MINWIDTH );

		if( size.height() < COMMENT_MINHEIGHT )
			size.setHeight( COMMENT_MINHEIGHT );
		else{
			size.setHeight( size.height() + 4 );
		}
		boundingBox_.setSize( size );
	}

}

void	BlueprintComment::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event){
	// 编辑注释信息
	backupState();

	KismetCommentEditWindow w(this);
	w.exec();
}


void BlueprintComment::copyState(QDataStream& stream, int type){
	BlueprintObject::copyState(stream, type);
	if (type == BST_DISPLAY){
		stream << oldComment_ << oldColor_ ;
	}

}

void BlueprintComment::backupState(){
	BlueprintObject::backupState();
	oldComment_ = comment_;
	oldColor_ = textColor_;

}

bool BlueprintComment::isStateChanged(int type){
	bool result = BlueprintObject::isStateChanged(type);

	if (result){
		return true;
	}

	if (type == BST_DISPLAY){
		return (oldComment_ != comment_) || (oldColor_ != textColor_);
	}

	return false;
}