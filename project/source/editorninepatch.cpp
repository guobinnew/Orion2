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
#include "editorninepatch.h"
#include <QtGui>
#include <QDialog>
#include <QVector2D>
#include <math.h>
#include "levelscene.h"
#include "commondefine.h"
#include "componentdefine.h"
#include "resourcelinker.h"
#include "spritedefine.h"
using namespace ORION;

EditorNinePatch::EditorNinePatch(ObjectTypeResourceObject* prototype, QGraphicsItem * parent)
	:EditorRegion(prototype, parent){

	boundingBox_.setRect(0, 0, 128, 128);
	canflip_ = false;
	borderFill_ = 1;
	centerFill_ = 1;
	seam_ = 1;
	setAcceptDrops(true);
	// 必须是TileBG插件实例
	if (prototype){
		Q_ASSERT(hashStringLower(typePrototype_->prototype_->objectName()) == COMP_NINEPATCH);
		postinit();
	}
}

EditorNinePatch::~EditorNinePatch(){
}

void EditorNinePatch::drawShape(QPainter *painter){

	painter->setOpacity(opacity_);
	// 如果没有指定背景，则绘制背景框
	if (typePrototype_->staticImage_ != NULL){
		if (boundingBox_.width() > 0 && boundingBox_.height() > 0){
			painter->setClipping(true);
			painter->setClipRect(boundingBox_);

			QRectF rect;
			//  左上角
			rect.setRect(boundingBox_.left(), boundingBox_.top(), margin_.left(), margin_.top());
			fillImage(painter, PATCH_LT, rect, borderFill_);

			// 顶
			rect.setRect(boundingBox_.left() + margin_.left(), boundingBox_.top(), boundingBox_.width() - margin_.left() - margin_.right(), margin_.top());
			fillImage(painter, PATCH_TOP, rect, borderFill_);

			// 右上角
			rect.setRect(boundingBox_.right() - margin_.right(), boundingBox_.top(), margin_.right(), margin_.top());
			fillImage(painter, PATCH_RT, rect, borderFill_);

			// 左
			rect.setRect(boundingBox_.left(), boundingBox_.top() + margin_.top(), margin_.left(), boundingBox_.height() - margin_.top() - margin_.bottom());
			fillImage(painter, PATCH_LEFT, rect, borderFill_);

			// 中
			rect.setRect(boundingBox_.left() + margin_.left(), boundingBox_.top() + margin_.top(), 
				boundingBox_.width() - margin_.left() - margin_.right(), boundingBox_.height() - margin_.top() - margin_.bottom());
	
			if (centerFill_ == 2){
				if (rect.width() >0 && rect.height() > 0 )
					painter->fillRect(rect, SPRITE_FILLCOLOR);
			}
			else{
				fillImage(painter, PATCH_CENTER, rect, centerFill_);
			}

			// 右
			rect.setRect(boundingBox_.right() - margin_.right(), boundingBox_.top() + margin_.top(), margin_.right(), boundingBox_.height() - margin_.top() - margin_.bottom());
			fillImage(painter, PATCH_RIGHT, rect, borderFill_);

			// 左下角
			rect.setRect(boundingBox_.left(), boundingBox_.bottom() - margin_.bottom(), margin_.left(),  margin_.bottom());
			fillImage(painter, PATCH_LB, rect, borderFill_);

			// 底
			rect.setRect(boundingBox_.left() + margin_.left(), boundingBox_.bottom() - margin_.bottom(),  boundingBox_.width() - margin_.left() - margin_.right(), margin_.bottom());
			fillImage(painter, PATCH_BOTTOM, rect, borderFill_);

			// 右下角
			rect.setRect(boundingBox_.right() - margin_.right(), boundingBox_.bottom() - margin_.bottom(), margin_.right(), margin_.bottom());
			fillImage(painter, PATCH_RB, rect, borderFill_);
		}
	}
	else{
		painter->fillRect(boundingBox_, SPRITE_FILLCOLOR);
	}

	// 如果是选择状态
	if (isSelected()){
		QPen pen(Qt::red);
		painter->setPen(pen);
		painter->drawRect(boundingBox_);
	}

}

void	EditorNinePatch::dropEvent(QGraphicsSceneDragDropEvent * event){
	if (event->mimeData()->hasFormat(MIME_ORION_GALLERY)){
		QByteArray itemData = event->mimeData()->data(MIME_ORION_GALLERY);
		QDataStream stream(&itemData, QIODevice::ReadOnly);

		LevelScene* level = (LevelScene*)scene();
		// 获取索引号
		int index;
		stream >> index;
		Q_ASSERT(index == 1);
		ResourceHash key;
		stream>>key;

		if (typePrototype_->setImageResource(key)){
			postinit();
			update();

			// 通知刷新对象类型
			level->notifyUpdateObjectType(this);
		}

		event->accept();
	}

	EditorRegion::dropEvent(event);
}

void EditorNinePatch::postinit(bool resize){

	QSizeF size = boundingBox_.size();
	prepareGeometryChange();

	// 锚点
	int index = getParameterEnum(UDQ_T("Hotspot"), typePrototype_->prototype_->objectName());
	switch (index)
	{
	case 0:  // 左上角
	{
		hotspot_ = QPointF(0, 0);
	}
	break;
	case 1:  // 
	{
		hotspot_ = QPointF(0.5, 0);
	}
	break;
	case 2:
	{
		hotspot_ = QPointF(1, 0);
	}
	break;
	case 3:
	{
		hotspot_ = QPointF(0, 0.5);
	}
	break;
	case 4:
	{
		hotspot_ = QPointF(1, 0.5);
	}
	break;
	case 5:
	{
		hotspot_ = QPointF(0, 1);
	}
	break;
	case 6:
	{
		hotspot_ = QPointF(0.5, 1);
	}
	break;
	case 7:
	{
		hotspot_ = QPointF(1, 1);
	}
	break;
	default:
	{
		hotspot_ = QPointF(0.5, 0.5);
	}
	break;
	}

	boundingBox_.setRect(-size.width() * hotspot_.x(), -size.height() * hotspot_.y(), size.width(), size.height());

	// 边框大小
	QString margin = getParameterString(UDQ_T("LeftMargin"), typePrototype_->prototype_->objectName());
	margin_.setLeft(margin.toFloat());
	margin = getParameterString(UDQ_T("RightMargin"), typePrototype_->prototype_->objectName());
	margin_.setRight(margin.toFloat());
	margin = getParameterString(UDQ_T("TopMargin"), typePrototype_->prototype_->objectName());
	margin_.setTop(margin.toFloat());
	margin = getParameterString(UDQ_T("BottomMargin"), typePrototype_->prototype_->objectName());
	margin_.setBottom(margin.toFloat());

	// 分割图片
	if (typePrototype_->staticImage_ != NULL){  // 静态图片的中心锚点始终为中心
		splitImage(typePrototype_->staticImage_->image_);
	}

	borderFill_ = getParameterEnum(UDQ_T("Edges"), typePrototype_->prototype_->objectName());
	centerFill_ = getParameterEnum(UDQ_T("Fill"), typePrototype_->prototype_->objectName());
	seam_ = getParameterEnum(UDQ_T("Seams"), typePrototype_->prototype_->objectName());

	isAdjusting_ = true;
	updateAnchor();
	isAdjusting_ = false;
}
QImage EditorNinePatch::copyImage(QImage& img, QRect& rect){

	if (rect.left() < 0){
		rect.setLeft(0);
	}

	if (rect.top() < 0){
		rect.setTop(0);
	}

	if (rect.right() > img.width()){
		rect.setRight(img.width());
	}

	if (rect.bottom() > img.height()){
		rect.setBottom(img.height());
	}

	if (rect.width() == 0 || rect.height() == 0){
		return QImage();
	}

	return img.copy(rect);

}
void EditorNinePatch::splitImage(QImage& img){
	patchImages_.clear();
	// 左上
	QRect rect;
	//  左上角
	rect.setRect(0, 0, margin_.left(), margin_.top());
	patchImages_.insert(PATCH_LT, copyImage(img, rect));

	// 顶
	rect.setRect(margin_.left(),0, img.width() - margin_.left() - margin_.right(), margin_.top());
	patchImages_.insert(PATCH_TOP, copyImage(img, rect));

	// 右上角
	rect.setRect( img.width() - margin_.right(), 0, margin_.right(), margin_.top());
	patchImages_.insert(PATCH_RT, copyImage(img, rect));

	// 左
	rect.setRect(0, margin_.top(), margin_.left(), img.height() - margin_.top() - margin_.bottom());
	patchImages_.insert(PATCH_LEFT, copyImage(img, rect));

	// 中
	rect.setRect(margin_.left(), margin_.top(), img.width() - margin_.left() - margin_.right(), img.height() - margin_.top() - margin_.bottom());
	patchImages_.insert(PATCH_CENTER, copyImage(img, rect));

	// 右
	rect.setRect(img.width() - margin_.right(), margin_.top(), margin_.right(), img.height() - margin_.top() - margin_.bottom());
	patchImages_.insert(PATCH_RIGHT, copyImage(img, rect));

	// 左下角
	rect.setRect(0, img.height() - margin_.bottom(), margin_.left(), margin_.bottom());
	patchImages_.insert(PATCH_LB, copyImage(img, rect));

	// 底
	rect.setRect( margin_.left(), img.height() - margin_.bottom(), img.width() - margin_.left() - margin_.right(), margin_.bottom());
	patchImages_.insert(PATCH_BOTTOM, copyImage(img, rect));

	// 右下角
	rect.setRect(img.width() - margin_.right(), img.height() - margin_.bottom(), margin_.right(), margin_.bottom());
	patchImages_.insert(PATCH_RB, copyImage(img, rect));

}

void EditorNinePatch::fillImage(QPainter* painter, int pos, const QRectF& rect, int style){
	if (rect.width() == 0 || rect.height() == 0){
		return;
	}
	QImage img = patchImages_.value(pos);
	if (img.isNull()){
		painter->fillRect(rect, SPRITE_FILLCOLOR);
		return;
	}

	if (style == 1){  // 拉伸
		painter->drawImage(rect, img);
	}
	else{
		painter->fillRect(rect,  img);
	}



}

void EditorNinePatch::dragEnterEvent(QGraphicsSceneDragDropEvent * event){

	if (event->mimeData()->hasFormat(MIME_ORION_GALLERY)){
		setSelected(true);
		update();
		event->accept();
		return;
	}

	EditorRegion::dragEnterEvent(event);
}

void EditorNinePatch::dragLeaveEvent(QGraphicsSceneDragDropEvent * event){
	if (event->mimeData()->hasFormat(MIME_ORION_GALLERY)){
		setSelected(false);
		update();
		event->accept();
		return;
	}
	EditorRegion::dragLeaveEvent(event);
}

void EditorNinePatch::dragMoveEvent(QGraphicsSceneDragDropEvent * event){
	EditorRegion::dragMoveEvent(event);
}
