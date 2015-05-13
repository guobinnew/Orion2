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

#ifndef WELCOMEWINDOW_H
#define WELCOMEWINDOW_H

#include <QDialog>
#include <QIcon>
#include <QListWidgetItem>

namespace Ui {
class WelcomeWindow;
}

class WelcomeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit WelcomeWindow(QWidget *parent = 0);
    ~WelcomeWindow();

	// 准备新项目列表
	void prepareNewProjectList();
	int prepareExistProjectList();
	// 检查新建项目合法性
	bool checkValid();

	bool openProjectFromDir(const QString& path, const QString& name = QString());
signals:
	void openProject();

private:
    Ui::WelcomeWindow *ui;

	QIcon defaultIcon_;   // 缺省项目图标

private slots:
	void  on_browserpushButton_clicked();
	void  on_refreshpushButton_clicked();

	void  on_newpushButton_clicked();
	void  on_projectpushButton_clicked();

	void  on_createpushButton_clicked();

	void on_namelineEdit_textChanged(const QString & text);
	void on_projectlistWidget_itemDoubleClicked(QListWidgetItem * item);

};

#endif // WELCOMEWINDOW_H
