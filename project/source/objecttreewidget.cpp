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


#include "objecttreewidget.h"
#include <QtGui>
#include <QMenu>
#include <QHeaderView>
#include <QPixmap>
#include <QCheckBox>
#include "commondefine.h"
#include "leveldefine.h"
#include "tooldefine.h"
#include "menumanager.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

ObjectTreeWidget::ObjectTreeWidget(QWidget *parent) : OrionTreeWidget(parent), project_(NULL), associateItem_(true),
rootEntityItem_(NULL),rootFamilyItem_(NULL),rootSoundItem_(NULL),rootMovieItem_(NULL){
	setDragEnabled(true);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setIconSize(QSize(24, 24));
	setDragDropMode(QAbstractItemView::DragDrop );

	mmimage_ = QImage(UDQ_T(":/images/objecttype.png")).scaledToWidth(64);
	familyimage_ = QImage(UDQ_T(":/images/family.png")).scaledToWidth(64);
	soundimage_ = QImage(UDQ_T(":/images/gallery_audio.png")).scaledToWidth(64);
	movieimage_ = QImage(UDQ_T(":/images/default_project.png")).scaledToWidth(64);


}


ObjectTreeWidget::~ObjectTreeWidget(){

}

void ObjectTreeWidget::clearAllItems(){
	clear();

	rootEntityItem_ = NULL;
	rootFamilyItem_ = NULL;
	rootMovieItem_ = NULL;
	rootSoundItem_ = NULL;
}

void ObjectTreeWidget::init(ProjectResourceObject* proj){
	project_ = proj;

	clearAllItems();

	QTreeWidgetItem* rootItem = NULL;
	if (topLevelItemCount() == 0){
		rootItem = new QTreeWidgetItem(ITEM_PROJECT);
		rootItem->setText(0, proj->objectName());
		rootItem->setData(0, ITEM_HASHKEY, proj->hashKey());
		addTopLevelItem(rootItem);
	}
	else{
		rootItem = topLevelItem(0);
	}

	if (rootEntityItem_ == NULL){
		// 添加实体组
		rootEntityItem_ = new QTreeWidgetItem(rootItem, ITEM_ENTITYGROUP);
		rootEntityItem_->setText(0, UDQ_TR("实体类型"));
	}

	if (rootFamilyItem_ == NULL ){
		// 添加集合组
		rootFamilyItem_ = new QTreeWidgetItem(rootItem, ITEM_FAMILYGROUP);
		rootFamilyItem_->setText(0, UDQ_TR("集合"));
	}

	if (rootSoundItem_ == NULL){
		// 添加集合组
		rootSoundItem_ = new QTreeWidgetItem(rootItem, ITEM_SOUNDGROUP);
		rootSoundItem_->setText(0, UDQ_TR("声音"));
	}

	if (rootMovieItem_ == NULL){
		// 添加集合组
		rootMovieItem_ = new QTreeWidgetItem(rootItem, ITEM_MOVIEGROUP);
		rootMovieItem_->setText(0, UDQ_TR("电影"));
	}

	if (proj){
		// 添加实体类型
		foreach(ObjectTypeResourceObject* obj, proj->objTypes_){
			if (obj->flags() & URF_TagGarbage)
				continue;

			addObjectTypeItem(obj);
		}

		// 添加集合
		foreach(FamilyResourceObject* family, proj->families_){
			addFamilyItem(family);
		}

		// 添加Sound
		foreach(SoundResourceObject* sound, proj->sounds_){
			addSoundItem(sound);
		}

		// 添加Movie
		foreach(MovieResourceObject* movie, proj->movies_){
			addMovieItem(movie);
		}
	}
	expandAll();
}

void ObjectTreeWidget::dragEnterEvent(QDragEnterEvent * event){
	if (!event->mimeData()->hasFormat(MIME_ORION_OBJECTTYPE)) {
		event->ignore();
		return;
	}

	QTreeWidget::dragEnterEvent(event);
}

void ObjectTreeWidget::dropEvent(QDropEvent * event){

	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	Q_ASSERT(ws != NULL);

	// 获取当前电子书
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != 0 && !(proj->flags()  & URF_TagGarbage));

	// 查找当前父对象
	QPoint pos = event->pos();

	// 判断当前树对象
	QTreeWidgetItem* destItem = itemAt(pos);
	if ( (destItem == NULL) ){
		event->ignore();
		return;
	}


	if (event->mimeData()->hasFormat(MIME_ORION_OBJECTTYPE)) {

		QByteArray itemData = event->mimeData()->data(MIME_ORION_OBJECTTYPE);
		ResourceHash key;
		{
			QDataStream stream(&itemData, QIODevice::ReadOnly);
			stream >> key;
		}
		DEFINE_RESOURCE_HASHKEY_VALID(ObjectTypeResourceObject, type, key);

		if (destItem->type() == ITEM_FAMILY){
			FamilyResourceObject* family = findFamily(destItem);
			if (!type->prototype_->hasAttribute(EF_SINGLEGLOBAL) && !family->members_.contains(type)){  // 唯一实例对象不能加入集合
				// 必须类型相同采用加入
				if (family->members_.size() > 0){
					ObjectTypeResourceObject* oldType = family->members_.at(0);
					if (oldType->prototype_ != type->prototype_){
						QMessageBox::warning(this, UDQ_TR("集合操作"), UDQ_TR("集合中的对象必须属于相同类型插件"));
						event->ignore();
						return;
					}
				}

				family->members_.append(type);
				QTreeWidgetItem* child = new QTreeWidgetItem(destItem, ITEM_FAMILYMEMBER);
				child->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);
				child->setData(0, ITEM_HASHKEY, type->hashKey());
				expandItem(destItem);
				refreshItem(child);
			}
		}
		else if (destItem->type() == ITEM_OBJECTTYPE || destItem->type() == ITEM_CATEGORY || destItem->type() == ITEM_ENTITYGROUP){
			Q_ASSERT(type->treeItem_ != NULL);
			QTreeWidgetItem* parent = type->treeItem_->parent();

			if (destItem->type() == ITEM_OBJECTTYPE )
				destItem = destItem->parent();

			if (parent != destItem){
				parent->takeChild(parent->indexOfChild(type->treeItem_));
				destItem->addChild(type->treeItem_);
				expandItem(destItem);
			}
		}
		else{
			Q_UNREACHABLE();
		}
	
		event->accept();
		return;

	}

	event->ignore();

}

QMimeData * ObjectTreeWidget::mimeData(const QList<QTreeWidgetItem *> items) const{

	Q_ASSERT( items.size() == 1 );
	Q_ASSERT( items.at(0) == currentItem() );


	QTreeWidgetItem* item = items.at(0);
	if (item->parent() == NULL || ( item->type() != ITEM_OBJECTTYPE && item->type() != ITEM_FAMILYMEMBER && item->type() != ITEM_FAMILY ) )  // 根实体不能拖放, 只有对象类型可以拖放
		return NULL;

	//  只能单选
	ResourceHash hash = item->data(0, ITEM_HASHKEY).toUInt();
	QMimeData* mmdata = new QMimeData;

	QByteArray itemData;
	{
		QDataStream dataStream(&itemData, QIODevice::WriteOnly);
		dataStream << hash;
	}

	if (item->type() == ITEM_FAMILY){
		mmdata->setData(MIME_ORION_FAMILY, itemData);
	}
	else{
		mmdata->setData(MIME_ORION_OBJECTTYPE, itemData);
	}

	return mmdata;
}

void ObjectTreeWidget::deleteGroupItem(QTreeWidgetItem* item){
	if (item == NULL || item->type() != ITEM_CATEGORY){
		return;
	}

	QTreeWidgetItem* parent = item->parent();
	Q_ASSERT(parent != NULL);

	int num = item->childCount();
	QTreeWidgetItem* child = NULL;
	for (int i = 0; i < num; i++){
		child = item->takeChild(i);
		parent->addChild(child);
	}

	delete item;
}

QStringList ObjectTreeWidget::mimeTypes() const{
	return QStringList() << QString(MIME_ORION_OBJECTTYPE) << QString(MIME_ORION_FAMILY);
}

void  ObjectTreeWidget::contextMenuEvent(QContextMenuEvent * event){

	QTreeWidgetItem* selectedItem = currentItem();
	if (selectedItem == NULL){
		event->ignore();
		return;
	}

	// 弹出右键菜单
	QMenu menu(this);
	QAction* act;

	if (selectedItem->type() == ITEM_OBJECTTYPE ||
		selectedItem->type() == ITEM_FAMILY ||
		selectedItem->type() == ITEM_FAMILYMEMBER ){
		act = gMenuManager->getAction(ORION_OBJECTTYPE_DELETE);
		menu.addAction(act);

		menu.addSeparator();
		act = gMenuManager->getAction(ORION_EDIT_OBJECTTYPE);
		menu.addAction(act);
	}

	if (selectedItem->type() == ITEM_CATEGORY || selectedItem->type() == ITEM_ENTITYGROUP ){
		act = gMenuManager->getAction(ORION_OBJECTTYPE_ADDCATEGORY);
		menu.addAction(act);

		if (selectedItem->type() == ITEM_CATEGORY){
			act = gMenuManager->getAction(ORION_OBJECTTYPE_DELETE);
			menu.addAction(act);
		}

	}

	if (selectedItem->type() == ITEM_SOUND ){
		act = gMenuManager->getAction(ORION_EDIT_SOUND);
		menu.addAction(act);
	}

	if (selectedItem->type() == ITEM_MOVIE){
		act = gMenuManager->getAction(ORION_EDIT_MOVIE);
		menu.addAction(act);
	}

	QRect rect = visualItemRect(selectedItem);
	menu.exec(event->globalPos());
	event->accept();
}


QTreeWidgetItem* ObjectTreeWidget::addObjectTypeItem(ObjectTypeResourceObject* type){
	QTreeWidgetItem* parent = findTreePath(type->categroy_, rootEntityItem_);
	Q_ASSERT(parent != NULL);

	QTreeWidgetItem* item = new QTreeWidgetItem( ITEM_OBJECTTYPE );
	Qt::ItemFlags flag = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
	if (!type->prototype_->hasAttribute(EF_SINGLEGLOBAL)){  // 全局单实例对象不能拖放
		flag |= Qt::ItemIsDragEnabled;
	}
	item->setFlags(flag);
	item->setData(0, ITEM_HASHKEY, type->hashKey());
	refreshItem(item);
	parent->addChild(item);
	expandItem(parent);

	return item;
}

void  ObjectTreeWidget::refreshItem(QTreeWidgetItem* item){
	if (item == NULL)
		return;

	ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
	DEFINE_RESOURCE_HASHKEY(ResourceObject, obj, key);
	item->setText(0, obj->description());

	if (UIsExactKindOf(ObjectTypeResourceObject, obj)){
		ObjectTypeResourceObject* type = (ObjectTypeResourceObject*)obj;

		if (associateItem_){
			type->treeItem_ = item;
		}


		if (type->prototype_->hasAttribute(EF_ANIMATIONS) || type->prototype_->hasAttribute(EF_TEXTURE)){
			QPixmap img = type->thumbnailImage();
			if (img.isNull()){
				img = QPixmap::fromImage(type->prototype_->icon_);
			}
			item->setIcon(0, img);
		}
		else{
			item->setIcon(0, QPixmap::fromImage(type->prototype_->icon_));
		}
	}
	else if (UIsExactKindOf(FamilyResourceObject, obj)){
		FamilyResourceObject* family = (FamilyResourceObject*)obj;
		if (associateItem_){
			family->treeItem_ = item;
		}
		item->setIcon(0, QPixmap::fromImage(familyimage_));
	}
}

QTreeWidgetItem* ObjectTreeWidget::addFamilyItem(FamilyResourceObject* family){

	QTreeWidgetItem* item = new QTreeWidgetItem(ITEM_FAMILY);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled );
	item->setData(0, ITEM_HASHKEY, family->hashKey());
	refreshItem(item);
	rootFamilyItem_->addChild(item);

	//  添加成员
	foreach(ObjectTypeResourceObject* type, family->members_){
		QTreeWidgetItem* child = new QTreeWidgetItem(item, ITEM_FAMILYMEMBER);
		child->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);
		child->setData(0, ITEM_HASHKEY, type->hashKey());
		refreshItem(item);
	}

	expandItem(rootFamilyItem_);

	return item;
}


QTreeWidgetItem* ObjectTreeWidget::addSoundItem(SoundResourceObject* sound){

	QTreeWidgetItem* item = new QTreeWidgetItem(ITEM_SOUND);
	item->setText(0, sound->description());
	item->setIcon(0, QPixmap::fromImage(soundimage_));
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
	item->setData(0, ITEM_HASHKEY, sound->hashKey());
	rootSoundItem_->addChild(item);

	expandItem(rootSoundItem_);

	return item;
}

QTreeWidgetItem* ObjectTreeWidget::addMovieItem(MovieResourceObject* movie){
	QTreeWidgetItem* item = new QTreeWidgetItem(ITEM_MOVIE);
	item->setText(0, movie->description());
	item->setIcon(0, QPixmap::fromImage(movieimage_));
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
	item->setData(0, ITEM_HASHKEY, movie->hashKey());
	rootMovieItem_->addChild(item);

	expandItem(rootMovieItem_);

	return item;
}



void ObjectTreeWidget::startDrag(Qt::DropActions supportedActions){
	QTreeWidgetItem* item = currentItem();
	if (item) {
		QMimeData *data = mimeData(selectedItems());
		if (!data)
			return;
		QRect rect;
		rect.adjust(horizontalOffset(), verticalOffset(), 0, 0);
		QDrag *drag = new QDrag(this);
		drag->setPixmap(QPixmap::fromImage(mmimage_));
		drag->setMimeData(data);
		drag->setHotSpot(QPoint(drag->pixmap().width() / 2, drag->pixmap().height() / 2));
		drag->exec(Qt::MoveAction);
	}
}

FamilyResourceObject* ObjectTreeWidget::findFamily(QTreeWidgetItem* item){
	if (item == NULL)
		return NULL;

	ResourceHash key = 0;
	if (item->type() == ITEM_FAMILY){
		key = item->data(0, ITEM_HASHKEY).toUInt();
	}
	else if (item->type() == ITEM_FAMILYMEMBER){
		Q_ASSERT(item->parent() != NULL);
		key = item->parent()->data(0, ITEM_HASHKEY).toUInt();
	}
	DEFINE_RESOURCE_HASHKEY(FamilyResourceObject, family, key);
	return family;
}

void ObjectTreeWidget::deleteObjectType(ResourceHash key){

	DEFINE_RESOURCE_HASHKEY(ObjectTypeResourceObject, type, key);
	if (type == NULL)
		return;


	QTreeWidgetItem* item = NULL;
	for (int i = 0; i < rootFamilyItem_->childCount(); i++){
		item = rootFamilyItem_->child(i);
		FamilyResourceObject* family = findFamily( item);
		Q_ASSERT(family != NULL);
		family->members_.removeOne(type);
	}
	ResourceHash hash = 0;
	// 遍历所有子节点，删除对应节点
	foreach(QTreeWidgetItem* item, findItems(type->description(), Qt::MatchExactly | Qt::MatchRecursive )){
		if (item->type() == ITEM_OBJECTTYPE || item->type() == ITEM_FAMILYMEMBER){
			Q_ASSERT(item->parent() != NULL);
			hash = item->data(0, ITEM_HASHKEY).toUInt();
			if (hash == key){
				item->parent()->removeChild(item);
				delete item;
			}
		}
	}

}

void ObjectTreeWidget::refreshFamilyMember(ResourceHash key){
	DEFINE_RESOURCE_HASHKEY(ObjectTypeResourceObject, type, key);
	if (type == NULL)
		return;

	ResourceHash hash = 0;
	// 遍历所有子节点，删除对应节点
	foreach(QTreeWidgetItem* item, findItems(type->description(), Qt::MatchExactly | Qt::MatchRecursive)){
		if (item->type() == ITEM_FAMILYMEMBER){
			refreshItem(item);
		}
	}
}

QTreeWidgetItem* ObjectTreeWidget::addGroup(QTreeWidgetItem* parent ){
	if (parent == NULL){
		parent = rootEntityItem_;
	}

	Q_ASSERT(parent != NULL);
	// 生成唯一名
	QString name = UDQ_TR("新组");
	int tid = 1;
	for (int i = 0; i < parent->childCount(); i++){
		if (parent->child(i)->text(0) == name){
			i = 0;
			name = UDQ_TR("新组%1").arg(tid++);
		}
	}

	QTreeWidgetItem* child = new QTreeWidgetItem(ITEM_CATEGORY);
	child->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
	child->setText(0, name);
	child->setData(0, ITEM_VALUE, name);
	parent->addChild(child);

	return child;
}


QString ObjectTreeWidget::getItemPath(QTreeWidgetItem* item){
	if (item == NULL)
		return QString();

	QString path;
	QTreeWidgetItem* current = item;

	if (current->type() != ITEM_CATEGORY){
		current = current->parent();  // 取父节点
	}

	while (current != NULL && current->type() != ITEM_ENTITYGROUP){
		path.push_front(current->text(0));

		current = current->parent();
		if (current && current->type() != ITEM_ENTITYGROUP){
			path.push_front(UDQ_T("."));
		}
	}

	return path;

}

QTreeWidgetItem* ObjectTreeWidget::findTreePath(const QString& path, QTreeWidgetItem* root){
	QTreeWidgetItem* parent = NULL;
	QTreeWidgetItem* child = NULL;

	if (root == NULL){
		return NULL;
	}

	// CateGory路径格式为“A.B.C”
	if (path.isEmpty()){
		return root;
	}

	QStringList entries = path.split(UDQ_T("."));
	foreach(QString entry, entries){
		Q_ASSERT(entry.size() > 0);

		if (parent == NULL){
			for (int i = 0; i < root->childCount(); i++){
				QTreeWidgetItem* item = root->child(i);
				if (item->type() != ITEM_CATEGORY || item->text(0) != entry){
					continue;
				}
				parent = item;
				break;
			}

			if (parent == NULL){
				// 创建新项
				parent = new QTreeWidgetItem(ITEM_CATEGORY);
				parent->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
				parent->setText(0, entry);
				parent->setData(0, ITEM_VALUE, entry);
				root->addChild(parent);
			}
			continue;
		}

		for (int i = 0; i < parent->childCount(); i++){
			QTreeWidgetItem* item = parent->child(i);
			if (item->type() != ITEM_CATEGORY || item->text(0) != entry){
				continue;
			}
			child = item;
			break;
		}

		if (child == NULL){
			// 创建新项
			child = new QTreeWidgetItem(ITEM_CATEGORY);
			child->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
			child->setText(0, entry);
			child->setData(0, ITEM_VALUE, entry);
			parent->addChild(child);
		}
		parent = child;
		child = NULL;
	}

	return parent;

}


void ObjectTreeWidget::updateObjectTypeCategory(){
	if (project_ == NULL)
		return;

	// 添加实体类型
	foreach(ObjectTypeResourceObject* obj, project_->objTypes_){
		if (obj->flags() & URF_TagGarbage)
			continue;
		obj->categroy_ = getItemPath(obj->treeItem_);
	}

}