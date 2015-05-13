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

#include "kismetflowcontrol.h"
#include <QtGui>
#include <math.h>
#include "kismetdefine.h"
#include "kismetscene.h"
#include "tooldefine.h"


BlueprintBranch::BlueprintBranch(QGraphicsItem * parent) : BlueprintModel(parent){
	prepare();
	adjust();
}

BlueprintBranch::~BlueprintBranch(){

}

void BlueprintBranch::init(){
	BlueprintModelPort* modelport = NULL;
	foreach(QString pt, addPorts_){
		modelport = new BlueprintModelPort(BlueprintModelPort::EVT_CTRL_IN, UDQ_T("ctrl"), pt);
		modelport->setObjectFlags(BlueprintObject::OF_CAN_DELETE);
		inCtrlPorts_.append(modelport);
		modelport->setParentItem(this);
		iidPorts_.insert(modelport->iid(), modelport);
	}
	addPorts_.clear();
}

void BlueprintBranch::prepare(){
	removeAllPort();

	title_ = UDQ_TR("分支控制");
	color_ = CONTROL_COLOR;

	// 准备控制输入
	BlueprintModelPort* modelport = NULL;
	modelport = new BlueprintModelPort(BlueprintModelPort::EVT_CTRL_IN, UDQ_T("ctrl"), UDQ_T("in1"));
	inCtrlPorts_.append(modelport);
	modelport->setParentItem(this);
	iidPorts_.insert(modelport->iid(), modelport);

	modelport = new BlueprintModelPort(BlueprintModelPort::EVT_CTRL_IN, UDQ_T("ctrl"), UDQ_T("in2"));
	inCtrlPorts_.append(modelport);
	modelport->setParentItem(this);
	iidPorts_.insert(modelport->iid(), modelport);

	// 准备控制输出
	modelport = new BlueprintModelPort(BlueprintModelPort::EVT_CTRL_OUT, UDQ_T("ctrl"), UDQ_T("true"));
	outCtrlPorts_.append(modelport);
	modelport->setParentItem(this);
	iidPorts_.insert(modelport->iid(), modelport);

	modelport = new BlueprintModelPort(BlueprintModelPort::EVT_CTRL_OUT, UDQ_T("ctrl"), UDQ_T("false"));
	outCtrlPorts_.append(modelport);
	modelport->setParentItem(this);
	iidPorts_.insert(modelport->iid(), modelport);

	// 输入数据端口
	QStringList items;
	items << UDQ_TR("AND") << UDQ_TR("OR");
	logicPort_ = new BlueprintModelPort(BlueprintModelPort::EVT_DATA_IN, UDQ_T("enum"), UDQ_T("logic"), items);
	inPorts_.append(logicPort_);
	logicPort_->setParentItem(this);
	iidPorts_.insert(logicPort_->iid(), logicPort_);

	// 准备输入按钮
	BlueprintButtonControl* btn = NULL;
	btn = new BlueprintButtonControl(BlueprintButtonControl::BTN_ADD_IN, UDQ_TR("in"), UDQ_T("ctrl"), QIcon(UDQ_T(":/images/add.png")));
	inButtons_.append(btn);
	btn->setParentItem(this);

}

void BlueprintBranch::encodeData(QDataStream& stream){
	BlueprintModel::encodeData(stream);

	// 新添加的端口
	addPorts_.clear();
	foreach(BlueprintModelPort* port, inCtrlPorts_)
	{
		if (port->canDelete()){
			addPorts_.append(port->name());
		}
	} 
	stream << addPorts_;
}

void BlueprintBranch::decodeData(QDataStream& stream){
	BlueprintModel::decodeData(stream);
	stream >> addPorts_;
	// 补充
	init();
	adjust();
}

bool BlueprintBranch::isORLogic(){
	QString logic = logicPort_->getValue();
	return logic == UDQ_T("OR");
}

bool BlueprintBranch::validate(QStringList& msg){
	BlueprintModel::validate(msg);

	//////////////////////////////////////////////////////////////////////////
	// 如果是分支
	QList<BlueprintModel*> upmodels = upstreamCtrl(KISMET_FLOW_BRANCH);
	if (upmodels.size() > 1){
		isError_ = true;
		msg.append(ERROR_MSG.arg(UDQ_TR("分支模型的控制输入中不能接受来自1个以上的分支模型!")));
	}
	else if (upmodels.size() == 1){
		QList<BlueprintModelPort*> ports = upmodels[0]->modelOutPort(this);
		if (ports.size() > 1){
			isError_ = true;
			msg.append(ERROR_MSG.arg(UDQ_TR("分支模型的控制输入来自同一个分支模型的2个不同控制输出!")));
		}
		else {
			Q_ASSERT(ports.size() == 1);
			QString portname = ports[0]->name();
			if (portname == UDQ_T("true") && !hasUpStream(KISMET_MODEL, PT_CONDITION)){  // 如果来自true端口，则必须有条件输入
				isError_ = true;
				msg.append(ERROR_MSG.arg(UDQ_TR("分支模型的必须有条件输入!")));
			}
			else if (portname == UDQ_T("false") && !hasUpStream(KISMET_MODEL, PT_CONDITION)){//如果上游模型是分支的False端口，而且没有其他条件输入，则False端口不能连接
				BlueprintModelPort* falseport = outCtrlPort(UDQ_T("false"));
				if (falseport && falseport->hasLink()){
					isError_ = true;
					msg.append(ERROR_MSG.arg(UDQ_TR("没有其他条件输入的分支模型不能使用False输出端口!")));
				}
			}
		}
	}

	return !isError_;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

BlueprintSequence::BlueprintSequence(QGraphicsItem * parent) : BlueprintModel(parent){
	prepare();
	adjust();
}

BlueprintSequence::~BlueprintSequence(){

}

void BlueprintSequence::init(){
	BlueprintModelPort* modelport = NULL;
	foreach(QString pt, addPorts_){
		modelport = new BlueprintModelPort(BlueprintModelPort::EVT_CTRL_OUT, UDQ_T("ctrl"), pt);
		modelport->setObjectFlags(BlueprintObject::OF_CAN_DELETE);
		outCtrlPorts_.append(modelport);
		modelport->setParentItem(this);
		iidPorts_.insert(modelport->iid(), modelport);
	}
	addPorts_.clear();
}

void BlueprintSequence::prepare(){
	removeAllPort();

	title_ = UDQ_TR("顺序执行");
	color_ = CONTROL_COLOR;

	// 准备控制输入
	BlueprintModelPort* modelport = NULL;
	modelport = new BlueprintModelPort(BlueprintModelPort::EVT_CTRL_IN, UDQ_T("ctrl"), UDQ_T("in"));
	inCtrlPorts_.append(modelport);
	modelport->setParentItem(this);
	iidPorts_.insert(modelport->iid(), modelport);

	// 准备控制输出
	modelport = new BlueprintModelPort(BlueprintModelPort::EVT_CTRL_OUT, UDQ_T("ctrl"), UDQ_T("out1"));
	outCtrlPorts_.append(modelport);
	modelport->setParentItem(this);
	iidPorts_.insert(modelport->iid(), modelport);

	modelport = new BlueprintModelPort(BlueprintModelPort::EVT_CTRL_OUT, UDQ_T("ctrl"), UDQ_T("out2"));
	outCtrlPorts_.append(modelport);
	modelport->setParentItem(this);
	iidPorts_.insert(modelport->iid(), modelport);

	// 准备输入按钮
	BlueprintButtonControl* btn = NULL;
	btn = new BlueprintButtonControl(BlueprintButtonControl::BTN_ADD_OUT, UDQ_TR("out"), UDQ_T("ctrl"), QIcon(UDQ_T(":/images/add.png")));
	outButtons_.append(btn);
	btn->setParentItem(this);

}

void BlueprintSequence::encodeData(QDataStream& stream){
	BlueprintModel::encodeData(stream);

	// 新添加的端口
	addPorts_.clear();
	foreach(BlueprintModelPort* port, outCtrlPorts_)
	{
		if (port->canDelete()){
			addPorts_.append(port->name());
		}
	}
	stream << addPorts_;
}

void BlueprintSequence::decodeData(QDataStream& stream){
	BlueprintModel::decodeData(stream);
	stream >> addPorts_;
	// 补充
	init();
	adjust();
}

bool BlueprintSequence::validate(QStringList& msg){
	BlueprintModel::validate(msg);

	//////////////////////////////////////////////////////////////////////////
	// seq模块不能直接相连
	if (hasDownStream(KISMET_FLOW_SEQUENCE)){
		isError_ = true;
		msg.append(ERROR_MSG.arg(UDQ_TR("序列模型不能与序列模型直接相连!")));
	}

	return !isError_;
}