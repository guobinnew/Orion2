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
#include "spritestructwindow.h"
#include "tooldefine.h"
#include "ui_spritestructwindow.h"
#include "framemanager.h"
#include "mainspriteframe.h"
#include "renamewindow.h"
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;

SpriteStructWindow::SpriteStructWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpriteStructWindow)
{
    ui->setupUi(this);

	initFromDb();
}

SpriteStructWindow::~SpriteStructWindow()
{
    delete ui;
}

void	SpriteStructWindow::resizeEvent(QResizeEvent * event){
	QSize newSize = event->size() - QSize(WIDGET_MARGIN * 2, WIDGET_MARGIN * 2);
	ui->layoutWidget->setGeometry(QRect(QPoint(WIDGET_MARGIN, WIDGET_MARGIN), newSize));
}

void SpriteStructWindow::initFromDb(){

	// 加载列表
	for (TResourceObjectIterator<SpriteResourceObject> it; it; ++it){
		SpriteResourceObject* sprite = (SpriteResourceObject*)(*it);

		if (sprite->flags() & URF_TagGarbage)
			continue;

		QString desc = sprite->description();
		if (desc.isEmpty()){
			desc = sprite->objectName();
			sprite->setDescription(desc);
			sprite->setFlags(URF_TagSave);
		}

		ui->treeWidget->addSpriteItem(sprite);
	}
}

void SpriteStructWindow::on_delpushButton_clicked(){

	QTreeWidgetItem* item = ui->treeWidget->currentItem();
	if (item == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请选择一个动画资源!"));
		return;
	}

	ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
	DEFINE_RESOURCE_HASHKEY_VALID(SpriteResourceObject, sprite, key);

	if (QMessageBox::question(this, windowTitle(), UDQ_TR("确认删除动画资源[%1]?").arg(sprite->description()), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No){
		return;
	}

	sprite->setFlags(URF_TagGarbage);
	ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(item));
	delete item;

	emit deleteSprite(key);
}

void SpriteStructWindow::on_addpushButton_clicked(){

	// 新建一个精灵对象
	DEFINE_RESOURCE_PATH(PackageResourceObject, pkgObj, DEFAULT_PACKAGE);
	Q_ASSERT(pkgObj != NULL);

	// 保存到数据库中
	SpriteResourceObject* sprite = TCreateResource<SpriteResourceObject>(pkgObj, UDQ_T(""));
	Q_ASSERT(sprite != 0);

	sprite->setDescription(sprite->objectName());
	sprite->setFlags(URF_TagSave);
	// 保存到本地
	SAVE_PACKAGE_RESOURCE(SpriteResourceObject, sprite);

	ui->treeWidget->addSpriteItem(sprite);

}

void SpriteStructWindow::on_treeWidget_itemChanged(QTreeWidgetItem * item, int column){
	if (item && column == 0 ){
		ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
		DEFINE_RESOURCE_HASHKEY_VALID(SpriteResourceObject, sprite, key);
		sprite->setDescription(item->text(0));
		sprite->setFlags(URF_TagSave);

		MainSpriteFrame* frame = (MainSpriteFrame*)gFrameManager->getWindow(FrameManager::WID_SPRITE);
		Q_ASSERT(frame != NULL);
		frame->updateSpriteName();
	}
}

void SpriteStructWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem * item, int column){

	if (item == NULL)
		return;

	// 保存当前的精灵
	MainSpriteFrame* frame = (MainSpriteFrame*)gFrameManager->getWindow(FrameManager::WID_SPRITE);
	Q_ASSERT(frame != NULL);
	frame->saveCurrentSprite();

	ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
	frame->loadSprite(key, item->text(0));

}

void SpriteStructWindow::on_savepushButton_clicked(){
	// 保存当前精灵
	MainSpriteFrame* frame = (MainSpriteFrame*)gFrameManager->getWindow(FrameManager::WID_SPRITE);
	Q_ASSERT(frame != NULL);
	frame->submitCurrentSprite();
}

void SpriteStructWindow::on_saveallpushButton_clicked(){
	// 保存当前精灵
	MainSpriteFrame* frame = (MainSpriteFrame*)gFrameManager->getWindow(FrameManager::WID_SPRITE);
	Q_ASSERT(frame != NULL);
	frame->submitAllSprite();
}

void SpriteStructWindow::on_refreshpushButton_clicked(){
	// 保存当前精灵
	MainSpriteFrame* frame = (MainSpriteFrame*)gFrameManager->getWindow(FrameManager::WID_SPRITE);
	Q_ASSERT(frame != NULL);
	frame->refreshCurrentSprite();
}

void SpriteStructWindow::on_addseqpushButton_clicked(){
	// 保存当前精灵
	MainSpriteFrame* frame = (MainSpriteFrame*)gFrameManager->getWindow(FrameManager::WID_SPRITE);
	Q_ASSERT(frame != NULL);
	frame->addSequenceItem();
}