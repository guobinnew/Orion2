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


#ifndef ORION_TILELISTWIDGET_H
#define ORION_TILELISTWIDGET_H

#include <QListWidget>
#include "resourceobject.h"
using namespace ORION;

class TileListWidget : public QListWidget{
	Q_OBJECT

public:
	TileListWidget(QWidget *parent = 0);
	~TileListWidget();

	void clearTiles();
	void addTile(QImage& img);

	void setHostId(ResourceHash id){
		hostId_ = id;
	}

protected:
	virtual QMimeData *	mimeData(const QList<QListWidgetItem *> items) const;
	virtual void contextMenuEvent(QContextMenuEvent * event);

private:
	ResourceHash hostId_;  // 素材ID
	QList<QImage>   frames_;  // 动画帧图像列表（与列表索引相同）
};

#endif // ORION_TILELISTWIDGET_H
