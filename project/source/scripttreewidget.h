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


#ifndef ORION_SCRIPT_TREEWIDGET_H
#define ORION_SCRIPT_TREEWIDGET_H

#include <QTreeWidget>
#include <QPushButton>
#include "resourceobject.h"
#include "oriontreewidget.h"
using namespace ORION;

class ScriptTreeWidget : public OrionTreeWidget{
	Q_OBJECT

public:
	ScriptTreeWidget(QWidget *parent = 0);
	~ScriptTreeWidget();

	void init(ProjectResourceObject* proj);

	// 删除对象类型
	void deleteScriptItem(ResourceHash key);
	QTreeWidgetItem* addScripttem(BlueprintResourceObject* script);

	QIcon scriptimage_;
	QIcon funcimage_;

	ProjectResourceObject* project_;
protected:
	virtual void contextMenuEvent(QContextMenuEvent * event);

private slots:
	void deleteScript();
};

#endif // PICTUREWIDGET_H
