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
#include "editorsprite.h"
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

EditorSprite::EditorSprite(ObjectTypeResourceObject* prototype, QGraphicsItem * parent)
	:EditorRegion(prototype, parent){

	hotspot_ = QPointF(0.5, 0.5);
	boundingBox_.setRect(-32, -32, 64, 64);

	setAcceptDrops(true);
	// 必须是Sprite插件实例
	if (prototype){
		Q_ASSERT(hashStringLower(typePrototype_->prototype_->objectName()) == COMP_SPRITE);
		postinit();
	}
}

EditorSprite::~EditorSprite(){
}

void EditorSprite::drawShape(QPainter *painter){

	// 如果没有指定精灵，则绘制背景框
	// 否则绘制缺省动画的起始帧（未必是第1帧）
	if (typePrototype_->animation_ != NULL || typePrototype_->staticImage_ != NULL){

		painter->save();
		painter->setOpacity(opacity_);

		// 应用混合模式
		QPainter::CompositionMode blendMode_;  // 混合模式

		switch (blend_)
		{
		case FBT_ADDITIVE:
			blendMode_ = QPainter::CompositionMode_Plus;
			break;
		case FBT_XOR:
			blendMode_ = QPainter::CompositionMode_Xor;
			break;
		case FBT_COPY:
			blendMode_ = QPainter::CompositionMode_Source;
			break;
		case FBT_DEST_OVER:
			blendMode_ = QPainter::CompositionMode_DestinationOver;
			break;
		case FBT_SRC_IN:
			blendMode_ = QPainter::CompositionMode_SourceIn;
			break;
		case FBT_DEST_IN:
			blendMode_ = QPainter::CompositionMode_DestinationIn;
			break;
		case FBT_SRC_OUT:
			blendMode_ = QPainter::CompositionMode_SourceOut;
			break;
		case FBT_DEST_OUT:
			blendMode_ = QPainter::CompositionMode_DestinationOut;
			break;
		case FBT_SRC_ATOP:
			blendMode_ = QPainter::CompositionMode_SourceAtop;
			break;
		case FBT_DEST_ATOP:
			blendMode_ = QPainter::CompositionMode_DestinationAtop;
			break;
		default:
			blendMode_ = QPainter::CompositionMode_SourceOver;
			break;
		}

		painter->setCompositionMode(blendMode_);

		if (initImage_.isNull()){
			refreshImage();  // 重新初始化
		}
		painter->drawImage(boundingBox_, initImage_);
		painter->restore();
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

void	EditorSprite::dropEvent(QGraphicsSceneDragDropEvent * event){
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

void EditorSprite::refreshImage(){
	if (typePrototype_->animation_ != NULL){
		SpriteSequenceResourceObject* seq = getDefaultAnimation();
		if (seq){
			int index = getParameterInt(UDQ_T("InitFrame"), typePrototype_->prototype_->objectName());

			initImage_ = seq->frameImage(index);
			QPointF anchor = seq->frameCenterAnchor(index);
		}
	}
	else if (typePrototype_->staticImage_ != NULL){  // 静态图片的中心锚点始终为中心
		initImage_ = typePrototype_->staticImage_->image_;
	}
}

void EditorSprite::postinit(bool resize){
	// 设置大小
	QSize size(128, 128);
	QSizeF origin(0.5, 0.5);
	if (typePrototype_->animation_ != NULL){
		SpriteSequenceResourceObject* seq = getDefaultAnimation();
		if (seq){
			int index =getParameterInt(UDQ_T("InitFrame"), typePrototype_->prototype_->objectName());

			initImage_ = seq->frameImage(index);
			QPointF anchor = seq->frameCenterAnchor(index);

			origin.setWidth(anchor.x());
			origin.setHeight(anchor.y());
		}
	}
	else if (typePrototype_->staticImage_ != NULL){  // 静态图片的中心锚点始终为中心
		size = typePrototype_->staticImage_->image_.size();
		initImage_ = typePrototype_->staticImage_->image_;
	}

	prepareGeometryChange();

	if (resize){
		Q_ASSERT(size.width() > 0 && size.height() > 0);
		boundingBox_.setRect(-size.width() * hotspot_.x(), -size.height() * hotspot_.y(), size.width(), size.height());
	}
	else{
		boundingBox_.setRect(-boundingBox_.width() * hotspot_.x(), -boundingBox_.height() * hotspot_.y(), boundingBox_.width(), boundingBox_.height());
	}

	isAdjusting_ = true;
	updateAnchor();
	isAdjusting_ = false;
}

void EditorSprite::dragEnterEvent(QGraphicsSceneDragDropEvent * event){

	if (event->mimeData()->hasFormat(MIME_ORION_GALLERY)){
		setSelected(true);
		update();
		event->accept();
		return;
	}

	EditorRegion::dragEnterEvent(event);
}

void EditorSprite::dragLeaveEvent(QGraphicsSceneDragDropEvent * event){
	if (event->mimeData()->hasFormat(MIME_ORION_GALLERY)){
		setSelected(false);
		update();
		event->accept();
		return;
	}
	EditorRegion::dragLeaveEvent(event);
}

void EditorSprite::dragMoveEvent(QGraphicsSceneDragDropEvent * event){
	EditorRegion::dragMoveEvent(event);
}

SpriteSequenceResourceObject* EditorSprite::getDefaultAnimation(){

	Q_ASSERT(typePrototype_ != NULL );

	SpriteSequenceResourceObject* defseq = typePrototype_->getDefaultAnimation();
	SpriteSequenceResourceObject* seq = NULL;
	QString seqName = getParameterString(UDQ_T("InitSequence"), typePrototype_->prototype_->objectName());

	if (!seqName.isEmpty() && typePrototype_->animation_ ){
		seq = typePrototype_->animation_->sequence(seqName);
	}

	if (seq == NULL){
		seq = defseq;
	}

	return seq;
}
