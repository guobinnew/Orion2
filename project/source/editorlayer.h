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
#ifndef ORION_EDITORLAYER_H
#define ORION_EDITORLAYER_H

#include "editoritem.h"
#include <QGraphicsBlurEffect>
#include <QGraphicsSimpleTextItem>
#include <QFont>
#include <QVector3D>
#include <QDataStream>
#include <QByteArray>
#include <QPainterPath>
#include "leveldefine.h"
#include "editorregion.h"
#include "resourceobject.h"
using namespace ORION;

// 基础区域单元（区域中可以显示一个触发器或素材图片，视频播放器、音频播放器或其他）
class EditorLayer : public EditorItem{
public:
	// 实体类型
	ORION_TYPE_DECLARE(LEVEL_LAYER)

	EditorLayer(LayerResourceObject* layer, QGraphicsItem * parent = 0);
	virtual ~EditorLayer();

	// 绘制触发区域形状
	virtual void drawShape(QPainter *painter);
	virtual void resize(const QSizeF& newSize, bool update = true);

	// 图层名
	QString layerName();

	LayerResourceObject* layer_;

	void addInstance(EditorRegion* instance);
	void removeInstance(EditorRegion* instance);
	// 根据类型来更新对象
	void updateRegions(ObjectTypeResourceObject* obj);
	QList<EditorRegion*>   instances_;  // 实例列表

	void moveFirst(EditorRegion* instance);
	void moveLast(EditorRegion* instance);
};


#endif // ORION_EDITORREGION_H
