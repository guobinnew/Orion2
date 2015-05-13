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


#include "spritetreewidget.h"
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

SpriteTreeWidget::SpriteTreeWidget(QWidget *parent) : OrionTreeWidget(parent){
	defaultSpriteIcon_ = QIcon(UDQ_T(":/images/animations.png"));
	setSelectionMode(QAbstractItemView::SingleSelection);
	setIconSize(QSize(24, 24));

}


SpriteTreeWidget::~SpriteTreeWidget(){

}

void  SpriteTreeWidget::contextMenuEvent(QContextMenuEvent * event){

	QTreeWidgetItem* selectedItem = currentItem();
	if (selectedItem == NULL){
		event->ignore();
		return;
	}

	// 弹出右键菜单
	QMenu menu(this);
	QAction* act;

	if (selectedItem->type() == ITEM_SPRITE  ){
		act = gMenuManager->getAction(ORION_SPRTIE_COPY);
		menu.addAction(act);
	}
	else{
	}

	QRect rect = visualItemRect(selectedItem);
	menu.exec(mapToGlobal(QPoint(rect.left(), rect.bottom())));
	event->accept();
}

QTreeWidgetItem* SpriteTreeWidget::addSpriteItem(SpriteResourceObject* sprite){
	QTreeWidgetItem* item = new QTreeWidgetItem(ITEM_SPRITE);
	item->setText(0, sprite->description());
	item->setIcon(0, defaultSpriteIcon_);
	item->setData(0, ITEM_HASHKEY, sprite->hashKey());
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
	addTopLevelItem(item);

	return item;
}
