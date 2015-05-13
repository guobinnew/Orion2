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

#ifndef ORION_KISTMET_EDITOR_H
#define ORION_KISTMET_EDITOR_H

#include <QMainWindow>
#include <QUndoStack>
#include "kismetview.h"
#include "kismetstructwindow.h"
#include "kismetinstancewindow.h"
#include "kismeteventwindow.h"
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

class MainKismetFrame : public QMainWindow{
	Q_OBJECT

public:
	MainKismetFrame(ProjectResourceObject* proj = 0, QWidget *parent = 0);
	~MainKismetFrame();

	 KismetView* sceneView(){
		return editorView_;
	}

	 KismetInstanceWindow* instanceWidget(){
		 return instWidget_;
	}

	KismetStructWindow* structWidget(){
		return structWidget_;
	}

	KismetEventWindow* eventWidget(){
		return eventWidget_;
	}

	QUndoStack* undoStack(){
		return undoStack_;
	}

	// 加载
	void loadScript(BlueprintResourceObject* script);
	void clearScript();
	//  刷新当前打开的脚本
	void refreshScript();
	// 更新变量类型
	void updateVariableType(ScriptVariable* var);
	void removeVariable(ScriptVariable* var);

protected:
	void closeEvent(QCloseEvent *event);
	void showEvent(QShowEvent * event);

public slots:
	void pointerMode();
	void addGroupItem();
	void addCommentItem();

	void addBranchItem();
	void addMathItem();
	void addSequenceItem();

	//////////////////////////////////////////////////////////////////////////
	// 菜单消息
	//////////////////////////////////////////////////////////////////////////
	void refreshBlueprint();
	void saveBlueprint();
	void clearBlueprint();
	void checkBlueprint();

	//////////////////////////////////////////////////////////////////////////
	// undo
	//////////////////////////////////////////////////////////////////////////
	void entityModified(QGraphicsItem* item, const QByteArray& data);
	void entityInserted(QGraphicsItem* item);
	void entityDeleted(const QList<QGraphicsItem*>& items);
	void entityMoved(const QList<QGraphicsItem*>& items);
	void entityResized(QGraphicsItem* item);
	void entityCopy(const QList<QGraphicsItem*>& items, const QPointF& pos);
	

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
	// 检查模型有效性
	bool checkValid( QStringList& error, QStringList& warning );

	///////////////////////////////////////////////////////////////////////////
	// 工具条
	//////////////////////////////////////////////////////////////////////////
	QToolBar *editorToolBar_;       // 工具条
	QToolBar *undoToolBar_;       // Undo

	QActionGroup* modeGroup_;  // 编辑模式组

	//////////////////////////////////////////////////////////////////////////
	// 私有变量
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Workspace
	//////////////////////////////////////////////////////////////////////////

	KismetView*  editorView_;

	QDockWidget *structDockWidget_;	 // 浮动条窗口
	QDockWidget *instanceDockWidget_;		 // 浮动条窗口
	QDockWidget *eventDockWidget_;		 // 浮动条窗口

	KismetStructWindow* structWidget_;  // 态势结构图
	KismetInstanceWindow* instWidget_;
	KismetEventWindow* eventWidget_;

	QUndoStack *undoStack_;    // 撤销重做 
	QAction* undoAction_;
	QAction* redoAction_;

};

#endif // KISMET_MAINFRAME_H
