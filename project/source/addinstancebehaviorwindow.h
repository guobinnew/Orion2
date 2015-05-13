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

#ifndef ADDINSTANCEBEHAVIORWINDOW_H
#define ADDINSTANCEBEHAVIORWINDOW_H

#include <QDialog>
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class AddInstanceBehaviorWindow;
}

class AddInstanceBehaviorWindow : public QDialog
{
    Q_OBJECT

public:
	explicit AddInstanceBehaviorWindow(const QStringList& names, QWidget *parent = 0);
    ~AddInstanceBehaviorWindow();
	bool canSave();
	void init();
	QString newbehavior_;
	ResourceHash hash_;
 private:
    Ui::AddInstanceBehaviorWindow *ui;
	QStringList names_;

	// ≤È’“¿‡ƒø
	QTreeWidgetItem* findCateory(const QString& cate, bool create = true);

private slots:
	void  on_cancelpushButton_clicked();
	void  on_okpushButton_clicked();
};

#endif // ADDINSTANCEBEHAVIORWINDOW_H
