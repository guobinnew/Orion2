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

#include "layerwindow.h"
#include "ui_layerwindow.h"
#include "tooldefine.h"
#include "tilemapwidget.h"
#include "framemanager.h"
#include "mainmanagerframe.h"
#include "levelview.h"
#include "levelscene.h"
#include "tilemapeditwindow.h"

LayerWindow::LayerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayerWindow)
{
    ui->setupUi(this);

	TileMapWidget* widget = new TileMapWidget(ui->scrollArea);
	ui->scrollArea->setWidget(widget);

	connect(widget, SIGNAL(changeBrush(const QRect&)), this, SLOT(updateBrush(const QRect&)));
}

LayerWindow::~LayerWindow()
{
    delete ui;
}

void	LayerWindow::resizeEvent(QResizeEvent * event){
	QSize newSize = event->size() - QSize(WIDGET_MARGIN * 2, WIDGET_MARGIN * 2);
	ui->layoutWidget->setGeometry(QRect(QPoint(WIDGET_MARGIN, WIDGET_MARGIN), newSize));
}

void LayerWindow::loadTileMap(ObjectTypeResourceObject* objType, const QSize& grid ){
	objType_ = objType;
	gridSize_ = grid;

	TileMapWidget* widget = (TileMapWidget*)(ui->scrollArea->widget());
	if (objType_->staticImage_){
		widget->init(objType_->staticImage_->image_, grid, objType_->tilemapData_);
	}
	else{
		widget->init(QImage(), grid, objType_->tilemapData_);
	}
}

void LayerWindow::on_pointerpushButton_clicked(){
	// 设置场景模式
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);

	LevelScene* scene = (LevelScene*)frame->sceneView()->scene();
	scene->setMode(LevelScene::DOM_POINTER);

	TileMapWidget* widget = (TileMapWidget*)(ui->scrollArea->widget());
	widget->setSelectMode(0);

	updateBrush(QRect());
}

void LayerWindow::on_penpushButton_clicked(){
	// 设置场景模式
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);

	LevelScene* scene = (LevelScene*)frame->sceneView()->scene();
	scene->setMode(LevelScene::DOM_TILEEDIT);

	TileMapWidget* widget = (TileMapWidget*)(ui->scrollArea->widget());
	widget->setSelectMode(0);

	updateBrush(widget->activeRegion());
}

void LayerWindow::on_rectpushButton_clicked(){
	// 设置场景模式
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);

	LevelScene* scene = (LevelScene*)frame->sceneView()->scene();
	scene->setMode(LevelScene::DOM_TILEEDIT);

	TileMapWidget* widget = (TileMapWidget*)(ui->scrollArea->widget());
	widget->setSelectMode(1);

	updateBrush(widget->activeRegion());
}

void LayerWindow::on_delpushButton_clicked(){
	// 设置场景模式
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);

	LevelScene* scene = (LevelScene*)frame->sceneView()->scene();
	scene->setMode(LevelScene::DOM_TILEEDIT);

	TileMapWidget* widget = (TileMapWidget*)(ui->scrollArea->widget());
	widget->setSelectMode(1);

	updateBrush(widget->activeRegion());
}

void LayerWindow::updateBrush(const QRect& region ){

	// 更新场景刷子
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);
	LevelScene* scene = (LevelScene*)frame->sceneView()->scene();
	scene->setTileBrush(region, ui->delpushButton->isChecked() ? 1 : 0 );

}

void LayerWindow::on_zoominpushButton_clicked(){
	TileMapWidget* widget = (TileMapWidget*)(ui->scrollArea->widget());
	widget->zoomIn();
}

void LayerWindow::on_zoomoutpushButton_clicked(){
	TileMapWidget* widget = (TileMapWidget*)(ui->scrollArea->widget());
	widget->zoomOut();
}

void LayerWindow::clear(){
	ui->pointerpushButton->setChecked(true);
	TileMapWidget* widget = (TileMapWidget*)(ui->scrollArea->widget());
	widget->clear();
}


void LayerWindow::on_collisionpushButton_clicked(){

	if (gridSize_.width() == 0 || gridSize_.height() == 0){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("格子大小为0,无法编辑碰撞网格"));
		return;
	}
	// 获取当前选择的第一个Tile块
	TileMapWidget* widget = (TileMapWidget*)(ui->scrollArea->widget());
	// 打开碰撞网格编辑
	TileMapEditWindow w(objType_, gridSize_, widget->currentIndex() );
	w.exec();

	// 更新碰撞网格数据
	widget->updateCollision(objType_->tilemapData_);
}