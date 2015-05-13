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
**  Orion2 is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#ifndef LAYERWINDOW_H
#define LAYERWINDOW_H

#include <QWidget>
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class LayerWindow;
}

class LayerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LayerWindow(QWidget *parent = 0);
    ~LayerWindow();

	void loadTileMap(ObjectTypeResourceObject* objType, const QSize& grid );

	// Çå¿Õ
	void clear();
protected:
	virtual void resizeEvent(QResizeEvent * event);

private:
    Ui::LayerWindow *ui;
	ObjectTypeResourceObject* objType_;
	QSize gridSize_;

private slots:
    void updateBrush(const QRect& region);
    void on_pointerpushButton_clicked();
	void on_penpushButton_clicked();
	void on_rectpushButton_clicked();
	void on_delpushButton_clicked();

	void on_collisionpushButton_clicked();
	void on_zoominpushButton_clicked();
	void on_zoomoutpushButton_clicked();

};

#endif // LAYERWINDOW_H
