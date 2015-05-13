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
#include "editorlevel.h"
#include <QtGui>
#include <QDialog>
#include <QVector2D>
#include <math.h>
#include "levelscene.h"
#include "commondefine.h"
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;

const int LEVEL_FONTSIZE = 12;

EditorLevelText::EditorLevelText(const QString& text, QPointF pos, QGraphicsItem *parent) : QGraphicsSimpleTextItem(parent){

	setZValue(LEVEL_DEPTH_PROFILE);
	setFlags(ItemIgnoresTransformations);
	setText(text);
	setPos(pos);
	setBrush(Qt::black);

	QFont font;
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPixelSize(LEVEL_FONTSIZE);
	setFont(font);
	setVisible(false);

}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


EditorLevel::EditorLevel(LevelResourceObject* level, QGraphicsItem * parent)
	:EditorItem( parent), level_(level), fillColor_(LEVEL_FILLCOLOR), marginfillColor_(LEVELMARGIN_FILLCOLOR){

	boundingBox_.setRect(0, 0, level->size_.width(), level->size_.height());

	isLocking_ = true;
	canflip_ = false;
	setSizeFlag(ANCHORFLAG_RESIZE_R);
	
	setFlag(QGraphicsItem::ItemIsMovable, false);
	setFlag(QGraphicsItem::ItemIsFocusable, false);
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	
	text_ = new EditorLevelText(QString(), QPointF(0, 0), this);
}

EditorLevel::~EditorLevel(){
}

void EditorLevel::drawMargin(QPainter *painter){
	// 绘制四个margin 块
	QRectF bound = boundingRect();

	bound.adjust(-level_->margin_.width(), -level_->margin_.height(),
		level_->margin_.width(), level_->margin_.height());

	painter->fillRect(bound, marginfillColor_);

}
void EditorLevel::drawShape(QPainter *painter){

	// 填充margin
	drawMargin(painter);

	QPen borderPen;
	// 绘制边框颜色
	borderPen.setColor(BORDER_COLOR);
	borderPen.setStyle(Qt::SolidLine);
	painter->setPen(borderPen);
	// 绘制矩形
	painter->drawRect(boundingBox_);
}

void EditorLevel::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event){

}


QVariant EditorLevel::itemChange(GraphicsItemChange change, const QVariant &value){

	if (change == QGraphicsItem::ItemSelectedChange  && scene()) {
		bool flag = value.toBool();
		if (!flag){
			setFlag(QGraphicsItem::ItemIsSelectable, false);
		}
		text_->setVisible(flag);
		updateText();
	}

	return EditorItem::itemChange(change, value);
}

QString  EditorLevel::currentTitleString(){
	return QString(UDQ_TR("尺寸: %1x%2   边宽: %3,%4  旋转: %5度"))
		.arg(qRound(boundingBox_.width()))
		.arg(qRound(boundingBox_.height()))
		.arg(level_->margin_.width())
		.arg(level_->margin_.height())
		.arg(qRound(rotation()));
}

void EditorLevel::adjustText(){
	//  根据旋转角度找到最右边
	QList<QPointF> pts;
	pts.append(boundingBox_.topLeft());
	pts.append(boundingBox_.topRight());
	pts.append(boundingBox_.bottomLeft());
	pts.append(boundingBox_.bottomRight());

	QPointF bottomleft;
	for (int i = 0; i < pts.size(); i++) {
		QPointF mappt = mapToScene(pts[i]);
		if (i == 0 || mappt.x() < bottomleft.x()){
			bottomleft.setX(mappt.x());
		}

		if (i == 0 || mappt.y() > bottomleft.y()){
			bottomleft.setY(mappt.y());
		}
	}

	text_->setPos(mapFromScene(bottomleft));
}

void EditorLevel::updateText(){
	if (text_->isVisible()){
		text_->setText(currentTitleString());
		adjustText();
	}
}

void EditorLevel::updateChildren(){
	updateText();
	level_->size_ = boundingBox_.size().toSize();

	// 更新所有图层大小
	((LevelScene*)scene())->resizeAllLayer(boundingBox_.size());
}

bool EditorLevel::toggleSelectable(){
	bool flag = !(flags() & QGraphicsItem::ItemIsSelectable);
	setFlag(QGraphicsItem::ItemIsSelectable, flag);
	return flag;
}


void EditorLevel::refresh(){
	resize(level_->size_);
}

void EditorLevel::resize(const QSizeF& newSize, bool flag){
	prepareGeometryChange();
	boundingBox_.setRect(0, 0, newSize.width(), newSize.height());

	isAdjusting_ = true;
	updateAnchor();
	isAdjusting_ = false;

	if (flag)
		update();
}