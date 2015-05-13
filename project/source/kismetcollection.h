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
**  Orion2 is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion2.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/
#ifndef  ORION_KISMET_COLLECTION_H
#define ORION_KISMET_COLLECTION_H

#include <QFont>
#include <vector>
#include "kismetdefine.h"
#include "kismetobject.h"
#include "kismetmodelport.h"
#include "resourceobject.h"
using namespace ORION;

//////////////////////////////////////////////////////////////////////////

class BlueprintCollection : public BlueprintModelPort{

public:
	// 实体类型
	ORION_TYPE_DECLARE(KISMET_COLLECTION);

	BlueprintCollection(ResourceHash key = 0);
	virtual ~BlueprintCollection();

	//  重载函数
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

	// 获取控制点
	QPointF controlPoint(int orient);

	void setFamily(ResourceHash key);

	// 编码
	virtual void encodeData(QDataStream& stream);
	virtual void decodeData(QDataStream& stream);
	virtual bool validate(QStringList& msg);
	QString collectionName(){
		return prototype_->description();
	}

	virtual QJsonValue expNode(int dt = -1);

	FamilyResourceObject* prototype_; 

protected:
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	QVariant itemChange(GraphicsItemChange change, const QVariant & value);
};



#endif // KISMET_TIMER_H
