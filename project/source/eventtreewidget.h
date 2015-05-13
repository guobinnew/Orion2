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


#ifndef ORION_EVENT_TREEWIDGET_H
#define ORION_EVENT_TREEWIDGET_H

#include "oriontreewidget.h"
#include "resourceobject.h"
#include <QJsonObject>
using namespace ORION;

class EventTreeWidget : public OrionTreeWidget{
	Q_OBJECT

public:
	EventTreeWidget(QWidget *parent = 0);
	~EventTreeWidget();

protected:
	virtual QMimeData *	mimeData(const QList<QTreeWidgetItem *> items) const;
	virtual void startDrag(Qt::DropActions supportedActions);
};

#endif // ORION_TILELISTWIDGET_H
