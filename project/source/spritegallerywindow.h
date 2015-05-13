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
#ifndef SPRITEGALLERYWINDOW_H
#define SPRITEGALLERYWINDOW_H

#include <QWidget>
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class SpriteGalleryWindow;
}

class SpriteGalleryWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SpriteGalleryWindow(QWidget *parent = 0);
    ~SpriteGalleryWindow();

	// 初始化精灵素材列表
	void init(SpriteResourceObject* sprite);
	void clear();

protected:
	virtual void resizeEvent(QResizeEvent * event);

private:
    Ui::SpriteGalleryWindow *ui;
	SpriteResourceObject* sprite_; 
	bool isClearing_;
	
private slots:
	void on_addpushButton_clicked();
	void on_replacepushButton_clicked();
	void on_delpushButton_clicked();
	void on_listWidget_itemSelectionChanged();

	void addGalleryItem(unsigned int, unsigned int);


};

#endif // SPRITEGALLERYWINDOW_H
