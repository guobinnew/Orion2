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
#ifndef ORION_EDITORITEM_H
#define ORION_EDITORITEM_H

#include <QGraphicsItem>
#include <QGraphicsBlurEffect>
#include <QFont>
#include <QVector3D>
#include <QDataStream>
#include <QByteArray>
#include <QPainterPath>
#include "editordefine.h"
#include "resourceobject.h"
using namespace ORION;

class EditorItem;

class EditorAnchor : public QGraphicsItem{
public:
	// 实体类型
	ORION_TYPE_DECLARE(EDITOR_ANCHOR);

	EditorAnchor(EditorItem *host, int index, QGraphicsItem* parent = NULL);
	virtual ~EditorAnchor();

	//  重载函数
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

	void setAnchorIndex(int index){
		anchorIndex_ = index;
	}

	int anchorIndex(){
		return anchorIndex_;
	}
private:

protected:
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);

protected:
	EditorItem   *host_;   // 宿主
	QRectF  boundingBox_;     // 像素坐标		
	bool underMouse_;
	int anchorIndex_;  // 控制点索引号
	bool hostIsLock_;  

	QByteArray oldValues_;

};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
// 基础区域单元（区域中可以显示一个触发器或素材图片，视频播放器、音频播放器或其他）
class EditorItem : public QGraphicsItem{
	friend class EditorAnchor;

public:
	// 实体类型
	ORION_TYPE_DECLARE(EDITOR_ITEM)

	EditorItem(QGraphicsItem * parent = 0);
	EditorItem(const QRectF& rect, const QSizeF& origin = QSizeF(0.5,0.5), QGraphicsItem * parent = 0);
	virtual ~EditorItem();

	ResourceHash iid(){
		return iid_;
	}

	//  重载函数
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

	// 输出编码
	QByteArray encode( bool def = false );
	void decode( QByteArray& data, bool def =false );

	// 绘制触发区域形状
	virtual void drawShape(QPainter *painter);
	// 编码
	virtual void encodeData(QDataStream& stream);
	virtual void decodeData(QDataStream& stream);
	// 清理
	virtual void clear(){};
	// 更新孩子
	virtual void updateChildren(){}
	// 后初始化
	virtual void postinit(bool resize = true);

	// 缺省数据
	virtual void encodeDefault(QDataStream& stream);
	virtual void decodeDefault(QDataStream& stream);

	// 锁定
	bool isLocking();
	void toggleLock();

	// 设置标志
	void setSizeFlag(int flag){
		sizeFlags_ = flag;
	}
	
	virtual void resize(const QSizeF& newSize, bool update = true);

	// 返回支持的锚点ID
	QList<int> anchors();
	// 返回锚点位置
	QPointF anchorPos(int id, const QRectF& bound );

	// 更新锚点
	virtual  QPointF updateAnchor(int id, const QPointF& pos);
	
	QRectF selectBound();
	// 清除控制点
	virtual void clearAnchor();

	bool initAnchor();
	void anchorChanged(int id, const QPointF& pos);
	void showAnchor(bool visible);
	void hideAnchor();
	void updateAnchor(const QList<int>& except = QList<int>());

	void setAlpha(qreal a);

	qreal getAlpha(){
		return opacity_;
	}

	static ResourceHash makeUniqueId();
	void adjustSize(QSize& newSize, bool keepAspect = true);
	QPointF rotateAnchor();

	void cacheAspect(){
		oldAspect_ = boundingBox_.size();
	}

protected:
	virtual void keyPressEvent(QKeyEvent * event);
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
protected:
	QRectF  boundingBox_;  // 像素坐标
	bool isLocking_;  // 是否锁定
	int sizeFlags_;  // 改变大小标志
	ResourceHash iid_;  // 唯一ID
	int anchorId_;  // 当前anchorid
	GraphicsItemFlags oldFlags_;
	QPointF hotspot_;  // 锚点位置（0,0左上角，0.5,0.5中心）
	bool canflip_;  // 图形是否支持反转（宽高为负）

	qreal opacity_;  // 透明度
	unsigned int version_;  // 数据版本

	bool isAdjusting_;  // 正在调整位置
	QSizeF oldAspect_;  // 缩放比例
	// 初始化
	void init();
	QMap<int, EditorAnchor*> anchors_;  // anchor对象

	QByteArray oldValues_;
};


#endif // ORION_EDITORREGION_H
