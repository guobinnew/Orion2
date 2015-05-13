/************************************************************************
**
**  Copyright(C) 2015  guobin  <guobin.gb@alibaba - inc.com>
**
**  This file is part of Orion2.
**
**  Orion2 is free software : you can redistribute it and / or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation.
**
**  Orion2 is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion.If not, see < http://www.gnu.org/licenses/>.
**
*************************************************************************/

#include "gallerytreewidget.h"
#include <QtGui>
#include <QMenu>
#include "commondefine.h"
#include "editordefine.h"
#include "tooldefine.h"
#include "menumanager.h"
using namespace ORION;

GalleryTreeWidget::GalleryTreeWidget(QWidget *parent) : OrionTreeWidget(parent){
	setIconSize(QSize(24, 24));
	setHeaderHidden(true);
}

GalleryTreeWidget::~GalleryTreeWidget(){

}

QString GalleryTreeWidget::getCurrentItemPath(){
	return getItemPath(currentItem());
}

QString GalleryTreeWidget::getItemPath(QTreeWidgetItem* item){
	if (item == NULL || item->type() == ITEM_UNCLASS )
		return QString();

	QString path;
	QTreeWidgetItem* current = item;
	while (item != NULL && item->type() == ITEM_CATEGORY ){
		path.push_front(item->text(0));

		item = item->parent();
		if (item && item->type() == ITEM_CATEGORY){
			path.push_front(UDQ_T("."));
		}
	}

	return path;

}

void  GalleryTreeWidget::contextMenuEvent(QContextMenuEvent * event){

	QTreeWidgetItem* selectedItem = currentItem();
	if (selectedItem == NULL){
		event->ignore();
		return;
	}

	// 弹出右键菜单
	QMenu menu(this);
	QAction* act;

	if (selectedItem->type() == ITEM_CATEGORY ){
		// 添加
		act = gMenuManager->getAction(ORION_GALLERY_ADDCATEGORY);
		menu.addAction(act);

		// 删除
		act = gMenuManager->getAction(ORION_GALLERY_DELETECATEGORY);
		menu.addAction(act);
	}
	else if ( selectedItem->type() == ITEM_UNCLASS){
		// 未分类. 只能添加根分类
		act = gMenuManager->getAction(ORION_GALLERY_ADDCATEGORY);
		menu.addAction(act);

	}
	else{
		Q_UNREACHABLE();
	}

	QRect rect = visualItemRect(selectedItem);
	menu.exec(mapToGlobal(QPoint(rect.left(), rect.bottom())));
	event->accept();
}

QString GalleryTreeWidget::makeUniqueChildName(QTreeWidgetItem* item, const QString& prefix ){

	QString childName = prefix;
	if (childName.isEmpty()){
		childName = UDQ_T("Category");
	}

	int index = 1;
	while (findChild(item, childName) != NULL){
		childName = QString(UDQ_T("%1%2")).arg(prefix).arg(index++);
	}

	return childName;
}

QTreeWidgetItem* GalleryTreeWidget::findChild(QTreeWidgetItem* item, const QString& name){

	if (item == NULL){
		for (int i = 0; i < topLevelItemCount(); i++){
			if (topLevelItem (i)->text(0) == name){
				return topLevelItem(i);
			}
		}
	}
	else{
		for (int i = 0; i < item->childCount(); i++){
			if (item->child(i)->text(0) == name){
				return item->child(i);
			}
		}
	}

	return NULL;
}

void GalleryTreeWidget::dropEvent(QDropEvent * event){
	
	if (event->mimeData()->hasFormat(MIME_ORION_GALLERY)) {
		// 查找当前父对象
		QPoint pos = event->pos();

		// 判断当前树对象
		QTreeWidgetItem* selectItem = currentItem();
		QTreeWidgetItem* destItem = itemAt(pos);

		if (destItem == NULL || destItem == selectItem ){
			event->ignore();
			return;
		}

		QByteArray itemData = event->mimeData()->data(MIME_ORION_GALLERY);
		QDataStream stream(&itemData, QIODevice::ReadOnly);

		int num;
		stream >> num;

		ResourceHash entityH;
		for (int i = 0; i < num; i++){
			stream >> entityH;
			DEFINE_RESOURCE_HASHKEY_VALID(GalleryResourceObject, gallery, entityH);

			if (destItem->type() == ITEM_CATEGORY){
				//  移除空项
				gallery->categories_.removeAll(QString());
				gallery->categories_.append(getItemPath(destItem));
				gallery->categories_.removeDuplicates();
			}
			else if (destItem->type() == ITEM_UNCLASS){
				gallery->categories_.clear();
			}

			gallery->setFlags(URF_TagSave);
			if (UIsExactKindOf(ImageResourceObject, gallery)){
				SAVE_PACKAGE_RESOURCE(ImageResourceObject, gallery);
			}
			else if (UIsExactKindOf(AudioResourceObject, gallery)){
				SAVE_PACKAGE_RESOURCE(AudioResourceObject, gallery);
			}
			else if (UIsExactKindOf(VideoResourceObject, gallery)){
				SAVE_PACKAGE_RESOURCE(VideoResourceObject, gallery);
			}
		}
	
		emit galleryCategoryChanged();
		event->accept();

		return;
	}
	else{
		Q_UNREACHABLE();
	}

	event->ignore();

}


QStringList GalleryTreeWidget::mimeTypes() const{
	return QStringList() << QString(MIME_ORION_GALLERY);
}

// 发现兄弟节点是否重名（1表示自己，>=2表示有重名）
int GalleryTreeWidget::findDuplications(QTreeWidgetItem* item){
	int num = 0; 

	if (item == NULL)
		return num;

	QString name = item->text(0);
	if ( item->parent() == NULL ){
		for (int i = 0; i < topLevelItemCount(); i++){
			if (topLevelItem(i)->text(0) == name){
				num++;
			}
		}
	}
	else{
		for (int i = 0; i < item->parent()->childCount(); i++){
			if (item->parent()->child(i)->text(0) == name){
				num++;
			}
		}
	}

	return num;
}