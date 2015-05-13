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

#ifndef INSTANCEEDITWINDOW_H
#define INSTANCEEDITWINDOW_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "editorregion.h"
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class InstanceEditWindow;
}

class InstanceEditWindow : public QDialog
{
    Q_OBJECT

public:
	explicit InstanceEditWindow(EditorRegion* region, const  QList<ResourceHash>& layers, QWidget *parent = 0);
    ~InstanceEditWindow();

private:
    Ui::InstanceEditWindow *ui;

	EditorRegion* host_;  // 
	QMap<QString, ResourceHash> layerMap_;
	// 准备属性树
	void preparePropertyTree();

	QTreeWidgetItem* addPropertyItem(PluginVariable* var,  const QString& value, QTreeWidgetItem* parentItem);

	QHash<int, QTreeWidgetItem*>  attrMap_;  // 属性映射

	QTreeWidgetItem* varRoot_;
	QTreeWidgetItem* pluginRoot_;
	QTreeWidgetItem* behaviorRoot_;
	QTreeWidgetItem* effectRoot_;


	QStringList  blendMode_;  // 混合模式
	// 查找类目
	QTreeWidgetItem* findItem(const QString& name, QTreeWidgetItem* parent );

private slots:
	void  on_cancelpushButton_clicked();
	void  on_savepushButton_clicked();
	void on_editpushButton_clicked();

	void on_treeWidget_itemSelectionChanged();
};

#endif // INSTANCEEDITWINDOW_H
