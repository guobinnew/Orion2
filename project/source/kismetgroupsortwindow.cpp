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
**  Orion2 is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion2.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#include "kismetgroupsortwindow.h"
#include "ui_kismetgroupsortwindow.h"
#include "tooldefine.h"

KismetGroupSortWindow::KismetGroupSortWindow(KismetScene* scene, QWidget *parent) :
QDialog(parent), scene_(scene),
    ui(new Ui::KismetGroupSortWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	// 
	foreach(BlueprintGroup* grp, scene_->groupObjects()){
		QListWidgetItem* item = new QListWidgetItem(); 
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
		item->setText(grp->title());
		item->setIcon(QIcon(":/images/group_new.png"));
		item->setData(ITEM_VALUE, grp->iid());
		ui->listWidget->addItem(item);
	}
}

KismetGroupSortWindow::~KismetGroupSortWindow()
{
    delete ui;
}

void KismetGroupSortWindow::on_okpushButton_clicked(){
	QList<BlueprintGroup*>& grps = scene_->groupObjects();
	grps.clear();

	for (int i = 0; i < ui->listWidget->count(); i++){
		ResourceHash iid = ui->listWidget->item(i)->data(ITEM_VALUE).toUInt();
		grps.append((BlueprintGroup*)scene_->getObjectItem(iid));
	}

	accept();
}

void KismetGroupSortWindow::on_cancelpushButton_clicked(){

	reject();
}