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
#ifndef ANIMATIONCONTROLWINDOW_H
#define ANIMATIONCONTROLWINDOW_H

#include <QWidget>
#include "spritesequence.h"

namespace Ui {
class AnimationControlWindow;
}

class AnimationControlWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AnimationControlWindow(QWidget *parent = 0);
    ~AnimationControlWindow();

	void setHost(SpriteSequence* host);

private:
    Ui::AnimationControlWindow *ui;
	SpriteSequence* host_;  //
private slots:
	void on_playpushButton_clicked();
	void on_pausepushButton_clicked();
	void on_stoppushButton_clicked();
	void on_addpushButton_clicked();
	void on_minuspushButton_clicked();
	void on_backpushButton_clicked();
	void on_speeddoubleSpinBox_valueChanged(double d);

	void on_pingpongpushButton_clicked();
	void on_looppushButton_clicked();
	void on_repeatspinBox_valueChanged(int d);
	void on_endspinBox_valueChanged(int d);
};

#endif // ANIMATIONCONTROLWINDOW_H
