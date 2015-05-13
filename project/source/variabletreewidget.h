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


#ifndef ORION_VARIABLE_TREEWIDGET_H
#define ORION_VARIABLE_TREEWIDGET_H

#include <QTreeWidget>
#include <QPushButton>
#include "resourceobject.h"
#include "oriontreewidget.h"
using namespace ORION;

class VariableTreeWidget : public OrionTreeWidget{
	Q_OBJECT

public:
	VariableTreeWidget(QWidget *parent = 0);
	~VariableTreeWidget();

	void init(BlueprintResourceObject* proj);
	QTreeWidgetItem* addVariableItem(ScriptVariable* var);
	void deleteVariableItem(ScriptVariable* var);
	void refreshVariableItem(ScriptVariable* var);

	//  ºÏ≤È±‰¡ø
	int containVariable(const QString& name);

	QImage mmimage_;
	BlueprintResourceObject* script_;
	QStringList varTypes_;

	void saveAll();

signals:
	void dataTypeChanged(unsigned int key, const QString& dt);
	void valueChanged(unsigned int key, const QString& v);

public slots :
    void updateType();
	void updateValue();

protected:
	virtual QMimeData* mimeData ( const QList<QTreeWidgetItem *> items ) const;
	virtual QStringList	mimeTypes () const;
	virtual void startDrag(Qt::DropActions supportedActions);
};

#endif // PICTUREWIDGET_H
