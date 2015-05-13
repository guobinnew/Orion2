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


#include "scripttreewidget.h"
#include <QtGui>
#include <QMenu>
#include <QHeaderView>
#include <QPixmap>
#include <QCheckBox>
#include "commondefine.h"
#include "leveldefine.h"
#include "tooldefine.h"
#include "menumanager.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

ScriptTreeWidget::ScriptTreeWidget(QWidget *parent) : OrionTreeWidget(parent){

	setSelectionMode(QAbstractItemView::SingleSelection);
	setIconSize(QSize(32, 32));

	scriptimage_ = QIcon(UDQ_T(":/images/script.png"));

	BEGIN_ACTION();
	CONNECT_ACTION(ORION_KISMET_DELETE, this, deleteScript);
	END_ACTION();
}

ScriptTreeWidget::~ScriptTreeWidget(){

}

void ScriptTreeWidget::init(ProjectResourceObject* proj){
	project_ = proj;

	clear();

	foreach(BlueprintResourceObject* script, proj->blueprints_){
		if (script->flags() & URF_TagGarbage)
			continue;

		addScripttem(script);
	}

	expandAll();
}



void  ScriptTreeWidget::contextMenuEvent(QContextMenuEvent * event){

	QTreeWidgetItem* selectedItem = currentItem();
	if (selectedItem == NULL){
		event->ignore();
		return;
	}

	// 弹出右键菜单
	QMenu menu(this);
	QAction* act;

	if (selectedItem->type() == ITEM_BLUEPRINT ){
		act = gMenuManager->getAction(ORION_KISMET_DELETE);
		menu.addAction(act);
	}

	QRect rect = visualItemRect(selectedItem);
	menu.exec(event->globalPos());
	event->accept();
}


QTreeWidgetItem* ScriptTreeWidget::addScripttem(BlueprintResourceObject* script){
	QTreeWidgetItem* scriptItem = new QTreeWidgetItem(ITEM_BLUEPRINT);
	scriptItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	scriptItem->setText(0, script->description());
	scriptItem->setData(0, ITEM_HASHKEY, script->hashKey());
	scriptItem->setIcon(0, scriptimage_);
	addTopLevelItem(scriptItem);

	return scriptItem;
}


void ScriptTreeWidget::deleteScriptItem(ResourceHash key){

	DEFINE_RESOURCE_HASHKEY(BlueprintResourceObject, script, key);
	if (script == NULL)
		return;

	// 遍历所有子节点，删除对应节点
	foreach(QTreeWidgetItem* item, findItems(script->description(), Qt::MatchExactly | Qt::MatchRecursive)){
		if (item->type() == ITEM_BLUEPRINT){
			ResourceHash hash = item->data(0, ITEM_HASHKEY).toUInt();
			if (hash == key){
				delete item;
				break;
			}
		}
	}

	//  从项目中删除
	project_->blueprints_.removeOne(script);
	script->setFlags(URF_TagGarbage);
}

void ScriptTreeWidget::deleteScript(){
	QTreeWidgetItem* item = currentItem();

	if (item == NULL)
		return;

	if (QMessageBox::question(this, windowTitle(), UDQ_TR("确认删除脚本<%1>").arg(item->text(0))) == QMessageBox::No){
		return;
	}

	ResourceHash hash = item->data(0, ITEM_HASHKEY).toUInt();
	deleteScriptItem(hash);

}