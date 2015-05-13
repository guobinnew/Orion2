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
#ifndef  ORION_KISMET_VARIABLE_H
#define ORION_KISMET_VARIABLE_H

#include <QFont>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "kismetdefine.h"
#include "kismetobject.h"
#include "kismetmodelport.h"
#include "resourceobject.h"
using namespace ORION;

//////////////////////////////////////////////////////////////////////////

class BlueprintVariable : public BlueprintModelPort{

public:
	// 实体类型
	ORION_TYPE_DECLARE(KISMET_VARIABLE);

	BlueprintVariable();
	BlueprintVariable(ResourceHash key, ScriptVariable* sv);
	virtual ~BlueprintVariable();

	//  重载函数
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

	// 获取控制点
	virtual QPointF controlPoint( int orient = -1 );
	// 检查端口配对
	virtual bool checkPortPair(BlueprintModelPort* port);

	// 编码
	virtual void encodeData(QDataStream& stream);
	virtual void decodeData(QDataStream& stream);
	virtual bool validate(QStringList& msg);

	void postinit();

	QString varName(){
		return var_->name;
	}

	virtual QJsonValue expNode(int dt = -1);

	BlueprintResourceObject* script_;  // 脚本对象
	ScriptVariable* var_;  // 变量

protected:
	virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	virtual QVariant itemChange ( GraphicsItemChange change, const QVariant & value );
	virtual void keyPressEvent(QKeyEvent * event);
	virtual void keyReleaseEvent(QKeyEvent * event);

private:
	bool quickCloning_; 
};



#endif // KISMET_TIMER_H
