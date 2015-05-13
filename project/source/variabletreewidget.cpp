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


#include "variabletreewidget.h"
#include <QtGui>
#include <QMenu>
#include <QHeaderView>
#include <QLineEdit>
#include <QComboBox>
#include "commondefine.h"
#include "leveldefine.h"
#include "tooldefine.h"
#include "kismetdefine.h"
#include "menumanager.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

VariableTreeWidget::VariableTreeWidget(QWidget *parent) : OrionTreeWidget(parent){

	setDragEnabled(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setIconSize(QSize(24, 24));
	setDragDropMode(QAbstractItemView::DragOnly);

	setColumnCount(3);
	QHeaderView* headerview = header();
	headerview->setSectionResizeMode(0, QHeaderView::Stretch);
	headerview->setSectionResizeMode(1, QHeaderView::Fixed);
	headerview->setSectionResizeMode(2, QHeaderView::Stretch);

	mmimage_ = QImage(UDQ_T(":/images/variable.png")).scaledToWidth(64);
	varTypes_ << UDQ_T("int") << UDQ_T("float") << UDQ_T("string");

}


VariableTreeWidget::~VariableTreeWidget(){

}

void VariableTreeWidget::init(BlueprintResourceObject* script){
	script_ = script;

	int num = 0;
	if (script_){
		num = script_->variables_.size();
	}

	clear();

	if (script_){
		// 添加实体类型
		foreach(ScriptVariable* var, script_->variables_.values()){
			addVariableItem(var);
		}
	}

	resizeColumnToContents(0);
	resizeColumnToContents(2);
}

QMimeData * VariableTreeWidget::mimeData(const QList<QTreeWidgetItem *> items) const{

	Q_ASSERT( items.size() == 1 );
	Q_ASSERT( items.at(0) == currentItem() );

	QTreeWidgetItem* item = items.at(0);

	//  只能单选
	QMimeData* mmdata = new QMimeData;
	QByteArray itemData;
	{
		QDataStream dataStream(&itemData, QIODevice::WriteOnly);
		dataStream << script_->hashKey();
		dataStream << item->text(0);
	}

	mmdata->setData(MIME_KISMET_VARIABLE, itemData);
	return mmdata;
}

QStringList VariableTreeWidget::mimeTypes() const{
	return QStringList() << QString(MIME_KISMET_VARIABLE);
}


QTreeWidgetItem* VariableTreeWidget::addVariableItem(ScriptVariable* var){

	QTreeWidgetItem* item = new QTreeWidgetItem( ITEM_VARIABLE );
	item->setText(0, var->name);
	item->setData(0, ITEM_HASHKEY, hashString(var->name));
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemNeverHasChildren);
	addTopLevelItem(item);

	// 类型
	QComboBox* combox = new QComboBox();
	combox->setEditable(false);
	combox->addItems(varTypes_);
	combox->setCurrentIndex(combox->findText(var->datatype));
	setItemWidget(item, 1, combox);
	connect(combox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(updateType()));

	// 值
	QLineEdit* edit = new QLineEdit();
	edit->setText(var->value);
	setItemWidget(item, 2, edit);
	connect(edit, SIGNAL(textEdited(const QString&)), this, SLOT(updateValue()));

	return item;
}

void VariableTreeWidget::deleteVariableItem(ScriptVariable* var){
	ResourceHash key = hashString(var->name);
	for (int i = 0; i < topLevelItemCount(); i++){
		if (topLevelItem(i)->data(0, ITEM_HASHKEY).toUInt() == key){
			delete topLevelItem(i);
			break;
		}
	}
}

void VariableTreeWidget::refreshVariableItem(ScriptVariable* var){
	ResourceHash key = hashString(var->name);
	for (int i = 0; i < topLevelItemCount(); i++){
		if (topLevelItem(i)->data(0, ITEM_HASHKEY).toUInt() == key){
			QComboBox* combox = (QComboBox*)itemWidget(topLevelItem(i), 1);
			QLineEdit* edit = (QLineEdit*)itemWidget(topLevelItem(i), 2);
			combox->setCurrentText(var->datatype);
			disconnect(combox, SIGNAL(currentIndexChanged(const QString&)),0,0);
			connect(combox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(updateType()));
			edit->setText(var->value);
			break;
		}
	}
}

int VariableTreeWidget::containVariable(const QString& name){
	int num = 0;
	for (int i = 0; i < topLevelItemCount(); i++){
		if (topLevelItem(i)->text(0) == name){
			num++;
		}
	}
	return num;
}

void VariableTreeWidget::startDrag(Qt::DropActions supportedActions){
	QTreeWidgetItem* item = currentItem();
	if (item) {
		QMimeData *data = mimeData(selectedItems());
		if (!data)
			return;
		QRect rect;
		rect.adjust(horizontalOffset(), verticalOffset(), 0, 0);
		QDrag *drag = new QDrag(this);
		drag->setPixmap(QPixmap::fromImage(mmimage_));
		drag->setMimeData(data);
		drag->setHotSpot(QPoint(drag->pixmap().width() / 2, drag->pixmap().height() / 2));
		drag->exec(Qt::MoveAction);
	}
}

void VariableTreeWidget::updateType(){
	if (script_ == NULL)
		return;

	QTreeWidgetItem* item = NULL;
	for (int i = 0; i < topLevelItemCount(); i++){
		item = topLevelItem(i);
		ScriptVariable* var = script_->variables_.value(item->text(0));
		Q_ASSERT(var != NULL);

		QString newType = ((QComboBox*)itemWidget(item, 1))->currentText();
		if (var->datatype != newType){
			emit dataTypeChanged(hashString(var->name), newType);
		}
	}
}


void VariableTreeWidget::updateValue(){
	if (script_ == NULL)
		return;

	QTreeWidgetItem* item = NULL;
	for (int i = 0; i < topLevelItemCount(); i++){
		item = topLevelItem(i);
		ScriptVariable* var = script_->variables_.value(item->text(0));
		Q_ASSERT(var != NULL);
		emit valueChanged(hashString(var->name), ((QLineEdit*)itemWidget(item, 2))->text());
	}
}


void VariableTreeWidget::saveAll(){
	if (script_ == NULL)
		return;

	QTreeWidgetItem* item = NULL;
	for (int i = 0; i < topLevelItemCount(); i++){
		item = topLevelItem(i);
		ScriptVariable* var = script_->variables_.value(item->text(0));
		Q_ASSERT(var != NULL);
		var->datatype = ((QComboBox*)itemWidget(item, 1))->currentText();
		var->value = ((QLineEdit*)itemWidget(item, 2))->text();
	}
}