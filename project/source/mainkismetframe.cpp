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
#include <QMessageBox>
#include <QToolBar>
#include <QDockWidget>
#include <QStatusBar>

#include "mainkismetframe.h"
#include "commondefine.h"
#include "kismetscene.h"
#include "menumanager.h"
#include "kismetvalidatewindow.h"
#include "kismetundocommand.h"

#include <queue>
#include <boost/shared_ptr.hpp>

MainKismetFrame::MainKismetFrame(ProjectResourceObject* proj, QWidget *parent ) : QMainWindow(parent), 
editorView_(NULL), structWidget_(NULL), eventWidget_(NULL), instWidget_(NULL){
	init();
}

MainKismetFrame::~MainKismetFrame(){

}

void MainKismetFrame::init(){

	// 设置窗口图标和标题
	setWindowIcon( QIcon( UDQ_T(":/images/unique64.png") ) );
	setWindowTitle( UDQ_T("Orion2 Blueprint Editor") );

	// 撤销重做
	undoStack_ = new QUndoStack(this);
	undoStack_->setUndoLimit(100 );
	undoAction_ = undoStack_->createUndoAction(this, UDQ_TR("撤销"));
	undoAction_->setShortcuts(QKeySequence::Undo);
	undoAction_->setIcon(QIcon(UDQ_T(":/images/undo.png")));
	redoAction_ = undoStack_->createRedoAction(this, UDQ_TR("重做"));
	redoAction_->setShortcuts(QKeySequence::Redo);
	redoAction_->setIcon(QIcon(UDQ_T(":/images/redo.png")));

	initSceneView();

	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	createDockWindows();

}

// 初始化场景
void MainKismetFrame::initSceneView(){

	if( editorView_ == NULL ){
		editorView_ = new KismetView();
		setCentralWidget( editorView_ );
	}

	// 连接信号
	QGraphicsScene* oldScence = editorView_->scene();
	KismetScene* scene = new KismetScene();
	editorView_->setScene( scene );

	// 连接信号
	connect(scene, SIGNAL(pointerMode()),
		this, SLOT(pointerMode()));

	connect(scene, SIGNAL(entityModified(QGraphicsItem*, const QByteArray&)), this, SLOT(entityModified(QGraphicsItem*, const QByteArray&)));
	connect(scene, SIGNAL(entityInserted(QGraphicsItem*)), this, SLOT(entityInserted(QGraphicsItem*)));
	connect(scene, SIGNAL(entityDeleted(const QList<QGraphicsItem*>&)), this, SLOT(entityDeleted(const QList<QGraphicsItem*>&)));
	connect(scene, SIGNAL(entityMoved(const QList<QGraphicsItem*>&)), this, SLOT(entityMoved(const QList<QGraphicsItem*>&)));
	connect(scene, SIGNAL(entityResized(QGraphicsItem*)), this, SLOT(entityResized(QGraphicsItem*)));
	connect(scene, SIGNAL(entityCopy(const QList<QGraphicsItem*>&, const QPointF&)), this, SLOT(entityCopy(const QList<QGraphicsItem*>&, const QPointF&)));

	// 释放老地图
	if( oldScence != 0 ){
		delete oldScence;
	}
}


void MainKismetFrame::createActions(){

	gMenuManager->createActions( EDITOR_KISMET, this );

	BEGIN_ACTION();
	//////////////////////////////////////////////////////////////////////////
	// Scenario
	//////////////////////////////////////////////////////////////////////////
	CONNECT_ACTION(ORION_KISMET_SAVE, this, saveBlueprint);
	CONNECT_ACTION(ORION_KISMET_CLEAR, this, clearBlueprint);
	CONNECT_ACTION(ORION_KISMET_VALIDATE, this, checkBlueprint);

	CONNECT_ACTION(ORION_KISMET_POINTER, this, pointerMode);
	CONNECT_ACTION(ORION_KISMET_ADDGROUP, this, addGroupItem);
	CONNECT_ACTION(ORION_KISMET_ADDCOMMENT, this, addCommentItem);

	modeGroup_ = new QActionGroup(this);
	modeGroup_->addAction(gMenuManager->getAction(ORION_KISMET_POINTER));
	modeGroup_->addAction(gMenuManager->getAction(ORION_KISMET_ADDGROUP));
	modeGroup_->addAction(gMenuManager->getAction(ORION_KISMET_ADDCOMMENT));
	gMenuManager->getAction(ORION_KISMET_POINTER)->setChecked(true);

	CONNECT_ACTION(ORION_KISMET_ADDBRANCH, this, addBranchItem);
	CONNECT_ACTION(ORION_KISMET_ADDMATH, this, addMathItem);
	CONNECT_ACTION(ORION_KISMET_ADDSEQUENCE, this, addSequenceItem);


	//////////////////////////////////////////////////////////////////////////
	// View
	//////////////////////////////////////////////////////////////////////////
	END_ACTION();

	QGraphicsScene* scene = editorView_->scene();
	Q_ASSERT( scene != NULL );
	connectSceneEvent( scene );

}

void MainKismetFrame::connectSceneEvent(QGraphicsScene* scene){
	BEGIN_ACTION();
	CONNECT_ACTION(ORION_KISMET_ADDINSTANCEVAR, scene, addInstanceVariable);
	CONNECT_ACTION(ORION_KISMET_ADDINSTANCEINDEX, scene, addInstanceIndex);
	CONNECT_ACTION(ORION_KISMET_DELETEPORT, scene, deleteModelPort);
	CONNECT_ACTION(ORION_KISMET_CLEARPORTLINK, scene, clearModelPortLink);
	END_ACTION();
}

void MainKismetFrame::createMenus(){

}



void MainKismetFrame::createToolBars(){

	//////////////////////////////////////////////////////////////////////////
	// Model
	//////////////////////////////////////////////////////////////////////////
	editorToolBar_ = addToolBar(UDQ_TR("Blueprint"));
	editorToolBar_->addAction(gMenuManager->getAction(ORION_KISMET_SAVE));
	editorToolBar_->addSeparator();
	editorToolBar_->addAction(gMenuManager->getAction(ORION_KISMET_CLEAR));
	editorToolBar_->addAction(gMenuManager->getAction(ORION_KISMET_VALIDATE));
	editorToolBar_->addSeparator();
	editorToolBar_->addAction(undoAction_);
	editorToolBar_->addAction(redoAction_);
	editorToolBar_->addSeparator();
	editorToolBar_->addAction(gMenuManager->getAction(ORION_KISMET_POINTER));
	editorToolBar_->addSeparator();
	editorToolBar_->addAction(gMenuManager->getAction(ORION_KISMET_ADDGROUP));
	editorToolBar_->addAction(gMenuManager->getAction(ORION_KISMET_ADDCOMMENT));
	editorToolBar_->addSeparator();
	editorToolBar_->addAction(gMenuManager->getAction(ORION_KISMET_ADDBRANCH));
	editorToolBar_->addAction(gMenuManager->getAction(ORION_KISMET_ADDSEQUENCE));
	editorToolBar_->addSeparator();
	editorToolBar_->addAction(gMenuManager->getAction(ORION_KISMET_ADDMATH));

}

void MainKismetFrame::createStatusBar(){
	statusBar()->showMessage(UDQ_TR("就绪"));
}

void MainKismetFrame::createDockWindows(){

	structDockWidget_ = new QDockWidget(this);
	structDockWidget_->setAllowedAreas( Qt::LeftDockWidgetArea );
	structDockWidget_->setFeatures( QDockWidget::NoDockWidgetFeatures );
	addDockWidget(Qt::LeftDockWidgetArea, structDockWidget_);
	structWidget_ = new KismetStructWindow( structDockWidget_ );
	structDockWidget_->setWidget(structWidget_);


	instanceDockWidget_ = new QDockWidget(this);
	instanceDockWidget_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	instanceDockWidget_->setFeatures(/*QDockWidget::NoDockWidgetFeatures | */
		QDockWidget::DockWidgetMovable);
	addDockWidget(Qt::LeftDockWidgetArea, instanceDockWidget_);
	instWidget_ = new KismetInstanceWindow(instanceDockWidget_);
	instanceDockWidget_->setWidget(instWidget_);

	eventDockWidget_ = new QDockWidget(this);
	eventDockWidget_->setAllowedAreas(Qt::RightDockWidgetArea);
	eventDockWidget_->setFeatures(QDockWidget::NoDockWidgetFeatures);
	addDockWidget(Qt::RightDockWidgetArea, eventDockWidget_);
	eventWidget_ = new KismetEventWindow(eventDockWidget_);
	eventDockWidget_->setWidget(eventWidget_);

}

void MainKismetFrame::closeEvent(QCloseEvent *event){

}

void MainKismetFrame::showEvent(QShowEvent * event){

}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void MainKismetFrame::clearBlueprint(){

	// 对话框提示
	if (QMessageBox::question(this, UDQ_TR("事件脚本"), UDQ_TR("确认清空当前事件脚本？"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
		return;

	KismetScene* scene = (KismetScene*)editorView_->scene();
	BlueprintResourceObject* blueprint = scene->parentModelInstance();

	scene->clearScene();

	if (blueprint){
		blueprint->clearAll();
		instWidget_->init(blueprint);
	}


	//if( structWidget_->currentModel() == NULL ){
	//	structWidget_->clearModel();
	//	return;
	//}

	// // 如果是顶层模型,checkout当前态势
	//if( UIsExactKindOf(  ScenarioResourceObject,  scenarioWidget_->currentModel()->associateInstance() ) ){
	//	checkoutScene(  scenarioWidget_->currentModel() );
	//	scenarioWidget_->clearModel();
	//}
	//else{
	//	// 清空当前实例模型
	//	DevsScene* devsscene = (DevsScene*)scene();
	//	devsscene->clearScene();

	//	// 删除模型资源
	//	scenarioWidget_->clearEntityModel();
	//}

}

void MainKismetFrame::checkBlueprint(){

	KismetScene* scene = (KismetScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);

	if (!scene->validateScript()){
		// 提示，脚本有误
		KismetValidateWindow w(scene->scriptErrors());
		w.exec();
	}

}

void MainKismetFrame::saveBlueprint(){

	KismetScene* scene = (KismetScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);
	// 保存脚本
	if (!scene->saveCurrentScript()){
		// 提示，脚本有误
		return;
	}

	// 保存到文件
	WorkspaceResourceObject*  ws = ResourceObject::currentWorkspace();
	Q_ASSERT(ws != NULL);
	ProjectResourceObject* proj = ws->currentProject();
	if (proj){
		proj->setFlags(URF_TagSave);
		if (!(proj->flags() & URF_TagGarbage) && proj->getLinker() == 0){
			// 设置Linker
			QString projFile = proj->getOutermost()->objectName() + UDQ_T(".") + proj->fileType();
			ResourceLinker* linker = ResourceObject::getWorkspaceLinker(proj->getOutermost(), projFile);
			Q_ASSERT(linker != 0);
			proj->setLinker(linker);
		}
		SAVE_WORKSPACE_RESOURCE(ProjectResourceObject, proj);
	}

}

void MainKismetFrame::refreshBlueprint(){
	KismetScene* scene = (KismetScene*)editorView_->scene();
	scene->updateScriptText();

}

void MainKismetFrame::pointerMode(){
	gMenuManager->getAction(ORION_KISMET_POINTER)->setChecked(true);
	editorView_->setDragMode(QGraphicsView::ScrollHandDrag);

	KismetScene* devsscene = (KismetScene*)editorView_->scene();
	devsscene->exitInsertState();
}


void MainKismetFrame::addGroupItem(){

	editorView_->setDragMode(QGraphicsView::NoDrag);

	KismetScene* scene = (KismetScene*)editorView_->scene();
	scene->enterInsertGroup();
}

void MainKismetFrame::addCommentItem(){
	editorView_->setDragMode(QGraphicsView::NoDrag);

	KismetScene* scene = (KismetScene*)editorView_->scene();
	scene->enterInsertComment();
}

void MainKismetFrame::addBranchItem(){

	KismetScene* scene = (KismetScene*)editorView_->scene();
	if (scene->parentModelInstance() == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请先打开一个脚本"));
		return;
	}
	scene->addBranchItem();

}

void MainKismetFrame::addMathItem(){

	KismetScene* scene = (KismetScene*)editorView_->scene();
	if (scene->parentModelInstance() == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请先打开一个脚本"));
		return;
	}
	scene->addMathItem();
}


void MainKismetFrame::addSequenceItem(){

	KismetScene* scene = (KismetScene*)editorView_->scene();
	if (scene->parentModelInstance() == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请先打开一个脚本"));
		return;
	}
	BlueprintObject* obj = scene->addSequenceItem();

}

void MainKismetFrame::clearScript(){
	KismetScene* scene = (KismetScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);

	structWidget_->init(NULL);
	instWidget_->init(NULL);  // 变量列表
	scene->loadScript(NULL);

}


void MainKismetFrame::refreshScript(){
	KismetScene* scene = (KismetScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);
	scene->refreshScript();
}

void MainKismetFrame::loadScript(BlueprintResourceObject* script){

	KismetScene* scene = (KismetScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);

	// 保存上一个脚本
	if (!scene->saveCurrentScript()){
		//  保存失败，停止加载
		return;
	}

	// 清空undo缓存
	undoStack_->clear();

	if (script == NULL){
		QMessageBox::critical(this, windowTitle(), UDQ_TR("当前选择的脚本数据没有找到!"));
		return;
	}
	instWidget_->init(script);  // 变量列表

	//  加载新精灵
	scene->loadScript(script);
	scene->update();
}

void MainKismetFrame::updateVariableType(ScriptVariable* var){
	KismetScene* scene = (KismetScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);

	scene->updateVariableItem(var);
}

void MainKismetFrame::removeVariable(ScriptVariable* var){
	KismetScene* scene = (KismetScene*)editorView_->scene();
	Q_ASSERT(scene != NULL);

	scene->removeVariable(var->name);
}


void MainKismetFrame::entityModified(QGraphicsItem* item, const QByteArray& data){
	KismetScene* scene = (KismetScene*)sceneView()->scene();
	if (item->type() == KISMET_LINK ||
		item->type() == KISMET_LINK_SINGLE ||
		item->type() == KISMET_LINK_ARRAY){

		QDataStream stream(data);
		QList<QPointF> pts;
		stream >> pts;

		ModifyBluePrintLinkCommand* command = new ModifyBluePrintLinkCommand((BlueprintLink*)item, pts, scene);
		undoStack_->push(command);
		command->setEnable(true);
	}
	else{
		AddBluePrintModelCommand* command = new AddBluePrintModelCommand((BlueprintObject*)item, scene);
		undoStack_->push(command);
		command->setEnable(true);
	}
}

void MainKismetFrame::entityInserted(QGraphicsItem* item){
	KismetScene* scene = (KismetScene*)sceneView()->scene();
	if (item->type() == KISMET_LINK ||
		item->type() == KISMET_LINK_SINGLE ||
		item->type() == KISMET_LINK_ARRAY){
		AddBluePrintLinkCommand* command = new AddBluePrintLinkCommand((BlueprintLink*)item, scene);
		undoStack_->push(command);
		command->setEnable(true);
	}
	else if (item->type() == KISMET_MODEL_PORT){
		AddBluePrintModelPortCommand* command = new AddBluePrintModelPortCommand((BlueprintModelPort*)item, scene);
		undoStack_->push(command);
		command->setEnable(true);

		qDebug() << "AddPort";
	}
	else{
		AddBluePrintModelCommand* command = new AddBluePrintModelCommand((BlueprintObject*)item, scene);
		undoStack_->push(command);
		command->setEnable(true);
	}
}


void MainKismetFrame::entityResized(QGraphicsItem* item){
	KismetScene* scene = (KismetScene*)sceneView()->scene();

	if (item->type() == KISMET_GROUP){
		BlueprintGroup* grp = (BlueprintGroup*)item;
		QByteArray data;
		{
			QDataStream stream(&data, QIODevice::WriteOnly | QIODevice::Truncate);
			grp->copyState(stream, BlueprintObject::BST_MOVE);
		}

		ResizeBluePrintGroupCommand* command = new ResizeBluePrintGroupCommand(grp, data, scene);
		undoStack_->push(command);
		command->setEnable(true);

		qDebug() << "ResiceGroup";
	}
	else{
		Q_UNREACHABLE();
	}
}

void MainKismetFrame::entityDeleted(const QList<QGraphicsItem*>& items){
	KismetScene* scene = (KismetScene*)sceneView()->scene();

	QList<BlueprintLink*> links;
	QList<BlueprintObject*> objs;
	QList<BlueprintModelPort*> ports;
	foreach(QGraphicsItem* item, items){
		if (item->type() == KISMET_GROUP
			|| item->type() == KISMET_COMMENT
			|| item->type() == KISMET_MODEL
			|| item->type() == KISMET_INSTANCE
			|| item->type() == KISMET_FLOW_BRANCH
			|| item->type() == KISMET_ARITHMETIC
			|| item->type() == KISMET_VARIABLE
			|| item->type() == KISMET_FLOW_SEQUENCE
			|| item->type() == KISMET_COLLECTION
			|| item->type() == KISMET_INSTANCE_VARIABLE){
			objs.append((BlueprintObject*)item);
		}
		else if (item->type() == KISMET_LINK ||
			item->type() == KISMET_LINK_SINGLE ||
			item->type() == KISMET_LINK_ARRAY){
			links.append((BlueprintLink*)item);
		}
		else if (item->type() == KISMET_MODEL_PORT){
			ports.append((BlueprintModelPort*)item);
		}
	}

	if (links.size() == 0 && objs.size() == 0 && ports.size() == 0 ){
		return;
	}

	// 分开处理
	QUndoCommand* parentcmd = new QUndoCommand();
	if (links.size() > 0){
		DeleteBluePrintLinkCommand* linkcmd = new DeleteBluePrintLinkCommand(links, scene, parentcmd);
	}
	foreach(BlueprintObject* obj, objs){
		DeleteBluePrintModelCommand* cmd = new DeleteBluePrintModelCommand(obj, scene,parentcmd);
	}
	foreach(BlueprintModelPort* p, ports){
		DeleteBluePrintModelPortCommand* cmd = new DeleteBluePrintModelPortCommand(p, scene, parentcmd);
	}
	undoStack_->push(parentcmd);
}


void MainKismetFrame::entityMoved(const QList<QGraphicsItem*>& items)
{
	KismetScene* scene = (KismetScene*)sceneView()->scene();

	BlueprintObject* obj = NULL;
	QList<BlueprintObject*> objs;
	foreach(QGraphicsItem* item, items){
		if (item->type() == KISMET_GROUP
			|| item->type() == KISMET_COMMENT
			|| item->type() == KISMET_MODEL
			|| item->type() == KISMET_INSTANCE
			|| item->type() == KISMET_FLOW_BRANCH
			|| item->type() == KISMET_ARITHMETIC
			|| item->type() == KISMET_VARIABLE
			|| item->type() == KISMET_FLOW_SEQUENCE
			|| item->type() == KISMET_COLLECTION
			|| item->type() == KISMET_INSTANCE_VARIABLE){

			obj = (BlueprintObject*)item;
			if (obj->isStateChanged(BlueprintObject::BST_MOVE)){
				objs.append(obj);
			}
		}
	}

	if (objs.size() > 0){
		QUndoCommand* parentcmd = new QUndoCommand();
		QByteArray data;
		QList<MoveBluePrintModelCommand*> movecmds;
		foreach(BlueprintObject* child, objs){
			{
				QDataStream stream(&data, QIODevice::WriteOnly | QIODevice::Truncate);
				child->copyState(stream, BlueprintObject::BST_MOVE);
			}
			MoveBluePrintModelCommand* cmd = new MoveBluePrintModelCommand(child, data, scene, parentcmd);
			movecmds.append(cmd);
		}

		if (parentcmd->childCount() == 0){
			delete parentcmd;
			return;
		}

		undoStack_->push(parentcmd);
		foreach(MoveBluePrintModelCommand* cmd, movecmds){
			cmd->setEnable(true);
		}

		qDebug() << "Move Command";
	}
}

void MainKismetFrame::entityCopy(const QList<QGraphicsItem*>& items, const QPointF& pos){
	KismetScene* scene = (KismetScene*)sceneView()->scene();


	BlueprintObject* obj = NULL;
	QList<BlueprintObject*> objs;
	QSet<BlueprintLink*> links;
	foreach(QGraphicsItem* item, items){

		BlueprintObject* obj = (BlueprintObject*)item;
		Q_ASSERT(obj != NULL);

		if (item->type() == KISMET_MODEL ||
			item->type() == KISMET_ARITHMETIC ||
			item->type() == KISMET_FLOW_BRANCH ||
			item->type() == KISMET_FLOW_SEQUENCE ||
			item->type() == KISMET_INSTANCE_VARIABLE){
			// 记录所有端口连线
			BlueprintModel* model = (BlueprintModel*)obj;
			foreach(BlueprintModelPort* port, model->ports()){
				foreach(BlueprintLink* link, port->links()){
					links.insert(link);
				}
			}
		}
		else if (item->type() == KISMET_INSTANCE ||
			item->type() == KISMET_VARIABLE ||
			item->type() == KISMET_COLLECTION){
			//  记录端口连线
			foreach(BlueprintLink* link, obj->links()){
				links.insert(link);
			}
		}
		else if (item->type() == KISMET_GROUP ||
			item->type() == KISMET_COMMENT){
		}
		else{
			continue;
		}
		objs.append(obj);
	}

	// 清空寻找
	scene->clearSelection();

	if (objs.size() > 0){

		QPointF deltaPos = pos - objs[0]->scenePos();

		QMap<ResourceHash, ResourceHash> iidMaps;
		QUndoCommand* parentcmd = new QUndoCommand();
		foreach(BlueprintObject* child, objs){
			CopyBluePrintModelCommand* cmd = new CopyBluePrintModelCommand(child, deltaPos, scene, parentcmd);
			iidMaps.insert(child->iid(), cmd->newIID());
		}

		// 添加连线
		QByteArray data;
		QByteArray portdata[2];
		QList<QPointF> ctrlpoints;
		BlueprintObject* source = NULL;
		BlueprintObject* dest = NULL;

		foreach(BlueprintLink* link, links){
			//  必须是内部连线
			source = link->sourceNode();
			if (source->type() == KISMET_MODEL_PORT ){
				source = (BlueprintObject*)source->parentItem();
			}

			dest = link->destNode();
			if (dest->type() == KISMET_MODEL_PORT){
				dest = (BlueprintObject*)dest->parentItem();
			}

			if (!objs.contains(source) || !objs.contains(dest)){
				continue;
			}

			{
				QDataStream stream(&data, QIODevice::WriteOnly | QIODevice::Truncate);
				link->encode(stream);
			}
			// 替换端口IID
			{
				QDataStream stream(&data, QIODevice::ReadWrite);
				stream >> portdata[0] >> portdata[1]>>ctrlpoints;  // 源端口

				for (int i = 0; i < 2; i++)
				{
					QDataStream substream(&portdata[i], QIODevice::ReadWrite);
					int type;
					substream >> type;
					qint64 pos = substream.device()->pos();

					ResourceHash iid;
					substream >> iid;

					Q_ASSERT(iidMaps.contains(iid));
					substream.device()->seek(pos);
					substream << iidMaps[iid];
				}

				for (int i = 0; i < ctrlpoints.size(); i++){
					ctrlpoints[i] += deltaPos;
				}

				stream.device()->seek(0);
				stream << portdata[0] << portdata[1]<<ctrlpoints;
			}

			AddBluePrintLinkCommand* cmd = new AddBluePrintLinkCommand(data, scene, parentcmd);

		}

		if (parentcmd->childCount() == 0){
			delete parentcmd;
			return;
		}

		undoStack_->push(parentcmd);
		qDebug() << "Copy Command";
	}
}

