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

#include "selectspritewindow.h"
#include "ui_selectspritewindow.h"
#include <QMessageBox>
#include "tooldefine.h"

SelectSpriteWindow::SelectSpriteWindow(ResourceHash key, QWidget *parent) :
QDialog(parent), spriteH_(0),
    ui(new Ui::SelectSpriteWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	QTreeWidgetItem* item = NULL;
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

		item = ui->treeWidget->addSpriteItem(sprite);
		if (sprite->hashKey() == key){
			item->setSelected(true);
		}
	}


}

SelectSpriteWindow::~SelectSpriteWindow()
{
    delete ui;
}

void SelectSpriteWindow::on_savepushButton_clicked(){
	
	QTreeWidgetItem* item = ui->treeWidget->currentItem();
	if (item == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请选择一个动画"));
		return;
	}

	spriteH_ = item->data(0, ITEM_HASHKEY).toUInt();
	Q_ASSERT(spriteH_ != 0);
	accept();
}

void SelectSpriteWindow::on_cancelpushButton_clicked(){
	reject();
}