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

#include "eventtreewidget.h"
#include <QtGui>
#include <QMenu>
#include "commondefine.h"
#include "tooldefine.h"
#include "kismetdefine.h"
#include "componentdefine.h"
#include "menumanager.h"
#include "logmanager.h"
using namespace ORION;

EventTreeWidget::EventTreeWidget(QWidget *parent) : OrionTreeWidget(parent){

	setDragEnabled(true);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setIconSize(QSize(24, 24));
	setDragDropMode(QAbstractItemView::DragOnly);

}

EventTreeWidget::~EventTreeWidget(){

}

QMimeData * EventTreeWidget::mimeData(const QList<QTreeWidgetItem *> items) const{

	Q_ASSERT(items.size() == 1);
	Q_ASSERT(items[0]->type() != ITEM_GROUP );

	QMimeData* mmdata = new QMimeData;
	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);

	// 获取所在插件的key
	QTreeWidgetItem* rootItem = items[0]->parent();
	while (rootItem && rootItem->type() != ITEM_CATEGORY){
		rootItem = rootItem->parent();
	}
	ResourceHash key = 0;
	if (rootItem){
		key = rootItem->data(0, ITEM_HASHKEY).toUInt();
	}
	else{
		key = invisibleRootItem()->data(0, ITEM_HASHKEY).toUInt();
	}

	// 只添加第一个
	dataStream << key;  // 插件hash值
	dataStream << items[0]->data(0, ITEM_HASHKEY).toInt();
	ResourceHash inter = items[0]->data(0, ITEM_VALUE).toUInt();
	dataStream << items[0]->data(0,ITEM_VALUE).toUInt();

	mmdata->setData(MIME_KISMET_EVENT, itemData);
	return mmdata;
}

void EventTreeWidget::startDrag(Qt::DropActions supportedActions){
	QTreeWidgetItem* item = currentItem();
	if (item) {
		QMimeData *data = mimeData(selectedItems());
		if (!data)
			return;
		QRect rect;
		rect.adjust(horizontalOffset(), verticalOffset(), 0, 0);
		QDrag *drag = new QDrag(this);

		QTreeWidgetItem* rootItem = item->parent();
		while (rootItem && rootItem->type() != ITEM_CATEGORY){
			rootItem = rootItem->parent();
		}

		ResourceHash key = 0;
		if (rootItem){
			key = rootItem->data(0, ITEM_HASHKEY).toUInt();
		}
		else{
			key = invisibleRootItem()->data(0, ITEM_HASHKEY).toUInt();
		}

		DEFINE_RESOURCE_HASHKEY_VALID(PluginResourceObject, plugin, key);
		drag->setPixmap(QPixmap::fromImage(plugin->icon_));
		drag->setMimeData(data);
		drag->setHotSpot(QPoint(drag->pixmap().width() / 2, drag->pixmap().height() / 2));
		drag->exec(Qt::MoveAction);
	}
}