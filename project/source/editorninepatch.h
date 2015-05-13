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
#ifndef  ORION_EDITOR9PATCH_H
#define ORION_EDITOR9PATCH_H

#include <QGraphicsItem>
#include <QFont>
#include <QVector3D>
#include <QMap>
#include <QDataStream>
#include <QByteArray>
#include <QPainterPath>
#include "leveldefine.h"
#include "editorregion.h"
#include "resourceobject.h"
using namespace ORION;

class EditorNinePatch : public EditorRegion{
public:
	// 实体类型
	ORION_TYPE_DECLARE(LEVEL_9PATCH)
	EditorNinePatch(ObjectTypeResourceObject* prototype, QGraphicsItem * parent = 0);
	virtual ~EditorNinePatch();

	// 绘制触发区域形状
	virtual void drawShape(QPainter *painter);
	// 刷新
	void postinit(bool resize = true);

	// 切割图片
	void splitImage(QImage& img);
	QImage copyImage(QImage& img, QRect& rect);
	// 填充图片
	void fillImage(QPainter* painter, int pos, const QRectF& rect, int style = 1);

	QRectF margin_;  // 边框大小
	int borderFill_;  // 边框填充模式（0Tile1拉伸）
	int centerFill_; // 中心填充模式（0Tile,1拉伸）
	int seam_;  // 边界缝隙处理
	enum{
		PATCH_LT = 0,
		PATCH_TOP,
		PATCH_RT,
		PATCH_LEFT,
		PATCH_CENTER,
		PATCH_RIGHT,
		PATCH_LB,
		PATCH_BOTTOM,
		PATCH_RB,
	};
	QMap<int, QImage> patchImages_;  // patch图片

protected:
	virtual void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
	virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
	virtual void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
	virtual void dropEvent(QGraphicsSceneDragDropEvent * event);

};


#endif // ORION_EDITORIMAGE_H
