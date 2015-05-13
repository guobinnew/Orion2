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

#include <QtGui>
#include <QtGlobal>
#include <QSettings>
#include <QWhatsThis>
#include <QStatusBar>
#include <QDockWidget>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QListWidget>
#include <QFileDialog>
#include <QProcess>
#include <QStandardPaths>
#include "commondefine.h"

#include "mainspriteframe.h"
#include "mainmanagerframe.h"
#include "spritescene.h"
#include "menumanager.h"
#include "renamewindow.h"
#include "configmanager.h"
#include "framemanager.h"

#include <queue>
#include <boost/shared_ptr.hpp>

MainSpriteFrame::MainSpriteFrame(QWidget *parent ) : QMainWindow(parent), editorView_(NULL) {
	init();
}

MainSpriteFrame::~MainSpriteFrame(){

}


void MainSpriteFrame::init(){

	// 设置窗口图标和标题
	setWindowIcon( QIcon( UDQ_T(":/images/unique64.png") ) );
	setWindowTitle( UDQ_T("Orion2 Sprite Editor") );

	initSceneView();

	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	createDockWindows();


}

// 初始化场景
void MainSpriteFrame::initSceneView(){

	if( editorView_ == NULL ){
		editorView_ = new SpriteView();
		setCentralWidget( editorView_ );
	}

	// 连接信号
	QGraphicsScene* oldScence = editorView_->scene();
	SpriteScene* scene = new SpriteScene();
	editorView_->setScene( scene );

	// 连接信号
	connect(scene, SIGNAL(error(const QString&)), this, SLOT(errorMessage(const QString&)));

	// 释放老地图
	if( oldScence != 0 ){
		delete oldScence;
	}
}

void MainSpriteFrame::createActions(){

	gMenuManager->createActions( EDITOR_SPRITE, this );

	BEGIN_ACTION();
	//////////////////////////////////////////////////////////////////////////
	// Scenario
	//////////////////////////////////////////////////////////////////////////
	CONNECT_ACTION(ORION_SPRTIE_SAVE, this, submitCurrentSprite);
	CONNECT_ACTION(ORION_SPRTIE_SAVEALL, this, submitAllSprite);

	CONNECT_ACTION(ORION_SPRTIE_REFRESH, this, refreshCurrentSprite);
	CONNECT_ACTION(ORION_SPRTIE_DELETE, this, deleteSelectedItem);

	CONNECT_ACTION(ORION_SPRTIE_ADDSEQ, this, addSequenceItem);
	CONNECT_ACTION(ORION_SPRTIE_DELSEQ, this, removeSequenceItem);
	CONNECT_ACTION(ORION_SPRTIE_RENAMESEQ, this, renameSequenceItem);

	CONNECT_ACTION(ORION_SPRITE_DEFAULTSEQ, this, defaultSequenceItem);

	/*modeGroup_ = new QActionGroup(this);
	modeGroup_->addAction( gMenuManager->getAction(FORCE_EDITOR_POINTER) );
	modeGroup_->addAction( gMenuManager->getAction(FORCE_EDITOR_MODELGROUP) );*/
	//gMenuManager->getAction(FORCE_EDITOR_POINTER)->setChecked(true);

	//////////////////////////////////////////////////////////////////////////
	// View
	//////////////////////////////////////////////////////////////////////////
	END_ACTION();

	QGraphicsScene* scene = editorView_->scene();
	Q_ASSERT( scene != NULL );
	connectSceneEvent( scene );

}

void MainSpriteFrame::connectSceneEvent(QGraphicsScene* scene){
	BEGIN_ACTION();

	CONNECT_ACTION(ORION_SPRTIE_COPYANCHOR, scene, applyAnchorToSequence);

	END_ACTION();
}


void MainSpriteFrame::createMenus(){

}


void MainSpriteFrame::createToolBars(){

}

void MainSpriteFrame::createStatusBar(){
	statusBar()->showMessage(UDQ_TR("就绪"));
}

void MainSpriteFrame::createDockWindows(){

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	structDockWidget_ = new QDockWidget(this);
	structDockWidget_->setAllowedAreas(Qt::AllDockWidgetAreas);
	structDockWidget_->setFeatures(QDockWidget::NoDockWidgetFeatures);
	addDockWidget(Qt::RightDockWidgetArea, structDockWidget_);

	structWindow_ = new SpriteStructWindow(structDockWidget_);
	structDockWidget_->setWidget(structWindow_);
	structDockWidget_->setWindowTitle(structWindow_->windowTitle());

	connect(structWindow_, SIGNAL(deleteSprite(unsigned int)), this, SLOT(deleteSprite(unsigned int)));

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	galleryDockWidget_ = new QDockWidget(this);
	galleryDockWidget_->setAllowedAreas(Qt::RightDockWidgetArea);
	galleryDockWidget_->setFeatures(/*QDockWidget::NoDockWidgetFeatures | */
		QDockWidget::DockWidgetMovable);
	addDockWidget(Qt::RightDockWidgetArea, galleryDockWidget_);

	galleryWindow_ = new SpriteGalleryWindow(galleryDockWidget_);
	galleryDockWidget_->setWidget(galleryWindow_);
	galleryDockWidget_->setWindowTitle(galleryWindow_->windowTitle());

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	tileDockWidget_ = new QDockWidget(this);
	tileDockWidget_->setAllowedAreas(Qt::RightDockWidgetArea );
	tileDockWidget_->setFeatures(/*QDockWidget::NoDockWidgetFeatures | */
		QDockWidget::DockWidgetMovable);
	addDockWidget(Qt::RightDockWidgetArea, tileDockWidget_);

	tilesetWindow_ = new TileSetWindow(tileDockWidget_);
	tileDockWidget_->setWidget(tilesetWindow_);
	tileDockWidget_->setWindowTitle(tilesetWindow_->windowTitle());



}

void MainSpriteFrame::closeEvent(QCloseEvent *event){

}

bool MainSpriteFrame::saveCurrentSprite(){

	// 保存当前精灵
	SpriteScene* scene = (SpriteScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);

	// 保存到数据库中
	SpriteResourceObject* sprite = scene->spriteObject();
	if (sprite == NULL){
		return false;
	}

	// 保存上一个精灵
	scene->save();
	return true;

}

void MainSpriteFrame::submitCurrentSprite(){

	// 先保存
	if ( !saveCurrentSprite()){
		return;
	}

	// 保存当前精灵
	SpriteScene* scene = (SpriteScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);

	// 保存到数据库中
	SpriteResourceObject* sprite = scene->spriteObject();

	// 保存到本地
	SAVE_PACKAGE_RESOURCE(SpriteResourceObject, sprite);
	QMessageBox::information(this, windowTitle(), UDQ_TR("精灵[%1]保存成功!").arg(sprite->description()));

}

void MainSpriteFrame::submitAllSprite(){

	// 保存当前精灵
	SpriteScene* scene = (SpriteScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);
	scene->save();

	// 保存所有库
	ApplyResourceLinkers<SpriteResourceObject>();
	
	// 
	QMessageBox::information(this, windowTitle(), UDQ_TR("所有精灵对象保存完毕!"));
}

void MainSpriteFrame::refreshCurrentSprite(){
	// 刷新当前精灵，因为图片改变，更新动画帧的图片缓存
	SpriteScene* scene = (SpriteScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);

	// 属性序列
	scene->refreshSequences();

}



void MainSpriteFrame::deleteSelectedItem(){

	if (QMessageBox::question(this, windowTitle(), UDQ_TR("确认删除选中的对象（不可恢复）?")) == QMessageBox::No){
		return;
	}

	SpriteScene* scene = (SpriteScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);

	scene->removeSelectedObjects();
}


void MainSpriteFrame::removeSequenceItem(){

	if (QMessageBox::question(this, windowTitle(), UDQ_TR("确认删除选中的动画序列（不可恢复）?")) == QMessageBox::No){
		return;
	}
	SpriteScene* scene = (SpriteScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);
	scene->removeSelectedSequence();
}

void MainSpriteFrame::applyAnchorToSequence(){

}


void MainSpriteFrame::addSequenceItem(){

	SpriteScene* scene = (SpriteScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);

	// 保存到数据库中
	SpriteResourceObject* sprite = scene->spriteObject();
	if (sprite == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请先打开一个精灵对象，然后添加动画序列"));
		return;
	}

	// 如果是第一个序列
	QString seqName;
	if (sprite->sequences_.size() == 0){
		seqName = UDQ_T("Default");
		sprite->defaultSeq_ = seqName;
	}

	// 保存到数据库中
	SpriteSequenceResourceObject* seq = TAddChildObject<SpriteSequenceResourceObject>(sprite, seqName);
	Q_ASSERT(seq != 0);

	QRectF rect = scene->viewRegionRect();
	seq->pos_ = rect.center();

	sprite->setFlags(URF_TagSave);
	scene->addSequenceItem(seq);

}

void MainSpriteFrame::defaultSequenceItem(){
	SpriteScene* scene = (SpriteScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);

	SpriteSequenceResourceObject* seq = scene->currentSequence();
	if (seq != NULL){
			//  修改缺省对象
			SpriteResourceObject* sprite = (SpriteResourceObject*)seq->outer();
			sprite->defaultSeq_ = seq->description();
			sprite->setFlags(URF_TagSave);
	}
}

void MainSpriteFrame::renameSequenceItem(){
	SpriteScene* scene = (SpriteScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);

	SpriteSequenceResourceObject* seq = scene->currentSequence();
	if (seq != NULL){
		QString oldName = seq->description();
		RenameWindow w(seq, false);
		if (w.exec() == QDialog::Accepted){
			//  修改缺省对象
			SpriteResourceObject* sprite = (SpriteResourceObject*)seq->outer();
			if (sprite->defaultSeq_ == oldName){
				sprite->defaultSeq_ = seq->description();
			}
			sprite->setFlags(URF_TagSave);
		}
	}
}



void  MainSpriteFrame::loadSprite(ResourceHash key, const QString& title){
	DEFINE_RESOURCE_HASHKEY(SpriteResourceObject, sprite, key);
	if (sprite == NULL){
		QMessageBox::critical(this, windowTitle(), UDQ_TR("当前选择的精灵[%1]数据没有找到!").arg(title));
		return;
	}

	tilesetWindow_->clear();
	galleryWindow_->init(sprite);

	SpriteScene* scene = (SpriteScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);

	// 保存上一个精灵
	scene->save();

	//  加载新精灵
	scene->load(sprite);
}

void MainSpriteFrame::errorMessage(const QString& msg){
	QMessageBox::critical(this, windowTitle(), msg);
}

void MainSpriteFrame::updateSpriteName(){
	SpriteScene* scene = (SpriteScene*)editorView_->scene();
	scene->updateAnimText();
}

void MainSpriteFrame::deleteSprite(unsigned int key){
	//  如果当前场景正在编辑，清空编辑器
	SpriteScene* scene = (SpriteScene*)editorView_->scene();
	SpriteResourceObject* cursprite = scene->spriteObject();
	if (cursprite && cursprite->hashKey() == key){
		scene->clearSprite();
		tilesetWindow_->clear();
		galleryWindow_->clear();
	}

	// 更新所有的对象类型
	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	if (ws){
		// 获取当前电子书
		ProjectResourceObject* proj = ws->currentProject();
		if (proj != NULL){
			foreach(ObjectTypeResourceObject* obj, proj->objTypes_){
				if (obj->flags() & URF_TagGarbage)
					continue;

				if (obj->animation_ &&obj->animation_->hashKey() == key){  // 对象类型为删除的动画
					obj->clearImageResource();
				}
			}
		}
	}

	// 通知当前编辑器进行更新
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);
	frame ->refreshLayers();

	// 彻底删除动画
	DEFINE_RESOURCE_HASHKEY_VALID(SpriteResourceObject, sprite, key);
	SAVE_PACKAGE_RESOURCE(SpriteResourceObject, sprite);

}