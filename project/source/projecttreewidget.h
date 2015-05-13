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


#ifndef ORION_PROJECT_TREEWIDGET_H
#define ORION_PROJECT_TREEWIDGET_H

#include <QTreeWidget>
#include <QPushButton>
#include <QSignalMapper>
#include "resourceobject.h"
#include "oriontreewidget.h"
using namespace ORION;

class ProjectTreeWidget : public OrionTreeWidget{
	Q_OBJECT

public:
	ProjectTreeWidget(QWidget *parent = 0);
	~ProjectTreeWidget();

	QTreeWidgetItem* getProjectItem();

	QTreeWidgetItem* addProjectItem( ProjectResourceObject* proj );
	QTreeWidgetItem* addLevelItem(LevelResourceObject* level, QTreeWidgetItem* parent =NULL);

	QTreeWidgetItem* addNewLayerItem(QTreeWidgetItem* parent);
	QTreeWidgetItem* addLayerItem(LayerResourceObject* layer, QTreeWidgetItem* parent );
	void updateItemIcon(QTreeWidgetItem* item);

	// 更新场景
	void updateLevelItem(QTreeWidgetItem* levelItem);
	void updateLayerItem(LayerResourceObject* layer, QTreeWidgetItem* layerItem);

	LevelResourceObject* getLevel(QTreeWidgetItem* item);

	QIcon defaultLevelIcon_;      // Icon
	QIcon defaultLayerIcon_;      // Icon
signals:
	void layerMoved(unsigned int key, int order);

protected:
	virtual void	dropEvent ( QDropEvent * event );
	virtual QMimeData *	mimeData ( const QList<QTreeWidgetItem *> items ) const;
	virtual QStringList	mimeTypes () const;
	virtual void contextMenuEvent(QContextMenuEvent * event);


private:
	bool isUpdating_;//
	QSignalMapper *signalMapper_;

private slots:
    void addEffect();
};

#endif // PICTUREWIDGET_H
