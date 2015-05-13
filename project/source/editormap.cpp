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
#include "editormap.h"
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

EditorMap::EditorMap(ObjectTypeResourceObject* prototype, QGraphicsItem * parent)
	:EditorRegion(prototype, parent){

	boundingBox_.setRect(0, 0, 128, 128);
	hotspot_ = QPointF(0, 0);
	canflip_ = false;
	tileImage_.load(UDQ_T(":/images/chinamap.png"));
	setAcceptDrops(false);
	setSizeFlag(ANCHORFLAG_RESIZE);
	
}

EditorMap::~EditorMap(){
}

void EditorMap::drawShape(QPainter *painter){

	// 绘制图标
	painter->drawImage(boundingBox_, tileImage_);

	// 如果是选择状态
	if (isSelected()){
		QPen pen(Qt::red);
		painter->setPen(pen);
		painter->drawRect(boundingBox_);
	}

}

void EditorMap::postinit(bool resize){

}
