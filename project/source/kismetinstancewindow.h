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

#ifndef KISMETINSTANCEWINDOW_H
#define KISMETINSTANCEWINDOW_H

#include <QWidget>
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class KismetInstanceWindow;
}

class KismetInstanceWindow : public QWidget
{
    Q_OBJECT

public:
    explicit KismetInstanceWindow(QWidget *parent = 0);
    ~KismetInstanceWindow();

	void init(BlueprintResourceObject* script);

	// 添加变量
	void addVariable(ScriptVariable* var);
	void deleteVariable(ScriptVariable* var);
	void refreshVariable(ScriptVariable* var);


protected:
	virtual void resizeEvent(QResizeEvent * event);

private:
    Ui::KismetInstanceWindow *ui;
	BlueprintResourceObject* script_;  // 脚本

private slots:
	void on_addpushButton_clicked();
	void on_delpushButton_clicked();
	void on_treeWidget_itemChanged(QTreeWidgetItem * item, int column);

	void dataTypeChanged(unsigned int key, const QString& dt);
	void valueChanged(unsigned int key, const QString& v);
};

#endif // KISMETINSTANCEWINDOW_H
