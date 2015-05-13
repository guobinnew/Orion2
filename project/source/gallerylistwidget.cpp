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

#include "gallerylistwidget.h"
#include <QtGui>
#include <QMenu>
#include "commondefine.h"
#include "editordefine.h"
#include "tooldefine.h"
#include "menumanager.h"
using namespace ORION;

GalleryListWidget::GalleryListWidget(QWidget *parent) : QListWidget(parent){
	autoResize_ = false;
	setViewMode(QListView::IconMode);
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::DragDrop);

}

GalleryListWidget::~GalleryListWidget(){

}

void  GalleryListWidget::contextMenuEvent(QContextMenuEvent * event){

	QList<QListWidgetItem*> items = selectedItems();
	if (items.size() == 0){
		event->ignore();
		return;
	}

	// 必须全部是图片资源
	QList<ResourceHash> imgkeys;
	foreach(QListWidgetItem* item, items){
		ResourceHash key =item->data(ITEM_HASHKEY).toUInt();
		DEFINE_RESOURCE_HASHKEY(ImageResourceObject, img, key);
		if (img){
			imgkeys.append(key);
		}
	}

	// 弹出右键菜单
	QMenu menu(this);
	// 添加菜单

	if (items.size() > 0){
		// 添加
		menu.addAction(gMenuManager->getAction(ORION_GALLERY_COPY));
	}

	if (imgkeys.size() == 1){
		// 添加
		menu.addAction(gMenuManager->getAction(ORION_GALLERY_SPLIT));
	}

	if (imgkeys.size() > 1){
		// 添加
		menu.addAction(gMenuManager->getAction(ORION_GALLERY_MERGE));
	}

	if (imgkeys.size() >= 1){
		menu.addAction(gMenuManager->getAction(ORION_GALLERY_TRANSPARENT));
	}

	QRect rect = visualItemRect(currentItem());
	menu.exec(event->globalPos());
	event->accept();
}

QMimeData * GalleryListWidget::mimeData(const QList<QListWidgetItem *> items) const{

	Q_ASSERT(items.size() >= 1);

	QMimeData* mmdata = new QMimeData;
	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);

	dataStream << items.size();

	foreach(QListWidgetItem* item,items){
		ResourceHash hash = item->data(ITEM_HASHKEY).toUInt();
		Q_ASSERT(item->type() == ITEM_GALLERY);	
		dataStream << hash;
	}

	mmdata->setData(MIME_ORION_GALLERY, itemData);
	
	return mmdata;
}

void	GalleryListWidget::keyPressEvent(QKeyEvent * event){
	if (isPressDelete(event->key()) && currentItem() != NULL ){
		emit removeFromCategory( currentItem() );
	}

	QListWidget::keyPressEvent(event);
}

