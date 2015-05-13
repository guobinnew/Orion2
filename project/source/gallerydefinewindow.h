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
#ifndef ORION_GALLERYDEFINEWINDOW_H
#define ORION_GALLERYDEFINEWINDOW_H

#include <QDialog>
#include <QListWidget>
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;

namespace Ui {
class GalleryDefineWindow;
}

class GalleryDefineWindow : public QDialog
{
    Q_OBJECT

public:
    explicit GalleryDefineWindow(QWidget *parent = 0);
    ~GalleryDefineWindow();

	void initFromDb(void);
protected:
	virtual void	closeEvent(QCloseEvent * event);
private:
    Ui::GalleryDefineWindow *ui;

	// 根据路径（“A.B.C”）查找对应项，如果没有则创建
	QTreeWidgetItem* findTreePath(const QString& path, QTreeWidget* tree);
	// 向列表中添加
	void insertGalleryItemToList(GalleryResourceObject* gallery);
	void addGalleryItemToList(GalleryResourceObject* gallery);
	void updateGalleryItem(QListWidgetItem* item, GalleryResourceObject* gallery);

	QTreeWidgetItem* unknowCategoryItem_;  //  未分类

	// 更新当前分类列表
	void updateCategory(const QString& cate, const QString& tag = UDQ_T("") );

	QStringList categoryPaths_;   // 所有类目路径
	unsigned int filter_; // 过滤类型
	QString currentCategoryPath_; // 当前显示的类目路径
	QString oldItemName_;// 

	bool modified_;  // 是否修改过

	QList<GalleryResourceObject*> deletedGalleries_; // 待删除的资源

private slots:
	void on_savepushButton_clicked();
	void on_deletepushButton_clicked();
	void on_cancelpushButton_clicked();
	void on_importpushButton_clicked();
	void on_exportpushButton_clicked();
	void on_addpushButton_clicked();

	void on_treeWidget_itemSelectionChanged();
	void on_treeWidget_itemChanged(QTreeWidgetItem * item, int column);
	void on_treeWidget_itemDoubleClicked(QTreeWidgetItem * item, int column);

	void on_imagecheckBox_stateChanged(int state);
	void on_audiocheckBox_stateChanged(int state);
	void on_videocheckBox_stateChanged(int state);

	void on_taglineEdit_textEdited(const QString & text);

	void addGalleryCategory();
	void deleteGalleryCategory();
	void updateCurrentCategory();
	void removeGalleryCategory(QListWidgetItem* item);

	void mergeImage();
	void transparentImageBG();
	void copyGallery();
	void splitImage();
};

#endif // GALLERYDEFINEWINDOW_H
