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

#include "propertytreewidget.h"
#include <QtGui>
#include <QMenu>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFontDialog>
#include <QColorDialog>
#include <QHBoxLayout>
#include "commondefine.h"
#include "editordefine.h"
#include "tooldefine.h"
#include "menumanager.h"
#include "colorselectwindow.h"
#include "fontselectwindow.h"
using namespace ORION;

PropertyTreeWidget::PropertyTreeWidget(QWidget *parent) : OrionTreeWidget(parent){

	setColumnCount(2);

}

PropertyTreeWidget::~PropertyTreeWidget(){

}


QTreeWidgetItem* PropertyTreeWidget::addGategory(const QString& name){
	Q_ASSERT(!name.isEmpty());

	QTreeWidgetItem* item = new QTreeWidgetItem( ITEM_CATEGORY );
	item->setText(0, name);
	addTopLevelItem(item);

	return item;
}

QTreeWidgetItem* PropertyTreeWidget::addItem(QTreeWidgetItem* parent, const QString& name, const QString& desc , const QString& text){
	QTreeWidgetItem* item = new QTreeWidgetItem(parent, ITEM_ATTRIBUTE);
	item->setText(0, name);
	item->setToolTip(0, desc);
	item->setText(1, text);
	return item;
}



QTreeWidgetItem* PropertyTreeWidget::addItemLineEdit(QTreeWidgetItem* parent, const QString& name, const QString& desc, const QString& text, QValidator* valid){
	QTreeWidgetItem* item = new QTreeWidgetItem(parent, ITEM_ATTRIBUTE);
	item->setText(0, name);
	item->setToolTip(0,desc);

	QLineEdit* edit = new QLineEdit();
	edit->setText(text);
	setItemWidget(item, 1, edit);

	return item;

}

QTreeWidgetItem* PropertyTreeWidget::addItemIntEdit(QTreeWidgetItem* parent, const QString& name, const QString& desc, int value){
	return addItemLineEdit(parent, name, desc, QString::number(value), new QIntValidator());
}
QTreeWidgetItem* PropertyTreeWidget::addItemDoubleEdit(QTreeWidgetItem* parent, const QString& name, const QString& desc, double value){
	return addItemLineEdit(parent, name, desc, QString::number(value), new QDoubleValidator());
}

QTreeWidgetItem* PropertyTreeWidget::addItemCombox(QTreeWidgetItem* parent, const QString& name, const QString& desc, const QStringList& items, int index){

	QTreeWidgetItem* item = new QTreeWidgetItem(parent, ITEM_ATTRIBUTE);
	item->setText(0, name);
	item->setToolTip(0,desc);

	QComboBox* box = new QComboBox();
	box->addItems(items);
	box->setCurrentIndex(index);
	setItemWidget(item, 1, box);

	return item;
}

QTreeWidgetItem* PropertyTreeWidget::addItemSpinbox(QTreeWidgetItem* parent, const QString& name, const QString& desc, const QSize& minmax, int value){
	QTreeWidgetItem* item = new QTreeWidgetItem(parent, ITEM_ATTRIBUTE);
	item->setText(0, name);
	item->setToolTip(0, desc);

	QSpinBox* box = new QSpinBox();
	box->setMinimum(minmax.width());
	box->setMaximum(minmax.height());
	box->setValue(value);
	setItemWidget(item, 1, box);

	return item;
}

QTreeWidgetItem* PropertyTreeWidget::addItemDoubleSpinbox(QTreeWidgetItem* parent, const QString& name, const QString& desc , const QSizeF& minmax, int value ){
	QTreeWidgetItem* item = new QTreeWidgetItem(parent, ITEM_ATTRIBUTE);
	item->setText(0, name);
	item->setToolTip(0, desc);

	QDoubleSpinBox* box = new QDoubleSpinBox();
	box->setMinimum(minmax.width());
	box->setMaximum(minmax.height());
	box->setValue(value);
	setItemWidget(item, 1, box);

	return item;
}


QTreeWidgetItem* PropertyTreeWidget::addItemFont(QTreeWidgetItem* parent, const QString& name, const QString& desc , const QString& fontstr ){
	QTreeWidgetItem* item = new QTreeWidgetItem(parent, ITEM_ATTRIBUTE);
	item->setText(0, name);
	item->setToolTip(0, desc);

	FontSelectWindow* w = new FontSelectWindow();
	w->setFontString(fontstr);
	setItemWidget(item, 1, w);
	return item;
}

QTreeWidgetItem* PropertyTreeWidget::addItemColorPicker(QTreeWidgetItem* parent, const QString& name, const QString& desc, const QString& clr ){
	QTreeWidgetItem* item = new QTreeWidgetItem(parent, ITEM_ATTRIBUTE);
	item->setText(0, name);
	item->setToolTip(0, desc);

	ColorSelectWindow* w = new ColorSelectWindow();
	w->setColor(clr);
	setItemWidget(item, 1, w);

	return item;
}


QString PropertyTreeWidget::getItemValue(QTreeWidgetItem* item){
	if (item == NULL){
		return QString();
	}

	QWidget* w = itemWidget(item, 1);

	QLineEdit* edit = qobject_cast<QLineEdit*>(w);
	if (edit){
		return edit->text();
	}

	QComboBox* combo = qobject_cast<QComboBox*>(w);
	if (combo){
		return combo->currentText();
	}

	QSpinBox* spin = qobject_cast<QSpinBox*>(w);
	if (spin){
		return spin->text();
	}

	FontSelectWindow* font = qobject_cast<FontSelectWindow*>(w);
	if (font){
		return font->getFontString();
	}

	ColorSelectWindow* color = qobject_cast<ColorSelectWindow*>(w);
	if (color){
		return color->getColorString();
	}

	return QString();
}


int  PropertyTreeWidget::getItemIndex(QTreeWidgetItem* item){
	if (item == NULL){
		return -1;
	}

	QWidget* w = itemWidget(item, 1);
	QComboBox* combo = qobject_cast<QComboBox*>(w);
	if (combo){
		return combo->currentIndex();
	}

	return -1;
}