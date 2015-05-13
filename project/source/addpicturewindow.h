/************************************************************************
**
**  Copyright (C) 2014  guobin  <guobin.gb@alibaba-inc.com>
**
**  This file is part of Orion.
**
**  Orion is free software: you can redistribute it and/or modify
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
#ifndef ORION_ADDPICTUREWINDOW_H
#define ORION_ADDPICTUREWINDOW_H

#include <QDialog>
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;

namespace Ui {
class AddPictureWindow;
}

class AddPictureWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AddPictureWindow(const QString& filePath, const QStringList& categories, QWidget *parent = 0);
    ~AddPictureWindow();


	ImageResourceObject* newImage_;  //  新建的图像资源

private:
    Ui::AddPictureWindow *ui;
	bool canSave(void);
	QString format_;  // 文件格式

private slots:
	void  on_cancelpushButton_clicked();
	void  on_savepushButton_clicked();


};

#endif // ADDPICTUREWINDOW_H
