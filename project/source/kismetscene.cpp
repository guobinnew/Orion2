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
**  along with Orion2.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/
#include <QtGui>
#include <QMenu>
#include <QMessageBox>
#include "kismetscene.h"
#include <math.h>
#include "commondefine.h"
#include "kismetdefine.h"
#include "menumanager.h"
#include "configmanager.h"

#include "leveldefine.h"
#include "kismetmodel.h"
#include "kismetmodelport.h"
#include "kismetlinkarray.h"
#include "kismetlinksingle.h"
#include "configmanager.h"
#include "kismetview.h"
#include "mainkismetframe.h"
#include "screenshotwindow.h"
#include "kismetvalidatewindow.h"

using namespace ORION;

const int SCENE_SIZE = 25600;

KismetScene::KismetScene(QObject *parent) : QGraphicsScene(parent){

	setSceneRect(0, 0, SCENE_SIZE, SCENE_SIZE);

	// 边框
	QGraphicsRectItem* borderItem = new QGraphicsRectItem(QRect(0, 0, SCENE_SIZE, SCENE_SIZE));
	borderItem->setPos(0,0);
	borderItem->setVisible( true );
	addItem( borderItem );

	scriptText_ = new QGraphicsSimpleTextItem();
	scriptText_->setVisible(false);
	scriptText_->setZValue(EDITOR_DEPTH_LOGO);
	scriptText_->setFlags(QGraphicsItem::ItemIgnoresTransformations);
	scriptText_->setBrush(Qt::blue);
	QFont font(UDQ_TR("微软雅黑"), 24);
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPixelSize(24);
	font.setBold(true);
	scriptText_->setFont(font);
	addItem(scriptText_);

	menuHostItem_ = NULL;
	mapMode_ = DOM_POINTER;
	parentInstance_ = NULL;
	bgColor_ = Qt::lightGray;

}

//////////////////////////////////////////////////////////////////////////

KismetScene::~KismetScene(){

}

// 设置模式
void KismetScene::setMode(KISMET_MODE mode){

	mapMode_  = mode;
	if( mapMode_ == DOM_POINTER ){
		clearSelection();
	}
}


void KismetScene::drawBackground(QPainter * painter, const QRectF & rect){
	// 绘制底色
	painter->fillRect( rect, bgColor_ );

	// 确定尺寸和位置
	QSize viewRect = views().at(0)->maximumViewportSize();

	// 修改文字
	if (scriptText_ && scriptText_->isVisible()){
		QFontMetricsF fm(scriptText_->font());
		qreal pixelsWide = fm.width(scriptText_->text());
		qreal pixelsHigh = fm.height();
		scriptText_->setPos(views().at(0)->mapToScene((viewRect.width() - pixelsWide) / 2.0, pixelsHigh + 4));
	}

}
// 获取可见区域（场景坐标系）
QRectF KismetScene::viewRegionRect(void){

	// 获取当前缩放比例
	QSize viewRect = views().at(0)->maximumViewportSize();

	QPointF viewLTPos = views().at(0)->mapToScene( 0, 0 );
	if( viewLTPos.rx() < 0 )
		viewLTPos.rx() = 0;
	if( viewLTPos.ry() < 0 )
		viewLTPos.ry() = 0;

	QRectF sRect = sceneRect();

	QPointF viewRBPos = views().at(0)->mapToScene( viewRect.width(), viewRect.height() );
	if( viewRBPos.rx() > sRect.width() )
		viewRBPos.rx() = sRect.width();
	if( viewRBPos.ry() > sRect.height() )
		viewRBPos.ry() = sRect.height();

	return QRectF(viewLTPos, viewRBPos );
}

//////////////////////////////////////////////////////////////////////////
// 拖放处理
//////////////////////////////////////////////////////////////////////////

void KismetScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event){
	clearSelection();

	if (event->mimeData()->hasFormat(MIME_KISMET_EVENT) ||
		event->mimeData()->hasFormat(MIME_KISMET_VARIABLE) ||
		event->mimeData()->hasFormat(MIME_ORION_OBJECTTYPE) ||
		event->mimeData()->hasFormat(MIME_ORION_FAMILY) ){
		event->accept();
	}
	else{
		event->ignore();
	}
}

void KismetScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event){
	event->accept();
}

void KismetScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event){
	event->accept();
}

void KismetScene::dropEvent(QGraphicsSceneDragDropEvent *event){
	setMode( DOM_POINTER );
	gMenuManager->getAction(ORION_KISMET_POINTER)->setChecked(true);

	// 
	WorkspaceResourceObject*  ws= ResourceObject::currentWorkspace();
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != NULL);

	if (parentInstance_ == NULL){
		QMessageBox::warning(views().at(0), UDQ_TR("添加对象"), UDQ_TR("请先打开一个脚本模型"));
		event->ignore();
		return;
	}

	BlueprintObject* obj = NULL;

	if (event->mimeData()->hasFormat(MIME_KISMET_EVENT)) {

		QByteArray itemData = event->mimeData()->data(MIME_KISMET_EVENT);
		obj = addEventItem(event->scenePos(), itemData);
		event->accept();
	} 
	else if (event->mimeData()->hasFormat(MIME_ORION_OBJECTTYPE)){
		QByteArray itemData = event->mimeData()->data(MIME_ORION_OBJECTTYPE);
		ResourceHash key;
		{
			QDataStream stream(itemData);
			stream >> key;
		}
		obj = addInstanceItem(event->scenePos(), key);
		event->accept();
	}
	else if (event->mimeData()->hasFormat(MIME_ORION_FAMILY)){
		QByteArray itemData = event->mimeData()->data(MIME_ORION_FAMILY);
		ResourceHash key;
		{
			QDataStream stream(itemData);
			stream >> key;
		}
		obj = addCollectionItem(event->scenePos(), key);
		event->accept();
	}
	else if (event->mimeData()->hasFormat(MIME_KISMET_VARIABLE)){
		QByteArray itemData = event->mimeData()->data(MIME_KISMET_VARIABLE);
		ResourceHash key;
		QString name;
		{
			QDataStream stream(itemData);
			stream >> key >> name;
		}
		obj = addVariableItem(event->scenePos(), key, name);
		event->accept();
	}
	else {
		event->ignore();
	}

	if (obj){
		emit entityInserted(obj);
	}
}

//////////////////////////////////////////////////////////////////////////

BlueprintInstance*  KismetScene::addInstanceItem(const QPointF& pos, ResourceHash key){
	BlueprintInstance* inst = new BlueprintInstance(key);
	inst->setPos(pos);
	addObjectItem(inst);
	return inst;
}


BlueprintCollection* KismetScene::addCollectionItem(const QPointF& pos, ResourceHash key){
	BlueprintCollection* col = new BlueprintCollection(key);
	col->setPos(pos);
	addObjectItem(col);
	return col;
}


//////////////////////////////////////////////////////////////////////////
QString KismetScene::newGroupName(){
	const QString baseName = UDQ_TR("事件组");
	QString newTitle = baseName;
	int index = 1;
	bool found = true;
	while (found){

		found = false;
		foreach(BlueprintGroup* grp, grpObjects_){
			if (grp->title() == newTitle){
				found = true;
				break;
			}
		}

		if (!found){
			break;
		}

		newTitle = baseName + QString::number(index++);
	}

	return newTitle;
	
}

void KismetScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event){
	QGraphicsScene::contextMenuEvent(event);
}

void KismetScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent){
	QGraphicsScene::mouseDoubleClickEvent(mouseEvent);

	if (!mouseEvent->isAccepted() && mapMode_ == DOM_POINTER){
		selectObjectType(mouseEvent->scenePos());
	}
}

void KismetScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent){
	if( mouseEvent->button() == Qt::LeftButton ){

		if (parentInstance_ == NULL){
			emit pointerMode();
			return;
		}

		BlueprintObject* obj = NULL;
		switch( mapMode_ ){
		case DOM_ADDGROUP:
			{
				QColor color = Qt::gray;
				color.setAlpha(0);

				// 生成新组名
				obj = addGroupItem(newGroupName(), mouseEvent->scenePos());
				emit entityInserted(obj);
			}
			break;
		case DOM_ADDCOMMENT:
			{
				obj = addCommentItem(UDQ_TR("请填写注释"), mouseEvent->scenePos());
				emit entityInserted(obj);
			}
			break;
		default:
		{
			// 记录状态
			QList<QGraphicsItem*> items = selectedItems();
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
					objs.append((BlueprintObject*)item);
				}
			}

			foreach(BlueprintObject* obj, objs){
				// 记录当前状态
				obj->backupState();
			}


			if (objs.size() > 0 && mouseEvent->modifiers() & Qt::ShiftModifier){
				//  复制所有选中的对象
				emit entityCopy(items, mouseEvent->scenePos());
			}
		}
		}
	}
	else if( mouseEvent->button() == Qt::RightButton ){
		switch( mapMode_ ){
		case DOM_ADDGROUP:
		case DOM_ADDCOMMENT:
			{
				emit pointerMode();
				return;
			}
			break;
		case DOM_POINTER:
		{
		}
		break;
		}
	}
	QGraphicsScene::mousePressEvent( mouseEvent );
}

void KismetScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent){
	if (mouseEvent->button() == Qt::LeftButton){
		if (mapMode_ == DOM_POINTER){

			if (mouseEvent->modifiers() & Qt::ShiftModifier){
				// 如果按下shift键，则复制
			}
			else{
				QList<QGraphicsItem*> items = selectedItems();
				emit entityMoved(items);
			}
		}
	}

	QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void KismetScene::keyPressEvent(QKeyEvent * keyEvent){
	if( keyEvent->key() == Qt::Key_Alt && mapMode_ == DOM_POINTER){
		// 进入框选模式
	    views().at(0)->setDragMode( QGraphicsView::RubberBandDrag );
	}
	else if (keyEvent->key() == Qt::Key_Delete && mapMode_ == DOM_POINTER){
		//
		QList<QGraphicsItem*> items = selectedItems();
		emit entityDeleted(items);
	}
	

	QGraphicsScene::keyPressEvent( keyEvent );
}

QList<BlueprintGroup*> KismetScene::selectedGroups(){
	QList<BlueprintGroup*> grps;
	foreach(QGraphicsItem* item, selectedItems()){
		if (item->type() == KISMET_GROUP){
			grps.append((BlueprintGroup*)item);
		}
	}
	return grps;
}

QRect KismetScene::validRegionGroup(BlueprintGroup* grp){
	QRectF region(0, 0, 0, 0);
	if ( grp ){
		region = grp->sceneBoundingRect();
	}
	region.adjust(-32, -32, 32, 64);
	return region.toRect();
}

void KismetScene::screenShotGroup(BlueprintGroup* grp){
	if (grp == NULL)
		return;

	// 必须打开一个工程
	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	Q_ASSERT(ws != NULL);
	// 获取当前电子书
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != NULL);

	QString n = proj->objectName() + UDQ_T("_") + parentInstance_->description() + UDQ_T("_") + grp->title();
	QRect r = validRegionGroup(grp);
	ScreenShotWindow w(n, r.size());
	if (w.exec() == QDialog::Accepted){
		// 创建图片
		QImage image(w.imgSize(), QImage::Format_ARGB32);
		image.fill(w.bgColor().rgba());

		scriptText_->setVisible(false);
		bgColor_ = w.bgColor();

		// 绘制图片
		QPainter painter(&image);
		render(&painter, QRectF(), r);

		bgColor_ = Qt::lightGray;
		scriptText_->setVisible(true);
		// 保存图片
		image.save(w.filePath(), "PNG");
	}

}

void KismetScene::keyReleaseEvent(QKeyEvent * keyEvent){

	if( keyEvent->key() == Qt::Key_Alt && mapMode_ == DOM_POINTER ){
		views().at(0)->setDragMode( QGraphicsView::ScrollHandDrag );
		return;
	}
	else if( keyEvent->key() == Qt::Key_F11 && mapMode_ == DOM_POINTER &&  parentInstance_ != NULL ){

		// 必须打开一个工程
		WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
		Q_ASSERT(ws != NULL);
		// 获取当前电子书
		ProjectResourceObject* proj = ws->currentProject();
		Q_ASSERT(proj != NULL);

		QString n = proj->objectName() + UDQ_T("_") +  parentInstance_->description();
		QRect r;
		//保存当前图像
		//计算当前图像有效大小Rect;
		if ( keyEvent->modifiers() & Qt::ControlModifier ){  // 如果同时按下CTRL，则抓取当前屏幕中
			r = validRegionViewPort();
		}
		else{
			// 检查当前选中的组对象
			QList<BlueprintGroup*> groups = selectedGroups();
			if (groups.size() > 0){
				foreach(BlueprintGroup* grp, groups){
					screenShotGroup(grp);
				}
				return;
			}
			else{
				r = validRegion();
			}

		}

		ScreenShotWindow w( n, r.size() );
		if( w.exec() == QDialog::Accepted ){
			// 创建图片
			QImage image( w.imgSize(), QImage::Format_ARGB32);
			image.fill( w.bgColor().rgba() );
			scriptText_->setVisible(false);
			bgColor_ = w.bgColor();
			// 绘制图片
			QPainter painter( &image );
			render(&painter, QRectF(), r);

			bgColor_ = Qt::lightGray;
			scriptText_->setVisible(true);
			// 保存图片
			image.save( w.filePath(), "PNG");
		}
		return;
	}

	QGraphicsScene::keyReleaseEvent( keyEvent );
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

//  清空所有连接
void KismetScene::clearLink(){

	foreach(BlueprintModel* model, modelObjects_){
		model->clearPortLink();
	}
}

// 重设
void KismetScene::clearScene(){
	// 执行清理工作
	clearSelection();

	foreach(BlueprintObject* obj, iidObjects_.values()){
		deleteObjectItem(obj);
	}

	modelObjects_.clear();
	varObjects_.clear();
	instObjects_.clear();
	assetObjects_.clear();
	grpObjects_.clear();
	iidObjects_.clear();

}

// 保存
bool KismetScene::saveCurrentScript(){

	if (parentInstance_ == NULL){
		scriptErrors_.clear();
		return true;
	}

	if (!validateScript()){
		// 提示，脚本有误
		KismetValidateWindow w(scriptErrors());
		if (w.exec() != QDialog::Accepted){
			return false;  // 保存失败
		}
	}
	
	KismetView* view = (KismetView*)views().at(0);

	// 保存视点
	parentInstance_->matrix_ = view->transform();
	parentInstance_->viewport_ = viewRegionRect().center();
	parentInstance_->zoomStep_ = view->zoomStep();

	// 保存模型
	parentInstance_->events_.clear();
	foreach(BlueprintModel* model, modelObjects_){
		parentInstance_->events_.append(model->encode());
	}

	//  变量
	foreach(BlueprintInstance* inst, instObjects_){
		parentInstance_->events_.append(inst->encode());
	}
	foreach(BlueprintVariable* var, varObjects_){
		parentInstance_->events_.append(var->encode());
	}
	foreach(BlueprintCollection* col, aggrObjects_){
		parentInstance_->events_.append(col->encode());
	}

	parentInstance_->assets_.clear();
	foreach(BlueprintObject* asset, assetObjects_){
		if (asset->type() == KISMET_COMMENT){
			parentInstance_->assets_.append(asset->encode());
		}
	}

	// 保存group
	parentInstance_->groups_.clear();
	foreach(BlueprintGroup* grp, grpObjects_){
		parentInstance_->groups_.append(grp->encode());
	}

	// 保存link
	QByteArray dat;

	parentInstance_->links_.clear();
	foreach(QGraphicsItem* item, items()){
		if (item->type() == KISMET_LINK_ARRAY){
			BlueprintLinkArray* link = qgraphicsitem_cast<BlueprintLinkArray*>(item);
			{
				QDataStream stream(&dat, QIODevice::WriteOnly | QIODevice::Truncate);
				link->encode(stream);
			}
			parentInstance_->links_.append(dat);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 生成Json脚本
	saveToJson();

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void KismetScene::addObjectItem(BlueprintObject* obj ){
	
	BlueprintModel* model = dynamic_cast<BlueprintModel*>(obj);
	if ( model ){
		modelObjects_.append((BlueprintModel*)obj);
	}
	else if (obj->type() == KISMET_VARIABLE){
		varObjects_.append((BlueprintVariable*)obj);
	}
	else if (obj->type() == KISMET_INSTANCE){
		instObjects_.append((BlueprintInstance*)obj);
	}
    else if(obj->type() == KISMET_GROUP){
		grpObjects_.append((BlueprintGroup*)obj);
	}
	else if (obj->type() == KISMET_COMMENT){
		assetObjects_.append(obj);
	}
	else if (obj->type() == KISMET_COLLECTION){
		aggrObjects_.append((BlueprintCollection*)obj);
	}
	else{
		return;
	}
	iidObjects_.insert(obj->iid(), obj);
	addItem(obj);

}

void KismetScene::removeObjectItem(BlueprintObject* obj){
	obj->setSelected(false);
	obj->addGroup(NULL);

	BlueprintModel* model = dynamic_cast<BlueprintModel*>(obj);
	if ( model ){
		((BlueprintModel*)obj)->clearPortLink();
	}
	else if (obj->type() == KISMET_VARIABLE){
		((BlueprintVariable*)obj)->clearLink();
	}
	else if (obj->type() == KISMET_INSTANCE){
		((BlueprintInstance*)obj)->clearLink();
	}
	else if (obj->type() == KISMET_COLLECTION){
		((BlueprintCollection*)obj)->clearLink();
	}
	else if (obj->type() == KISMET_GROUP){
		((BlueprintGroup*)obj)->removeAllSubObject();
	}
	else if (obj->type() == KISMET_COMMENT){
	}
	removeItem(obj);
}

void KismetScene::deleteObjectItem(BlueprintObject* obj){
	removeObjectItem( obj);

	BlueprintModel* model = dynamic_cast<BlueprintModel*>(obj);
	if (model ){
		modelObjects_.removeOne((BlueprintModel*)obj);
	}
	else if (obj->type() == KISMET_VARIABLE){
		varObjects_.removeOne((BlueprintVariable*)obj);
	}
	else if (obj->type() == KISMET_INSTANCE){
		instObjects_.removeOne((BlueprintInstance*)obj);
	}
	else if (obj->type() == KISMET_COLLECTION){
		aggrObjects_.removeOne((BlueprintCollection*)obj);
	}
	else if (obj->type() == KISMET_GROUP){
		grpObjects_.removeOne((BlueprintGroup*)obj);
	}
	else if (obj->type() == KISMET_COMMENT){
		assetObjects_.removeOne(obj);
	}

	iidObjects_.remove(obj->iid());
	delete obj;
}

//////////////////////////////////////////////////////////////////////////

BlueprintLink* KismetScene::addLinkItem(BlueprintObject* source, BlueprintObject* dest, QList<QPointF>& ctrlPts){
	BlueprintLink* link = new BlueprintLinkArray(source, dest, ctrlPts, false);
	addItem(link);
	return link;
}

BlueprintGroup* KismetScene::addGroupItem(QByteArray& data, ResourceHash newid ){
	BlueprintGroup* group = new BlueprintGroup();
	group->decode(data); 
	if (newid != 0){
		group->setIID(newid);
	}
	addObjectItem(group);
	group->postInit();

	return group;
}

BlueprintComment* KismetScene::addCommentItem(QByteArray& data, ResourceHash newid){
	BlueprintComment* comment = new BlueprintComment();
	comment->decode(data);
	if (newid != 0){
		comment->setIID(newid);
	}
	addObjectItem(comment);
	return comment;
}

BlueprintObject* KismetScene::decodePortFromData(QByteArray& data){
	QDataStream stream(data);
	int type;
	ResourceHash iid;
	stream >> type >> iid;
	BlueprintObject* obj = getObjectItem(iid);
	if (obj == NULL)
		return NULL;

	if (type == 1){
		BlueprintModel* model = (BlueprintModel*)obj;
		QString name;
		int orient;
		stream >> name >> orient;
		obj = model->findPort(name, orient);
	}
	return obj;
}

BlueprintLink* KismetScene::addLinkItem(QByteArray& data){

	QDataStream stream(data);
	QByteArray endarr;

	stream >> endarr;
	BlueprintObject* source = decodePortFromData(endarr);

	stream >> endarr;
	BlueprintObject* dest = decodePortFromData(endarr);

	if (source == NULL || dest == NULL){
		return NULL;
	}

	QList<QPointF> ctrlPts;
	stream >> ctrlPts;
	return addLinkItem(source, dest, ctrlPts);

}

BlueprintObject*  KismetScene::addModelItem(QByteArray& data, ResourceHash newid){
	QDataStream stream(data);
	int type;
	stream >> type;

	BlueprintObject* obj = NULL;
	if (type == KISMET_MODEL){
		obj = new BlueprintModel();
	}
	else if (type == KISMET_VARIABLE){
		obj = new BlueprintVariable();
	}
	else if (type == KISMET_INSTANCE){
		obj = new BlueprintInstance();
	}
	else if (type == KISMET_COLLECTION){
		obj = new BlueprintCollection();
	}
	else if (type == KISMET_FLOW_BRANCH){
		obj = new BlueprintBranch();
	}
	else if (type == KISMET_ARITHMETIC){
		obj = new BlueprintArithmetic();
	}
	else if (type == KISMET_FLOW_SEQUENCE){
		obj = new BlueprintSequence();
	}
	else if (type == KISMET_INSTANCE_VARIABLE){
		obj = new BlueprintInstanceVariable();
	}
	else if (type == KISMET_INSTANCE_INDEX){
	}
	else if (type == KISMET_GROUP){
		obj = new BlueprintGroup();
	}
	else if (type == KISMET_COMMENT){
		obj = new BlueprintComment();
	}

	obj->decode(data);
	if (newid != 0){
		obj->setIID(newid);
	}
	addObjectItem(obj);
	obj->postInit();

	return obj;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

BlueprintModel* KismetScene::addEventItem(const QPointF& pos, const QByteArray& data){
	QDataStream stream(data);
	ResourceHash hash;
	int type;
	ResourceHash interH;
	stream >> hash >> type >> interH;

	// 检查指定类型的插件是否有实例
	DEFINE_RESOURCE_HASHKEY_VALID(PluginResourceObject, plugin, hash);
	if (plugin->hasAttribute(EF_SINGLEGLOBAL)){
		WorkspaceResourceObject*  ws = ResourceObject::currentWorkspace();
		Q_ASSERT(ws != NULL);
		ProjectResourceObject* proj = ws->currentProject();
		Q_ASSERT(proj != NULL);
		// 检查是否存在
		ObjectTypeResourceObject* single = proj->getObjectType(plugin);
		if (single == NULL){
			QMessageBox::warning(views().at(0), UDQ_TR("添加对象"), UDQ_TR("请先在项目中添加对象实例!"));
			return NULL;
		}
	}

	BlueprintModel* model = new BlueprintModel(hash, type, interH);
	model->setPos(pos);
	addObjectItem(model);
	return model;
}

BlueprintVariable* KismetScene::addVariableItem(const QPointF& pos, ResourceHash key, const QString& name){
	DEFINE_RESOURCE_HASHKEY_VALID(BlueprintResourceObject, script, key);

	ScriptVariable* sv = script->variables_.value(name);
	
	BlueprintVariable* var = new BlueprintVariable(key, sv );
	var->setPos(pos);
	addObjectItem(var);
	return var;
}


// 加载
void KismetScene::loadScript(BlueprintResourceObject* blueprint){
	clearScene();

	parentInstance_ = blueprint;

	if (parentInstance_ == NULL){
		scriptText_->setText(UDQ_T(""));
		scriptText_->setVisible(false);
		return;
	}

	WorkspaceResourceObject*  ws= ResourceObject::currentWorkspace();
	Q_ASSERT( ws != NULL );
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != NULL);

	// 添加视角
	if (blueprint->viewport_.rx() > 0 && blueprint->viewport_.ry() > 0){
		KismetView* view = (KismetView*)views().at(0);

		view->setTransform(blueprint->matrix_);
		view->centerOn(blueprint->viewport_);
		view->setZoomStep(blueprint->zoomStep_);
	}

	scriptText_->setText(parentInstance_->description());
	scriptText_->setVisible(true);

	// 添加模型
	foreach(QByteArray arr, blueprint->events_){
		addModelItem(arr);
	}

	// 添加注释
	foreach(QByteArray arr, blueprint->assets_){
		addCommentItem(arr);
	}
	
	// 添加group
	foreach(QByteArray arr, blueprint->groups_){
		addGroupItem(arr);
	}

	// 添加link
	QList<QByteArray> nullLinks;
	foreach(QByteArray arr, blueprint->links_){
		BlueprintLink* link = addLinkItem(arr);
		if (link == NULL){
			nullLinks.append(arr);
			continue;
		}
	}

	// 删除无用的链接
	foreach(QByteArray arr, nullLinks){
		blueprint->links_.removeOne(arr);
	}

}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void KismetScene::addModelPort(){
	Q_ASSERT( menuHostItem_ != NULL );
	BlueprintModel* entity = dynamic_cast<BlueprintModel*>(menuHostItem_);

}

void KismetScene::addInstanceVariable(){
	// 添加
	Q_ASSERT(menuHostItem_ != NULL);
	BlueprintInstance* instance = dynamic_cast<BlueprintInstance*>(menuHostItem_);
	Q_ASSERT(instance != NULL);

	// 在实例对象右边添加一个对象；如果在Group中，则也放入相同的Group中
	BlueprintInstanceVariable* var = addInstanceVarItem();
	QPointF pos = instance->scenePos();
	var->setPos(pos.x() + var->boundingRect().width(), pos.y() );
	BlueprintGroup* grp = instance->eventGroup();
	if (grp){
		var->addGroup(grp);
	}
	emit entityInserted(var);

	// 添加连接
	BlueprintLink* link = new BlueprintLinkArray(instance, 0, QList<QPointF>(), true);
	link->upgradeLink(var->hostPort());
	addItem(link);

}

void KismetScene::addInstanceIndex(){

}
//////////////////////////////////////////////////////////////////////////

void KismetScene::enterInsertGroup(){
	clearSelection();
	setMode( DOM_ADDGROUP );
}


void KismetScene::enterInsertComment(){
	clearSelection();
	setMode( DOM_ADDCOMMENT );
}


void KismetScene::exitInsertState(){
	setMode(DOM_POINTER);
}

BlueprintGroup* KismetScene::addGroupItem(const QString& text, const QPointF& pos ){
	BlueprintGroup* group = new BlueprintGroup(text);
	group->setPos( pos );
	addObjectItem(group);
	return group;
}

BlueprintComment* KismetScene::addCommentItem(const QString& text, const QPointF& pos ){
	BlueprintComment* comment = new BlueprintComment(text);
	comment->setPos( pos );
	addObjectItem(comment);
	return comment;
}

BlueprintBranch* KismetScene::addBranchItem(){
	// 在屏幕中间添加一个控制分支
	BlueprintBranch* branch = new BlueprintBranch();
	// 计算屏幕中心位置
	QRectF viewRect = viewRegionRect();
	branch->setPos(viewRect.center());
	addObjectItem(branch);
	emit entityInserted(branch);
	return branch;
}

BlueprintArithmetic* KismetScene::addMathItem(){
	// 在屏幕中间添加一个控制分支
	BlueprintArithmetic* math = new BlueprintArithmetic();
	// 计算屏幕中心位置
	QRectF viewRect = viewRegionRect();
	math->setPos(viewRect.center());
	addObjectItem(math);
	emit entityInserted(math);
	return math;
}

BlueprintSequence* KismetScene::addSequenceItem(){
	// 在屏幕中间添加一个控制分支
	BlueprintSequence* seq = new BlueprintSequence();
	// 计算屏幕中心位置
	QRectF viewRect = viewRegionRect();
	seq->setPos(viewRect.center());
	addObjectItem(seq);
	emit entityInserted(seq);
	return seq;
}

BlueprintInstanceVariable* KismetScene::addInstanceVarItem(){
	// 在屏幕中间添加一个控制分支
	BlueprintInstanceVariable* var = new BlueprintInstanceVariable();
	// 计算屏幕中心位置
	QRectF viewRect = viewRegionRect();
	var->setPos(viewRect.center());
	addObjectItem(var);
	emit entityInserted(var);
	return var;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

QStringList KismetScene::groupNames(){

	QStringList names;
	foreach( BlueprintObject* asset, assetObjects_ ){
		if( asset->type() == KISMET_GROUP )
			names.append( asset->title() );
	}
	names.removeDuplicates();
	return names;
}

// 计算当前图像的有效区域
QRect KismetScene::validRegion(){
	QRectF region(0,0,0,0);

	foreach( QGraphicsItem* item, items() ){
		if( item->type() == KISMET_MODEL ||
			item->type() == KISMET_GROUP ||
			item->type() == KISMET_COMMENT ||
			item->type() == KISMET_LINK_ARRAY){
				// 获取每个实体在屏幕中位置
				if( region.width() == 0 ){
					region =  item->sceneBoundingRect();
				}
				else{
					region = region.united( item->sceneBoundingRect() );
				}
		}
	}

	region.adjust(-64,-64,64,64);
	return region.toRect();
}


QRect KismetScene::validRegionViewPort(){
	QRectF region(0, 0, 0, 0);

	QRectF viewRegion = viewRegionRect();

	foreach(QGraphicsItem* item, items(viewRegion, Qt::ContainsItemShape)){
		if (item->type() >= KISMET_OBJECT){
			// 获取每个实体在屏幕中位置
			if (region.width() == 0){
				region = item->sceneBoundingRect();
			}
			else{
				region = region.united(item->sceneBoundingRect());
			}
		}
	}

	region.adjust(-32, -32, 32, 32);
	return region.toRect();
}

QList<BlueprintVariable*> KismetScene::variableItems(ScriptVariable* var){
	QList<BlueprintVariable*> vars;
	foreach(BlueprintVariable* item, varObjects_){
		if (item->var_ == var){
			vars.append(item);
		}
	}
	return vars;
}

QList<BlueprintVariable*>  KismetScene::variables(const QString& name){
	QList<BlueprintVariable*> results;
	ScriptVariable* var = parentInstance_->variables_.value(name);
	if (var){
		results = variableItems(var);
	}
	return results;
}



void KismetScene::removeVariable(const QString& name){
	ScriptVariable* rmvar = parentInstance_->variables_.value(name);
	if (rmvar == NULL)
		return;

	removeVariableItem(rmvar);

	if (rmvar){
		parentInstance_->variables_.remove(name);
	}
}

void KismetScene::updateVariable(const QString& name){
	ScriptVariable* rmvar = parentInstance_->variables_.value(name);
	if (rmvar == NULL)
		return;

	foreach(BlueprintVariable* var, varObjects_){
		if (var->var_ == rmvar){
			var->update();
		}
	}
}

void KismetScene::updateScriptText(){
	if ( parentInstance_ ){
		scriptText_->setText(parentInstance_->description());
		scriptText_->setVisible(true);
	}
	else{
		scriptText_->setText(UDQ_T(""));
		scriptText_->setVisible(false);
	}
	update();
}

void KismetScene::updateVariableItem(ScriptVariable* var){

	foreach(BlueprintVariable* item, varObjects_){
		if (item->var_ == var){
			item->updateDataType();
		}
	}

}

void KismetScene::removeVariableItem(ScriptVariable* var){
	foreach(BlueprintVariable* item, variableItems(var)){
		deleteObjectItem(item);
	}
}


BlueprintObject*  KismetScene::getObjectItem(ResourceHash iid){
	if (iidObjects_.contains(iid)){
		return iidObjects_[iid];
	}
	return NULL;
}

bool KismetScene::validateScript(){
	
	scriptErrors_.clear();

	//////////////////////////////////////////////////////////////////////////
	// 合法性检查
	foreach(BlueprintModel* model, modelObjects_){
		model->validate(scriptErrors_);
		model->update();
	}

	// 变量
	foreach(BlueprintVariable* var, varObjects_){
		var->validate(scriptErrors_);
	}

	foreach(BlueprintInstance* inst, instObjects_){
		inst->validate(scriptErrors_);
	}

	foreach(BlueprintCollection* aggr, aggrObjects_){
		aggr->validate(scriptErrors_);
	}

	//////////////////////////////////////////////////////////////////////////
	// 统计错误
	bool hasError = false;
	foreach (QString e, scriptErrors_){
		QStringList items = e.split(UDQ_T("@"));
		if (items.size() > 1){
			hasError = (items[0].toUInt() == 1);
		}
		else{
			hasError = true;
		}

		if (hasError){
			break;
		}
	}

	parentInstance_->hasError_ = hasError;
	return scriptErrors_.size() == 0;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

const int NULL_GROUP = 99999999;
void KismetScene::saveToJson(){
	// 生成JSON数据
	parentInstance_->scriptJson_.clear();
	if (parentInstance_->hasError_){  // 如果模型有错误则不生成代码
		return;
	}

	QList<BlueprintModel*> blocks;

	BlueprintModel* upmodel = NULL;
	BlueprintObject* upport = NULL;

	// 扫描2遍, 第一遍处理条件，第二遍处理分支
	QList<BlueprintModel*> branches;
	foreach(BlueprintModel* model, modelObjects_){

		if (model->type() == KISMET_FLOW_BRANCH){
			branches.append(model);
			continue;
		}

		if (model->inCtrlPorts_.size() == 0){  //  没有上游模型
			if (model->modelType() == PT_CONDITION  && (model->hasDownStream(KISMET_MODEL, PT_ACTION) || model->hasDownStream(KISMET_FLOW_SEQUENCE))){
				// 检查条件模型的直接下游为动作
				blocks.append(model);
			}
		}
		else{  // 有上游模型

		}
	}

	foreach(BlueprintModel* model, branches){
		// 检查上游模型是否 没有分支或seq
		if (!model->hasUpStream(KISMET_FLOW_BRANCH) && !model->hasUpStream(KISMET_FLOW_SEQUENCE)){
				blocks.append(model);
		}
	}

	// 生成JSON, 由于目前不知道函数索引，因此采用API哈希值代替，最后导出时替换
	QJsonArray scriptCodes;

	// 生成全局变量
	foreach(ScriptVariable* var, parentInstance_->variables_.values()){
		QJsonArray varCode;
		//[1, "XXX", 0, 0, false, false, sid, false],
		varCode.append((int)1);
		varCode.append(var->name);
		varCode.append(var->datatype == UDQ_T("string") ? 1 : 0);
		if (var->datatype == UDQ_T("int")){
			varCode.append(var->value.toInt());
		}
		else if (var->datatype == UDQ_T("float")){
			varCode.append(var->value.toDouble());
		}
		else{
			varCode.append(var->value);
		}

		varCode.append(false); //  全局变量
		varCode.append(false); // 常量
		varCode.append((qint64)BlueprintObject::makeUniqueId());

		varCode.append(false); // 保留

		scriptCodes.append(varCode);
	}

	// 根据分组进行排序
	QMultiMap<int, BlueprintModel*> sortBlocks;
	int index;
	foreach(BlueprintModel* model, blocks){
		if (model->eventGroup() == NULL){
			index = NULL_GROUP;
		}
		else{
			index = groupIndex(model->eventGroup());
		}
		Q_ASSERT(index >= 0);
		sortBlocks.insert(index, model);
	}

	// 排序
	QList<int> blockIndex = sortBlocks.uniqueKeys();
	qSort(blockIndex.begin(), blockIndex.end());

	foreach(int i, blockIndex){
		QJsonArray modelCode;
		// 处理组内数据
		foreach(BlueprintModel* model, sortBlocks.values(i)){  // 目前不考虑组嵌套
			// TRUE分支
			QJsonArray code = generateBlockCode(model);
			for (int i = 0; i < code.size(); i++){
				modelCode.append(code[i]);
			}
		}

		// 找到父对象
		if (i != NULL_GROUP){
			// 创建组代码块
			QJsonArray groupCode = grpObjects_[i]->expNode().toArray();
			groupCode.append(modelCode);  // 追加子事件模块
			scriptCodes.append(groupCode);
		}
		else{
			for (int i = 0; i < modelCode.size(); i++){
				scriptCodes.append(modelCode[i]);
			}
		}
	}

	QJsonDocument doc(scriptCodes);
	parentInstance_->scriptJson_ = doc.toJson(QJsonDocument::Compact);

	//////////////////////////////////////////////////////////////////////////
	// 测试
	qDebug() << parentInstance_->scriptJson_;
}

const QString NAME_SYSTEM = UDQ_TR("System");
QJsonArray KismetScene::generateBlockCode(BlueprintModel* head){
	static const QString  grpElseH = QString::number(hashString(UDQ_T("cr.system_object.prototype.cnds.Else")));

	QString outPort = UDQ_T("true");
	BlueprintModel* downmodel = head->downStreamCtrl(outPort);
	if (head->type() == KISMET_MODEL){
		// 如果是条件，true端口没有连接动作或序列则跳过；如果是分支的话，则生成
		if (downmodel == NULL || (downmodel->modelType() != PT_ACTION && downmodel->type() != KISMET_FLOW_SEQUENCE) ){
			outPort = UDQ_T("false");
			downmodel = head->downStreamCtrl(outPort);
			if (downmodel == NULL || (downmodel->modelType() != PT_ACTION && downmodel->type() != KISMET_FLOW_SEQUENCE) ){
				return QJsonArray();  // 空代码
			}
		}
	}

	QJsonArray codes;

	//////////////////////////////////////////////////////////////////////////
	// 正常分支
	QJsonArray code;
	code.append((int)0);
	code.append(QJsonValue());

	if (head->type() == KISMET_MODEL){  // 单个条件
		code.append(false);
		code.append(QJsonValue());
		code.append((qint64)BlueprintObject::makeUniqueId());

		PluginResourceObject* plugin = head->pluginObject();
		Q_ASSERT(plugin != NULL);

		QJsonArray cnds;
		BlueprintModelPort* port = head->outCtrlPort(outPort);
		Q_ASSERT(port && port->hasLink());
		cnds.append(head->portExpNode(port));
		code.append(cnds);

	}
	else if (head->type() == KISMET_FLOW_BRANCH){ // 多个条件
		BlueprintBranch* branch = (BlueprintBranch*)head;
		code.append(branch->isORLogic());
		code.append(QJsonValue());
		code.append((qint64)head->iid());

		QJsonArray cnds;
		foreach(BlueprintModelPort* port, head->inCtrlPorts_){
			QList<BlueprintLink *> links = port->links();
			if (links.size() == 0){  // 跳过空输入
				continue;
			}

			Q_ASSERT(links.size() == 1);
			BlueprintObject* upport = links[0]->sourceNode() == port ? links[0]->destNode() : links[0]->sourceNode();
			BlueprintModel* upmodel = dynamic_cast<BlueprintModel*>(upport->parentItem());
			Q_ASSERT(upmodel != NULL);

			if (upmodel->type() == KISMET_MODEL && upmodel->modelType() == PT_CONDITION){  // 只处理条件输入
				cnds.append(upmodel->portExpNode((BlueprintModelPort*)upport));
			}		
		}
		code.append(cnds);
	}
	else{
		Q_UNREACHABLE();
	}

	// 动作
	code.append(generateActionsCode(downmodel));

	// 子事件
	code.append(generateSubsCode(downmodel));

	codes.append(code);

	//////////////////////////////////////////////////////////////////////////
	// Else模块
	QJsonArray elseCode = generateBlockElseCode(head);
	for (int i = 0; i < elseCode.size(); i++){
		codes.append(elseCode[i]);
	}

	return codes;
}

QJsonArray KismetScene::generateActionsCode(BlueprintModel* head, bool isElse){
	if (head == NULL){
		return QJsonArray();
	}

	// 动作
	QJsonArray acts;

	BlueprintModel* actHead = NULL;
	if (head->type() == KISMET_MODEL && head->modelType() == PT_ACTION){
		actHead = head;
	}
	else if (head->type() == KISMET_FLOW_SEQUENCE){  // 下游为序列
		// 检查第一个输出端口
		actHead = head->downStreamCtrl(UDQ_T("out1"));
		if (actHead && actHead->type() == KISMET_MODEL && actHead->modelType() == PT_ACTION){
		}
		else{
			// 不是动作模型
			actHead = NULL;
		}
	}
	else if (isElse & head->type() == KISMET_FLOW_BRANCH){  // 如果是分支
		// 检查第一个输出是否为action
		actHead = head->downStreamCtrl(UDQ_T("true"));
		if (actHead && actHead->type() == KISMET_MODEL && actHead->modelType() == PT_ACTION){
		}
		else{
			// 不是动作模型
			actHead = NULL;
		}
	}

	while (actHead != NULL){
		// 必须是动作
		Q_ASSERT(actHead->type() == KISMET_MODEL && actHead->modelType() == PT_ACTION);
		acts.append(actHead->portExpNode());
		actHead = actHead->downStreamCtrl(UDQ_T("out"));
	};
	
	return acts;
}

QJsonArray KismetScene::generateSubsCode(BlueprintModel* head, bool isElse ){
	QJsonArray subs;
	BlueprintModel* subHead = NULL;
	if (head && head->type() == KISMET_FLOW_SEQUENCE){  // 下游为序列
		// 判断是否有条件或分支
		foreach(BlueprintModelPort* p, head->outCtrlPorts_){
			subHead = head->downStreamCtrl(p->name());
			if (subHead){
				if (subHead->type() == KISMET_FLOW_BRANCH){  // 只处理分支
					QJsonArray subcode = generateBlockCode(subHead);
					for (int i = 0; i < subcode.size(); i++){
						subs.append(subcode[i]);
					}
				}
				else if (subHead->type() == KISMET_MODEL && subHead->modelType() == PT_ACTION){
					// 序列模型直接连接动作
					if (p->name() != UDQ_T("out1")){  // 不是第一个输出端口
						QJsonArray code;
						code.append((int)0);
						code.append(QJsonValue());
						code.append(false);
						code.append(QJsonValue());
						code.append((qint64)BlueprintObject::makeUniqueId());
						code.append(QJsonArray());
						// 动作
						code.append(generateActionsCode(subHead));
						subs.append(code);
					}
				}
				else if (subHead->type() == KISMET_FLOW_SEQUENCE){  // 不允许多个序列直接相连
					Q_UNREACHABLE();
				}
			}
		}
	}
	else if (head && head->type() == KISMET_FLOW_BRANCH){  // 分支
		if (!isElse){
			QJsonArray subcode = generateBlockCode(head);
			for (int i = 0; i < subcode.size(); i++){
				subs.append(subcode[i]);
			}
		}
	}
	return subs;
}

QJsonArray KismetScene::generateBlockElseCode(BlueprintModel* head){
	static const QString  grpElseH = QString::number(hashString(UDQ_T("cr.system_object.prototype.cnds.Else")));

	if ( head  == NULL ||  head->type() != KISMET_FLOW_BRANCH){  // 只有分支对象能够有ELSE
		return QJsonArray();
	}

	BlueprintModel* downmodel = head->downStreamCtrl(UDQ_T("false"));
	if (downmodel == NULL){
		return QJsonArray();
	}

	// 正常模块
	QJsonArray codes;

	QJsonArray elseCode;
	elseCode.append((int)0);
	elseCode.append(QJsonValue());
	elseCode.append(false);  // 不能为OR逻辑
	elseCode.append(QJsonValue());
	elseCode.append((qint64)BlueprintObject::makeUniqueId());

	// 条件
	QJsonArray cnds;
	QJsonArray elsecnd;
	elsecnd.append((int)-1);
	elsecnd.append(grpElseH);  // 导出时替换为索引
	elsecnd.append(QJsonValue());
	elsecnd.append((int)0);
	elsecnd.append(false);
	elsecnd.append(false);
	elsecnd.append(false);
	elsecnd.append((qint64)BlueprintObject::makeUniqueId());
	elsecnd.append(false);
	cnds.append(elsecnd);

	// 获取其他条件
	if (downmodel->type() == KISMET_FLOW_BRANCH){
		BlueprintBranch* branch = (BlueprintBranch*)downmodel;
		foreach(BlueprintModelPort* port, branch->inCtrlPorts_){
			QList<BlueprintLink *> links = port->links();
			if (links.size() == 0){
				continue;
			}

			Q_ASSERT(links.size() == 1);
			BlueprintObject* upport = links[0]->sourceNode() == port ? links[0]->destNode() : links[0]->sourceNode();
			BlueprintModel* upmodel = dynamic_cast<BlueprintModel*>(upport->parentItem());
			Q_ASSERT(upmodel != NULL);

			if (upmodel == head){
				continue;
			}
			if (upmodel->type() == KISMET_MODEL && upmodel->modelType() == PT_CONDITION){  // 只处理条件输入
				cnds.append(upmodel->portExpNode((BlueprintModelPort*)upport));
			}
		}
	}
	elseCode.append(cnds);

	// 动作
	elseCode.append(generateActionsCode(downmodel, true));
	// 子事件
	elseCode.append(generateSubsCode(downmodel, true));
	codes.append(elseCode);

	////////////////////////////////////////////////////////////////////////////
	// Else/Else模型
	QJsonArray elseelseCode = generateBlockElseCode(downmodel);
	for (int i = 0; i < elseelseCode.size(); i++){
		codes.append(elseelseCode[i]);
	}

	return codes;
}

void KismetScene::deleteModelPort(){

	Q_ASSERT(menuHostItem_ != NULL);
	//BlueprintModelPort* port = dynamic_cast<BlueprintModelPort*>(menuHostItem_);
	//Q_ASSERT(port != NULL);
	//// 获取父模型
	//BlueprintModel* parent = (BlueprintModel*)port->parentItem();
	//Q_ASSERT(parent != NULL);
	//parent->removePort(port);

	QList<QGraphicsItem*> items;
	items << menuHostItem_;
	emit entityDeleted(items);

}

void  KismetScene::clearModelPortLink(){
	Q_ASSERT(menuHostItem_ != NULL);
	BlueprintModelPort* port = dynamic_cast<BlueprintModelPort*>(menuHostItem_);
	Q_ASSERT(port != NULL);

	port->clearLink();

	// 删除连线

}

void KismetScene::selectObjectType(const QPointF& pos){

	SelectObjectTypeWindow w;
	QSize ws = w.size();
	//  移动到鼠标位置
	QSize viewRect = views().at(0)->maximumViewportSize();
	QPointF topleft = views().at(0)->mapToScene(0, 0);
	QPointF bottomright = views().at(0)->mapToScene(viewRect.width(), viewRect.height());
	
	QPointF center = views().at(0)->mapToScene(viewRect.width()/2, viewRect.height()/2);

	QPointF winpos = pos;
	// 判断上下位置
	if (bottomright.y() - pos.y() < ws.height()){

		if (viewRect.height() < ws.height()){
			winpos.setY(topleft.y());
		}
		else{
			winpos.setY(bottomright.y() - ws.height());
		}
	}

	// 判断左右位置
	if (bottomright.x() - pos.x() < ws.width()){
		if (viewRect.width() < ws.width()){
			winpos.setX(topleft.x());
		}
		else{
			winpos.setX(bottomright.x() - ws.width());
		}
	}

	// 显示窗口
	QPoint vpos = views().at(0)->mapFromScene(winpos);
	w.move(views().at(0)->mapToGlobal(vpos));
	if (w.exec() == QDialog::Accepted){
		// 添加对象类型
		ResourceObject* obj = ResourceObject::findResource(w.typeHash_);
		Q_ASSERT(obj != NULL && !(obj->flags() & URF_TagGarbage));

		if (UIsExactKindOf(ObjectTypeResourceObject, obj)){
			//  添加对象实例
			addInstanceItem(pos, w.typeHash_);
		}
		else if(UIsExactKindOf(FamilyResourceObject, obj)){
			addCollectionItem(pos, w.typeHash_);
		}
	}
}

void KismetScene::notifyObjectInsert(QGraphicsItem* item){
	emit entityInserted(item);
}

void KismetScene::notifyLinkChanged(BlueprintLink* link){

	QByteArray dat;
	{
		QDataStream stream(&dat, QIODevice::WriteOnly);
		link->copyState(stream);
	}

	emit entityModified(link, dat);
}

void KismetScene::refreshScript(){
	//  检查模型
	QList<BlueprintModel*> deleteModels;
	foreach(BlueprintModel* model, modelObjects_){
		// 检查接口是否存在
		if (model->getInterface() == NULL){
			deleteModels.append(model);
		}
	}

	foreach(BlueprintModel* model, deleteModels){
		removeObjectItem(model);
	}

	update();

}