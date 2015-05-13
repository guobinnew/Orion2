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

#include "kismetinstanceindex.h"
#include <QtGui>
#include <math.h>
#include "kismetdefine.h"
#include "kismetscene.h"
#include "tooldefine.h"


BlueprintInstanceIndex::BlueprintInstanceIndex(QGraphicsItem * parent) : BlueprintModel(parent){
	prepare();
	adjust();
}

BlueprintInstanceIndex::~BlueprintInstanceIndex(){

}

void BlueprintInstanceIndex::init(){

}

void BlueprintInstanceIndex::prepare(){
	removeAllPort();

	title_ = UDQ_TR("表达式: 对象成员索引");
	color_ = EXPRESS_COLOR;

	// 准备数据输入
	hostPort_ = new BlueprintModelPort(BlueprintModelPort::EVT_DATA_IN, UDQ_T("object"), UDQ_T("object"));
	inPorts_.append(hostPort_);
	hostPort_->setParentItem(this);
	iidPorts_.insert(hostPort_->iid(), hostPort_);

	BlueprintModelPort* modelport = NULL;
	modelport = new BlueprintModelPort(BlueprintModelPort::EVT_DATA_IN, UDQ_T("int"), UDQ_T("index"));
	inPorts_.append(modelport);
	modelport->setParentItem(this);
	iidPorts_.insert(modelport->iid(), modelport);

	// 准备数据输出
	modelport = new BlueprintModelPort(BlueprintModelPort::EVT_DATA_OUT, UDQ_T("instance"), UDQ_T("v"));
	outPorts_.append(modelport);
	modelport->setParentItem(this);
	iidPorts_.insert(modelport->iid(), modelport);
}


// 获取表达式
QJsonValue BlueprintInstanceIndex::expNode(int dt){
	QJsonArray exp;

	exp.append((int)20);

	QList<BlueprintLink *> links = hostPort_->links();
	Q_ASSERT(links.size() > 0);
	BlueprintObject* upport = links[0]->sourceNode() == hostPort_ ? links[0]->destNode() : links[0]->sourceNode();
	Q_ASSERT(upport->type() == KISMET_INSTANCE);
	BlueprintInstance* instance = (BlueprintInstance*)upport;
	exp.append(QString::number(instance->prototype_->hashKey()));  // 导出时替换

	// 
	QString name = inport(UDQ_T("name"))->getValue();
	Q_ASSERT(name.size() > 0);

	// 获取变量定义
	PluginVariable* var = instance->prototype_->getVariableDefine(name);
	Q_ASSERT(var != NULL);
	exp.append(var->datatype == UDQ_T("string"));

	// 索引表达式
	BlueprintModelPort* indexPort = inport(UDQ_T("index"));
	Q_ASSERT(indexPort != NULL);
	QString index = indexPort->getValue();
	if (index.isEmpty() && !indexPort->hasLink()){
		exp.append(QJsonValue());  // 必须为null
	}
	else{
		exp.append(portExpNode(indexPort));
	}
	// 变量索引位置
	exp.append(QString::number(hashString(instance->prototype_->description() + UDQ_T(".") + var->name)));  // 导出时替换

	return exp;
}