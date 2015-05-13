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

#ifndef  ORION_KISMET_LINK_SINGLE_H
#define ORION_KISMET_LINK_SINGLE_H

#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QFont>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "kismetdefine.h"
#include "kismetobject.h"
#include "kismetlink.h"

//////////////////////////////////////////////////////////////////////////

class BlueprintLinkSingleAnchor;

class BlueprintLinkSingle : public BlueprintLink{
public:
	// 实体类型
	ORION_TYPE_DECLARE( KISMET_LINK_SINGLE );

	BlueprintLinkSingle(BlueprintObject *sourceNode, BlueprintObject *destNode, const QPointF& srcPt = QPointF(0, 0), const QPointF& desPt = QPointF(0, 0), bool temp = false);
	virtual ~BlueprintLinkSingle();

	void adjust();

	// 绘制曲线
	virtual void drawLine(QPainter *painter, const QStyleOptionGraphicsItem *option);

	// 编码
	virtual void encode( QDataStream& stream );

	// 升级曲线
	virtual void upgradeLink(BlueprintObject *node);

	// 更新锚点
	virtual  QPointF updateAnchor( BlueprintLinkAnchor* anchor  );
	// 更新端口
	virtual  void updatePort(BlueprintObject* node);

	// 清除控制点
	virtual void clearAnchor();

	// 显示控制点
	virtual void showAnchor(bool visible = true);

	// 切换显示锚点
	virtual void toggleAnchor();

	QPainterPath shape() const;

	void setSourceCtrlPoint( const QPointF& pt ){
		sourceCtrlPoint_ = pt;
	}

	void setDestCtrlPoint( const QPointF& pt ){
		destCtrlPoint_ = pt;
	}

	// 控制点
	virtual void changeControlPoints(const QList<QPointF>& ctrlPts);
	virtual QList<QPointF> controlPoints();

protected:
	QPointF sourceCtrlPoint_;
	QPointF destCtrlPoint_;

	BlueprintLinkSingleAnchor* sourceAnchor_;
	BlueprintLinkSingleAnchor* destAnchor_;

	void initAnchor();

};


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

class BlueprintLinkSingleAnchor : public BlueprintLinkAnchor{
public:
	// 实体类型
	ORION_TYPE_DECLARE( KISMET_LINK_ANCHOR_SINGLE );

	BlueprintLinkSingleAnchor(BlueprintLink* host, int type, QGraphicsItem* parent = NULL);
	virtual ~BlueprintLinkSingleAnchor();
	

	int anchorType(){
		return anchorType_;
	}

protected:
	int anchorType_;  // 控制点类型
};



#endif
