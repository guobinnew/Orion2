/************************************************************************
**
**  Copyright (C) 2014  guobin  <guobin.gb@alibaba-inc.com>
**
**  This file is part of Orion.
**
**  Orion is free software: you can redistribute it and/or modify
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
#include "addpicturewindow.h"
#include "ui_addpicturewindow.h"
#include <QMessageBox>
#include "tooldefine.h"


AddPictureWindow::AddPictureWindow(const QString& filePath, const QStringList& categories, QWidget *parent) :
QDialog(parent),
    ui(new Ui::AddPictureWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	// 加载包列表
	for (TResourceObjectIterator<PackageResourceObject> it; it; ++it){
		PackageResourceObject* pkg = (PackageResourceObject*)(*it);
		ui->pkgcomboBox->addItem(pkg->objectName(), pkg->hashKey());
	}

	//  类目
	QStringList cates;
	cates.append(UDQ_TR("未分类"));
	cates.append(categories);
	ui->catecomboBox->addItems(cates);

	// 加载文件
	ui->prevwidget->showPicture(filePath);

	QFileInfo fileInfo(filePath);
	ui->namelineEdit->setText(fileInfo.baseName());
	format_ = fileInfo.suffix();

	// 图像信息
	ui->infolabel->setText(ui->prevwidget->acquireInfo());
}

AddPictureWindow::~AddPictureWindow()
{
    delete ui;
}

void  AddPictureWindow::on_cancelpushButton_clicked(){
	reject();
}

bool AddPictureWindow::canSave(void){
	if (ui->pkgcomboBox->currentIndex() < 0){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请指定一个资源包!"));
		return false;
	}

	// 检查是否合法
	LINEEDIT_VALID(ui->namelineEdit, windowTitle(), UDQ_TR("资源ID"), REG_GALLERYID);

	// 检查是否存在
	ImageResourceObject* image = TFindResource<ImageResourceObject>(
		ui->pkgcomboBox->currentText(),
		ui->namelineEdit->text(), false);

	if (image != NULL){
		// 图像已经存在，提示是否覆盖
		if (QMessageBox::question(this, windowTitle(), UDQ_TR("图像已经存在，是否覆盖?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No){
			return false;
		}
	}

	return true;
}

void  AddPictureWindow::on_savepushButton_clicked(){
	if (!canSave())
		return;

	// 保存到数据库中
	newImage_ = TFindResource<ImageResourceObject>(
		ui->pkgcomboBox->currentText(),
		ui->namelineEdit->text(), true);
	Q_ASSERT(newImage_ != 0);

	if (ui->catecomboBox->currentIndex() > 0){
		newImage_->categories_.append(ui->catecomboBox->currentText());
	}

	newImage_->tags_ = ui->taglineEdit->text().split(UDQ_T(";"));
	newImage_->setContent( ui->prevwidget->getPictureData());
	newImage_->format_ = format_;
	newImage_->isGroup_ = false;
	newImage_->frames_.clear();
	newImage_->frames_.append(QRect(0, 0, newImage_->image_.width(), newImage_->image_.height()));

	// 生成缩略图
	newImage_->image_ = ui->prevwidget->getPicture();
	newImage_->makeThumbnail();

	newImage_->clearFlags(URF_TagGarbage);
	newImage_->setFlags(URF_TagSave);

	// 保存到本地
	SAVE_PACKAGE_RESOURCE(ImageResourceObject, newImage_);

	accept();

}
