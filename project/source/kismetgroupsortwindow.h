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

#ifndef KISMETGROUPSORTWINDOW_H
#define KISMETGROUPSORTWINDOW_H

#include <QDialog>
#include "kismetscene.h"

namespace Ui {
class KismetGroupSortWindow;
}

class KismetGroupSortWindow : public QDialog
{
    Q_OBJECT

public:
    explicit KismetGroupSortWindow(KismetScene* scene, QWidget *parent = 0);
    ~KismetGroupSortWindow();

private:
    Ui::KismetGroupSortWindow *ui;
	KismetScene* scene_;

private slots:
	void on_okpushButton_clicked();
	void on_cancelpushButton_clicked();
};

#endif // KISMETGROUPSORTWINDOW_H
