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

#include "aboutwindow.h"
#include "ui_aboutwindow.h"
#include "configmanager.h"
#include "commondefine.h"
using namespace ORION;

AboutWindow::AboutWindow(QWidget *parent) :
QDialog(parent),
    ui(new Ui::AboutWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	QString html = ui->textEdit->toHtml();
	html.replace(UDQ_T("%BUILDVERSION%"), gConfigManager->buildVersion());
	ui->textEdit->setHtml(html);
}

AboutWindow::~AboutWindow()
{
    delete ui;
}

void AboutWindow::on_okpushButton_clicked(){
	reject();
}