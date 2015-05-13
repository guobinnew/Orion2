/************************************************************************
**
**  Copyright (C) 2015  guobin  <guobin.gb@alibaba-inc.com>
**
**  This file is part of Orion2.
**
**  Orion is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation.
**
**  Orion2 is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion2.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#ifndef ORION_MAINFRAME_MANAGER_H
#define ORION_MAINFRAME_MANAGER_H

#include <QMainWindow>
#include <QUndoStack>
#include "levelview.h"
#include "welcomewindow.h"
#include "gallerycategorywindow.h"
#include "workspacewindow.h"
#include "layerwindow.h"
#include "tilebrushwindow.h"
#include "entitycomponentwindow.h"
#include "mainspriteframe.h"
#include "mainkismetframe.h"
#include "objectpropertywindow.h"

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
class QListWidget;
class QTextEdit;


class MainManagerFrame : public QMainWindow{
	Q_OBJECT

public:
    MainManagerFrame( QWidget *parent = 0);
	~MainManagerFrame();

	LevelView* sceneView(){
		return levelView_;
	}

	QUndoStack* undoStack(){
		return undoStack_;
	}

	WorkspaceWindow* workspaceWindow(){
		return workspaceWindow_;
	}

	ObjectPropertyWindow* propertyWindow(){
		return propertyWindow_;
	}


	// 清空编辑器
	void clearScene();
	void clearKismet();

	// 切换场景
	void switchLevel(ResourceHash key);
	// 删除场景
	void removeLevel(ResourceHash key);
	// 新建图层
	void addLayerToScene(ResourceHash key);
	// 删除图层
	void removeLayerFromScene(ResourceHash key);
	// 刷新图层次序
	void reorderLayers();

public slots:
    void exitApplication();
	void saveCurrentModel();
	void closeCurrentWorkspace();
	void publishProject();

	// 素材库管理
	void galleryManage();
	void spriteManage();
	void pluginManage();
	void kismetManage();

	// 工具
	void previewProject();
	void debugProject();
	void debugSetup();

	// 关于
	void about();
	void welcome();
	// 加载项目
	void prepareProject();


	// 更新图层
	void updateLayer(unsigned int key);
	// 更新图层
	void activeLayer(unsigned int key);


	// 刷新图层显示
	void refreshLayers();
	void refreshCurrentLayer();
	void refreshRegions( ObjectTypeResourceObject* obj);
	// 添加新对象类型
	void addObjectType(unsigned int hashkey);
	void refreshObjectType(unsigned int hashkey);
	// 更新TileMap界面
	void refreshTileMap(QGraphicsItem* entity);
	// 刷新对象类型
	void refreshObjectTypes();
	// 删除对象类型
	void deleteObjectType(unsigned int hashkey);
	// 删除集合
	void deleteFamily(unsigned int hashkey);

	// 更新属性
	void updateProperty(QTreeWidgetItem* item);
	void refreshProperty(QTreeWidgetItem* item);
	void refreshProperty(EditorItem* item);

	void itemChanged(QGraphicsItem* item);
	void selectionChanged();

	// undo
	void entityModified(QGraphicsItem* item, const QByteArray& data);
	void entityInserted(QGraphicsItem* item, unsigned int hashkey);
	void entityDeleted(QGraphicsItem* item, unsigned int hashkey);

private:
	// 初始化
	void init();
	// 创建菜单动作
	void createActions();
	void connectSceneEvent(QGraphicsScene* scene);
	void connectWorkspaceEvent(QWidget* widget);
	// 创建菜单
	void createMenus();
	// 创建工具条
	void createToolBars();
	// 创建状态栏
	void createStatusBar();
	// 创建浮动窗口
	void createDockWindows();
	// 创建工具条
	void createToolBox();

protected:
	virtual void	closeEvent ( QCloseEvent * event );
	virtual void	resizeEvent(QResizeEvent * event);

private:
	// 初始化场景
	void initView();

	QMenu   *modelMenu_;   // 工程菜单
	QMenu   *editMenu_;    // 编辑菜单
	QMenu   *galleryMenu_;    // 资源菜单
	QMenu  *debugMenu_;  // 调试菜单
	QMenu  *windowMenu_;  // 窗口菜单
    QMenu   *helpMenu_;    // 帮助菜单
	
	QToolBar *modelToolBar_;    // 模型工具条
	QToolBar *editToolBar_;    // 模型工具条
	QToolBar *debugToolBar_;    // 调试工具条

	QAction* undoAction_;
	QAction* redoAction_;

    QDockWidget *workspaceDockWidget_;	//工作空间浮动条窗口
	WorkspaceWindow* workspaceWindow_; 

	QDockWidget *tileDockWidget_;		//图层浮动条窗口
	LayerWindow* layerWindow_; // TileMap窗口
	
	QDockWidget *galleryDockWidget_;		// 素材库浮动条窗口
	GalleryCategoryWindow* categoryWindow_;

	QDockWidget *componentDockWidget_;		// 组件窗口
	EntityComponentWindow* componentWindow_;

	QDockWidget *propertyDockWidget_;		// 属性窗口
	ObjectPropertyWindow* propertyWindow_;


	// 初始化工作空间
	void initWorkspace();

	LevelView* levelView_;  // 场景视图

	WelcomeWindow* welcomeWindow_; // 欢迎窗口

	MainSpriteFrame* spriteEditor_;  // 精灵编辑器
	MainKismetFrame* kismetEditor_;  // 脚本编辑器

	QUndoStack *undoStack_;    // 撤销重做 

	QTimer* saveTimer_;  // 定时保存
};

#endif // RMS_MAINFRAME_H
