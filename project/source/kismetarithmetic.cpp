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

#include "kismetarithmetic.h"
#include <QtGui>
#include <math.h>
#include "kismetdefine.h"
#include "kismetscene.h"
#include "tooldefine.h"


BlueprintArithmetic::BlueprintArithmetic(QGraphicsItem * parent) : BlueprintModel(parent){
	prepare();
	adjust();
}

BlueprintArithmetic::~BlueprintArithmetic(){

}

void BlueprintArithmetic::init(){
	BlueprintModelPort* modelport = NULL;
	foreach(QString pt, addPorts_){
		modelport = new BlueprintModelPort(BlueprintModelPort::EVT_DATA_IN, UDQ_T("any"), pt);
		modelport->setObjectFlags(BlueprintObject::OF_CAN_DELETE);
		inPorts_.append(modelport);
		modelport->setParentItem(this);
		iidPorts_.insert(modelport->iid(), modelport);
	}
	addPorts_.clear();
}

void BlueprintArithmetic::prepare(){
	removeAllPort();

	title_ = UDQ_TR("算术运算");
	color_ = MATH_COLOR;

	BlueprintModelPort* modelport = NULL;

	mathTypes_ << UDQ_TR("+") << UDQ_TR("-") << UDQ_TR("*") << UDQ_TR("/") << UDQ_TR("%") << UDQ_TR("power");
	mathPort_ = new BlueprintModelPort(BlueprintModelPort::EVT_DATA_IN, UDQ_T("enum"), UDQ_T("math"), mathTypes_);
	inPorts_.append(mathPort_);
	mathPort_->setParentItem(this);
	iidPorts_.insert(mathPort_->iid(), mathPort_);

	// 准备数据输入
	modelport = new BlueprintModelPort(BlueprintModelPort::EVT_DATA_IN, UDQ_T("any"), UDQ_T("num1"));
	inPorts_.append(modelport);
	modelport->setParentItem(this);
	iidPorts_.insert(modelport->iid(), modelport);

	modelport = new BlueprintModelPort(BlueprintModelPort::EVT_DATA_IN, UDQ_T("any"), UDQ_T("num2"));
	inPorts_.append(modelport);
	modelport->setParentItem(this);
	iidPorts_.insert(modelport->iid(), modelport);

	// 准备数据输出
	modelport = new BlueprintModelPort(BlueprintModelPort::EVT_DATA_OUT, UDQ_T("any"), UDQ_T("res"));
	outPorts_.append(modelport);
	modelport->setParentItem(this);
	iidPorts_.insert(modelport->iid(), modelport);

	// 准备输入按钮
	BlueprintButtonControl* btn = NULL;
	btn = new BlueprintButtonControl(BlueprintButtonControl::BTN_ADD_IN, UDQ_TR("in"), UDQ_T("any"), QIcon(UDQ_T(":/images/add.png")), UDQ_T("num"));
	inButtons_.append(btn);
	btn->setParentItem(this);

}

void BlueprintArithmetic::encodeData(QDataStream& stream){
	BlueprintModel::encodeData(stream);

	// 新添加的端口
	addPorts_.clear();
	foreach(BlueprintModelPort* port, inPorts_)
	{
		if (port->canDelete()){
			addPorts_.append(port->name());
		}
	} 
	stream << addPorts_;
}

void BlueprintArithmetic::decodeData(QDataStream& stream){
	BlueprintModel::decodeData(stream);
	stream >> addPorts_;
	// 补充
	init();
	adjust();
}

int BlueprintArithmetic::mathType(){

	QString t = mathPort_->getValue();
	return mathTypes_.indexOf(t) + 4;

}

// 获取表达式
QJsonValue BlueprintArithmetic::expNode(int dt){
	QJsonValue exp;

	// 检查有效的输入端口
	QList<BlueprintModelPort*> validPorts;
	foreach(BlueprintModelPort* p, inPorts_){
		if (p->hasLink() && p->links().size() == 1){
			validPorts.append(p);
		}
		else if (!p->hasLink()){
			QString v = p->getValue();
			bool r = false;
			v.toDouble(&r);
			if (r){  // 为有效的数值
				validPorts.append(p);
			}
		}
	}

	Q_ASSERT(validPorts.size() > 0);

	// 如果只有一个端口有效，则转换为常量处理
	if (validPorts.size() == 1){ // 转换为常量
	
		if (!validPorts[0]->hasLink()){  // 根据端口类型生成exp
			exp = validPorts[0]->expNode();
		}
		else{  // 直接返回上游端口exp
			QList<BlueprintLink *> links = validPorts[0]->links();
			Q_ASSERT(links.size() == 1);
			BlueprintObject* upport = links[0]->sourceNode() == validPorts[0] ? links[0]->destNode() : links[0]->sourceNode();
			
			// 检查所属模型
			BlueprintModel* upmodel = dynamic_cast<BlueprintModel*>(upport->parentItem());
			if (upmodel){
				exp = upmodel->expNode();
			}
			else{
				exp = upport->expNode(validPorts[0]->dataTypeId());
			}
		}	
	}
	else{
		QJsonArray arr;
		//
		int level = validPorts.size();
		for (int i = 1; i < level; i++){
			if (i == 1){  // 第一层
				arr.append(mathType());
				arr.append(portExpNode(validPorts[i-1]));
				arr.append(portExpNode(validPorts[i]));
			}
			else{  // 向上
				QJsonArray newArr;
				newArr.append(mathType());
				newArr.append(arr);
				newArr.append(portExpNode(validPorts[i]));
				arr = newArr;
			}
		}
		exp = arr;
	}

	return exp;
}