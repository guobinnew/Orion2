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

#ifndef ORION_GALLERYCATEGORYWINDOW_H
#define ORION_GALLERYCATEGORYWINDOW_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "commondefine.h"
#include "gallerytreewidget.h"
#include "gallerylistwidget.h"

namespace Ui {
class GalleryCategoryWindow;
}

class GalleryCategoryWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GalleryCategoryWindow(QWidget *parent = 0);
    ~GalleryCategoryWindow();

	// 刷新目录树
	void updateCategoryTree();
	// 更新当前分类列表
	void updateCategoryPreview(const QString& cate, const QString& tag = UDQ_T(""));
	void updateGalleryItem(QListWidgetItem* item, GalleryResourceObject* gallery);
	void insertGalleryItemToList(GalleryResourceObject* gallery);
	void addGalleryItemToList(GalleryResourceObject* gallery);

	// 更新当前分类列表
	void updateCategory(const QString& cate, const QString& tag = UDQ_T(""));

protected:
	virtual void	resizeEvent(QResizeEvent * event);

private:
    Ui::GalleryCategoryWindow *ui;
	QStringList categoryPaths_;   // 所有类目路径
	QTreeWidgetItem* unknowCategoryItem_;  //  未分类

	GalleryTreeWidget* galleryTree_;
	GalleryListWidget* previewList_;

	QTreeWidgetItem* findTreePath(const QString& path, QTreeWidget* tree);
	QIcon defaultIcon_;      // Icon
private slots:
    void treeItemSelectionChanged();
	void on_filterineEdit_textEdited(const QString & text);

	void on_importpushButton_clicked();
	void on_addpushButton_clicked();

	void addGalleryCategory();
	void deleteGalleryCategory();
	void updateCurrentCategory();
	void removeGalleryCategory(QListWidgetItem* item);

	void transparentImageBG();
	void copyGallery();


};

#endif // GALLERYCATEGORYWINDOW_H
