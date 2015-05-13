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


#include "projecttreewidget.h"
#include <QtGui>
#include <QMenu>
#include <QHeaderView>
#include <QRadioButton>
#include <QCheckBox>
#include "commondefine.h"
#include "leveldefine.h"
#include "tooldefine.h"
#include "menumanager.h"


const QString QSS_VISIBLE = UDQ_T("QCheckBox::indicator:unchecked {image:url(:/images/unchecked.png);}\n\
								  								  								  QCheckBox::indicator:checked {image:url(:/images/checked.png);}");
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

ProjectTreeWidget::ProjectTreeWidget(QWidget *parent) : OrionTreeWidget(parent), isUpdating_(false){

	defaultLevelIcon_ = QIcon(UDQ_T(":/images/emptylevel.png"));
	defaultLayerIcon_ = QIcon(UDQ_T(":/images/layer.png"));

	setColumnCount(1);
	QHeaderView* headerview = header();
	headerview->setSectionResizeMode(0, QHeaderView::Stretch);

	BEGIN_ACTION();
	CONNECT_ACTION(ORION_EDIT_ADDEFFECT, this, addEffect);
	END_ACTION();
}


ProjectTreeWidget::~ProjectTreeWidget(){

}


QTreeWidgetItem* ProjectTreeWidget::getProjectItem(){
	Q_ASSERT(topLevelItemCount() <= 1);

	if (topLevelItemCount() == 0)
		return NULL;

	return topLevelItem(0);
}

QTreeWidgetItem* ProjectTreeWidget::addProjectItem(ProjectResourceObject* proj){

	QTreeWidgetItem* rootItem = new QTreeWidgetItem(ITEM_PROJECT);
	rootItem->setText(0, proj->objectName());
	rootItem->setData(0, ITEM_HASHKEY, proj->hashKey());
	proj->leveltreeItem_ = rootItem;

	return rootItem;

}

QTreeWidgetItem*  ProjectTreeWidget::addLevelItem(LevelResourceObject* level, QTreeWidgetItem* parent){

	QTreeWidgetItem* rootItem = getProjectItem();
	Q_ASSERT(rootItem != NULL);

	QTreeWidgetItem* levelItem = new QTreeWidgetItem(rootItem, ITEM_LEVEL);

	QString desc = level->description();
	if (desc.isEmpty()){
		desc = level->objectName();
		level->setDescription(desc);
		level->setFlags(URF_TagSave);
	}
	levelItem->setText(0, desc);

	levelItem->setData(0, ITEM_HASHKEY, level->hashKey());
	levelItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);
	level->treeItem_ = levelItem;
	updateItemIcon(levelItem);

	return levelItem;
}

QTreeWidgetItem* ProjectTreeWidget::addNewLayerItem(QTreeWidgetItem* parent){
	QTreeWidgetItem* layerItem = new QTreeWidgetItem(parent, ITEM_LAYER);
	layerItem->setIcon(0, defaultLayerIcon_);
	layerItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
	return layerItem;
}
QTreeWidgetItem* ProjectTreeWidget::addLayerItem(LayerResourceObject* layer, QTreeWidgetItem* parent){
	Q_ASSERT(layer != NULL);

	QTreeWidgetItem* layerItem = addNewLayerItem(parent);
	updateLayerItem(layer, layerItem);

	return layerItem;
}

// 更新单个图标
void ProjectTreeWidget::updateItemIcon(QTreeWidgetItem* item){
	Q_ASSERT(item != NULL);

	ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
	DEFINE_RESOURCE_HASHKEY_VALID(LevelResourceObject, level, key);

	if (level->thumbnail_.isNull()){
		item->setIcon(0, defaultLevelIcon_);
	}
	else{
		item->setIcon(0, level->thumbnail_);
	}
}


void ProjectTreeWidget::dropEvent(QDropEvent * event){

	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	Q_ASSERT(ws != NULL);

	// 获取当前电子书
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != 0 && !(proj->flags()  & URF_TagGarbage));

	// 查找当前父对象
	QPoint pos = event->pos();

	// 判断当前树对象
	QTreeWidgetItem* destItem = itemAt(pos);
	if (destItem == NULL || (destItem->type() != ITEM_LEVEL && destItem->type() != ITEM_LAYER)){
		event->ignore();
		return;
	}

	// 图层移动，只能在同一个Level中调换次序
	// 检查是否相同
	ResourceHash newhash = destItem->data(0, ITEM_HASHKEY).toUInt();
	if (event->mimeData()->hasFormat(MIME_ORION_LAYER)) {

		QByteArray itemData = event->mimeData()->data(MIME_ORION_LAYER);
		ResourceHash layerH;
		ResourceHash levelH;
		{
			QDataStream stream(&itemData, QIODevice::ReadOnly);
			stream >> layerH >> levelH;
		}

		// 不能是自身
		if (destItem->type() == ITEM_LAYER &&  newhash == layerH){
			event->ignore();
			return;
		}

		DEFINE_RESOURCE_HASHKEY_VALID(LevelResourceObject, srcLevel, levelH);
		DEFINE_RESOURCE_HASHKEY_VALID(LayerResourceObject, srcLayer, layerH);

		if (destItem->type() == ITEM_LAYER){ // 移动到当前图层的前面
			// 移动到当前页面的前面
			DEFINE_RESOURCE_HASHKEY_VALID(LayerResourceObject, destLayer, newhash);
			Q_ASSERT(destLayer->treeItem_ == destItem);

			if (destLayer->level_ == srcLevel){  // 同一场景

				int destIndex = srcLevel->treeItem_->indexOfChild(destItem) ;
				int srcIndex = srcLevel->treeItem_->indexOfChild(srcLayer->treeItem_);

				int offset = destIndex - srcIndex;
				if (offset < -1 || offset > 0){
					if (offset < 0){
						destIndex++;
					}

					emit layerMoved( layerH, destIndex );
				}
				else{
					event->ignore();
					return;
				}


			}
			else{  
				//// 复制到新场景
				//// 移动到目标组的末尾
				//LevelResourceObject* destLevel = destLayer->level_;
				//int destIndex = destLevel->treeItem_->indexOfChild(destItem) + 1;
				//LayerResourceObject* layer = destLevel->addNewLayer(UDQ_T(""));
				//Q_ASSERT(layer != 0);
				//// 复制图层数据
				//srcLayer->copyObject(layer);
				//destLevel->moveLayer(-1, destIndex);
				//// 修改树控件
				//updateLevelItem(destLevel->treeItem_);
				//expandItem(destLevel->treeItem_);

				event->ignore();
				return;
			}
		}
		else if (destItem->type() == ITEM_LEVEL){			// 复制到目标场景的末尾

			DEFINE_RESOURCE_HASHKEY_VALID(LevelResourceObject, destLevel, newhash);
			int destIndex = 0;
			if (destLevel != srcLevel){

				//LayerResourceObject* layer = destLevel->addNewLayer(UDQ_T(""));
				//Q_ASSERT(layer != 0);
				//// 复制图层数据
				//srcLayer->copyObject(layer);
				//destLevel->moveLayer(-1, destIndex);  // 移动到最底层（队列最前面）
				//// 修改树控件
				//updateLevelItem(destLevel->treeItem_);

				event->ignore();
				return;
			}
			else{  // 同一场景
				int srcIndex = srcLevel->treeItem_->indexOfChild(srcLayer->treeItem_);
				if (srcIndex > 0){
					emit layerMoved(layerH, destIndex);
				}
				else{
					event->ignore();
					return;
				}
			}
			
			proj->setFlags(URF_TagSave);
			event->accept();
			return;
		}
	}

	event->ignore();

}

QMimeData * ProjectTreeWidget::mimeData(const QList<QTreeWidgetItem *> items) const{

	Q_ASSERT(items.size() == 1);
	Q_ASSERT(items.at(0) == currentItem());
	//  只能单选
	QTreeWidgetItem* item = items.at(0);
	if (item->type() != ITEM_LAYER)  // 根实体不能拖放
		return NULL;

	ResourceHash hash = item->data(0, ITEM_HASHKEY).toUInt();
	QMimeData* mmdata = new QMimeData;
	DEFINE_RESOURCE_HASHKEY(LayerResourceObject, layer, hash);
	Q_ASSERT(layer != NULL);

	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	dataStream << hash << layer->level_->hashKey();
	mmdata->setData(MIME_ORION_LAYER, itemData);

	return mmdata;
}

QStringList ProjectTreeWidget::mimeTypes() const{
	return QStringList() << QString(MIME_ORION_LAYER);
}

void  ProjectTreeWidget::contextMenuEvent(QContextMenuEvent * event){

	QTreeWidgetItem* selectedItem = currentItem();
	if (selectedItem == NULL){
		event->ignore();
		return;
	}

	// 弹出右键菜单
	QMenu menu(this);
	QAction* act;

	if (selectedItem->type() == ITEM_LEVEL || selectedItem->type() == ITEM_LAYER){
		act = gMenuManager->getAction(ORION_EDIT_ADDEFFECT);
		menu.addAction(act);

		menu.addSeparator();
		act = gMenuManager->getAction(ORION_EDIT_DELETE);
		menu.addAction(act);

	}
	else{
	}

	menu.exec(event->globalPos());
	event->accept();
}

LevelResourceObject* ProjectTreeWidget::getLevel(QTreeWidgetItem* item){
	if (item == NULL || item->type() == ITEM_PROJECT){
		return NULL;
	}

	while (item->type() != ITEM_LEVEL && item->parent()){
		item = item->parent();
	}

	ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
	DEFINE_RESOURCE_HASHKEY_VALID(LevelResourceObject, level, key);
	return level;
}

// 更新场景
void ProjectTreeWidget::updateLevelItem(QTreeWidgetItem* levelItem){
	if (levelItem == NULL)
		return;

	isUpdating_ = true;

	ResourceHash key = levelItem->data(0, ITEM_HASHKEY).toUInt();
	DEFINE_RESOURCE_HASHKEY_VALID(LevelResourceObject, level, key);

	while (level->layers_.size() > levelItem->childCount()){
		addNewLayerItem(levelItem);
	}

	while (level->layers_.size() < levelItem->childCount()){
		levelItem->removeChild(levelItem->child(0));
	}

	for (int i = 0; i < level->layers_.size(); i++){
		updateLayerItem(level->layers_[i], levelItem->child(i));
	}

	isUpdating_ = false;
}

void ProjectTreeWidget::updateLayerItem(LayerResourceObject* layer, QTreeWidgetItem* layerItem){

	QString desc = layer->description();
	if (desc.isEmpty()){
		desc = layer->objectName();
		layer->setDescription(desc);
		layer->setFlags(URF_TagSave);
	}
	layerItem->setText(0, desc);
	layerItem->setData(0, ITEM_HASHKEY, layer->hashKey());
	layer->treeItem_ = layerItem;

}

void ProjectTreeWidget::addEffect(){
	QTreeWidgetItem* item = currentItem();
	if (item == NULL)
		return;

	Q_ASSERT(item->type() == ITEM_LEVEL || item->type() == ITEM_LAYER);

	// 启动添加特效窗口

}