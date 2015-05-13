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

#include "gallerydefinewindow.h"
#include "ui_gallerydefinewindow.h"
#include "addpicturewindow.h"
#include "addaudiowindow.h"
#include "addvideowindow.h"
#include "importgallerywindow.h"
#include "mergeimagewindow.h"
#include "resourcereferencecheckwindow.h"
#include "splitimagewindow.h"
#include <QFileDialog>
#include <QColorDialog>
#include "commondefine.h"
#include "tooldefine.h"
#include "menumanager.h"
#include "imagehelper.h"
#include "resourcertti.h"
using namespace ORION;



GalleryDefineWindow::GalleryDefineWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GalleryDefineWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	Qt::WindowFlags oldflag = windowFlags();
	oldflag &= !Qt::WindowContextHelpButtonHint;
	setWindowFlags(oldflag);

	initFromDb();

	filter_ = FILTER_ALL;
	
	connect(ui->treeWidget, SIGNAL(galleryCategoryChanged()), this, SLOT(updateCurrentCategory()));
	connect(ui->listWidget, SIGNAL(removeFromCategory(QListWidgetItem*)), this, SLOT(removeGalleryCategory(QListWidgetItem*)));

	BEGIN_ACTION();
	CONNECT_ACTION(ORION_GALLERY_ADDCATEGORY, this, addGalleryCategory);
	CONNECT_ACTION(ORION_GALLERY_DELETECATEGORY, this, deleteGalleryCategory);
	CONNECT_ACTION(ORION_GALLERY_MERGE, this, mergeImage);
	CONNECT_ACTION(ORION_GALLERY_TRANSPARENT, this, transparentImageBG);
	CONNECT_ACTION(ORION_GALLERY_COPY, this, copyGallery);
	CONNECT_ACTION(ORION_GALLERY_SPLIT, this, splitImage);
	END_ACTION();

	ui->treeWidget->setCurrentItem(unknowCategoryItem_);
	modified_ = false;
}

GalleryDefineWindow::~GalleryDefineWindow()
{

	BEGIN_ACTION();
	DISCONNECT_ACTION(ORION_GALLERY_ADDCATEGORY);
	DISCONNECT_ACTION(ORION_GALLERY_DELETECATEGORY);
	DISCONNECT_ACTION(ORION_GALLERY_MERGE);
	END_ACTION();

    delete ui;
}

void GalleryDefineWindow::initFromDb(void){

	unknowCategoryItem_ = new QTreeWidgetItem(ITEM_UNCLASS);
	unknowCategoryItem_->setText(0, UDQ_TR("未分类"));
	unknowCategoryItem_->setFlags(Qt::ItemIsSelectable  | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
	ui->treeWidget->addTopLevelItem(unknowCategoryItem_);

	QStringList tagList;

	// 加载列表
	for (TResourceObjectIterator<GalleryResourceObject> it; it; ++it){
		GalleryResourceObject* gallery = (GalleryResourceObject*)(*it);

		if (gallery->flags() & URF_TagGarbage)
			continue;

		// 构建所有类目
		foreach(QString cate, gallery->categories_){
			if (cate.isEmpty() || categoryPaths_.contains(cate))
				continue;

			categoryPaths_.append(cate);

			QTreeWidgetItem* parent = findTreePath(cate, ui->treeWidget);
			Q_ASSERT(parent != NULL);

		}
	
		tagList << gallery->tags_;
	}

	// 移除重复项
	tagList.removeDuplicates();

}

QTreeWidgetItem* GalleryDefineWindow::findTreePath(const QString& path, QTreeWidget* tree){
	QTreeWidgetItem* parent = NULL;
	QTreeWidgetItem* child = NULL;

	// CateGory路径格式为“A.B.C”
	// 依次建立路径中的Item项
	if (path.isEmpty()){
		return tree->topLevelItem(0);  // 默认未分类
	}

	QStringList entries = path.split(UDQ_T("."));
	foreach(QString entry, entries){
		Q_ASSERT(entry.size() > 0);

		if (parent == NULL){
			for (int i = 0; i < tree->topLevelItemCount(); i++){
				QTreeWidgetItem* item = tree->topLevelItem(i);
				if (item->type() != ITEM_CATEGORY || item->text(0) != entry ){
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
				tree->addTopLevelItem(parent);
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
			parent->addChild(child);
		}
		parent = child;
		child = NULL;
	}

	return parent;  

}


void GalleryDefineWindow::on_cancelpushButton_clicked(){

	if (modified_){
		if (QMessageBox::question(this, windowTitle(), UDQ_TR("是否保存对素材库的修改?")) == QMessageBox::Yes){
			on_savepushButton_clicked();
		}
	}

	reject();
}

void GalleryDefineWindow::on_savepushButton_clicked(){

	// 删除素材
	foreach(GalleryResourceObject* gallery, deletedGalleries_){
		gallery->setFlags(URF_TagGarbage);
	}

	// 保存所有库
	ApplyResourceLinkers<ImageResourceObject>();
	ApplyResourceLinkers<AudioResourceObject>();
	ApplyResourceLinkers<VideoResourceObject>();
	modified_ = false;
	// 
	QMessageBox::information(this, windowTitle(), UDQ_TR("素材数据保存完毕!"));
}

void GalleryDefineWindow::on_deletepushButton_clicked(){

	// 删除当前选择的资源
	QListWidgetItem* item = ui->listWidget->currentItem();

	if (item == NULL){
		QMessageBox::warning(this, UDQ_TR("删除资源"), UDQ_TR("请选择一个要删除的资源!"));
		return;
	}

	// 检查资源引用情况（如果有引用，则不能删除）
	// 不考虑工程引用( 目前只考虑动画、声音、电影应用)
	// 工程中已用已删除资源，则提供容错处理
	// 引用关联性检查
	ResourceHash key = item->data(ITEM_HASHKEY).toUInt();
	ResourceReferenceCheckWindow w(key);

	if (w.isUsed()){
		w.exec();  // 显示引用信息,  无法删除
	}
	else{
		if (QMessageBox::question(this, windowTitle(), UDQ_TR("确认删除选中的素材?")) == QMessageBox::No){
			return;
		}

		DEFINE_RESOURCE_HASHKEY_VALID(GalleryResourceObject, gallery, key);
		ui->listWidget->takeItem(ui->listWidget->row(item));
		delete item;

		modified_ = true;
		deletedGalleries_.append(gallery);
	}

}

void GalleryDefineWindow::on_addpushButton_clicked(){
	// 打开文件
	QString filepath = QFileDialog::getOpenFileName(this, UDQ_TR("从文件导入素材"), QString(),
		GalleryResourceObject::fileFilters() );

	if (filepath.isEmpty())
		return;

	// 检查资源类型
	QFileInfo fileInfo(filepath);
	QString extName = fileInfo.suffix();

	GalleryResourceObject* gallery = NULL;
	if (ImageResourceObject::isValidFile(extName)){
		// 添加图片素材
		AddPictureWindow w(filepath, categoryPaths_, this);
		if (w.exec() == QDialog::Accepted){
			gallery = w.newImage_;
		}
	}
	else if (AudioResourceObject::isValidFile(extName)){
		// 添加音频素材
		AddAudioWindow w(filepath, categoryPaths_, this);
		if (w.exec() == QDialog::Accepted){
			gallery = w.newAudio_;
		}
	}
	else if (VideoResourceObject::isValidFile(extName)){
		// 添加视频素材
		AddVideoWindow w(filepath, categoryPaths_, this);
		if (w.exec() == QDialog::Accepted){
			gallery = w.newVideo_;
		}
	}

	// 更新列表显示
	if (gallery != NULL){
		// 
		QString currentCategory = ui->treeWidget->getCurrentItemPath();
		if (gallery->isBelong(currentCategory)){
			// 添加新项
			insertGalleryItemToList(gallery);
		}
		modified_ = true;
	}

	return;
}

void GalleryDefineWindow::on_exportpushButton_clicked(){
	static QString title = UDQ_TR("导出素材");
	// 将资源导出到本地
	QList<QListWidgetItem*> selitems = ui->listWidget->selectedItems();
	if (selitems.size() == 0){
		QMessageBox::warning(this, title, UDQ_TR("请选择一个要导出的资源!"));
		return;
	}

	QString dirpath = QFileDialog::getExistingDirectory(this, title);
	if (dirpath.isEmpty()){
		return;
	}

	foreach(QListWidgetItem* item, selitems){

		ResourceHash key = item->data(ITEM_HASHKEY).toUInt();
		DEFINE_RESOURCE_HASHKEY_VALID(GalleryResourceObject, gallery, key);

		QString filepath = QString(UDQ_T("%1/%2.%3")).arg(dirpath).arg(gallery->objectName()).arg(gallery->getFormat());

		// 判断选择的资源类型
		if (UIsExactKindOf(ImageResourceObject, gallery)){

			ImageResourceObject* image = UDynamicCast(ImageResourceObject, gallery);
			if (image->format_ == UDQ_T("jpg")){
				image->image_.save(filepath, "JPG");
			}
			else if (image->format_ == UDQ_T("bmp")){
				image->image_.save(filepath, "BMP");
			}
			else if (image->format_ == UDQ_T("png")){
				image->image_.save(filepath, "PNG");
			}
		}
		else if (UIsExactKindOf(AudioResourceObject, gallery)){

			AudioResourceObject* audio = UDynamicCast(AudioResourceObject, gallery);
			QFile file(filepath);
			if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
				QMessageBox::warning(this, title, UDQ_TR("音频文件[%1]导出失败!").arg(filepath));
			}
			file.write(audio->content_);
			file.close();

		}
		else if (UIsExactKindOf(VideoResourceObject, gallery)){
			VideoResourceObject* video = UDynamicCast(VideoResourceObject, gallery);
			QFile file(filepath);
			if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
				QMessageBox::warning(this, title, UDQ_TR("视频文件[%1]导出失败!").arg(filepath));
			}
			file.write(video->content_);
			file.close();
		}
	}
	
	QMessageBox::information(this, title, UDQ_TR("素材导出完毕!"));
}


void GalleryDefineWindow::insertGalleryItemToList(GalleryResourceObject* gallery){
	QListWidgetItem* item = NULL;
	// 检查是否已经存在
	for (int i = 0; i < ui->listWidget->count(); i++){
		if (ui->listWidget->item(i)->text() == gallery->objectName()){
			item = ui->listWidget->item(i);
			break;
		}
	}

	if (item == NULL){
		addGalleryItemToList(gallery);
	}
	else{
		// 更新Item
		updateGalleryItem(item, gallery);
	}

}

// 向列表中添加
void GalleryDefineWindow::addGalleryItemToList(GalleryResourceObject* gallery){
	QListWidgetItem* item = new QListWidgetItem(ui->listWidget, ITEM_GALLERY);
	updateGalleryItem(item, gallery);
}

void GalleryDefineWindow::updateGalleryItem(QListWidgetItem* item, GalleryResourceObject* gallery){
	if (UIsExactKindOf(ImageResourceObject, gallery)){
		item->setIcon(gallery->thumbnail_);
	}
	else if (UIsExactKindOf(AudioResourceObject, gallery)){
		item->setIcon(QIcon(UDQ_T(":/images/gallery_audio.png")));
	}
	else if (UIsExactKindOf(VideoResourceObject, gallery)){
		item->setIcon(QIcon(UDQ_T(":/images/gallery_video.png")));
	}
	item->setText(gallery->objectName());
	item->setData(ITEM_HASHKEY, gallery->hashKey());
}


void GalleryDefineWindow::on_treeWidget_itemSelectionChanged(){

	QTreeWidgetItem* item = ui->treeWidget->currentItem();
	if (item == NULL){
		ui->listWidget->clear();
		return;
	}

	updateCurrentCategory();

}

void GalleryDefineWindow::updateCategory(const QString& cate, const QString& tag ){
	ui->listWidget->clear();

	// 加载列表
	for (TResourceObjectIterator<GalleryResourceObject> it; it; ++it){
		GalleryResourceObject* gallery = (GalleryResourceObject*)(*it);

		if (gallery->flags() & URF_TagGarbage)
			continue;

		if (gallery->isBelong(cate)){

			if (filter_ != FILTER_ALL){
				//匹配类型
				if (!(filter_ & FILTER_IMAGE) && UIsExactKindOf(ImageResourceObject, gallery)){
					continue;
				}

				if (!(filter_ & FILTER_AUDIO) && UIsExactKindOf(AudioResourceObject, gallery)){
					continue;
				}

				if (!(filter_ & FILTER_VIDEO) && UIsExactKindOf(VideoResourceObject, gallery)){
					continue;
				}
			}

			// 严格匹配标签 或名字
			if (!tag.isEmpty() && !gallery->tags_.contains(tag) && !gallery->objectName().contains(tag, Qt::CaseInsensitive) ){
				continue;
			}

			addGalleryItemToList(gallery);
		}

	}
}


void GalleryDefineWindow::on_imagecheckBox_stateChanged(int state){
	if (state == Qt::Checked){
		// 重新更新
		filter_ |= FILTER_IMAGE;
	}
	else{
		filter_ &= ~FILTER_IMAGE;
	}

	updateCurrentCategory();
}

void GalleryDefineWindow::on_audiocheckBox_stateChanged(int state){
	if (state == Qt::Checked){
		// 重新更新
		filter_ |= FILTER_AUDIO;
	}
	else{
		filter_ &= ~FILTER_AUDIO;
	}

	updateCurrentCategory();
}

void GalleryDefineWindow::on_videocheckBox_stateChanged(int state){
	if (state == Qt::Checked){
		// 重新更新
		filter_ |= FILTER_VIDEO;
	}
	else{
		filter_ &= ~FILTER_VIDEO;
	}

	updateCurrentCategory();
}


void GalleryDefineWindow::on_taglineEdit_textEdited(const QString & text){
	updateCurrentCategory();
}


void GalleryDefineWindow::addGalleryCategory(){
	//
	QTreeWidgetItem* item = ui->treeWidget->currentItem();

	QTreeWidgetItem* newItem = new QTreeWidgetItem(ITEM_CATEGORY);
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable  | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);

	if (item == NULL || item->type() == ITEM_UNCLASS){
		newItem->setText(0, ui->treeWidget->makeUniqueChildName(NULL, UDQ_TR("新类目")));
		ui->treeWidget->addTopLevelItem(newItem);
	}
	else if (item->type() == ITEM_CATEGORY){
		newItem->setText(0, ui->treeWidget->makeUniqueChildName(item, UDQ_TR("新类目")));
		item->addChild(newItem);
		ui->treeWidget->expandItem(item);
	}
	else{
		delete newItem;
		Q_UNREACHABLE();
	}

	// 调整分类

}

void GalleryDefineWindow::deleteGalleryCategory(){
	QTreeWidgetItem* item = ui->treeWidget->currentItem();

	if (item == NULL || item->type() == ITEM_UNCLASS )
		return;

	QString path = ui->treeWidget->getItemPath(item);
	// 遍历所有素材
	for (TResourceObjectIterator<GalleryResourceObject> it; it; ++it){
		GalleryResourceObject* gallery = (GalleryResourceObject*)(*it);

		if (gallery->flags() & URF_TagGarbage)
			continue;

		// 构建所有类目
		gallery->categories_.removeOne(item->text(0));
		gallery->setFlags(URF_TagSave);

		QStringList newCategories;
		foreach(QString cate, gallery->categories_){
			if (!(cate.isEmpty() || cate.startsWith(path))){
				newCategories.append(cate);
			}
		}
		gallery->categories_ = newCategories;
	}

	// 删除该项
	delete item;

}

void GalleryDefineWindow::updateCurrentCategory(){
	updateCategory(ui->treeWidget->getCurrentItemPath(), ui->taglineEdit->text());
}

void GalleryDefineWindow::on_treeWidget_itemChanged(QTreeWidgetItem * item, int column){
	if (item == NULL)
		return;

	Q_ASSERT(column == 0);

	// 检查新名字是否合法，如果不合法，则恢复原始名字
	if (item->text(0) == oldItemName_)
		return;

	if (ui->treeWidget->findDuplications(item) > 1){
		item->setText(0, oldItemName_);
		return;
	}

	// 修改名字
	if (item->type() == ITEM_CATEGORY ){
		QString newPath = ui->treeWidget->getItemPath(item);
		// 遍历所有素材
		for (TResourceObjectIterator<GalleryResourceObject> it; it; ++it){
			GalleryResourceObject* gallery = (GalleryResourceObject*)(*it);

			if (gallery->flags() & URF_TagGarbage)
				continue;

			// 重新调整类目
			gallery->setFlags(URF_TagSave);

			QStringList newCategories;
			foreach(QString cate, gallery->categories_){
				if ( cate.startsWith(currentCategoryPath_)){
					cate.replace(0, currentCategoryPath_.size(),  newPath);
					newCategories.append(cate);
				}
				else{
					newCategories.append(cate);
				}
			}
			gallery->categories_ = newCategories;
		}

		updateCurrentCategory();
	}
}

void GalleryDefineWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem * item, int column){
	currentCategoryPath_ = ui->treeWidget->getCurrentItemPath();
	oldItemName_ = item->text(0);
}

void GalleryDefineWindow::removeGalleryCategory(QListWidgetItem* item){
	Q_ASSERT(item != NULL);

	if (ui->treeWidget->currentItem()->type() == ITEM_UNCLASS){
		return;
	}

	ResourceHash key = item->data(ITEM_HASHKEY).toUInt();
	DEFINE_RESOURCE_HASHKEY_VALID(GalleryResourceObject, gallery, key);

	gallery->categories_.removeOne(ui->treeWidget->getCurrentItemPath());
	gallery->setFlags(URF_TagSave);

	delete item;

}

void GalleryDefineWindow::on_importpushButton_clicked(){
	QTreeWidgetItem* item = ui->treeWidget->currentItem();
	QString cate;
	if (item){
		cate = ui->treeWidget->getItemPath(item);
	}
	ImportGalleryWindow w(cate);
	w.exec();

	//   刷新当前显示
	updateCurrentCategory();

}

void GalleryDefineWindow::mergeImage(){
	QList<QListWidgetItem*> items = ui->listWidget->selectedItems();

	if (items.size() < 2){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("至少选择2个以上图片进行合并!"));
		return;
	}

	QList<ResourceHash> keys;
	foreach(QListWidgetItem* item, items){
		ResourceHash key = item->data(ITEM_HASHKEY).toUInt();
		DEFINE_RESOURCE_HASHKEY(ImageResourceObject, img, key);
		if (img){
			keys.append(key);
		}
	}

	MergeImageWindow w(keys);
	if (w.exec() == QDialog::Accepted){
		//  刷新列表
		updateCurrentCategory();
	}

}

void GalleryDefineWindow::transparentImageBG(){
	QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
	Q_ASSERT(items.size() > 0);

	// 弹出选择颜色对话框
	QColor color = QColorDialog::getColor(Qt::white, this, windowTitle());
	if (color.isValid()){
		// 修改图片内容
		foreach(QListWidgetItem* item, items){
			ResourceHash key = item->data(ITEM_HASHKEY).toUInt();
			DEFINE_RESOURCE_HASHKEY(ImageResourceObject, img, key);
			if (img){
				// 将指定颜色替换为透明色
				QImage newImage = transparentImage(img->image_, color);
				img->setImage(newImage);
				// 更新列表显示
				item->setIcon(img->thumbnail_);
				img->setFlags(URF_TagSave);
				SAVE_PACKAGE_RESOURCE(ImageResourceObject, img);
			}
		}
	}
}

void GalleryDefineWindow::copyGallery(){
	QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
	Q_ASSERT(items.size() > 0);

	ResourceObject* newGallery = NULL;
	foreach(QListWidgetItem* item, items){
		ResourceHash key = item->data(ITEM_HASHKEY).toUInt();
		DEFINE_RESOURCE_HASHKEY_VALID(GalleryResourceObject, gallery, key);

		QString newName = ResourceObject::makeUniqueObjectName(gallery->outer(), gallery);

		// 判断选择的资源类型
		if (UIsExactKindOf(ImageResourceObject, gallery)){
			ImageResourceObject* image = UDynamicCast(ImageResourceObject, gallery);
			// 复制
			ImageResourceObject* newGallery = TCloneObject<ImageResourceObject>(image, newName);
			// 保存到本地
			newGallery->clearFlags(URF_TagGarbage);
			newGallery->setFlags(URF_TagSave);
			SAVE_PACKAGE_RESOURCE(ImageResourceObject, newGallery);
		}
		else if (UIsExactKindOf(AudioResourceObject, gallery)){

			AudioResourceObject* audio = UDynamicCast(AudioResourceObject, gallery);
			// 复制
			AudioResourceObject* newGallery = TCloneObject<AudioResourceObject>(audio, newName);
			newGallery->clearFlags(URF_TagGarbage);
			newGallery->setFlags(URF_TagSave);
			SAVE_PACKAGE_RESOURCE(AudioResourceObject, newGallery);
		}
		else if (UIsExactKindOf(VideoResourceObject, gallery)){
			VideoResourceObject* video = UDynamicCast(VideoResourceObject, gallery);
			// 复制
			VideoResourceObject* newGallery = TCloneObject<VideoResourceObject>(video, newName);
			newGallery->clearFlags(URF_TagGarbage);
			newGallery->setFlags(URF_TagSave);
			SAVE_PACKAGE_RESOURCE(VideoResourceObject, newGallery);
		}
	}

	//   刷新当前显示
	updateCurrentCategory();

}

void GalleryDefineWindow::closeEvent(QCloseEvent * event){
	// 提示保存
	if (modified_){
		if (QMessageBox::question(this, windowTitle(), UDQ_TR("是否保存对素材库的修改?")) == QMessageBox::Yes){
			on_savepushButton_clicked();
		}
	}
}

void GalleryDefineWindow::splitImage(){
	QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
	Q_ASSERT(items.size() == 1);

	// 仅取第一个图像
	ResourceHash imgkey = items[0]->data(ITEM_HASHKEY).toUInt();
	DEFINE_RESOURCE_HASHKEY(ImageResourceObject, img, imgkey);
	if (img == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请选择一个合适的图像资源!"));
		return;
	}

	QSize imgsize = img->image_.size();
	if (imgsize.width() < 8 || imgsize.height() < 8){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("选择的图像尺寸太小，无法分割!"));
		return;
	}

	SplitImageWindow w(imgkey);
	w.exec();
}