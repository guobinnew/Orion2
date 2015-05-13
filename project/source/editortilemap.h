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
#ifndef  ORION_EDITORTILEMAP_H
#define ORION_EDITORTILEMAP_H

#include <QGraphicsItem>
#include <QFont>
#include <QVector3D>
#include <QMap>
#include <QDataStream>
#include <QByteArray>
#include <QPainterPath>
#include <QGraphicsPixmapItem>
#include "leveldefine.h"
#include "editorregion.h"
#include "resourceobject.h"
using namespace ORION;

class EditorTileBrush : public QGraphicsItem{
public:
	ORION_TYPE_DECLARE(LEVEL_TILEBRUSH)
	EditorTileBrush(QGraphicsItem * parent = 0);
	virtual ~EditorTileBrush();

	void setImage(const QImage& img);
	void setRect(const QRect& rect);
	// 更新状态
	void updateState(int state);
	//  重载函数
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

private:
	QRectF boundingBox_;
	QImage image_; // 图像
	QBitArray flip_;  //  状态
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

class EditorTileMap : public EditorRegion{
public:
	// 实体类型
	ORION_TYPE_DECLARE(LEVEL_TILEMAP)
	EditorTileMap(ObjectTypeResourceObject* prototype, QGraphicsItem * parent = 0);
	virtual ~EditorTileMap();

	// 绘制触发区域形状
	virtual void drawShape(QPainter *painter);
	// 刷新
	void postinit(bool resize = true);

	// 更新刷子
	void updateBrush(const QRect& region, int mode);

	// 编码
	virtual void encodeData(QDataStream& stream);
	virtual void decodeData(QDataStream& stream);
	// 缺省数据
	virtual void encodeDefault(QDataStream& stream);
	virtual void decodeDefault(QDataStream& stream);

	// 生成刷子数据
	void makeBrushData(const QRect& region);
	void addTileBlock(const QPoint& pos);

	QMap<QPair<int, int>, QImage> bufferImage_;  // 图像缓存
	QImage tileImage_;  // tile图像
	QSize grid_;  // 格子大小
	QMap<QPair<int,int>, QSize>  mapData_;  // width 索引，height为翻转状态（0，7）
	QList<QRect>  brushData_;  // 刷子数据
	QSize brushSize_;  // 刷子大小
	QBitArray state_;  // 翻转状态（0旋转颠倒，1翻转颠倒，  4水平翻转  5垂直翻转， 6 对角翻转 ）
	int mode_;  // 刷子模式

	bool isEditing_;  // 是否正在编辑
protected:
	virtual void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
	virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
	virtual void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
	virtual void dropEvent(QGraphicsSceneDragDropEvent * event);
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);
	virtual void	mousePressEvent(QGraphicsSceneMouseEvent * event);
	virtual void	mouseMoveEvent(QGraphicsSceneMouseEvent * event);
	virtual void	hoverMoveEvent(QGraphicsSceneHoverEvent * event);
	virtual void	hoverEnterEvent(QGraphicsSceneHoverEvent * event);
	virtual void	hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
	EditorTileBrush* brushItem_;  // 刷子

};


#endif // ORION_EDITORIMAGE_H
