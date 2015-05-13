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

#include "tilesetwindow.h"
#include "tooldefine.h"
#include "ui_tilesetwindow.h"
#include "imagehelper.h"
#include "resourceobject.h"
using namespace ORION;

TileSetWindow::TileSetWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TileSetWindow)
{
    ui->setupUi(this);
}

TileSetWindow::~TileSetWindow()
{
    delete ui;
}

void	TileSetWindow::resizeEvent(QResizeEvent * event){
	QSize newSize = event->size() - QSize(WIDGET_MARGIN * 2, WIDGET_MARGIN * 2);
	ui->layoutWidget->setGeometry(QRect(QPoint(WIDGET_MARGIN, WIDGET_MARGIN), newSize));
}

void TileSetWindow::clear(){
	ui->listWidget->clearTiles();
	ui->listWidget->setHostId(0);
}

void TileSetWindow::loadGallery(SpriteResourceObject* sprite, ResourceHash id){

	clear();

	if (sprite == NULL)
		return;

	ui->listWidget->setHostId(id);

	ResourceHash key = sprite->imgSources_.value(id);
	DEFINE_RESOURCE_HASHKEY(ImageResourceObject, img, key);
	if (img != NULL){
		foreach(QRect r, img->frames_){
			// 从原始图像提取部分图片
			QImage itemImg = img->image_.copy(r);
			ui->listWidget->addTile(itemImg);
		}
	}
	
}
