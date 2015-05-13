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
#include "editortext.h"
#include <QtGui>
#include <QDialog>
#include <QVector2D>
#include <math.h>
#include "levelscene.h"
#include "commondefine.h"
#include "componentdefine.h"
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;

EditorText::EditorText(ObjectTypeResourceObject* prototype, QGraphicsItem * parent)
	:EditorRegion(prototype, parent){
	canflip_ = false;

	if (prototype){
		Q_ASSERT(hashStringLower(typePrototype_->prototype_->objectName()) == COMP_TEXT);
		postinit();
	}
}

EditorText::~EditorText(){
}

void EditorText::drawShape(QPainter *painter){
	painter->setOpacity(opacity_);
	painter->setFont(font_);
	painter->setPen(color_);

	// 绘制文字
	painter->drawText(boundingBox_, flags_, text_);
}

// 刷新
void EditorText::postinit(bool resize){
	// 设置大小

	text_ = getParameterString(UDQ_T("Text"), typePrototype_->prototype_->objectName());

	QString fontStr = getParameterString(UDQ_T("Font"), typePrototype_->prototype_->objectName());
	if (fontStr.size() >= 0){ 
		 font_.fromString(fontStr);
	}
	else{
		font_ = defaultFont();
	}

	QString clr = getParameterString(UDQ_T("Color"), typePrototype_->prototype_->objectName());
	color_ = QColor(clr);

	flags_ = 0;
	int tmpstr = getParameterEnum(UDQ_T("Wrapping"), typePrototype_->prototype_->objectName());
	if (tmpstr == 0 ){
		flags_ |= Qt::TextWordWrap;
	}

	tmpstr = getParameterEnum(UDQ_T("HorizontalAlignment"), typePrototype_->prototype_->objectName());
	if (tmpstr ==0){
		flags_ |= Qt::AlignLeft;
	}
	else if (tmpstr == 2){
		flags_ |= Qt::AlignRight;
	}
	else{
		flags_ |= Qt::AlignHCenter;
	}

	tmpstr = getParameterEnum(UDQ_T("VerticalAlignment"), typePrototype_->prototype_->objectName());
	if (tmpstr ==0){
		flags_ |= Qt::AlignTop;
	}
	else if (tmpstr == 2){
		flags_ |= Qt::AlignBottom;
	}
	else{
		flags_ |= Qt::AlignVCenter;
	}



	if (text_.isEmpty()){
		text_ = UDQ_TR("空白文字");
	}

	QSizeF size = boundingBox_.size();
	if (resize){
		// 调整大小
		size = computeText( flags_, text_, font_);
	}

	prepareGeometryChange();
	int hotspot = getParameterEnum(UDQ_T("Hotspot"), typePrototype_->prototype_->objectName());
	if (hotspot == 0 ){
		hotspot_  = QPointF(0,0);
	}
	else{
		hotspot_ = QPointF(0.5, 0.5);
	}
	boundingBox_.setRect(-size.width() * hotspot_.x(), -size.height() * hotspot_.y(), size.width(), size.height());
	isAdjusting_ = true;
	updateAnchor();
	isAdjusting_ = false;
}

void EditorText::encodeData(QDataStream& stream){
	EditorRegion::encodeData(stream);

	stream << text_;
	stream << font_.toString();
	stream << color_;
	stream << lineHeight_;

}

void EditorText::decodeData(QDataStream& stream){
	EditorRegion::decodeData(stream);

	stream >> text_;

	QString fntstr;
	stream >> fntstr;
	font_.fromString(fntstr);

	stream >> color_;
	stream >> lineHeight_;

}

// 缺省数据
void EditorText::encodeDefault(QDataStream& stream){
	EditorRegion::encodeDefault(stream);

	stream << text_;
	stream << font_.toString();
	stream << color_;
	stream << lineHeight_;
}

void EditorText::decodeDefault(QDataStream& stream){
	EditorRegion::decodeDefault(stream);

	stream >> text_;

	QString fntstr;
	stream >> fntstr;
	font_.fromString(fntstr);

	stream >> color_;
	stream >> lineHeight_;
}


//////////////////////////////////////////////////////////////////////////

QFont EditorText::defaultFont(){
	QFont font;
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPixelSize(24);
	font.setFamily(UDQ_L8("宋体"));
	return font;
}
