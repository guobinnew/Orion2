#include "selecttilesetwindow.h"
#include "ui_selecttilesetwindow.h"
#include <QFileDialog>
#include "commondefine.h"
#include "mergeimagewindow.h"
#include "splitimagewindow.h"
#include "menumanager.h"
#include "tooldefine.h"

SelectTilesetWindow::SelectTilesetWindow(ResourceHash id, QWidget *parent) :
QDialog(parent), replaceId_(id),
ui(new Ui::SelectTilesetWindow)
{
	ui->setupUi(this);
	setFixedSize(size());

	Qt::WindowFlags oldflag = windowFlags();
	oldflag &= !Qt::WindowContextHelpButtonHint;
	setWindowFlags(oldflag);

	initFromDb();

	BEGIN_ACTION();
	CONNECT_ACTION(ORION_GALLERY_MERGE, this, mergeImage);
	CONNECT_ACTION(ORION_GALLERY_SPLIT, this, splitImage);

	END_ACTION();

	ui->treeWidget->setCurrentItem(unknowCategoryItem_);
}

SelectTilesetWindow::~SelectTilesetWindow()
{
	BEGIN_ACTION();
	DISCONNECT_ACTION(ORION_GALLERY_MERGE);
	END_ACTION();

	delete ui;
}


void SelectTilesetWindow::initFromDb(void){

	unknowCategoryItem_ = new QTreeWidgetItem(ITEM_UNCLASS);
	unknowCategoryItem_->setText(0, UDQ_TR("未分类"));
	unknowCategoryItem_->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
	ui->treeWidget->addTopLevelItem(unknowCategoryItem_);

	QStringList tagList;

	// 加载列表
	for (TResourceObjectIterator<ImageResourceObject> it; it; ++it){
		ImageResourceObject* gallery = (ImageResourceObject*)(*it);

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
	ui->treeWidget->expandAll();

}

QTreeWidgetItem* SelectTilesetWindow::findTreePath(const QString& path, QTreeWidget* tree){
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

void SelectTilesetWindow::on_treeWidget_itemSelectionChanged(){

	QTreeWidgetItem* item = ui->treeWidget->currentItem();
	if (item == NULL){
		ui->listWidget->clear();
		return;
	}

	updateCurrentCategory();

}


void SelectTilesetWindow::updateCurrentCategory(){

	QString itemPath;
	QTreeWidgetItem* item = ui->treeWidget->currentItem();

	if (item != NULL && item->type() != ITEM_UNCLASS){
		QTreeWidgetItem* current = item;
		while (item != NULL && item->type() == ITEM_CATEGORY){
			itemPath.push_front(item->text(0));

			item = item->parent();
			if (item && item->type() == ITEM_CATEGORY){
				itemPath.push_front(UDQ_T("."));
			}
		}
	}

	updateCategory(itemPath);
}

void SelectTilesetWindow::updateCategory(const QString& cate, const QString& tag){
	ui->listWidget->clear();

	// 加载列表
	for (TResourceObjectIterator<ImageResourceObject> it; it; ++it){
		ImageResourceObject* gallery = (ImageResourceObject*)(*it);

		if (gallery->flags() & URF_TagGarbage)
			continue;

		if (gallery->isBelong(cate)){
			//匹配类型
			if (!UIsExactKindOf(ImageResourceObject, gallery)){
				continue;
			}

			// 严格匹配标签
			if (!tag.isEmpty() && !gallery->tags_.contains(tag)){
				continue;
			}

			addGalleryItemToList(gallery);
		}

	}

}


// 向列表中添加
void SelectTilesetWindow::addGalleryItemToList(ImageResourceObject* gallery){

	QListWidgetItem* item = new QListWidgetItem(ui->listWidget, ITEM_GALLERY);
	item->setIcon(gallery->thumbnail_);
	item->setData(ITEM_DESC, gallery->objectName());
	item->setData(ITEM_HASHKEY, gallery->hashKey());
	item->setData(ITEM_CONTEXT, gallery->image_.size());


}

void SelectTilesetWindow::on_savepushButton_clicked(){
	QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
	if (items.size() == 0){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请选择一个素材"));
		return;
	}

	if (replaceId_ > 0 && items.size() != 1 ){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("只能选择一个素材进行替换"));
		return;
	}

	foreach(QListWidgetItem* item, items){

		ResourceHash key = item->data(ITEM_HASHKEY).toUInt();
		emit addGallery(key, replaceId_ );
	}

	if (replaceId_ > 0)
		accept();
}

void SelectTilesetWindow::on_cancelpushButton_clicked(){
	if (replaceId_ == 0)
		accept();
	else
		reject();
}

void SelectTilesetWindow::on_listWidget_itemSelectionChanged(){

	QListWidgetItem* item = ui->listWidget->currentItem();

	ResourceHash key = item->data(ITEM_HASHKEY).toUInt();
	DEFINE_RESOURCE_HASHKEY_VALID(ImageResourceObject, img, key);

	ui->prevwidget->showPicture(img->content_);
	if (img->isGroup_){
		ui->prevwidget->showFrames(img->frames_);
	}
	else{
		ui->prevwidget->hideFrames();
	}

	// 显示信息
	QSize imgsize = ui->prevwidget->getPicture().size();
	QString info = UDQ_TR("%1: 图像大小 %2x%3")
		.arg(img->objectName())
		.arg(imgsize.width())
		.arg(imgsize.height());
	ui->infolabel->setText(info);

}


void SelectTilesetWindow::mergeImage(){
	QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
	Q_ASSERT(items.size() > 1);

	QList<ResourceHash> keys;
	foreach(QListWidgetItem* item, items){
		keys.append(item->data(ITEM_HASHKEY).toUInt());
	}

	MergeImageWindow w(keys);
	if (w.exec() == QDialog::Accepted){
		//  刷新列表
		updateCurrentCategory();
	}

}

void SelectTilesetWindow::splitImage(){
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