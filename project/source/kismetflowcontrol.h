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
#ifndef ORION_KISMET_FLOWCONTROL_H
#define ORION_KISMET_FLOWCONTROL_H

#include "kismetdefine.h"
#include "kismetmodel.h"
#include "resourceobject.h"
using namespace ORION;

////////////////////////////////////////////////////////////////////////////
// 分支控制IFELSE
//////////////////////////////////////////////////////////////////////////

class BlueprintBranch  : public BlueprintModel{
public:
	// 实体类型
	ORION_TYPE_DECLARE(KISMET_FLOW_BRANCH);

	BlueprintBranch(QGraphicsItem * parent = 0);
	virtual ~BlueprintBranch();

	void prepare();

	virtual void init();
	// 编码
	virtual void encodeData(QDataStream& stream);
	virtual void decodeData(QDataStream& stream);
	virtual bool validate(QStringList& msg);
	// 是否为OR模块
	bool isORLogic();

private:
	BlueprintModelPort* logicPort_;
	QStringList addPorts_;  // 新添加的端口
};

//////////////////////////////////////////////////////////////////////////
// 顺序执行
//////////////////////////////////////////////////////////////////////////

class BlueprintSequence: public BlueprintModel{
public:
	// 实体类型
	ORION_TYPE_DECLARE(KISMET_FLOW_SEQUENCE);

	BlueprintSequence(QGraphicsItem * parent = 0);
	virtual ~BlueprintSequence();

	void prepare();

	virtual void init();
	// 编码
	virtual void encodeData(QDataStream& stream);
	virtual void decodeData(QDataStream& stream);
	virtual bool validate(QStringList& msg);
private:
	QStringList addPorts_;  // 新添加的端口
};


#endif
