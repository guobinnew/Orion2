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

#ifndef  ORION_KISMET_MODELPORT_H
#define ORION_KISMET_MODELPORT_H

#include <QFont>
#include <QList>
#include <QGraphicsProxyWidget>

#include "kismetdefine.h"
#include "kismetobject.h"
#include "kismetlink.h"

class BlueprintModelPort;

class BlueprintControl : public QGraphicsProxyWidget{
public:
	// 实体类型
	ORION_TYPE_DECLARE(KISMET_MODEL_PORT_WIDGET);

	BlueprintControl(const QString& datatype, const QStringList& enums = QStringList(), QGraphicsItem * parent = 0);
	virtual ~BlueprintControl();
	//  重载函数
	QRectF boundingRect() const;
	void paintWindowFrame(QPainter *painter, const QStyleOptionGraphicsItem *option,
		QWidget *widget);

	// 获取数据值
	QString value();
	// 设置数据值
	void setValue(const QString& v);

	// 是否为combo类型
	bool isComboType();

	// 获取值索引，仅对枚举类型有效
	int valueIndex(const QString& v);

protected:
	void	hoverEnterEvent(QGraphicsSceneHoverEvent * event);
	void	hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

	void prepareWidget(const QString& datatype, const QStringList& enums = QStringList());
	QWidget* widget_;  // 数据控件
	QString datatype_; 
	QStringList enums_;
};



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

class  BlueprintModelPort : public BlueprintObject{
public:
	// 实体类型
	ORION_TYPE_DECLARE( KISMET_MODEL_PORT );

	enum PortType{
		EVT_CTRL_IN = 1,
		EVT_CTRL_OUT = 2,
		EVT_DATA_IN = 7,
		EVT_DATA_OUT  = 8,
	};

	BlueprintModelPort(int orient, const QString& datatype, const QString&name = QString(), const QStringList& enums = QStringList(), QGraphicsItem * parent = 0);
	virtual ~BlueprintModelPort();

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

	// 含子控件的包围盒
	QRectF ctrlBoundBox();
	// 是否为无效名
	bool isNullName();

	// 获取控制点
	virtual QPointF controlPoint( int orient = -1 );
	// 获取点
	virtual QPointF endPoint( void );

	// 检查端口配对
	virtual bool checkPortPair(BlueprintModelPort* port);
	virtual bool validate(QStringList& msg);
	// 更新链接
	void updateLink();
	// 更新颜色
	void updateDataType();

	// 设置宽度
	void setWidth( int width ){
		textWidth_ = width;
	}

	int getWidth( void ){
		return textWidth_;
	}

	// 设置名称
	void setName( const QString& name ){
		name_ = name;
	};

	QString name( void){
		return name_;
	}

	void setDescription( const QString& text ){
		desp_ = text;
	};

	QString description( void){
		return desp_;
	}

	//  获取标题名
	virtual QString title(){
		return name_;
	}

	int orient(){
		return orient_;
	}

	void setUnderMouse( bool flag );

	// 初始准备
	void prepare();
	// 调整大小位置
	void adjust();

	// 数据类型颜色
	QColor portColor(const QString& dt);

	virtual QColor linkColor(){
		return portcolor_;
	}

	QString getValue();
	void setValue(const QString& v);


	QString dataType(){
		return dataType_;
	}

	int dataTypeId();

	QString orientString();

	// 获取表达式
	virtual QJsonValue expNode(int dt = -1);

	virtual void copyState(QDataStream& stream, int type);

protected:
	virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
	virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
	virtual QVariant itemChange ( GraphicsItemChange change, const QVariant & value );
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
protected:
	QString name_;
	int orient_; // 朝向
	QString dataType_;  // 数据类型
	qreal oldZ_;  // 旧Z

	QColor portcolor_; // 颜色

	bool underMouse_; // 颜色
	bool underLink_; // 是否进行连线
	QString desp_;  // 注释

	QPainterPath path_;  // 绘制图形
	BlueprintControl* ctrl_;   // 控件子对象

	BlueprintLink* tempLink_; // 临时线
	QList<QPointF> tempLineCtrlPts_; // 临时控制点

	int textWidth_; //文本宽度
};

#endif
