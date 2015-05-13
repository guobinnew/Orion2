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

#ifndef SELECTSPRITEWINDOW_H
#define SELECTSPRITEWINDOW_H

#include <QDialog>
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class SelectSpriteWindow;
}

class SelectSpriteWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SelectSpriteWindow(ResourceHash key, QWidget *parent = 0);
    ~SelectSpriteWindow();

	ResourceHash spriteH_;
private:
    Ui::SelectSpriteWindow *ui;

private slots:
    void on_savepushButton_clicked();
    void on_cancelpushButton_clicked();

};

#endif // SELECTSPRITEWINDOW_H
