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
#ifndef ORION_KISMET_MODEL_H
#define ORION_KISMET_MODEL_H

#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QFont>
#include <QList>
#include <QPushButton>

#include "kismetdefine.h"
#include "kismetobject.h"
#include "kismetlink.h"
#include "kismetmodelport.h"
#include "resourceobject.h"
using namespace ORION;


class BlueprintButtonControl : public QGraphicsProxyWidget{
	Q_OBJECT
public:
	// 实体类型
	ORION_TYPE_DECLARE(KISMET_MODEL_BUTTON);

	enum{
		BTN_ADD_IN = 1,
		BTN_ADD_OUT,
	};

	BlueprintButtonControl(int type, const QString& text, const QString& datatype, const QIcon& ico = QIcon(), const QString& prefix = QString(), QGraphicsItem * parent = 0);
	virtual ~BlueprintButtonControl();

	QPushButton* button(){
		return button_;
	}

public slots:
	void addInCtrlPort();
	void addOutCtrlPort();
	void addInDataPort();
	void addOutDataPort();

protected:
	void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
	// 初始化按钮
	void prepareWidget(const QString& text, const QIcon& ico);

	QPushButton* button_;  // 数据控件
	int type_;  // 动作类型
	QString datatype_;  // 数据类型
	QString prefix_;  // 端口名前缀

};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


class BlueprintModel  : public BlueprintObject{
public:
	// 实体类型
	ORION_TYPE_DECLARE( KISMET_MODEL );

	BlueprintModel(QGraphicsItem * parent = 0);
	BlueprintModel(ResourceHash pluginHash, int type, ResourceHash inter, QGraphicsItem * parent = 0);
	virtual ~BlueprintModel();

	// 编码
	virtual void encodeData(QDataStream& stream);
	virtual void decodeData(QDataStream& stream);

	virtual bool validate(QStringList& msg);

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

	int modelType(){
		return type_;
	}

	//////////////////////////////////////////////////////////////////////////

	void setPortValue(const QString& name, const QString& v);
	void setPortValue(const QString& name, int v);
	void setPortValue(const QString& name, double v);
	void setPortValue(const QString& name, bool v);

	//////////////////////////////////////////////////////////////////////////
	BlueprintModelPort* addCtrlInPort(const QString& port, int flags = 0);
	BlueprintModelPort* addCtrlOutPort(const QString& port, int flags = 0);

	BlueprintModelPort* addDataInPort(const QString& port, const QString& datatype, int flags = 0, const QStringList& items = QStringList());
	BlueprintModelPort* addDataOutPort(const QString& port, const QString& datatype, int flags = 0, const QStringList& items = QStringList());

	// 替换端口ID
	void changePortIID(BlueprintModelPort* port, ResourceHash iid);

	void removePort(BlueprintModelPort* port);
	void deletePort(BlueprintModelPort* port, QList<BlueprintModelPort*>& container);
	//////////////////////////////////////////////////////////////////////////
	// 新端口名
	virtual QString newPortName( int type, const QString& prefix = QString() );

	// 清除所有端口
	void removeAllPort();

	//////////////////////////////////////////////////////////////////////////
	// 初始化
	virtual void init();
	// 调整大小
	virtual void adjust();

	virtual QPointF logicInPos( int index = -1);
	virtual QPointF logicOutPos(  int index = -1);

	//  获取标题名
	virtual QString title();
	// 类型标示
	virtual QString typeName();
	//  获取缩略图（64, 64）
	virtual QImage thumbnail();

	void toggleLock();
	bool isLocking( void ){
		return isLocking_;
	}

	// 根据IID查找端口( 所有端口必须有名字，而且必须唯一)
	BlueprintModelPort* findPort(ResourceHash iid);
	BlueprintModelPort* findPort(const QString& name, int orient );

	BlueprintModelPort* inport(const QString& name);
	BlueprintModelPort* outport(const QString& name);
	BlueprintModelPort* inCtrlPort(const QString& name);
	BlueprintModelPort* outCtrlPort(const QString& name);

	// 清空所有端口连接
	void clearPortLink();

	// 端口名称列表
	QStringList  portNames();
	QStringList  inportNames();
	QStringList  outportNames();

	// 更新端口
	void updatePort();

	// 准备端口
	void prepareCtrlPort(PluginInterface* inter);
	// 准备缺省端口
	void prepareDefPort(PluginInterface* inter);
	// 准备参数端口
	void prepareInPort(PluginVariable* var);
	// 控制按钮端口
	void prepareButtonPort(PluginInterface* inter);

	// 控制端口
	QList<BlueprintModelPort*>  inCtrlPorts_;  // 默认为1个，
	QList<BlueprintModelPort*>  outCtrlPorts_;  // 默认为1个，可能有多个
	// 数据端口
	QList<BlueprintModelPort*>  inPorts_;
	QList<BlueprintModelPort*>  outPorts_;

	QList<BlueprintButtonControl*> inButtons_;  // 输入端按钮
	QList<BlueprintButtonControl*> outButtons_;  // 输出端按钮

	// 返回所有端口
	QList<BlueprintModelPort*> ports(){
		return iidPorts_.values();
	}

	// 获取Port的表达式
	QJsonArray portExpNode(BlueprintModelPort* port = NULL);
	QJsonValue objTypeNode();

	//////////////////////////////////////////////////////////////////////////
	// 下游模型是否包含指定类型
	bool hasLinkModel(QList<BlueprintModelPort*> ports, int type, int subtype);
	bool hasDownStream(int type, int subtype = 0);
	// 上游模型是否包含指定类型
	bool hasUpStream(int type, int subtype = 0);

	// 获取下游模型
	BlueprintModel* downStreamCtrl( const QString& name );
	// 上游模型列表
	QList<BlueprintModel*> upstreamCtrl();
	QList<BlueprintModel*> upstreamCtrl(int type, int subtype = 0);

	// 查找对应输入端口
	QList<BlueprintModelPort*> modelInPort(BlueprintModel* model);
	QList<BlueprintModelPort*> modelOutPort(BlueprintModel* model);

	PluginResourceObject* pluginObject(){
		return plugin_;
	}

	ResourceHash interfaceId(){
		return interfaceID_;
	}

	PluginInterface* getInterface();

	//  是否为内部端口
	bool isInternalPort(BlueprintModelPort* port);

protected:
	virtual QVariant itemChange ( GraphicsItemChange change, const QVariant & value );
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
protected:
	QColor tilteColor();
	void drawLock( QPainter* painter );
	void drawPreview(QPainter* painter);

	QColor color_;//
	bool isLocking_; // 是否锁定

	QMap<ResourceHash, BlueprintModelPort*> iidPorts_; 

	int bomPos_; // 底层端口位置
	QImage lockImage_;

	PluginResourceObject* plugin_; // 所属插件
	BlueprintModelPort* objPort_;  // 内部对象端口
	BlueprintModelPort* behPort_;  // 内部行为端口
	int type_;  // 类型( 条件、动作和表达式 )
	QString title_;  // 标题
	ResourceHash interfaceID_;  // ID
	PluginInterface* interface_;   // 接口定义

};


#endif
