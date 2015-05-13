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
#ifndef SCREENSHOTWINDOW_H
#define SCREENSHOTWINDOW_H

#include <QDialog>

namespace Ui {
class ScreenShotWindow;
}

class ScreenShotWindow : public QDialog
{
    Q_OBJECT

public:
	explicit ScreenShotWindow(const QString& name, const QSize& source, QWidget *parent = 0);
    ~ScreenShotWindow();

	QSize imgSize(){
		return size_;
	}

	QColor bgColor(){
		return bgcolor_;
	}

	QString& filePath(){
		return filepath_;
	}

private:
	Ui::ScreenShotWindow *ui;
	QColor bgcolor_;  // 背景色
	QString filepath_;  // 文件路径
	QSize size_;  // 文件大小

private slots:
	void on_cancelpushButton_clicked();
	void on_okpushButton_clicked();
	void on_colorpushButton_clicked();
};

#endif // SCREENSHOTWINDOW_H
