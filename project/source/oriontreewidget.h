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


#ifndef ORION_TREEWIDGET_H
#define ORION_TREEWIDGET_H

#include <QTreeWidget>
#include "resourceobject.h"
using namespace ORION;

class OrionTreeWidget : public QTreeWidget{
	Q_OBJECT

public:
	OrionTreeWidget(QWidget *parent = 0);
	~OrionTreeWidget();

	QImage branchOpenImage_;  
	QImage branchCloseImage_;

	QTreeWidgetItem* findTopLevelItem(const QString& name, int column = 0) const;

protected:
	virtual void drawBranches(QPainter * painter, const QRect & rect, const QModelIndex & index) const;
};

#endif // PICTUREWIDGET_H
