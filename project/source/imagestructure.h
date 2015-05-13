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

#ifndef ORION_IMAGESTRUCTURE_H
#define ORION_IMAGESTRUCTURE_H

#include <QGraphicsItem>
#include <QFont>

#include "spritedefine.h"
#include "resourceobject.h"
using namespace ORION;

class ImageStructure : public QGraphicsItem{

public:
	// 实体类型
	ORION_TYPE_DECLARE(SPRITE_IMAGE)

	ImageStructure(double width, double height, double depth = DEPTH_SPRTIE_IMAGE);
	virtual ~ImageStructure();

	void setBoundingRect( const QRectF& newRect );
	//  重载函数
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

	// 设置原始图像
	void setImage(const QImage& img);

	// 更新图像翻转状态
	void updateState(const QBitArray& state);

	// 调整
	void adjust();
	// 
	void bufferImage();

	//  图像状态
	QBitArray imageState(){
		return state_;
	}

	// 翻转后图像大小
	QSize imageSize();
	// 翻转后图像
	QImage getImage(bool trans = true);

	QPointF offset_;  //  起始位置

private:
	QRectF  boundingBox_;     // 像素坐标		
	QImage  tile_;  //
	QImage  buffer_;  //  缓存
	QBitArray state_;  // 状态

};


#endif // ORION_IMAGESTRUCTURE_H
