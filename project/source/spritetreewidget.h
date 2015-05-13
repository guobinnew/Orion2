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


#ifndef ORION_SPRITE_TREEWIDGET_H
#define ORION_SPRITE_TREEWIDGET_H

#include <QTreeWidget>
#include <QPushButton>
#include "resourceobject.h"
#include "oriontreewidget.h"
using namespace ORION;

class SpriteTreeWidget : public OrionTreeWidget{
	Q_OBJECT

public:
	SpriteTreeWidget(QWidget *parent = 0);
	~SpriteTreeWidget();

	QIcon defaultSpriteIcon_;      // Icon

	QTreeWidgetItem* addSpriteItem(SpriteResourceObject* sprite);
	
protected:
	virtual void contextMenuEvent(QContextMenuEvent * event);
};

#endif // PICTUREWIDGET_H
