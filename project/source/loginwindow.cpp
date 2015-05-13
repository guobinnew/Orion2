/************************************************************************
**
**  Copyright (C) 2014  guobin  <guobin.gb@alibaba-inc.com>
**
**  This file is part of OSSSnake.
**
** OSSSnake is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation.
**
**  OSSSnake is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with OSSSnake.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "configmanager.h"
#include "commondefine.h"
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent) :
QDialog(parent), retryNum_(5),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
	setFixedSize(size());
	code_ = gConfigManager->loginCode();
	connect(ui->loginwidget, SIGNAL(inputPwd(const QByteArray&)), this, SLOT(processAuthorization(const QByteArray&)));
	ui->infolabel->setText(UDQ_TR("请输入解锁图案(可以尝试%1次)").arg(retryNum_));
}

LoginWindow::~LoginWindow()
{
    delete ui;
}


void LoginWindow::processAuthorization(const QByteArray& data){
	QString text;

	retryNum_--;
	if (data.size() == 0){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("输入图案不符合要求"));
	}
	else{

		if (code_ == data){
			ui->loginwidget->setLock(false);
			text = UDQ_TR("解锁成功");
			emit loginEvent(true);
			accept();
		}
	}

	if ( retryNum_ <= 0){
		ui->loginwidget->setLock(false);
		QMessageBox::warning(this, windowTitle(), UDQ_TR("解锁失败"));
		emit loginEvent(false);
		reject();
	}
	else{
		ui->loginwidget->setLock(true);
		text = UDQ_TR("输入错误, 请输入解锁图案(可以尝试%1次)").arg(retryNum_);
	}
	ui->infolabel->setText(text);

}

void LoginWindow::closeEvent(QCloseEvent * event){
	emit exitLogin();
}
