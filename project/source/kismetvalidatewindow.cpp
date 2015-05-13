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

#include "kismetvalidatewindow.h"
#include "ui_kismetvalidatewindow.h"
#include "commondefine.h"
#include "kismetdefine.h"

KismetValidateWindow::KismetValidateWindow(const QStringList& errors, QWidget *parent) :
QDialog(parent),
    ui(new Ui::KismetValidateWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	warningIcon_ = QIcon(UDQ_T(":/images/msg_warning.png"));
	errorIcon_ = QIcon(UDQ_T(":/images/msg_error.png"));

	ui->errorlistWidget->clear();

	bool error = false;
	foreach(QString e, errors){
		QStringList items = e.split(UDQ_T("@"));
		if (items.size() > 1){
			error =( items[0].toUInt() == 1);
		}
		else{
			error = true;
		}

		QListWidgetItem* item = new QListWidgetItem();
		item->setIcon(error ? errorIcon_ : warningIcon_);
		if (error){
			item->setBackgroundColor(CONDITION_COLOR);
		}

		item->setText(items.last());
		ui->errorlistWidget->addItem(item);
	}
}

KismetValidateWindow::~KismetValidateWindow()
{
    delete ui;
}


void KismetValidateWindow::on_cancelpushButton_clicked(){
	reject();
}

void KismetValidateWindow::on_savepushButton_clicked(){
	accept();
}