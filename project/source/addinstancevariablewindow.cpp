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

#include "addinstancevariablewindow.h"
#include "ui_addinstancevariablewindow.h"
#include "tooldefine.h"

AddInstanceVariableWindow::AddInstanceVariableWindow(const QStringList& items, QWidget *parent) :
QDialog(parent), 
    ui(new Ui::AddInstanceVariableWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	ui->idcomboBox->addItems(items);

}

AddInstanceVariableWindow::~AddInstanceVariableWindow()
{
    delete ui;
}


bool AddInstanceVariableWindow::canSave(){

	COMBOX_CHECK(ui->idcomboBox, windowTitle(), UDQ_TR("名称"));

	QString id = ui->idcomboBox->currentText();
	if (id.startsWith(UDQ_T("@"))){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("变量名不能以@开头"));
		return false;
	}

	QString dt = ui->typecomboBox->currentText();
	if (dt == UDQ_T("bool")){ // 调整bool值
		QString value = ui->valuelineEdit->text();
		if (value != UDQ_T("true") || value != UDQ_T("1")){
			ui->valuelineEdit->setText(UDQ_T("false"));
		}
		else{
			ui->valuelineEdit->setText(UDQ_T("true"));
		}
	}

	return true;
}

void  AddInstanceVariableWindow::on_cancelpushButton_clicked(){
	reject();
}

void  AddInstanceVariableWindow::on_okpushButton_clicked(){
	if (!canSave())
		return;

	QString name = ui->idcomboBox->currentText();
	if (ui->idcomboBox->findText(name) >= 0){
		if (QMessageBox::question(this, windowTitle(), UDQ_TR("变量[%1]已经存在，是否覆盖?").arg(name), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No){
			return;
		}
	}

	newvar_.clear();
	newvar_.append(name);
	newvar_.append(ui->typecomboBox->currentText());
	newvar_.append(ui->valuelineEdit->text());
	newvar_.append(ui->plainTextEdit->toPlainText());

	accept();
}