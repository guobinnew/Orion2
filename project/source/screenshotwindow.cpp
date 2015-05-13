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
#include "screenshotwindow.h"
#include "ui_screenshotwindow.h"
#include <QColorDialog>
#include <QMessageBox>
#include "commondefine.h"
#include "configmanager.h"

ScreenShotWindow::ScreenShotWindow(const QString& name, const QSize& source, QWidget *parent) :
QDialog(parent),
    ui(new Ui::ScreenShotWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	bgcolor_ = Qt::lightGray;  // 背景色

	ui->wspinBox->setValue(source.width());
	ui->hspinBox->setValue(source.height());

	//   默认路径
	ui->pathlineEdit->setText(gConfigManager->baseWorkspacePath() + UDQ_T("/ScreenShot"));

	QRegExp rx(REG_NAMEFULL);
	QValidator *validator = new QRegExpValidator(rx, this);
	ui->namelineEdit->setValidator(validator);
	ui->namelineEdit->setText(name);
}

ScreenShotWindow::~ScreenShotWindow()
{
    delete ui;
}

void ScreenShotWindow::on_cancelpushButton_clicked(){
	reject();
}

void ScreenShotWindow::on_okpushButton_clicked(){

	if (ui->namelineEdit->text().isEmpty()){
		QMessageBox::critical(this, windowTitle(), UDQ_TR("文件名不能为空!"));
		return;
	}

	size_.setWidth(ui->wspinBox->value());
	size_.setHeight(ui->hspinBox->value());

	if (ui->bgcheckBox->isChecked()){
		bgcolor_ = QColor(0, 0, 0, 0);
	}

	filepath_.append(ui->pathlineEdit->text());
	filepath_.append(UDQ_T("/"));
	filepath_.append(ui->namelineEdit->text());
	filepath_.append(UDQ_T(".png"));

	// 确保目录存在
	QDir dir;
	if (!dir.exists(ui->pathlineEdit->text())){
		dir.mkpath(ui->pathlineEdit->text());
	}


	accept();
}

void ScreenShotWindow::on_colorpushButton_clicked(){

	QColor color = QColorDialog::getColor(bgcolor_, this, UDQ_TR("选择背景颜色"), QColorDialog::ShowAlphaChannel);
	if (color.isValid()){
		bgcolor_ = color;
	}
}