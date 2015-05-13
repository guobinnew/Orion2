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

#ifndef PLUGINTYPEEDITWINDOW_H
#define PLUGINTYPEEDITWINDOW_H

#include <QDialog>
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class PluginTypeEditWindow;
}

class PluginTypeEditWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PluginTypeEditWindow(ObjectTypeResourceObject* objType, QWidget *parent = 0);
    ~PluginTypeEditWindow();

	bool canSave();
	void init();

	QTreeWidgetItem* findPropertyItem(const QString& value, QTreeWidgetItem* parentItem);

	QTreeWidgetItem* addVariableItem(PluginVariable* def, QTreeWidgetItem* parentItem);
	QTreeWidgetItem* addVariableItem(PluginVariable* def, const QString& value, QTreeWidgetItem* parentItem);
	
	QTreeWidgetItem* addBehaviorItem(const QString& name, QTreeWidgetItem* parentItem);
	QTreeWidgetItem* addEffectItem(const QString& name, QTreeWidgetItem* parentItem);

	// 收集已用命名
	QStringList attachmentNames();

private:
    Ui::PluginTypeEditWindow *ui;
	ObjectTypeResourceObject* objtype_;  // 对象类型

	QTreeWidgetItem* varRoot_;
	QTreeWidgetItem* behaviorRoot_;
	QTreeWidgetItem* effectRoot_;

	SpriteResourceObject* sprite_;
	ImageResourceObject* image_;
	void updateImage();

private slots:
	void  on_cancelpushButton_clicked();
	void  on_savepushButton_clicked();
	void on_varpushButton_clicked();
	void on_behaviorpushButton_clicked();
	void on_effectpushButton_clicked();

	void on_deltoolButton_clicked();
	void on_uptoolButton_clicked();
	void on_downtoolButton_clicked();

	void on_animpushButton_clicked();

};

#endif // PLUGINTYPEEDITWINDOW_H
