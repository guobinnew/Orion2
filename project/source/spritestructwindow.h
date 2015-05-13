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
#ifndef SPRITESTRUCTWINDOW_H
#define SPRITESTRUCTWINDOW_H

#include <QWidget>
#include <QTreeWidgetItem>

namespace Ui {
class SpriteStructWindow;
}

class SpriteStructWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SpriteStructWindow(QWidget *parent = 0);
    ~SpriteStructWindow();

protected:
	virtual void resizeEvent(QResizeEvent * event);

signals:
	void deleteSprite(unsigned int key);

private:
    Ui::SpriteStructWindow *ui;

	void initFromDb();

private slots:
	void on_addpushButton_clicked();
	void on_delpushButton_clicked();

	void on_savepushButton_clicked();
	void on_saveallpushButton_clicked();
	void on_refreshpushButton_clicked();
	void on_addseqpushButton_clicked();

	void on_treeWidget_itemChanged(QTreeWidgetItem * item, int column);
	void on_treeWidget_itemDoubleClicked(QTreeWidgetItem * item, int column);

};

#endif // SPRITESTRUCTWINDOW_H
