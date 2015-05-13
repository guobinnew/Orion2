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
#include "spritegallerywindow.h"
#include "tooldefine.h"
#include "ui_spritegallerywindow.h"
#include "selecttilesetwindow.h"
#include "mainspriteframe.h"
#include "framemanager.h"

SpriteGalleryWindow::SpriteGalleryWindow(QWidget *parent) :
QWidget(parent), sprite_(NULL), isClearing_(false),
ui(new Ui::SpriteGalleryWindow)
{
	ui->setupUi(this);
}

SpriteGalleryWindow::~SpriteGalleryWindow()
{
	delete ui;
}

void	SpriteGalleryWindow::resizeEvent(QResizeEvent * event){
	QSize newSize = event->size() - QSize(WIDGET_MARGIN * 2, WIDGET_MARGIN * 2);
	ui->layoutWidget->setGeometry(QRect(QPoint(WIDGET_MARGIN, WIDGET_MARGIN), newSize));
}

void SpriteGalleryWindow::on_addpushButton_clicked(){
	if (sprite_ == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请先选择一个精灵对象"));
		return;
	}

	SelectTilesetWindow w(0);
	connect(&w, SIGNAL(addGallery(unsigned int, unsigned int)), this, SLOT(addGalleryItem(unsigned int, unsigned int)));

	w.exec();

}


void SpriteGalleryWindow::addGalleryItem(unsigned int key, unsigned int id){
	// 添加精灵素材
	DEFINE_RESOURCE_HASHKEY_VALID(ImageResourceObject, img, key);
	QListWidgetItem* item = NULL;
	if (id == 0){
		id = hashString(QUuid::createUuid().toString());
		item = new QListWidgetItem();
		item->setData(ITEM_CONTEXT, id);
		ui->listWidget->addItem(item);
	}
	else{
		for (int i = 0; i < ui->listWidget->count(); i++){
			QListWidgetItem* child = ui->listWidget->item(i);
			if (id == child->data(ITEM_CONTEXT).toUInt()){
				item = child;
				break;
			}
		}
	}

	// 新建
	sprite_->addImageSource(id, key);
	item->setText(img->objectName() + QString(UDQ_T("(%1)")).arg(img->imageCount()));
	item->setIcon(img->thumbnail_);
	// 通知刷新显示
	sprite_->setFlags(URF_TagSave);

}


void SpriteGalleryWindow::clear(){
	isClearing_ = true;
	ui->listWidget->clear();
	isClearing_ = false;
}

void SpriteGalleryWindow::init(SpriteResourceObject* sprite){
	sprite_ = sprite;
	clear();

	if (sprite_ == NULL)
		return;

	// 添加素材列表
	foreach(ResourceHash id, sprite_->imgSources_.keys()){
		ResourceHash key = sprite_->imgSources_.value(id);
		QListWidgetItem* item = new QListWidgetItem();
		DEFINE_RESOURCE_HASHKEY(ImageResourceObject, img, key);
		if (img == NULL){
			// 图像失效
			item->setText(QString(UDQ_T("错误图像(%1)")).arg(key));
			item->setIcon(QIcon(UDQ_T(":/images/sprite_alert.png")));
		}
		else{
			item->setText(img->objectName() + QString(UDQ_T("(%1)")).arg(img->imageCount()));
			item->setIcon(img->thumbnail_);
		}
		item->setData(ITEM_CONTEXT, id);		
		ui->listWidget->addItem(item);
	}

}

void SpriteGalleryWindow::on_listWidget_itemSelectionChanged(){
	if (isClearing_)
		return;

	QListWidgetItem* item = ui->listWidget->currentItem();
	MainSpriteFrame* frame = (MainSpriteFrame*)gFrameManager->getWindow(FrameManager::WID_SPRITE);
	Q_ASSERT(frame != NULL);
	if (item == NULL){
		frame->tilesetWindow()->clear(); 
	}
	else{
		frame->tilesetWindow()->loadGallery(sprite_, item->data(ITEM_CONTEXT).toUInt());
	}

}

void SpriteGalleryWindow::on_replacepushButton_clicked(){

	if (sprite_ == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请先选择一个精灵对象"));
		return;
	}

	QListWidgetItem* item = ui->listWidget->currentItem();
	if (item == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请先要替换的素材!"));
		return;
	}

	ResourceHash id =  item->data(ITEM_CONTEXT).toUInt();
	SelectTilesetWindow w(id, this);
	connect(&w, SIGNAL(addGallery(ResourceHash, int, const QSize&, const QSize&, int)), this, SLOT(addGalleryItem(ResourceHash, int, const QSize&, const QSize&, int)));

	if (w.exec() == QDialog::Accepted){
		// 更新Tile列表
		QByteArray data = item->data(ITEM_CONTEXT).toByteArray();
		MainSpriteFrame* frame = (MainSpriteFrame*)gFrameManager->getWindow(FrameManager::WID_SPRITE);
		Q_ASSERT(frame != NULL);
		frame->tilesetWindow()->loadGallery(sprite_, id);

		// 刷新动画序列
		frame->refreshCurrentSprite();
	}
}

void SpriteGalleryWindow::on_delpushButton_clicked(){
	if (sprite_ == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请先选择一个精灵对象"));
		return;
	}

	QListWidgetItem* item = ui->listWidget->currentItem();
	if (item == NULL){
		return;
	}

	if (QMessageBox::question(this, windowTitle(), UDQ_TR("确认删除选中的素材，相关动画序列也显示错误?")) == QMessageBox::No){
		return;
	}

	sprite_->removeImageSource(item->data(ITEM_CONTEXT).toUInt());
	sprite_->setFlags(URF_TagSave);

	ui->listWidget->takeItem(ui->listWidget->row(item));
	delete item;

	// 刷新动画序列
	MainSpriteFrame* frame = (MainSpriteFrame*)gFrameManager->getWindow(FrameManager::WID_SPRITE);
	Q_ASSERT(frame != NULL);
	frame->refreshCurrentSprite();
}
