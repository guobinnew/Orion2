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
#include "animationcontrolwindow.h"
#include "ui_animationcontrolwindow.h"

AnimationControlWindow::AnimationControlWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnimationControlWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	setWindowOpacity(0.75);
}

AnimationControlWindow::~AnimationControlWindow()
{
    delete ui;
}

void AnimationControlWindow::setHost(SpriteSequence* host){
	if( host_ != host )
		host_ = host;

	if (host_){

		if (host_->isPlaying()){
			ui->playpushButton->setEnabled(false);
			ui->pausepushButton->setEnabled(true);
			ui->stoppushButton->setEnabled(true);
		}
		else if (host_->isPausing()){
			ui->playpushButton->setEnabled(true);
			ui->pausepushButton->setEnabled(false);
			ui->stoppushButton->setEnabled(true);
		}
		else{
			ui->playpushButton->setEnabled(true);
			ui->pausepushButton->setEnabled(false);
			ui->stoppushButton->setEnabled(false);
		}

		ui->backpushButton->setChecked(host->isPlayBack());
		ui->speeddoubleSpinBox->setValue(host->playSpeed());

		ui->pingpongpushButton->setChecked(host->isPingpong());
		ui->looppushButton->setChecked(host->isRepeat());
		ui->repeatspinBox->setValue(host->repeatCount());
		ui->endspinBox->setValue(host->repeatTo());

	}
	else{
		ui->playpushButton->setEnabled(false);
		ui->pausepushButton->setEnabled(false);
		ui->stoppushButton->setEnabled(false);
	}
}

void  AnimationControlWindow::on_playpushButton_clicked(){
	if (host_ == NULL)
		return;

	if (host_->frameCount() == 0)
		return;

	host_->playAnim(true);
	setHost(host_);
}
void  AnimationControlWindow::on_pausepushButton_clicked(){
	if (host_ == NULL)
		return;

	host_->playAnim(false);
	setHost(host_);
}
void  AnimationControlWindow::on_stoppushButton_clicked(){
	if (host_ == NULL)
		return;

	host_->resetAnim();
	setHost(host_);
}
void  AnimationControlWindow::on_addpushButton_clicked(){
	if (host_ == NULL)
		return;

	ui->speeddoubleSpinBox->setValue(ui->speeddoubleSpinBox->value() + ui->speeddoubleSpinBox->singleStep());
	host_->setPlaySpeed(ui->speeddoubleSpinBox->value());
}
void  AnimationControlWindow::on_minuspushButton_clicked(){
	if (host_ == NULL)
		return;
	ui->speeddoubleSpinBox->setValue(ui->speeddoubleSpinBox->value() - ui->speeddoubleSpinBox->singleStep());
	host_->setPlaySpeed(ui->speeddoubleSpinBox->value());
}

void  AnimationControlWindow::on_backpushButton_clicked(){
	if (host_ == NULL)
		return;
	host_->setPlayBack(ui->backpushButton->isChecked());
}

void  AnimationControlWindow::on_speeddoubleSpinBox_valueChanged(double d){
	if (host_ == NULL)
		return;
	host_->setPlaySpeed(d);
}

void AnimationControlWindow::on_pingpongpushButton_clicked(){
	if (host_ == NULL)
		return;
	host_->setPingPong(ui->pingpongpushButton->isChecked());
}

void AnimationControlWindow::on_looppushButton_clicked(){
	if (host_ == NULL)
		return;
	host_->setRepeat(ui->looppushButton->isChecked());
}

void AnimationControlWindow::on_repeatspinBox_valueChanged(int d){
	if (host_ == NULL)
		return;
	host_->setRepeatCount(d);
}

void AnimationControlWindow::on_endspinBox_valueChanged(int d){
	if (host_ == NULL)
		return;
	host_->setRepeatTo(d);
}