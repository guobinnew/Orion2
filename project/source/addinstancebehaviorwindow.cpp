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
#include "addinstancebehaviorwindow.h"
#include "ui_addinstancebehaviorwindow.h"
#include "tooldefine.h"
#include "leveldefine.h"

AddInstanceBehaviorWindow::AddInstanceBehaviorWindow(const QStringList& names, QWidget *parent) :
QDialog(parent), names_(names),
    ui(new Ui::AddInstanceBehaviorWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	init();

}

AddInstanceBehaviorWindow::~AddInstanceBehaviorWindow()
{
    delete ui;
}

bool AddInstanceBehaviorWindow::canSave(){
	QTreeWidgetItem* item = ui->treeWidget->currentItem();
	if (item == NULL || item->type() != ITEM_BEHAVIOR)
		return false;

	return true;
}

void AddInstanceBehaviorWindow::init(){

	QIcon folderIcon = QIcon(UDQ_T(":/images/folder.png"));
	QString cate;
	// 系统插件
	for (TResourceObjectIterator<PluginResourceObject> it; it; ++it){
		PluginResourceObject* plugin = (PluginResourceObject*)(*it);

		if (plugin->flags() & URF_TagGarbage || plugin->type_ != PIT_BEHAVIOR)
			continue;

		Q_ASSERT(plugin->categories_.size() > 0);
		cate = plugin->categories_[0];
		Q_ASSERT(cate != NULL);

		QTreeWidgetItem* categoryItem = findCateory(cate);

		// 添加插件Item
		QTreeWidgetItem* pluginItem = new QTreeWidgetItem(ITEM_BEHAVIOR);
		pluginItem->setText(0, plugin->title_);
		pluginItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
		pluginItem->setData(0, ITEM_HASHKEY, plugin->hashKey());
		pluginItem->setToolTip(0, plugin->description());
		categoryItem->addChild(pluginItem);

	}

	ui->treeWidget->expandAll();
}

QTreeWidgetItem* AddInstanceBehaviorWindow::findCateory(const QString& cate, bool create){

	QTreeWidgetItem* categoryItem = NULL;
	for (int i = 0; i < ui->treeWidget->topLevelItemCount(); i++){
		if (ui->treeWidget->topLevelItem(i)->text(0) == cate){
			categoryItem = ui->treeWidget->topLevelItem(i);
			break;
		}
	}

	if (categoryItem == NULL && create){
		categoryItem = new QTreeWidgetItem(ITEM_CATEGORY);
		categoryItem->setText(0, cate);
		categoryItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
		ui->treeWidget->addTopLevelItem(categoryItem);
	}

	return categoryItem;
}



void  AddInstanceBehaviorWindow::on_cancelpushButton_clicked(){
	reject();
}

void  AddInstanceBehaviorWindow::on_okpushButton_clicked(){
	if (!canSave())
		return;

	QTreeWidgetItem* item = ui->treeWidget->currentItem();
	hash_ = item->data(0, ITEM_HASHKEY).toUInt();
	DEFINE_RESOURCE_HASHKEY_VALID(PluginResourceObject, plugin, hash_);
	Q_ASSERT(plugin->type_ == PIT_BEHAVIOR);

	newbehavior_ = plugin->objectName();
	int index = 1;
	while (names_.contains(newbehavior_)){
		newbehavior_ = plugin->objectName() + QString::number(index++);
	}

	accept();
}