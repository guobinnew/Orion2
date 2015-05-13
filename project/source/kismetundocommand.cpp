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

#include "kismetundocommand.h"
#include <QtGui>
#include <QMenu>
#include "commondefine.h"
#include "tooldefine.h"
#include "kismetdefine.h"
#include "componentdefine.h"
#include "menumanager.h"
#include "logmanager.h"
#include "framemanager.h"
#include "kismetinstancewindow.h"
#include "mainkismetframe.h"
#include "kismetscene.h"
#include "kismetvariable.h"
#include "kismetlinkarray.h"
using namespace ORION;

AddVariableCommand::AddVariableCommand(QString& name, BlueprintResourceObject* script, QUndoCommand *parent)
	: QUndoCommand(parent), script_(script), name_(name)
{

}

AddVariableCommand::~AddVariableCommand()
{

}

void AddVariableCommand::undo()
{
	MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
	Q_ASSERT(frame != NULL);

	ResourceHash key = hashString(name_);
	ScriptVariable* var = script_->variables_.value(key);
	frame->instanceWidget()->deleteVariable(var);
	frame->removeVariable(var);

}

void AddVariableCommand::redo()
{
	MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
	Q_ASSERT(frame != NULL);

	ScriptVariable* var = script_->addNewVariable(name_);
	frame->instanceWidget()->addVariable(var);

}

//////////////////////////////////////////////////////////////////////////

DeleteVariableCommand::DeleteVariableCommand(ResourceHash key, BlueprintResourceObject* script, QUndoCommand *parent)
	: QUndoCommand(parent), script_(script)
{
	ScriptVariable* var = script_->variables_.value(key);
	name_ = var->name;
	datatype_ = var->datatype;
	value_ = var->value;
	desc_ = var->desc;
}

DeleteVariableCommand::~DeleteVariableCommand()
{

}

void DeleteVariableCommand::undo()
{
	MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
	Q_ASSERT(frame != NULL);

	ScriptVariable* var = script_->addNewVariable(name_);
	var->datatype = datatype_;
	var->value = value_;
	var->desc = desc_;
	frame->instanceWidget()->addVariable(var);

	//  添加删除的对象和连线
	KismetScene* scene = (KismetScene*)frame->sceneView()->scene();
	foreach(QByteArray arr,models_){
		scene->addModelItem(arr);
	}

	// 添加link
	foreach(QByteArray arr, links_){
		BlueprintLink* link = scene->addLinkItem(arr);
		Q_ASSERT(link != NULL);
	}

}

void DeleteVariableCommand::redo()
{
	QUndoCommand::redo();  // 删除连线和对象实例

	MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
	Q_ASSERT(frame != NULL);

	ResourceHash key = hashString(name_);
	ScriptVariable* var = script_->variables_.value(key);

	KismetScene* scene = (KismetScene*)frame->sceneView()->scene();
	QList<BlueprintVariable*> instvars = scene->variableItems(var);

	links_.clear();
	models_.clear();

	// 记录删除的模型对象
	QByteArray dat;
	foreach(BlueprintVariable* item, instvars){
		Q_ASSERT(item->type() == KISMET_VARIABLE);
		models_.append(item->encode());
		foreach(BlueprintLink* link, item->links()){
			QDataStream stream(&dat, QIODevice::WriteOnly | QIODevice::Truncate);
			link->encode(stream);
			links_.append(dat);
		}
	}

	frame->instanceWidget()->deleteVariable(var);
	frame->removeVariable(var);
}

//////////////////////////////////////////////////////////////////////////

RenameVariableCommand::RenameVariableCommand(ResourceHash key, const QString& newName, BlueprintResourceObject* scipt, QUndoCommand *parent)
	: QUndoCommand(parent), script_(scipt), newName_(newName)
{
	ScriptVariable* var = script_->variables_.value(key);
	oldName_ = var->name;
}

RenameVariableCommand::~RenameVariableCommand()
{

}

void RenameVariableCommand::undo()
{
	// 
	ResourceHash newKey = hashString(newName_);
	ScriptVariable* var = script_->variables_.value(newKey);
	var->name = oldName_;

	ResourceHash oldKey = hashString(oldName_);
	script_->variables_.take(newKey);
	script_->variables_.insert(oldKey, var);

	// 更新实例创建
	MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
	Q_ASSERT(frame != NULL);

	//  刷新实例列表
	frame->instanceWidget()->init(script_);
	frame->updateVariableType(var);
}

void RenameVariableCommand::redo()
{
	// 
	ResourceHash oldKey = hashString(oldName_);
	ScriptVariable* var = script_->variables_.value(oldKey);
	var->name = newName_;

	ResourceHash newKey = hashString(newName_);
	script_->variables_.take(oldKey);
	script_->variables_.insert(newKey, var);

	// 更新实例创建
	MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
	Q_ASSERT(frame != NULL);

	//  刷新实例列表
	frame->instanceWidget()->init(script_);
	frame->updateVariableType(var);

}

//////////////////////////////////////////////////////////////////////////

ModifyVariableCommand::ModifyVariableCommand(ResourceHash key, int type, const QString& newValue, BlueprintResourceObject* scipt, QUndoCommand *parent)
	: QUndoCommand(parent), script_(scipt), type_(type), newValue_(newValue)
{

	ScriptVariable* var = script_->variables_.value(key);
	name_ = var->name;
	if (type_ == MVC_DATATYPE){
		oldValue_ = var->datatype;
	}
	else if (type_ == MVC_VALUE){
		oldValue_ = var->value;
	}
	else{
		Q_UNREACHABLE();
	}
}

ModifyVariableCommand::~ModifyVariableCommand()
{

}

void ModifyVariableCommand::undo()
{
	MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
	Q_ASSERT(frame != NULL);
	// 
	ResourceHash key = hashString(name_);
	ScriptVariable* var = script_->variables_.value(key);

	if (type_ == MVC_DATATYPE){
		var->datatype = oldValue_;
		frame->instanceWidget()->refreshVariable(var);
		frame->updateVariableType(var);
	}
	else if (type_ == MVC_VALUE){
		var->value = oldValue_;
		// 刷新树信息
		frame->instanceWidget()->refreshVariable(var);
	}
	else{
		Q_UNREACHABLE();
	}
}

void ModifyVariableCommand::redo()
{
	// 更新实例创建
	MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
	Q_ASSERT(frame != NULL);
	// 
	ResourceHash key = hashString(name_);
	ScriptVariable* var = script_->variables_.value(key);

	if (type_ == MVC_DATATYPE){
		var->datatype = newValue_;
		frame->instanceWidget()->refreshVariable(var);
		frame->updateVariableType(var);
	}
	else if (type_ == MVC_VALUE){
		var->value = newValue_;
		// 刷新树信息
		frame->instanceWidget()->refreshVariable(var);
	}
	else{
		Q_UNREACHABLE();
	}
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

AddBluePrintModelCommand::AddBluePrintModelCommand(BlueprintObject* obj,  KismetScene* scene, QUndoCommand *parent)
	: QUndoCommand(parent), scene_(scene), enabled_(false)
{
	data_ = obj->encode();
	iid_ = obj->iid();
}

AddBluePrintModelCommand::~AddBluePrintModelCommand()
{

}

void AddBluePrintModelCommand::undo()
{
	//  删除对象
	scene_->deleteObjectItem(scene_->getObjectItem(iid_));
}

void AddBluePrintModelCommand::redo()
{
	if (enabled_){  // 执行新对象创建
		scene_->addModelItem(data_);
	}
}


//////////////////////////////////////////////////////////////////////////

CopyBluePrintModelCommand::CopyBluePrintModelCommand(BlueprintObject* obj, const QPointF& offset, KismetScene* scene, QUndoCommand *parent)
	: QUndoCommand(parent), scene_(scene), offset_(offset)
{
	data_ = obj->encode();
	iid_ = BlueprintObject::makeUniqueId();
}

CopyBluePrintModelCommand::~CopyBluePrintModelCommand()
{

}

void CopyBluePrintModelCommand::undo()
{
	//  删除对象
	scene_->deleteObjectItem(scene_->getObjectItem(iid_));
}

void CopyBluePrintModelCommand::redo()
{
	BlueprintObject* obj = scene_->addModelItem(data_, iid_);
	obj->setPos(obj->pos() + offset_);
	obj->setSelected(true);
}


//////////////////////////////////////////////////////////////////////////

DeleteBluePrintModelCommand::DeleteBluePrintModelCommand(BlueprintObject* obj,  KismetScene* scene, QUndoCommand *parent)
	: QUndoCommand(parent), scene_(scene), groupid_(0)
{
	if (obj->eventGroup()){
		groupid_ = obj->eventGroup()->iid();
	}
	data_ = obj->encode();
	iid_ = obj->iid();
}

DeleteBluePrintModelCommand::~DeleteBluePrintModelCommand()
{

}

void DeleteBluePrintModelCommand::undo()
{
	scene_->addModelItem(data_);
	// 添加link
	foreach(QByteArray arr, links_){
		BlueprintLink* link = scene_->addLinkItem(arr);
		Q_ASSERT(link != NULL);
	}
}

void DeleteBluePrintModelCommand::redo()
{
	// 记录连线
	links_.clear();
	BlueprintObject* obj = scene_->getObjectItem(iid_);

	QByteArray data;
	if (obj->type() == KISMET_MODEL || 
		obj->type() == KISMET_ARITHMETIC || 
		obj->type() == KISMET_FLOW_BRANCH ||
		obj->type() == KISMET_FLOW_SEQUENCE ||
		obj->type() == KISMET_INSTANCE_VARIABLE ){
		// 记录所有端口连线
		BlueprintModel* model = (BlueprintModel*)obj;
		foreach(BlueprintModelPort* port, model->ports()){
			foreach(BlueprintLink* link, port->links()){
				QDataStream stream(&data, QIODevice::WriteOnly | QIODevice::Truncate);
				link->encode(stream);
				links_.append(data);
			}
		}
	}
	else if (obj->type() == KISMET_INSTANCE ||
		obj->type() == KISMET_VARIABLE ||
		obj->type() == KISMET_COLLECTION){
		//  记录端口连线
	
		foreach(BlueprintLink* link, obj->links()){
			QDataStream stream(&data, QIODevice::WriteOnly | QIODevice::Truncate);
			link->encode(stream);
			links_.append(data);
		}
	}
	else if (obj->type() == KISMET_GROUP || obj->type() == KISMET_COMMENT ){
	}
	else{
		Q_UNREACHABLE();
	}

	//  删除对象
	scene_->deleteObjectItem(obj);
}

//////////////////////////////////////////////////////////////////////////

MoveBluePrintModelCommand::MoveBluePrintModelCommand(BlueprintObject* obj, const QByteArray& olddata, KismetScene* scene, QUndoCommand *parent)
	: QUndoCommand(parent), scene_(scene), enabled_(false), oldData_(olddata)
{
	iid_ = obj->iid();

	{
		QDataStream stream(&newData_, QIODevice::WriteOnly | QIODevice::Truncate);
		stream << obj->pos();
		stream << (unsigned int)(obj->eventGroup() ? obj->eventGroup()->iid() : 0);
	}

}

MoveBluePrintModelCommand::~MoveBluePrintModelCommand()
{

}

void MoveBluePrintModelCommand::undo()
{
	QDataStream stream(oldData_);
	QPointF oldPos;
	ResourceHash oldGrp;
	stream >> oldPos >> oldGrp;

	BlueprintObject* obj = scene_->getObjectItem(iid_);
	obj->setPos(oldPos);

	if (oldGrp > 0){
		BlueprintGroup* oldGroup = (BlueprintGroup*)scene_->getObjectItem(oldGrp);
		obj->addGroup(oldGroup);
	}
}

void MoveBluePrintModelCommand::redo()
{
	if (enabled_){ // 调整位置和新组
		QDataStream stream(newData_);
		QPointF newPos;
		ResourceHash newGrp;
		stream >> newPos >> newGrp;
		BlueprintObject* obj = scene_->getObjectItem(iid_);
		obj->setPos(newPos);

		if (newGrp > 0){
			BlueprintGroup* newGroup = (BlueprintGroup*)scene_->getObjectItem(newGrp);
			obj->addGroup(newGroup);
		}
	}
}


//////////////////////////////////////////////////////////////////////////

ResizeBluePrintGroupCommand::ResizeBluePrintGroupCommand(BlueprintGroup* obj, const QByteArray& oldData, KismetScene* scene, QUndoCommand *parent)
	: QUndoCommand(parent), scene_(scene), enabled_(false)
{
	{
		QDataStream stream(oldData);
		stream >> oldSize_;
	}

	iid_ = obj->iid();
	newSize_ = obj->boundingRect().size();
}

ResizeBluePrintGroupCommand::~ResizeBluePrintGroupCommand()
{

}

void ResizeBluePrintGroupCommand::undo()
{

	BlueprintObject* obj = scene_->getObjectItem(iid_);
	if (obj->type() == KISMET_GROUP){
		BlueprintGroup* grp = (BlueprintGroup*)obj;
		grp->resize(oldSize_);
		grp->updateSubObjects();
	}

}

void ResizeBluePrintGroupCommand::redo()
{
	if (enabled_){ // 调整位置和新组
		BlueprintObject* obj = scene_->getObjectItem(iid_);
		if (obj->type() == KISMET_GROUP){
			BlueprintGroup* grp = (BlueprintGroup*)obj;
			grp->resize(newSize_);
			grp->updateSubObjects();
		}
	}
}

//////////////////////////////////////////////////////////////////////////

ModifyBluePrintModelCommand::ModifyBluePrintModelCommand(BlueprintObject* obj, const QByteArray& newData, KismetScene* scene, QUndoCommand *parent)
	: QUndoCommand(parent), scene_(scene), newData_(newData)
{
	{
		QDataStream stream(&oldData_, QIODevice::WriteOnly | QIODevice::Truncate);
		obj->copyState(stream, BlueprintObject::BST_DISPLAY);
	} 
	iid_ = obj->iid();
}

ModifyBluePrintModelCommand::~ModifyBluePrintModelCommand()
{

}

void ModifyBluePrintModelCommand::undo()
{
	applyData(oldData_);
}

void ModifyBluePrintModelCommand::redo()
{
	applyData(newData_);
}

void ModifyBluePrintModelCommand::applyData(QByteArray& data){

	BlueprintObject* obj = scene_->getObjectItem(iid_);
	QDataStream stream(data);
	QString text;
	QColor color;

	if (obj->type() == KISMET_COMMENT){
		BlueprintComment* comment = (BlueprintComment*)obj;
		stream >> text >> color;
		comment->setTextColor(color);
		comment->changeText(text);
	}
	else if (obj->type() == KISMET_GROUP){
		BlueprintGroup* group = (BlueprintGroup*)obj;
		stream >> text >> color;
		bool active;
		stream >> active;
		group->setTitle(text);
		group->setFillColor(color);
		group->setActiveOnStart(active);
	}

	obj->update();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


AddBluePrintModelPortCommand::AddBluePrintModelPortCommand(BlueprintModelPort* port,  KismetScene* scene, QUndoCommand *parent)
	: QUndoCommand(parent), scene_(scene), enabled_(false)
{
	BlueprintModel* model = (BlueprintModel*)(port->parentItem());
	iid_ = model->iid();

	{
		QDataStream stream(&data_, QIODevice::WriteOnly | QIODevice::Truncate);
		port->copyState(stream, BlueprintObject::BST_DISPLAY);
	}
}

AddBluePrintModelPortCommand::~AddBluePrintModelPortCommand()
{

}

void AddBluePrintModelPortCommand::undo()
{
	//  删除端口
	BlueprintModel* parent = (BlueprintModel*)scene_->getObjectItem(iid_);
	Q_ASSERT(parent != NULL);

	QDataStream stream(data_);
	QString name;
	int orient;
	stream >> name >> orient;

	BlueprintModelPort* port = parent->findPort(name,orient);
	Q_ASSERT(port != NULL);

	parent->removePort(port);
}

void AddBluePrintModelPortCommand::redo()
{
	if (enabled_){  // 添加新端口
		//  删除端口
		BlueprintModel* parent = (BlueprintModel*)scene_->getObjectItem(iid_);
		Q_ASSERT(parent != NULL);

		QDataStream stream(data_);
		QString name;
		int orient;
		QString datatype;
		stream >> name >> orient >> datatype;

		BlueprintModelPort* port = NULL;
		if (orient == BlueprintModelPort::EVT_CTRL_IN){
			port = parent->addCtrlInPort(name, BlueprintObject::OF_CAN_DELETE);
		}
		else if (orient == BlueprintModelPort::EVT_CTRL_OUT){
			port = parent->addCtrlOutPort(name, BlueprintObject::OF_CAN_DELETE);
		}
		else if (orient == BlueprintModelPort::EVT_DATA_IN){
			port = parent->addDataInPort(name, datatype, BlueprintObject::OF_CAN_DELETE);
		}
		else if (orient == BlueprintModelPort::EVT_DATA_OUT){
			port = parent->addDataOutPort(name, datatype, BlueprintObject::OF_CAN_DELETE);
		}

		parent->adjust();
		parent->update();
	}
}

//////////////////////////////////////////////////////////////////////////

DeleteBluePrintModelPortCommand::DeleteBluePrintModelPortCommand(BlueprintModelPort* port, KismetScene* scene, QUndoCommand *parent)
	: QUndoCommand(parent), scene_(scene)
{
	BlueprintModel* model = (BlueprintModel*)(port->parentItem());
	iid_ = model->iid();

	{
		QDataStream stream(&data_, QIODevice::WriteOnly | QIODevice::Truncate);
		port->copyState(stream, BlueprintObject::BST_DISPLAY);
	}
}

DeleteBluePrintModelPortCommand::~DeleteBluePrintModelPortCommand()
{

}

void DeleteBluePrintModelPortCommand::undo()
{
	//  添加端口
	BlueprintModel* parent = (BlueprintModel*)scene_->getObjectItem(iid_);
	Q_ASSERT(parent != NULL);

	QDataStream stream(data_);
	QString name;
	int orient;
	QString datatype;
	stream >> name >> orient >> datatype;

	BlueprintModelPort* port = NULL;
	if (orient == BlueprintModelPort::EVT_CTRL_IN){
		port = parent->addCtrlInPort(name, BlueprintObject::OF_CAN_DELETE);
	}
	else if (orient == BlueprintModelPort::EVT_CTRL_OUT){
		port = parent->addCtrlOutPort(name, BlueprintObject::OF_CAN_DELETE);
	}
	else if (orient == BlueprintModelPort::EVT_DATA_IN){
		port = parent->addDataInPort(name, datatype, BlueprintObject::OF_CAN_DELETE);
	}
	else if (orient == BlueprintModelPort::EVT_DATA_OUT){
		port = parent->addDataOutPort(name, datatype, BlueprintObject::OF_CAN_DELETE);
	}
	parent->adjust();
	parent->update();

	//  添加连线
	foreach(QByteArray arr, links_){
		BlueprintLink* link = scene_->addLinkItem(arr);
		Q_ASSERT(link != NULL);
	}

}

void DeleteBluePrintModelPortCommand::redo()
{
	BlueprintModel* parent = (BlueprintModel*)scene_->getObjectItem(iid_);
	Q_ASSERT(parent != NULL);

	QDataStream stream(data_);
	QString name;
	int orient;
	stream >> name >> orient;

	BlueprintModelPort* port = parent->findPort(name, orient);
	Q_ASSERT(port != NULL);

	// 记录连线
	QByteArray dat;
	foreach(BlueprintLink* link, port->links()){
		QDataStream stream(&dat, QIODevice::WriteOnly | QIODevice::Truncate);
		link->encode(stream);
		links_.append(dat);
	}

	parent->removePort(port);
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


AddBluePrintLinkCommand::AddBluePrintLinkCommand(BlueprintLink* link, KismetScene* scene, QUndoCommand *parent)
	: QUndoCommand(parent), scene_(scene), enabled_(false)
{
	{
		QDataStream steam(&data_, QIODevice::WriteOnly |QIODevice::Truncate);
		link->encode(steam);
	}
}
AddBluePrintLinkCommand::AddBluePrintLinkCommand(const QByteArray& data, KismetScene* scene, QUndoCommand *parent)
	: QUndoCommand(parent), scene_(scene), enabled_(true), data_(data)
{
}


AddBluePrintLinkCommand::~AddBluePrintLinkCommand()
{

}

void AddBluePrintLinkCommand::undo()
{
	// 删除连线
	QByteArray endarr;
	QDataStream stream(data_);
	stream >> endarr;
	BlueprintObject* source = scene_->decodePortFromData(endarr);
	stream >> endarr;
	BlueprintObject* dest = scene_->decodePortFromData(endarr);
	
	// 找到目标Link
	BlueprintLink* link = NULL;
	foreach(BlueprintLink* l, source->links()){
		if (l->destNode() == dest){
			link = l;
			break;
		}
	}
	Q_ASSERT(link != NULL);

	link->showAnchor( false );
	link->clearAnchor();
	link->unlink();
	scene_->removeItem( link );
	delete link;
}

void AddBluePrintLinkCommand::redo()
{
	if (enabled_){ //
		BlueprintLink* link = scene_->addLinkItem(data_);
		Q_ASSERT(link != NULL);
	}
}

//////////////////////////////////////////////////////////////////////////

DeleteBluePrintLinkCommand::DeleteBluePrintLinkCommand(const QList<BlueprintLink*>& links, KismetScene* scene, QUndoCommand *parent)
	: QUndoCommand(parent), scene_(scene)
{
	QByteArray data;
	foreach(BlueprintLink* l, links){
		{
			QDataStream steam(&data, QIODevice::WriteOnly | QIODevice::Truncate);
			l->encode(steam);
		}
		links_.append(data);
	}
}

DeleteBluePrintLinkCommand::~DeleteBluePrintLinkCommand()
{

}

void DeleteBluePrintLinkCommand::undo()
{
	foreach(QByteArray data, links_){
		BlueprintLink* link = scene_->addLinkItem(data);
		Q_ASSERT(link != NULL);
	}
}

void DeleteBluePrintLinkCommand::redo()
{
	QByteArray endarr;
	BlueprintLink* link = NULL;
	foreach(QByteArray data, links_){
		// 删除连线
		QDataStream stream(data);
		stream >> endarr;
		BlueprintObject* source = scene_->decodePortFromData(endarr);
		stream >> endarr;
		BlueprintObject* dest = scene_->decodePortFromData(endarr);

		// 找到目标Link
		link = NULL;
		foreach(BlueprintLink* l, source->links()){
			if (l->destNode() == dest){
				link = l;
				break;
			}
		}
		Q_ASSERT(link != NULL);

		link->showAnchor(false);
		link->clearAnchor();
		link->unlink();
		scene_->removeItem(link);
		delete link;
	}
}

//////////////////////////////////////////////////////////////////////////

ModifyBluePrintLinkCommand::ModifyBluePrintLinkCommand(BlueprintLink* link, QList<QPointF>& oldCtrls, KismetScene* scene, QUndoCommand *parent)
	: QUndoCommand(parent), scene_(scene), oldCtrls_(oldCtrls), enabled_(false)
{
		{
			QDataStream steam(&data_, QIODevice::WriteOnly | QIODevice::Truncate);
			link->encode(steam);
		}
		newCtrls_ = link->controlPoints();
}

ModifyBluePrintLinkCommand::~ModifyBluePrintLinkCommand()
{

}

void ModifyBluePrintLinkCommand::undo()
{
	QByteArray endarr;
	QDataStream stream(data_);
	stream >> endarr;
	BlueprintObject* source = scene_->decodePortFromData(endarr);
	stream >> endarr;
	BlueprintObject* dest = scene_->decodePortFromData(endarr);

	// 找到目标Link
	BlueprintLink* link = NULL;
	foreach(BlueprintLink* l, source->links()){
		if (l->destNode() == dest){
			link = l;
			break;
		}
	}

	Q_ASSERT(link != NULL);
	// 更新控制点
	link->showAnchor(false);
	link->changeControlPoints(oldCtrls_);
	link->adjust();
}

void ModifyBluePrintLinkCommand::redo()
{

	if (enabled_){
		QByteArray endarr;	
		QDataStream stream(data_);
		stream >> endarr;
		BlueprintObject* source = scene_->decodePortFromData(endarr);
		stream >> endarr;
		BlueprintObject* dest = scene_->decodePortFromData(endarr);

		// 找到目标Link
		BlueprintLink* link = NULL;
		foreach(BlueprintLink* l, source->links()){
			if (l->destNode() == dest){
				link = l;
				break;
			}
		}

		Q_ASSERT(link != NULL);
		// 更新控制点
		link->showAnchor(false);
		link->changeControlPoints(newCtrls_);
		link->adjust();
	}

}