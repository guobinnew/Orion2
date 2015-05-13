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

#ifndef  ORION_KISMET_LINK_H
#define ORION_KISMET_LINK_H

#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "kismetdefine.h"
#include "kismetobject.h"

//////////////////////////////////////////////////////////////////////////

class BlueprintLinkAnchor;

// 实现简单的直线功能
class BlueprintLink : public QGraphicsPathItem{  

public:
	// 实体类型
	ORION_TYPE_DECLARE( KISMET_LINK );

	enum{
		END_SOURCE,  // 输出端口
		END_DEST,  // 输入端口
	};

	BlueprintLink(BlueprintObject *sourceNode, BlueprintObject *destNode, bool temp = false);
	virtual ~BlueprintLink();

	BlueprintObject *sourceNode() const;
	void setSourceNode(BlueprintObject *node);

	BlueprintObject *destNode() const;
	void setDestNode(BlueprintObject *node);

	void setUnderMouse( bool flag = true ){
		underMouse_ = flag;
	}

	// 调整连线（包括控制点）
	virtual void adjust();

	// 断开所有连线
	virtual void unlink();

	// 绘制连线
	virtual void drawLine(QPainter *painter, const QStyleOptionGraphicsItem *option);

	// 编码
	virtual void encode( QDataStream& stream );

	// 更新临时线
	virtual void updateTempLink( const QPointF& pos );

	// 升级曲线
	virtual void upgradeLink(BlueprintObject *node);

	// 更新锚点
	virtual  QPointF updateAnchor( BlueprintLinkAnchor* anchor  );
	virtual  void removeAnchor(BlueprintLinkAnchor* anchor);
	// 更新端口
	virtual  void updatePort(BlueprintObject* node);

	// 计算端口锚点
	virtual QPointF portAnchorPoint(BlueprintObject* node);

	// 清除控制点
	virtual void clearAnchor();

	// 显示控制点
	virtual void showAnchor(bool visible = true);

	// 切换显示锚点
	virtual void toggleAnchor();

	// 端口类型检查
	virtual bool validate(QStringList& msg);

	// 控制点
	virtual void changeControlPoints(const QList<QPointF>& ctrlPts){
	}

	virtual QList<QPointF> controlPoints(){
		return QList<QPointF>();
	}
	virtual void copyState(QDataStream& stream);

	QPainterPath shape() const;
protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
	virtual void keyPressEvent ( QKeyEvent * event );
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);

protected:
	QByteArray encodeEnd(BlueprintObject* obj, bool tmpl = false);
	BlueprintObject *source_;
	BlueprintObject *dest_;

	// 临时使用
	QPointF sourcePoint_;
	QPointF destPoint_;

	bool tempLink_; // 是否为临时线
	bool editCtrl_;  //  是否进入编辑控制点状态
	bool underMouse_;
	//  非持久
	bool isError_;  // 错误
	QList<QPointF> oldCtrlPoints_;  // 旧的控制点
	unsigned int version_;  //  版本
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

class BlueprintLinkAnchor : public QGraphicsItem{
public:
	// 实体类型
	ORION_TYPE_DECLARE( KISMET_LINK_ANCHOR );

	BlueprintLinkAnchor(BlueprintLink *host, QGraphicsItem* parent = NULL);
	virtual ~BlueprintLinkAnchor();

	//  重载函数
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint( QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget );

protected:
	virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
	virtual void keyPressEvent ( QKeyEvent * event );
	virtual QVariant itemChange ( GraphicsItemChange change, const QVariant & value );

protected:
	BlueprintLink   *host_;   // 宿主
	QRectF  boundingBox_;     // 像素坐标		
	bool underMouse_; 

};



#endif
