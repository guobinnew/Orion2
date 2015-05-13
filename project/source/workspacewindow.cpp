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

#include "workspacewindow.h"
#include "ui_workspacewindow.h"
#include <QResizeEvent>
#include <QMessageBox>
#include <QMenu>
#include "commondefine.h"
#include "tooldefine.h"
#include "configmanager.h"
#include "resourceobject.h"
#include "mainmanagerframe.h"
#include "framemanager.h"
#include "menumanager.h"
#include "tooldefine.h"
#include "leveldefinewindow.h"
#include "layerdefinewindow.h"
#include "sounddefinewindow.h"
#include "moviedefinewindow.h"
#include "selectsoundwindow.h"
#include "plugintypeeditwindow.h"
#include "levelscene.h"
#include "editorundocommand.h"
#include "logmanager.h"
#include "recyclelevelwindow.h"
using namespace ORION;

WorkspaceWindow::WorkspaceWindow(QWidget *parent) :
QWidget(parent), isClosing_(false),
    ui(new Ui::WorkspaceWindow)
{
    ui->setupUi(this);

	BEGIN_ACTION();
	CONNECT_ACTION(ORION_EDIT_DELETE, this, deleteProjectItem);
	CONNECT_ACTION(ORION_OBJECTTYPE_DELETE, this, deleteObjectItem)

	CONNECT_ACTION(ORION_OBJECTTYPE_ADDCATEGORY, this, addObjectGroup)

	CONNECT_ACTION(ORION_EDIT_SOUND, this, editSound)
	CONNECT_ACTION(ORION_EDIT_MOVIE, this, editMovie)
	CONNECT_ACTION(ORION_EDIT_OBJECTTYPE, this, editObjectType)

	END_ACTION();

	connect(ui->leveltreeWidget, SIGNAL(layerMoved(unsigned int, int)), this, SLOT(layerMoved(unsigned int, int)));

}

WorkspaceWindow::~WorkspaceWindow()
{
    delete ui;
}

void	WorkspaceWindow::resizeEvent(QResizeEvent * event){
	QSize newSize = event->size() - QSize(WIDGET_MARGIN * 2, WIDGET_MARGIN*2);
	ui->layoutWidget->setGeometry(QRect(QPoint(WIDGET_MARGIN, WIDGET_MARGIN), newSize));

	// 调整Tab控件大小
	QSize tabsize = ui->tabWidget->geometry().size();
	tabsize.setWidth(tabsize.width() - 8);
	tabsize.setHeight(tabsize.height() - 36);

	ui->verticalLayoutWidget->setGeometry(QRect(QPoint(WIDGET_MARGIN, WIDGET_MARGIN*2), tabsize));
	ui->layoutWidget1->setGeometry(QRect(QPoint(WIDGET_MARGIN, WIDGET_MARGIN*2), tabsize));
}


void WorkspaceWindow::updateLevel(ProjectResourceObject* proj){

	ui->leveltreeWidget->clear();

	QTreeWidgetItem* rootItem = new QTreeWidgetItem(ui->leveltreeWidget, ITEM_PROJECT);
	rootItem->setText(0, proj->objectName());
	rootItem->setData(0, ITEM_HASHKEY, proj->hashKey());
	proj->leveltreeItem_ = rootItem;


	QTreeWidgetItem* levelItem = NULL;
	QTreeWidgetItem* layerItem = NULL;

	foreach(LevelResourceObject* level, proj->levels_){
		if (level->flags() & URF_TagGarbage)
			continue;

		levelItem = ui->leveltreeWidget->addLevelItem(level);

		// 添加图层( 默认隐藏 )
		QTreeWidgetItem* layerItem = NULL;
		foreach(LayerResourceObject* layer, level->layers_){
			layerItem = ui->leveltreeWidget->addLayerItem(layer, levelItem);
			layerItem->setHidden(true);
		}
	}

	ui->leveltreeWidget->expandItem(rootItem);
}


void WorkspaceWindow::updateObject(ProjectResourceObject* proj){

	ui->objecttreeWidget->init( proj );

}
// 改变工作空间
void WorkspaceWindow::updateWorkspace(){

	// 更新态势树
	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	if (ws == NULL){
		QMessageBox::warning(this, UDQ_TR("新建项目"), UDQ_TR("请先打开一个项目!"));
		return;
	}

	// 获取当前电子书
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != 0 && !(proj->flags()  & URF_TagGarbage));

	updateLevel(proj);
	updateObject(proj);

	isClosing_ = false;

}


void WorkspaceWindow::updateWorkspaceIcon(){
	// 更新态势树
	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	if (ws == NULL){
		QMessageBox::warning(this, UDQ_TR("新建项目"), UDQ_TR("请先打开一个项目!"));
		return;
	}

	// 获取当前工程
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != 0 && !(proj->flags()  & URF_TagGarbage));

	// 添加组
	foreach(LevelResourceObject* level, proj->levels_){
		if (level->flags() & URF_TagGarbage)
			continue;
		// 添加页面
		ui->leveltreeWidget->updateItemIcon(level->treeItem_);
	}
}

void WorkspaceWindow::clearWorkspace(){

	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	Q_ASSERT(ws != NULL);

	// 清空所有信息
	ResourceObject::resetWorkspace(ws);

		// 清空树
	ui->leveltreeWidget-> clear();
	ui->objecttreeWidget->clearAllItems();

	setWindowTitle(UDQ_TR("工作空间"));
}


void  WorkspaceWindow::on_levelpushButton_clicked(){
	// 设置场景模式
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);

	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	if (ws == NULL){
		QMessageBox::warning(this, UDQ_TR("新建分组"), UDQ_TR("请先打开一个工程!"));
		return;
	}

	// 获取当前电子书
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != 0 && !(proj->flags()  & URF_TagGarbage));

	// 目前仅能打开一个项目，以后扩展到多个项目
	QTreeWidgetItem* rootItem = ui->leveltreeWidget->getProjectItem();

	LevelResourceObject* level = proj->addNewLevel(UDQ_T(""));
	Q_ASSERT(level != 0);
	QTreeWidgetItem* levelItem = ui->leveltreeWidget->addLevelItem(level);
	levelItem->setExpanded(false);

	// 设置保存标记
	proj->setFlags(URF_TagSave);

	Q_ASSERT(levelItem != NULL);
	ui->leveltreeWidget->expandItem(rootItem);
	ui->leveltreeWidget->scrollToItem(levelItem);

}

void  WorkspaceWindow::on_layerpushButton_clicked(){

	// 设置场景模式
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);

	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	if (ws == NULL){
		QMessageBox::warning(this, UDQ_TR("新建图层"), UDQ_TR("请先打开一个工程!"));
		return;
	}

	// 获取当前电子书
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != 0 && !(proj->flags()  & URF_TagGarbage));

	// 只能向打开的场景添加图层
	LevelScene* scene = (LevelScene*)(frame->sceneView()->scene());
	EditorLevel* levelItem = scene->getLevelItem();
	if (levelItem == NULL || levelItem->level_ == NULL ){
		QMessageBox::warning(this, UDQ_TR("新建图层"), UDQ_TR("请先打开一个场景!"));
		return;
	}

	LayerResourceObject* layer = levelItem->level_->addNewLayer(UDQ_T(""));
	Q_ASSERT(layer != 0);
	
	QUndoCommand* command = new AddLayerCommand(layer);
	frame->undoStack()->push(command);

}

void WorkspaceWindow::on_leveltreeWidget_currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous){
	// 设置场景模式
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);

	if (isClosing_){
		frame->propertyWindow()->clear();
	}
	else{

		if (current && current->type() == ITEM_LAYER){
			ResourceHash key = current->data(0, ITEM_HASHKEY).toUInt();
			frame->activeLayer(key);
		}
		frame->updateProperty(current);
	}

}

void WorkspaceWindow::deleteLevel(QTreeWidgetItem* item){
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);

	item->setSelected(false);

	// 删除单个页
	ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
	// 删除编辑器中的页面
	frame->removeLevel(key);

	// 删除页面项
	item->parent()->removeChild(item);
	delete item;
}


QTreeWidgetItem* WorkspaceWindow::addLayer(LayerResourceObject* layer){
	Q_ASSERT(layer->level_ && layer->level_->treeItem_);
	QTreeWidgetItem* item = ui->leveltreeWidget->addLayerItem(layer, layer->level_->treeItem_);

	return item;
}


// 删除单个页面
void WorkspaceWindow::deleteLayer(QTreeWidgetItem* item){
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);

	item->setSelected(false);

	// 删除单个页
	ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
	// 删除编辑器中的页面
	frame->removeLayerFromScene(key);

	// 删除页面项
	item->parent()->removeChild(item);
	delete item;
}

void WorkspaceWindow::deleteProjectItem(){
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);

	QTreeWidgetItem* selectedItem = ui->leveltreeWidget->currentItem();
	if (selectedItem != NULL){

		if (selectedItem->type() == ITEM_LEVEL){
			if (QMessageBox::question(this, windowTitle(), UDQ_TR("确认删除场景（可通过回收站恢复）？")) == QMessageBox::No){
				return;
			}
			// 删除选中的场景
			deleteLevel(selectedItem);
		}
		else if (selectedItem->type() == ITEM_LAYER){
			// 删除选中的图层
			ResourceHash key = selectedItem->data(0, ITEM_HASHKEY).toUInt();
			DEFINE_RESOURCE_HASHKEY_VALID(LayerResourceObject, layer, key);

			QUndoCommand* command = new DeleteLayerCommand(layer);
			frame->undoStack()->push(command);
		}
		else{
			Q_UNREACHABLE();
		}
	}
}

void WorkspaceWindow::addObjectGroup(){
	
	QTreeWidgetItem* item = ui->objecttreeWidget->currentItem();
	if (item == NULL)
		return;

	ui->objecttreeWidget->addGroup(item);

}

void WorkspaceWindow::deleteObjectItem(){
	// 删除对象类型
	QTreeWidgetItem* item = ui->objecttreeWidget->currentItem();
	if (item == NULL)
		return;

	if (item->type() == ITEM_OBJECTTYPE){
		// 
		if (QMessageBox::question(this, UDQ_TR("删除"), UDQ_TR("确认删除对象类型[%1]（其所有实例也会被删除,且不可恢复）?").arg(item->text(0)), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No){
			return;
		}
		ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
		emit deleteObjectType(key);

		// 删除
		ui->objecttreeWidget->clearSelection();
		ui->objecttreeWidget->deleteObjectType(key);
	}
	else if (item->type() == ITEM_FAMILY){
		// 删除集合
		if (QMessageBox::question(this, UDQ_TR("删除"), UDQ_TR("确认删除集合[%1]?").arg(item->text(0)), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No){
			return;
		}

		ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
		emit deleteFamily(key);

		// 删除
		ui->objecttreeWidget->rootFamilyItem_->removeChild(item);
	}
	else if (item->type() == ITEM_FAMILYMEMBER){

		ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
		DEFINE_RESOURCE_HASHKEY_VALID(ObjectTypeResourceObject, type, key);

		// 将对象类型从集合中移除
		FamilyResourceObject* family = ui->objecttreeWidget->findFamily(item);
		family->members_.removeOne(type);

		// 删除
		QTreeWidgetItem* parentItem = item->parent();
		parentItem->removeChild(item);
	}
	else if (item->type() == ITEM_CATEGORY){
		ui->objecttreeWidget->deleteGroupItem(item);
	}

}

void WorkspaceWindow::on_leveltreeWidget_itemDoubleClicked(QTreeWidgetItem * item, int column){

	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);

	// 切换场景
	if ( item && item->type() == ITEM_LEVEL && column == 0){

		LevelScene* scene = (LevelScene*)frame->sceneView()->scene();
		// 隐藏旧场景
		EditorLevel* levelItem = scene->getLevelItem();
		if (levelItem && levelItem->level_ ){
			QTreeWidgetItem* oldLevelItem = levelItem->level_->treeItem_;
			oldLevelItem->setExpanded(false);
			for (int i = 0; i < oldLevelItem->childCount(); i++){
				oldLevelItem->child(i)->setHidden(true);
			}
		}

		ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
		if (key == 0){
			gLogManager->logError(UDQ_TR("切换场景<%1>的key值无效，切换失败"));
			return;
		}

		// 更新图层数据
		item->setExpanded(true);
		for (int i = 0; i < item->childCount(); i++){
			item->child(i)->setHidden(false);
		}

		// 切换场景
		frame->switchLevel(key);
	}
}

void WorkspaceWindow::clearSelection(){

	ui->objecttreeWidget->clearSelection();
	ui->leveltreeWidget->clearSelection();
	isClosing_ = true;

}

void WorkspaceWindow::addObjectType(ResourceHash key){
	DEFINE_RESOURCE_HASHKEY(ObjectTypeResourceObject, type, key);
	if (key){
		ui->objecttreeWidget->addObjectTypeItem(type);
	}
}

void WorkspaceWindow::on_objecttreeWidget_currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous){
	// 设置场景模式
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);

	frame->updateProperty(current);
}

void WorkspaceWindow::refreshObjectType(ResourceHash key){
	
	DEFINE_RESOURCE_HASHKEY_VALID(ObjectTypeResourceObject, objType, key);

	// 缩略图
	QPixmap img;
	if (objType->prototype_->hasAttribute(EF_ANIMATIONS) || objType->prototype_->hasAttribute(EF_TEXTURE)){
		img = objType->thumbnailImage();
		if (img.isNull()){
			img = QPixmap::fromImage(objType->prototype_->icon_);
		}
	}

	objType->treeItem_->setText(0, objType->description());
	// 缩略图
	if (!img.isNull()){
		objType->treeItem_->setIcon(0, img);
	}

	// 更新Family

}

void WorkspaceWindow::refreshObjectTypes(){

	int num = ui->objecttreeWidget->rootEntityItem_->childCount();
	QTreeWidgetItem* item = NULL;
	for (int i = 0; i < num; i++){
		item = ui->objecttreeWidget->rootEntityItem_->child(i);
		ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
		DEFINE_RESOURCE_HASHKEY_VALID(ObjectTypeResourceObject, objType, key);
		item->setText(0, objType->description());

		// 缩略图
		if (objType->prototype_->hasAttribute(EF_ANIMATIONS) || objType->prototype_->hasAttribute(EF_TEXTURE)){
			QPixmap img = objType->thumbnailImage();
			if (img.isNull()){
				img = QPixmap::fromImage(objType->prototype_->icon_);
			}
			item->setIcon(0, img);
		}
	}

	// 更新Family

	// 更新Sound

	// 更新Movie
}

void  WorkspaceWindow::reorderLayerItems(LevelResourceObject* level){
	if (level->treeItem_ == NULL)
		return;
	// 重新调整
	ui->leveltreeWidget->updateLevelItem(level->treeItem_);
}

void WorkspaceWindow::layerMoved(unsigned int key, int order ){
	//  刷新当前场景
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);

	DEFINE_RESOURCE_HASHKEY_VALID(LayerResourceObject, layer, key);
	QUndoCommand* command = new MoveLayerCommand(layer, order);
	frame->undoStack()->push(command);
}

void  WorkspaceWindow::on_familypushButton_clicked(){
	// 新建脚本
	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	if (ws == NULL){
		QMessageBox::warning(this, UDQ_TR("新建集合"), UDQ_TR("请先打开一个工程!"));
		return;
	}

	// 获取当前电子书
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != 0 && !(proj->flags()  & URF_TagGarbage));

	FamilyResourceObject* family = proj->addNewFamily();
	Q_ASSERT(family != 0);
	QTreeWidgetItem* familyItem = ui->objecttreeWidget->addFamilyItem(family);
	
	// 设置保存标记
	proj->setFlags(URF_TagSave);
}

void  WorkspaceWindow::on_soundpushButton_clicked(){
	// 新建脚本
	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	if (ws == NULL){
		QMessageBox::warning(this, UDQ_TR("新建集合"), UDQ_TR("请先打开一个工程!"));
		return;
	}
	// 获取当前电子书
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != 0 && !(proj->flags()  & URF_TagGarbage));

	SelectSoundWindow w(0);
	if (w.exec() == QDialog::Accepted){
		// 添加新的声音
		DEFINE_RESOURCE_HASHKEY_VALID(SoundResourceObject, sound, w.soundKey_);
		ui->objecttreeWidget->addSoundItem( sound );
		// 设置保存标记
		proj->setFlags(URF_TagSave);
	}

}

void  WorkspaceWindow::on_moviepushButton_clicked(){
	// 新建脚本
	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	if (ws == NULL){
		QMessageBox::warning(this, UDQ_TR("新建集合"), UDQ_TR("请先打开一个工程!"));
		return;
	}

	// 获取当前电子书
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != 0 && !(proj->flags()  & URF_TagGarbage));

}

void WorkspaceWindow::editSound(){
	QTreeWidgetItem* currentItem = ui->objecttreeWidget->currentItem();
	if (currentItem){
		if (currentItem->type() == ITEM_SOUND){
			ResourceHash key = currentItem->data(0, ITEM_HASHKEY).toUInt();
			DEFINE_RESOURCE_HASHKEY_VALID(SoundResourceObject, sound, key);

			SoundDefineWindow w(sound);
			w.exec();

			// 更新名称
			currentItem->setText(0, sound->description());
		}
	}
}

void WorkspaceWindow::editMovie(){

}

void WorkspaceWindow::editObjectType(){
	QTreeWidgetItem* currentItem = ui->objecttreeWidget->currentItem();
	if (currentItem){
		if (currentItem->type() == ITEM_OBJECTTYPE){
			ResourceHash key = currentItem->data(0, ITEM_HASHKEY).toUInt();
			DEFINE_RESOURCE_HASHKEY_VALID(ObjectTypeResourceObject, objType, key);

			PluginTypeEditWindow w(objType);
			if (w.exec() == QDialog::Accepted){
				ui->objecttreeWidget->refreshItem(currentItem);
				// 刷新属性列表
				MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
				Q_ASSERT(frame != NULL);
				frame->updateProperty(currentItem);
			}
		}
	}
}

void WorkspaceWindow::on_objecttreeWidget_itemChanged(QTreeWidgetItem * item, int column){
	if (item && item->type() == ITEM_OBJECTTYPE && column == 0 ){
		ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
		DEFINE_RESOURCE_HASHKEY_VALID(ObjectTypeResourceObject, objType, key);
		ui->objecttreeWidget->refreshFamilyMember(objType->hashKey());
	}
	else if (item && item->type() == ITEM_CATEGORY && column == 0){
		// 检查重名
		bool dup = false;
		int num = item->parent()->childCount();
		QTreeWidgetItem* child = NULL;
		for (int i = 0; i < num; i++){
			child = item->parent()->child(i);
			if (child != item && child->text(0) == item->text(0)){
				// 发生重名，更改名字
				QMessageBox::warning(this, windowTitle(), UDQ_TR("发生重名"));
				item->setText(0, item->data(0, ITEM_VALUE).toString());
				dup = true;
				break;
			}
		}

		if (!dup){
			item->setData(0,  ITEM_VALUE, item->text(0));
		}
	}
}

void WorkspaceWindow::saveObjectType(){
	ui->objecttreeWidget->updateObjectTypeCategory();
}


void  WorkspaceWindow::on_trashpushButton_clicked(){
	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	if (ws == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请先打开一个工程!"));
		return;
	}

	// 获取当前电子书
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != 0 && !(proj->flags()  & URF_TagGarbage));

	RecycleLevelWindow w(proj);
	if (w.exec() == QDialog::Accepted){
		// 添加新场景对象
		QTreeWidgetItem* levelItem = ui->leveltreeWidget->addLevelItem(w.level_);
		// 添加图层( 默认隐藏 )
		QTreeWidgetItem* layerItem = NULL;
		foreach(LayerResourceObject* layer, w.level_->layers_){
			layerItem = ui->leveltreeWidget->addLayerItem(layer, levelItem);
			layerItem->setHidden(true);
		}
	}
}

void WorkspaceWindow::recycleObjectType(ObjectTypeResourceObject* obj){

	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	Q_ASSERT(ws != NULL);

	// 获取当前电子书
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != 0 && !(proj->flags()  & URF_TagGarbage));

	obj->clearFlags(URF_TagGarbage);
	proj->objTypes_.append(obj);

	ui->objecttreeWidget->addObjectTypeItem(obj);
}