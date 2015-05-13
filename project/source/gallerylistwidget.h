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

#ifndef ORION_GALLERYLISTWIDGET_H
#define ORION_GALLERYLISTWIDGET_H

#include <QListWidget>
#include "resourceobject.h"
using namespace ORION;

class GalleryListWidget : public QListWidget{
	Q_OBJECT

public:
	GalleryListWidget(QWidget *parent = 0);
	~GalleryListWidget();


	void setAutoResize(bool flag){
		autoResize_ = flag;
	}

private:
	bool autoResize_;  // 自动根据大小改变图片大小
signals:
	void removeFromCategory(QListWidgetItem* item);

protected:
	virtual QMimeData *	mimeData(const QList<QListWidgetItem *> items) const;
	virtual void contextMenuEvent(QContextMenuEvent * event);
	virtual void	keyPressEvent(QKeyEvent * event);
};

#endif // ORION_GALLERYTREEWIDGET_H
