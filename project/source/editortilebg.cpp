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
#include "editortilebg.h"
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

EditorTileBG::EditorTileBG(ObjectTypeResourceObject* prototype, QGraphicsItem * parent)
	:EditorRegion(prototype, parent){
	boundingBox_.setRect(0, 0, 128, 128);
	setAcceptDrops(true);
	// 必须是TileBG插件实例
	if (prototype){
		Q_ASSERT(hashStringLower(typePrototype_->prototype_->objectName()) == COMP_TILEDBG);
		postinit();
	}
}

EditorTileBG::~EditorTileBG(){
}

void EditorTileBG::drawShape(QPainter *painter){

	painter->setOpacity(opacity_);
	// 如果没有指定背景，则绘制背景框
	if (typePrototype_->staticImage_ != NULL){
		if ( boundingBox_.width() > 0 && boundingBox_.height() > 0 )
			painter->fillRect(boundingBox_, tileImage_);  // 用刷子
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

void	EditorTileBG::dropEvent(QGraphicsSceneDragDropEvent * event){
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
		}
	
		event->accept();
	}

	EditorRegion::dropEvent(event);
}

void EditorTileBG::postinit(bool resize){

	if (typePrototype_->staticImage_ != NULL){  // 静态图片的中心锚点始终为中心
		tileImage_ = typePrototype_->staticImage_->image_;
	}

	QSizeF size = boundingBox_.size();
	prepareGeometryChange();
	int hotspot = getParameterEnum(UDQ_T("Hotspot"), typePrototype_->prototype_->objectName());
	if (hotspot == 0){
		hotspot_ = QPointF(0, 0);
	}
	else{
		hotspot_ = QPointF(0.5, 0.5);
	}
	boundingBox_.setRect(-size.width() * hotspot_.x(), -size.height() * hotspot_.y(), size.width(), size.height());

	isAdjusting_ = true;
	updateAnchor();
	isAdjusting_ = false;
}

void EditorTileBG::dragEnterEvent(QGraphicsSceneDragDropEvent * event){

	if (event->mimeData()->hasFormat(MIME_ORION_GALLERY)){
		setSelected(true);
		update();
		event->accept();
		return;
	}

	EditorRegion::dragEnterEvent(event);
}

void EditorTileBG::dragLeaveEvent(QGraphicsSceneDragDropEvent * event){
	if (event->mimeData()->hasFormat(MIME_ORION_GALLERY)){
		setSelected(false);
		update();
		event->accept();
		return;
	}
	EditorRegion::dragLeaveEvent(event);
}

void EditorTileBG::dragMoveEvent(QGraphicsSceneDragDropEvent * event){
	EditorRegion::dragMoveEvent(event);
}

