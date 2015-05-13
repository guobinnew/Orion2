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
#ifndef ORION_WORKSPACEWINDOW_H
#define ORION_WORKSPACEWINDOW_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class WorkspaceWindow;
}

class WorkspaceWindow : public QWidget
{
    Q_OBJECT

public:
    explicit WorkspaceWindow(QWidget *parent = 0);
    ~WorkspaceWindow();

	// 更新工作空间
	void updateWorkspace();
	void updateLevel(ProjectResourceObject* proj);
	void updateObject(ProjectResourceObject* proj);

	// 保存对象类型
	void saveObjectType();

	void addObjectType(ResourceHash key);
	void refreshObjectType(ResourceHash key);
	// 更新图标
	void updateWorkspaceIcon();

	// 关闭工作空间
	void clearWorkspace();
	void clearSelection();

	// 添加图层Item
	QTreeWidgetItem* addLayer(LayerResourceObject* layer);

	// 删除单个页面
	void deleteLevel(QTreeWidgetItem* item);
	void deleteLayer(QTreeWidgetItem* item);

	// 刷新图层次序
	void reorderLayerItems(LevelResourceObject* level);

	// 恢复对象类型
	void recycleObjectType(ObjectTypeResourceObject* obj);

signals:
	void itemSelected(QTreeWidgetItem* item);
	void activeLayer(unsigned int key);
	void refreshLayers();
	void refreshCurrentLayer();
	void deleteObjectType(unsigned int key);
	void deleteFamily(unsigned int key);

protected:
	virtual void resizeEvent(QResizeEvent * event);
	
public slots:
    // 删除项目中的场景和图层
    void deleteProjectItem();
	// 删除对象类型、集合和容器等
	void deleteObjectItem();

	void refreshObjectTypes();
	void layerMoved(unsigned int key, int order );
	void addObjectGroup();
	void editSound();
	void editMovie();
	void editObjectType();

private slots:
	void  on_levelpushButton_clicked();
	void  on_layerpushButton_clicked();
	void  on_trashpushButton_clicked();
	void  on_familypushButton_clicked();
	void  on_soundpushButton_clicked();
	void  on_moviepushButton_clicked();

	void on_leveltreeWidget_currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);
	void on_leveltreeWidget_itemDoubleClicked(QTreeWidgetItem * item, int column);

	void on_objecttreeWidget_itemChanged(QTreeWidgetItem * item, int column);
	void on_objecttreeWidget_currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);

private:
    Ui::WorkspaceWindow *ui;
	WorkspaceResourceObject workspace_;  // 当前使用的工作空间


	bool isClosing_;  // 是否正在关闭
};

#endif // WORKSPACEWINDOW_H
