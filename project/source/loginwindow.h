/************************************************************************
**
**  Copyright (C) 2014  guobin  <guobin.gb@alibaba-inc.com>
**
**  This file is part of OSSSnake.
**
** OSSSnake is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation.
**
**  OSSSnake is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with OSSSnake.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#ifndef OSS_LOGINWINDOW_H
#define OSS_LOGINWINDOW_H

#include <QDialog>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();

protected:
	virtual void	closeEvent(QCloseEvent * event);

private:
    Ui::LoginWindow *ui;
	int retryNum_;  // 重试次数
	QByteArray code_;  // 输入的代码
signals:
	void loginEvent(bool success);
	void exitLogin();

private slots:
	void processAuthorization(const QByteArray& data);
};

#endif // LOGINWINDOW_H
