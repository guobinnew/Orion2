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

#include "addinstanceeffectwindow.h"
#include "ui_addinstanceeffectwindow.h"
#include "tooldefine.h"
#include "leveldefine.h"

AddInstanceEffectWindow::AddInstanceEffectWindow(const QStringList& names, QWidget *parent) :
QDialog(parent), names_(names),
    ui(new Ui::AddInstanceEffectWindow)
{
    ui->setupUi(this);
	setFixedSize(size());
	init();
}

AddInstanceEffectWindow::~AddInstanceEffectWindow()
{
    delete ui;
}

void AddInstanceEffectWindow::init(){

	QIcon folderIcon = QIcon(UDQ_T(":/images/folder.png"));
	QString cate;
	// 系统插件
	for (TResourceObjectIterator<PluginResourceObject> it; it; ++it){
		PluginResourceObject* plugin = (PluginResourceObject*)(*it);

		if (plugin->flags() & URF_TagGarbage || plugin->type_ != PIT_EFFECT)
			continue;

		Q_ASSERT(plugin->categories_.size() > 0);
		cate = plugin->categories_[0];
		Q_ASSERT(cate != NULL);

		QTreeWidgetItem* categoryItem = findCateory(cate);

		// 添加插件Item
		QTreeWidgetItem* pluginItem = new QTreeWidgetItem(ITEM_EFFECT);
		pluginItem->setText(0, plugin->title_);
		pluginItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		pluginItem->setData(0, ITEM_HASHKEY, plugin->hashKey());
		pluginItem->setToolTip(0, plugin->description());
		categoryItem->addChild(pluginItem);

	}

	ui->treeWidget->expandAll();
}


bool AddInstanceEffectWindow::canSave(){
	QTreeWidgetItem* item = ui->treeWidget->currentItem();
	if (item == NULL || item->type() != ITEM_EFFECT)
		return false;

	return true;
}

void  AddInstanceEffectWindow::on_cancelpushButton_clicked(){
	reject();
}

void  AddInstanceEffectWindow::on_okpushButton_clicked(){
	if (!canSave())
		return;

	QTreeWidgetItem* item = ui->treeWidget->currentItem();

	hash_ = item->data(0, ITEM_HASHKEY).toUInt();
	DEFINE_RESOURCE_HASHKEY_VALID(PluginResourceObject, plugin, hash_);
	Q_ASSERT(plugin->type_ == PIT_EFFECT);

	neweffect_ = plugin->objectName();
	int index = 1;
	while (names_.contains(neweffect_)){
		neweffect_ = plugin->objectName() + QString::number(index++);
	}

	accept();
}

QTreeWidgetItem* AddInstanceEffectWindow::findCateory(const QString& cate, bool create){

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
		categoryItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		ui->treeWidget->addTopLevelItem(categoryItem);
	}

	return categoryItem;
}
