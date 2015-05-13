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

#ifndef  ORION_SPRITE_EDITOR_H
#define ORION_SPRITE_EDITOR_H

#include <QMainWindow>
#include "spriteview.h"
#include "spritestructwindow.h"
#include "spritegallerywindow.h"
#include "tilesetwindow.h"
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;


class QAction;
class QActionGroup;
class QTreeWidget;
class QMenu;
class QTextEdit;
class QGraphicsView;
class QToolButton;
class QToolBox;
class QButtonGroup;
class QTableWidget;
class QGraphicsScene;
class QAbstractButton;

class MainSpriteFrame : public QMainWindow{
	Q_OBJECT

public:
	MainSpriteFrame( QWidget *parent = 0);
	~MainSpriteFrame();

	QGraphicsScene* scene();

	SpriteView* sceneView(){
		return editorView_;
	}

	// 加载精灵对象
	void loadSprite(ResourceHash key, const QString& title);
	bool saveCurrentSprite();

	SpriteGalleryWindow* galleryWindow(){
		return galleryWindow_;
	}

	SpriteStructWindow* structWindow(){
		return structWindow_;
	}

	TileSetWindow* tilesetWindow(){
		return tilesetWindow_;
	}

protected:
	void closeEvent(QCloseEvent *event);

public slots:
	void addSequenceItem();
	void submitCurrentSprite();
	void submitAllSprite();
	void refreshCurrentSprite();
	void renameSequenceItem();
	void removeSequenceItem();
	void deleteSelectedItem();
	void applyAnchorToSequence();

	void defaultSequenceItem();

	void errorMessage(const QString&);

	void updateSpriteName();
	void deleteSprite(unsigned int key);
private:
	// 初始化
	void init();
	// 创建菜单动作
	void createActions();
	void connectSceneEvent(QGraphicsScene* scene);
	// 创建菜单
	void createMenus();
	// 创建工具条
	void createToolBars();
	// 创建状态栏
	void createStatusBar();
	// 创建浮动窗口
	void createDockWindows();

private:
	// 初始化场景
	void initSceneView();
	// 更新编辑器修改标志
	void editorModified( bool flag );

	///////////////////////////////////////////////////////////////////////////
	// 工具条
	//////////////////////////////////////////////////////////////////////////

	QToolBar *spriteToolBar_;       // 工具条
	QActionGroup* modeGroup_;  // 编辑模式组

	//////////////////////////////////////////////////////////////////////////
	// 私有变量
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Workspace
	//////////////////////////////////////////////////////////////////////////

	SpriteView*  editorView_;

	QDockWidget *structDockWidget_;	 // 浮动条窗口
	SpriteStructWindow* structWindow_;

	QDockWidget *galleryDockWidget_;		 // 浮动条窗口
	SpriteGalleryWindow* galleryWindow_;   

	QDockWidget *tileDockWidget_;		 // 浮动条窗口
	TileSetWindow* tilesetWindow_;

public slots:
	//void addGroupItem();
	//void addCommentItem();
};

#endif // KISMET_MAINFRAME_H
