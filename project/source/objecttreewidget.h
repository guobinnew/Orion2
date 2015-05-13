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


#ifndef ORION_OBJECT_TREEWIDGET_H
#define ORION_OBJECT_TREEWIDGET_H

#include <QTreeWidget>
#include <QPushButton>
#include "resourceobject.h"
#include "oriontreewidget.h"
using namespace ORION;

class ObjectTreeWidget : public OrionTreeWidget{
	Q_OBJECT

public:
	ObjectTreeWidget(QWidget *parent = 0);
	~ObjectTreeWidget();

	void init(ProjectResourceObject* proj);
	QTreeWidgetItem* addObjectTypeItem(ObjectTypeResourceObject* type);
	QTreeWidgetItem* addFamilyItem(FamilyResourceObject* family);
	QTreeWidgetItem* addSoundItem(SoundResourceObject* sound);
	QTreeWidgetItem* addMovieItem(MovieResourceObject* movie);

	QString getItemPath(QTreeWidgetItem* item);
	QTreeWidgetItem* findTreePath(const QString& path, QTreeWidgetItem* root );
	QTreeWidgetItem* addGroup( QTreeWidgetItem* parent = NULL );
	// 删除对象类型
	void deleteObjectType(ResourceHash key);
	void refreshFamilyMember(ResourceHash key);

	FamilyResourceObject* findFamily(QTreeWidgetItem* item);

	void clearAllItems();
	// 刷新Item
	void refreshItem(QTreeWidgetItem* item);

	// 删除Group
	void deleteGroupItem(QTreeWidgetItem* item);

	// 更新所有对象类型类目
	void updateObjectTypeCategory();

	QTreeWidgetItem* rootEntityItem_;
	QTreeWidgetItem* rootFamilyItem_;
	QTreeWidgetItem* rootSoundItem_;
	QTreeWidgetItem* rootMovieItem_;

	QImage mmimage_;
	QImage familyimage_;
	QImage soundimage_;
	QImage movieimage_;

	ProjectResourceObject* project_;

	void setAssociateItem(bool flag){
		associateItem_ = flag;
	}
private:
	bool associateItem_;  // 是否关联Item

protected:
	virtual void	dropEvent ( QDropEvent * event );
	virtual QMimeData *	mimeData ( const QList<QTreeWidgetItem *> items ) const;
	virtual QStringList	mimeTypes () const;
	virtual void contextMenuEvent(QContextMenuEvent * event);
	virtual void startDrag(Qt::DropActions supportedActions);
	virtual void dragEnterEvent(QDragEnterEvent * event);
};

#endif // PICTUREWIDGET_H
