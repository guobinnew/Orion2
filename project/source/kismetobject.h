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
#ifndef  ORION_KISMTE_OBJECT_H
#define ORION_KISMTE_OBJECT_H

#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QFont>
#include<QJsonValue>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "kismetdefine.h"
#include "resourceobject.h"
using namespace ORION;

//////////////////////////////////////////////////////////////////////////
class BlueprintLink;
class BlueprintGroup;

class BlueprintObject  : public QGraphicsItem{
public:
	// 实体类型
	ORION_TYPE_DECLARE( KISMET_OBJECT );

	enum{
		OF_CAN_DELETE = 1,  //  可删除
		OF_NO_SAVE = 2,  // 不可保存
	};

	BlueprintObject(double depth = 0, QGraphicsItem * parent = 0);
	virtual ~BlueprintObject();

	ResourceHash iid(){
		return iid_;
	}

	void setIID(ResourceHash iid){
		iid_ = iid;
	}

	// 相对场景的中心
	virtual QPointF sceneCenter( void );

	//  获取标题名
	virtual QString title(){
		return QString();
	}

	// 类型标示
	virtual QString typeName(){
		return QString();
	}

	//  获取缩略图（64,64）
	virtual QImage thumbnail();

	// 获取控制点( -1表示不受控制)
	virtual QPointF controlPoint( int orient = -1 ){
		return QPointF();
	};

	// 添加连线
	void addLink(BlueprintLink *line);
	// 删除连线
	void removeLink(BlueprintLink *line);
	// 清除连线
	void clearLink();

	// 连线
	QList<BlueprintLink *> links() const{
		return lineList_;
	}

	bool hasLink(){
		return lineList_.size() > 0;
	}

	BlueprintGroup* eventGroup(){
		return evtgroup_;
	}

	void setEventGroup(BlueprintGroup* grp){
		evtgroup_ = grp;
	}

	//  简单绘制
	bool canSimpleDraw();

	void addGroup(BlueprintGroup* grp);

	//  重载函数
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

	virtual QColor linkColor(){
		return Qt::black;
	}

	// 输出编码
	QByteArray encode();
	void decode(QByteArray& data);

	// 编码
	virtual void encodeData(QDataStream& stream);
	virtual void decodeData(QDataStream& stream);

	virtual void postInit(){};
	// 获取表达式
	virtual QJsonValue expNode(int dt = -1){
		return QJsonValue();
	}
	// 自检
	virtual bool validate(QStringList& msg){
		isError_ = false;
		return !isError_;
	}

	unsigned int objectFlags() const{
		return objectflags_;
	}

	void setObjectFlags(unsigned int newFlags){
		objectflags_ |= newFlags;
	}

	void clearObjectFlags(unsigned int newFlags){
		objectflags_ &= ~newFlags;
	}

	bool canDelete(){
		return objectflags_ & OF_CAN_DELETE;
	}

	static QFont nameFont();
	static QFont textFont();
	static ResourceHash makeUniqueId();

	void setError(bool flag){
		isError_ = flag;
	}

	bool hasError(){
		return isError_;
	}

	// 状态枚举
	enum{
		BST_MOVE,
		BST_RESIZE,
		BST_DISPLAY,
	};

	virtual void backupState();
	virtual bool isStateChanged( int type );
	virtual void copyState(QDataStream& stream, int type);

protected:
	virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
	virtual QVariant itemChange ( GraphicsItemChange change, const QVariant & value );
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );	

protected:
	QRectF boundingBox_;
	QList<BlueprintLink *>    lineList_;
	BlueprintGroup* evtgroup_;  // 所在组
	ResourceHash iid_;  // 唯一ID
	int objectflags_;  // 标志
	unsigned int version_;  // 版本

	//  非持久
	bool isError_;  // 错误
	// undo
	QPointF oldPos_;  // 旧的位置
	QSizeF oldSize_;  // 旧的大小
	ResourceHash oldGroup_;  // 旧的事件组
};


#endif
