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
#ifndef ORION_RESOURCEREFERENCECHECKWINDOW_H
#define ORION_RESOURCEREFERENCECHECKWINDOW_H

#include <QDialog>
#include "ui_resourcereferencecheckwindow.h"
#include "resourceobject.h"
using namespace ORION;

class ResourceReferenceCheckWindow : public QDialog
{
	Q_OBJECT

public:
	ResourceReferenceCheckWindow( ResourceHash hashkey, QWidget *parent = 0);
	~ResourceReferenceCheckWindow();

	// 资源是否被引用
	bool isUsed(){
		return ( references_.size() > 0 );
	}

private:
	Ui::ResourceReferenceCheckWindow* ui;
	ResourceObject* res_;

	//  检查引用
	void checkReference();
	void checkSpriteReference( ResourceObject* sprite );
	void checkImageReference( ResourceObject* anim );
	
	QString resourceTypeToString(ResourceObject* obj);
	QString resourcetoString(ResourceObject* obj);

	QList<ResourceObject*> references_;  //  引用对象
private slots:
	void on_cancelpushButton_clicked();
	void on_savepushButton_clicked();
};

#endif // RESOURCEREFERENCECHECKWINDOW_H
