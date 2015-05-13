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

#ifndef ORION_IMAGESCENE_H
#define ORION_IMAGESCENE_H

#include <QGraphicsScene>
#include <QMap>
#include <QPixmap>
#include "imagestructure.h"
#include "imageanchor.h"
#include "imagecollision.h"

#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;

class ImageScene : public QGraphicsScene{
	Q_OBJECT

public:

	enum PREVIEW_MODE{
		POM_POINTER = 0,     // 初始状态（选择, 移动）
		POM_ANCHOR,
		POM_COLLISION,
	};

	ImageScene(QObject *parent = 0);
	virtual ~ImageScene();

	PREVIEW_MODE getMode( void ){
		return mapMode_;
	}

	// 获取可见区域（场景坐标系）
	QRectF viewRegionRect( void );

	ImageStructure* getStructure(){
		return structure_;
	}

	// 设置锚点位置
	void setAnchor( const QPointF& pos );
	void setImage(const QImage& img, const QBitArray& state );
	void setCollision(const QList<QPointF>& pts);

	void setAnchorVisible(bool flag);
	void setCollisionVisible(bool flag);

	void clearImage();

	void save(SequenceFrame* frame, const QString& anchor);

	// 获取碰撞网格
	QList<QPointF> collisions();

	// 恢复缺省网格
	void setDefaultCollision();

protected:
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	virtual void drawBackground ( QPainter * painter, const QRectF & rect );
	virtual void keyPressEvent ( QKeyEvent * keyEvent );

public slots:
	void setMode( PREVIEW_MODE mode );

private:
	// 初始化
	void init();
	QImage gridImage_;  // 背景网格
	ImageStructure* structure_;  // 结构图
	ImageAnchor* anchor_;
	ImageCollision* collision_;
	PREVIEW_MODE   mapMode_;    // 地图模式

};


#endif /*SCENARIOSCENE_H_*/
