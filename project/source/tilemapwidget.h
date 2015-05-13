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
#ifndef TILEMAPWIDGET_H
#define TILEMAPWIDGET_H

#include <QWidget>
#include "resourceobject.h"
using namespace ORION;

class TileMapWidget : public QWidget{
	Q_OBJECT

public:
	TileMapWidget(QWidget *parent = 0);
	~TileMapWidget();

	void init(const QImage& img, const QSize& grid, const QByteArray& collison );
	void updateBlock();
	void updateCollision(const QByteArray& collison);

	// 清除
	void clear();

	// 改变选择模式
	void setSelectMode(int mode);

	// 缩放
	void zoomOut();
	void zoomIn();

	QRect activeRegion(){
		return currentRect_.toRect();
	}

	
	int currentIndex(){
		return currentIndex_;
	}

protected:
	virtual void paintEvent ( QPaintEvent * event );
	virtual void mouseDoubleClickEvent ( QMouseEvent * event );
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);

signals:
	void changeBrush(const QRect& region );// 0表示添加，1表示删除

private:
	QImage image_;
	QSize gridsize_;  // 网格大小
	QMap<int, QList<QPointF> > collision_; // 碰撞网格
	int selectMode_;  // 选择模式0单选，1框选

	QSize validBlock_;   // 有效范围, 每次变换gridsize或图片时重新生成
	QRectF currentRect_;  // 当前活动Tile块
	int currentIndex_;   // 当前选择的块索引，如果选择多个块，则返回左上角块

	QRectF activeBlock(const QPoint& pos);
	QPoint startPos_;  // 框选起点

	int zoom_;  // 缩放比例，只能缩小



};

#endif // TILEMAPWIDGET_H
