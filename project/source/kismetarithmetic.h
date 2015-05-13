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
#ifndef ORION_KISMET_ARITHMETIC_H
#define ORION_KISMET_ARITHMETIC_H

#include "kismetdefine.h"
#include "kismetmodel.h"
#include "resourceobject.h"

using namespace ORION;


class BlueprintArithmetic  : public BlueprintModel{
public:
	// 实体类型
	ORION_TYPE_DECLARE(KISMET_ARITHMETIC);

	BlueprintArithmetic(QGraphicsItem * parent = 0);
	virtual ~BlueprintArithmetic();

	void prepare();

	virtual void init();
	// 编码
	virtual void encodeData(QDataStream& stream);
	virtual void decodeData(QDataStream& stream);

	// 获取当前数学运算类型
	int mathType();

	// 获取表达式
	virtual QJsonValue expNode(int dt = -1);

private:
	QStringList mathTypes_;
	BlueprintModelPort* mathPort_;  // 数学运算类型
	QStringList addPorts_;  // 新添加的端口
};


#endif
