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
#include "layerdefinewindow.h"
#include "ui_layerdefinewindow.h"
#include "tooldefine.h"
#include <QColorDialog>

LayerDefineWindow::LayerDefineWindow(LayerResourceObject* layer, QWidget *parent) :
QDialog(parent), layer_(layer),
    ui(new Ui::LayerDefineWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	ui->namelineEdit->setText(layer_->description());

	fillColor_ = layer_->bgColor_;
	fillColor_.setAlpha(255);

	ui->colorwidget->setFillColor(fillColor_);
	ui->alphahorizontalSlider->setValue(layer_->bgColor_.alpha());

	ui->scalespinBox->setValue(layer_->scale_);
	ui->pxspinBox->setValue(layer_->parallax_.x());
	ui->pyspinBox->setValue(layer_->parallax_.y());

	ui->visiblecheckBox->setChecked(layer_->initVisible_);
	ui->transparentcheckBox->setChecked(layer_->transparent_);
	ui->texturecheckBox->setChecked(layer_->ownTexture_);

	ui->rendercheckBox->setChecked(layer_->useRenderCell_);

}

LayerDefineWindow::~LayerDefineWindow()
{
    delete ui;
}



bool LayerDefineWindow::canSave(){
	LINEEDIT_CHECK(ui->namelineEdit, UDQ_TR("Í¼²ãÊôÐÔÉèÖÃ"), UDQ_TR("Ãû³Æ"));
	return true;
}

void  LayerDefineWindow::on_savepushButton_clicked(){

	layer_->setDescription(ui->namelineEdit->text());
	fillColor_.setAlpha(ui->alphahorizontalSlider->value());
	layer_->bgColor_ = fillColor_;

	layer_->initVisible_ = ui->visiblecheckBox->isChecked();
	layer_->ownTexture_ = ui->texturecheckBox->isChecked();
	layer_->transparent_ = ui->transparentcheckBox->isChecked();
	layer_->useRenderCell_ = ui->rendercheckBox->isChecked();

	layer_->scale_ = ui->scalespinBox->value();
	layer_->parallax_.setX(ui->pxspinBox->value());
	layer_->parallax_.setY(ui->pyspinBox->value());

	layer_->setFlags(URF_TagSave);
	accept();
}

void  LayerDefineWindow::on_cancelpushButton_clicked(){
	reject();
}

void  LayerDefineWindow::on_clrpushButton_clicked(){
	fillColor_ = QColorDialog::getColor(fillColor_, this, UDQ_TR("Ñ¡Ôñ±³¾°ÑÕÉ«"));
	ui->colorwidget->setFillColor(fillColor_);
}

void LayerDefineWindow::on_alphahorizontalSlider_valueChanged(int value){
	ui->alphalabel->setText(QString::number(value));
}
