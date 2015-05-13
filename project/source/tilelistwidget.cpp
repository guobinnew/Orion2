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

#include "tilelistwidget.h"
#include <QtGui>
#include <QMenu>
#include "commondefine.h"
#include "spritedefine.h"
#include "tooldefine.h"
#include "imagehelper.h"
#include "menumanager.h"
using namespace ORION;

TileListWidget::TileListWidget(QWidget *parent) : QListWidget(parent){

}

TileListWidget::~TileListWidget(){

}

void  TileListWidget::contextMenuEvent(QContextMenuEvent * event){

	QList<QListWidgetItem*> items = selectedItems();
	if (items.size() == 0){
		event->ignore();
		return;
	}
	event->ignore();
}

QMimeData * TileListWidget::mimeData(const QList<QListWidgetItem *> items) const{

	Q_ASSERT(items.size() >= 1);

	QMimeData* mmdata = new QMimeData;
	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);

	dataStream << hostId_;
	dataStream << items.size();

	// °´Ö¡Ë÷ÒýÅÅÐò
	QList<int> indexes;
	foreach (QListWidgetItem* item,  items){ 
		indexes.append(row(item));
	}
	qSort(indexes.begin(), indexes.end(), qGreater<int>());

	foreach( int index, indexes){
		dataStream << index;
	}

	mmdata->setData(MIME_ORION_TILE, itemData);
	return mmdata;
}

void TileListWidget::clearTiles(){
	frames_.clear();
	clear();
}

void TileListWidget::addTile( QImage& img){
	frames_.append(img);

	QListWidgetItem* item = new QListWidgetItem();
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
	item->setText(QString::number(frames_.size()));

	// ¼ÓÔØÎÄ¼þËõÂÔÍ¼
	QImage thumb = thnumImage(64, 64, img);
	item->setIcon(QIcon(QPixmap::fromImage(thumb)));
	addItem(item);
}