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

#ifndef  ORION_KISMET_LINK_ARRAY_H
#define ORION_KISMET_LINK_ARRAY_H

#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QFont>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "kismetdefine.h"
#include "kismetobject.h"
#include "kismetlink.h"

//////////////////////////////////////////////////////////////////////////

class BlueprintLinkArrayAnchor;

class BlueprintLinkArray : public BlueprintLink{

public:
	// 实体类型
	ORION_TYPE_DECLARE( KISMET_LINK_ARRAY );

	BlueprintLinkArray(BlueprintObject *sourceNode, BlueprintObject *destNode, const QList<QPointF>& ctrlPts, bool temp = false);
	virtual ~BlueprintLinkArray();

	virtual void adjust();

	void setControlPoints( const QList<QPointF>& ctrlPts ){
		ctrlPoints_ = ctrlPts;
	}

	virtual void changeControlPoints(const QList<QPointF>& ctrlPts);

	// 添加控制点
	void addControlPoint( const QPointF& pt );

	// 更新临时线
	virtual void updateTempLink( const QPointF& pos );

	// 绘制曲线
	virtual void drawLine(QPainter *painter, const QStyleOptionGraphicsItem *option);

	// 编码
	virtual void encode( QDataStream& stream );
	
	// 升级曲线
	virtual void upgradeLink(BlueprintObject *node);

	// 更新锚点
	virtual  QPointF updateAnchor( BlueprintLinkAnchor* anchor  );
	virtual  void removeAnchor(BlueprintLinkAnchor* anchor);
	// 更新端口
	virtual  void updatePort(BlueprintObject* node);

	// 清除控制点
	virtual void clearAnchor();

	// 显示控制点
	virtual void showAnchor(bool visible = true);

	// 切换显示锚点
	virtual void toggleAnchor();

	QPainterPath shape() const;

	// 控制点
	virtual QList<QPointF> controlPoints(){
		return ctrlPoints_;
	}

protected:
	QList<QPointF> ctrlPoints_;  // 控制点列表
	QList<BlueprintLinkArrayAnchor*> ctrlAnchors_;  // 锚点列表
	void initAnchor();
	bool directline_;  // 是否采用直线

};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

class BlueprintLinkArrayAnchor : public BlueprintLinkAnchor{
public:
	// 实体类型
	ORION_TYPE_DECLARE( KISMET_LINK_ANCHOR_ARRAY );

	BlueprintLinkArrayAnchor(BlueprintLink* host, int type, QGraphicsItem* parent = NULL);
	virtual ~BlueprintLinkArrayAnchor();

	//  重载函数
	void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );


	void setAnchorIndex( int index ){
		anchorIndex_ = index;
	}

	int anchorIndex(){
		return anchorIndex_;
	}
private:
	int anchorIndex_;  // 控制点索引号

};



#endif
