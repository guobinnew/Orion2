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

#include "componentlistwidget.h"
#include <QtGui>
#include <QMenu>
#include "commondefine.h"
#include "tooldefine.h"
#include "componentdefine.h"
#include "menumanager.h"
#include "logmanager.h"
using namespace ORION;

ComponentListWidget::ComponentListWidget(QWidget *parent) : QListWidget(parent){

	setDragEnabled(true);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setIconSize(QSize(24, 24));
	setDragDropMode(QAbstractItemView::DragDrop);
	setUniformItemSizes(true);

}

ComponentListWidget::~ComponentListWidget(){

}

QMimeData * ComponentListWidget::mimeData(const QList<QListWidgetItem *> items) const{

	Q_ASSERT(items.size() == 1);

	QMimeData* mmdata = new QMimeData;
	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);

	ResourceHash key = items[0]->data(ITEM_HASHKEY).toUInt();
	dataStream << key;

	mmdata->setData(MIME_ORION_ENTITY, itemData);
	return mmdata;
}


void ComponentListWidget::startDrag(Qt::DropActions supportedActions){
	QListWidgetItem* item = currentItem();
	if (item) {
		QMimeData *data = mimeData(selectedItems());
		if (!data)
			return;
		QRect rect;
		rect.adjust(horizontalOffset(), verticalOffset(), 0, 0);
		QDrag *drag = new QDrag(this);
		ResourceHash key = item->data(ITEM_HASHKEY).toUInt();
		DEFINE_RESOURCE_HASHKEY_VALID(PluginResourceObject, plugin, key);
		drag->setPixmap(QPixmap::fromImage(plugin->icon_));
		drag->setMimeData(data);
		drag->setHotSpot(QPoint(drag->pixmap().width() / 2, drag->pixmap().height() / 2));
		drag->exec(Qt::MoveAction);
	}
}