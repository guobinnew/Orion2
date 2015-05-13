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
#include "editorlayer.h"
#include <QtGui>
#include <QDialog>
#include <QVector2D>
#include <math.h>
#include "levelscene.h"
#include "commondefine.h"
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;


EditorLayer::EditorLayer(LayerResourceObject* layer, QGraphicsItem * parent)
	:EditorItem(parent), layer_(layer){

	boundingBox_.setRect(0, 0, layer_->level_->size_.width(), layer_->level_->size_.height());
	isLocking_ = true;

	setSizeFlag(ANCHORFLAG_NONE);
	setFlag(ItemIsSelectable, false);
	setFlag(ItemIsMovable, false);
	setFlag(ItemIsFocusable, false);
}

EditorLayer::~EditorLayer(){

}


void EditorLayer::drawShape(QPainter *painter){

	if (layer_->transparent_)
		return;

	// 设置透明度
	painter->setOpacity(layer_->opacity_ / 100.0);
	// 背景填充
	painter->fillRect(boundingBox_.adjusted(1,1,-1,-1), layer_->bgColor_);
}

QString EditorLayer::layerName(){
	QString name;
	if (layer_){
		name = layer_->description();
	}
	return name;
}

void EditorLayer::addInstance(EditorRegion* instance){
	instances_.append(instance);
	instance->setParentItem(this);
}

void EditorLayer::removeInstance(EditorRegion* instance){
	instance->setParentItem(NULL);
	instances_.removeOne(instance);
}

void EditorLayer::resize(const QSizeF& newSize, bool flag ){
	prepareGeometryChange();
	boundingBox_.setRect(0, 0, newSize.width(), newSize.height());

	isAdjusting_ = true;
	updateAnchor();
	isAdjusting_ = false;

	if ( flag )
		update();
}

void EditorLayer::updateRegions(ObjectTypeResourceObject* obj){

	foreach(EditorRegion* region, instances_){
		if (region->typePrototype_ == obj){
			region->postinit(false);
			region->update();
		}
	}

}

void EditorLayer::moveFirst(EditorRegion* instance){
	instances_.removeOne(instance);
	instances_.append(instance);

	// 更新Zvalue
	int depth = EDITOR_DEPTH_DEFAULT;
	foreach(EditorRegion* region, instances_){
		region->setZValue(depth++);
	}

	update();
}

void EditorLayer::moveLast(EditorRegion* instance){
	instances_.removeOne(instance);
	instances_.prepend(instance);

	// 更新Zvalue
	int depth = EDITOR_DEPTH_DEFAULT;
	foreach(EditorRegion* region, instances_){
		region->setZValue(depth++);
	}

	update();
}