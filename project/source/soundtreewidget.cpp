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


#include "soundtreewidget.h"
#include <QtGui>
#include <QMenu>
#include <QHeaderView>
#include <QRadioButton>
#include <QCheckBox>
#include "commondefine.h"
#include "leveldefine.h"
#include "tooldefine.h"
#include "menumanager.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

SoundTreeWidget::SoundTreeWidget(QWidget *parent) : OrionTreeWidget(parent){
	defaultSoundIcon_ = QIcon(UDQ_T(":/images/gallery_sound.png"));
	setSelectionMode(QAbstractItemView::SingleSelection);
	setIconSize(QSize(24, 24));

}


SoundTreeWidget::~SoundTreeWidget(){

}

void  SoundTreeWidget::contextMenuEvent(QContextMenuEvent * event){

	QTreeWidgetItem* selectedItem = currentItem();
	if (selectedItem == NULL){
		event->ignore();
		return;
	}

	// 弹出右键菜单
	QMenu menu(this);
	QAction* act;

	if (selectedItem->type() == ITEM_SOUND  ){
		act = gMenuManager->getAction(ORION_EDIT_SOUND);
		menu.addAction(act);
	}
	else{
	}

	QRect rect = visualItemRect(selectedItem);
	menu.exec(mapToGlobal(QPoint(rect.left(), rect.bottom())));
	event->accept();
}

QTreeWidgetItem* SoundTreeWidget::addSoundItem(SoundResourceObject* sound){
	QTreeWidgetItem* item = new QTreeWidgetItem(ITEM_SOUND);
	item->setText(0, sound->description());
	item->setIcon(0, defaultSoundIcon_);
	item->setData(0, ITEM_HASHKEY, sound->hashKey());
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
	addTopLevelItem(item);

	return item;
}
