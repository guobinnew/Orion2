/************************************************************************
**
**  Copyright(C) 2015  guobin  <guobin.gb@alibaba - inc.com>
**
**  This file is part of Orion2.
**
**  Orion2 is free software : you can redistribute it and / or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation.
**
**  Orion2 is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion.If not, see < http://www.gnu.org/licenses/>.
**
*************************************************************************/

#ifndef ORION_PROPERTYTREEWIDGET_H
#define ORION_PROPERTYTREEWIDGET_H

#include <QTreeWidget>
#include "resourceobject.h"
#include "oriontreewidget.h"
using namespace ORION;

class PropertyTreeWidget : public OrionTreeWidget{
	Q_OBJECT

public:
	PropertyTreeWidget(QWidget *parent = 0);
	~PropertyTreeWidget();

	QTreeWidgetItem* addGategory(const QString& name);

	QTreeWidgetItem* addItem(QTreeWidgetItem* parent, const QString& name, const QString& desc = QString(), const QString& text = QString());


	QTreeWidgetItem* addItemLineEdit(QTreeWidgetItem* parent, const QString& name, const QString& desc = QString(), const QString& text = QString(), QValidator* valid = NULL );
	QTreeWidgetItem* addItemIntEdit(QTreeWidgetItem* parent, const QString& name, const QString& desc = QString(), int value = 0 );
	QTreeWidgetItem* addItemDoubleEdit(QTreeWidgetItem* parent, const QString& name, const QString& desc = QString(), double value = 0.0f );

	QTreeWidgetItem* addItemCombox(QTreeWidgetItem* parent, const QString& name, const QString& desc = QString(), const QStringList& items = QStringList(), int index = -1);
	QTreeWidgetItem* addItemSpinbox(QTreeWidgetItem* parent, const QString& name, const QString& desc = QString(),  const QSize& minmax = QSize(0,100), int value = 0 );
	QTreeWidgetItem* addItemDoubleSpinbox(QTreeWidgetItem* parent, const QString& name, const QString& desc = QString(), const QSizeF& minmax = QSizeF(0, 1), int value = 0);

	QTreeWidgetItem* addItemFont(QTreeWidgetItem* parent, const QString& name, const QString& desc = QString(), const QString& fontstr  = QString() );
	QTreeWidgetItem* addItemColorPicker(QTreeWidgetItem* parent, const QString& name, const QString& desc = QString(), const QString& clr = QString() );

	QString getItemValue( QTreeWidgetItem* item);
	int getItemIndex(QTreeWidgetItem* item);

};

#endif // ORION_GALLERYTREEWIDGET_H
