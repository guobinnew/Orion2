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


#ifndef ORION_KISMET_UNDOCOMMAND_H
#define ORION_KISMET_UNDOCOMMAND_H

#include <QUndoCommand>
#include <QByteArray>
#include "kismetscene.h"
#include "kismetobject.h"
#include "resourceobject.h"
using namespace ORION;


//////////////////////////////////////////////////////////////////////////
// 变量
//////////////////////////////////////////////////////////////////////////

class AddVariableCommand : public QUndoCommand{
public:
	AddVariableCommand(QString& name, BlueprintResourceObject* scipt, QUndoCommand *parent = 0);
	~AddVariableCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

private:
	BlueprintResourceObject* script_;
	QString name_;
};

class DeleteVariableCommand : public QUndoCommand{
public:
	DeleteVariableCommand(ResourceHash key, BlueprintResourceObject* scipt, QUndoCommand *parent = 0);
	~DeleteVariableCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

	QList<QByteArray>  models_;
	QList<QByteArray> links_;

private:
	BlueprintResourceObject* script_;
	QString name_;
	QString datatype_;
	QString value_;
	QString desc_;
};



class RenameVariableCommand : public QUndoCommand{
public:
	RenameVariableCommand(ResourceHash key, const QString& newName, BlueprintResourceObject* scipt, QUndoCommand *parent = 0);
	~RenameVariableCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

private:
	BlueprintResourceObject* script_;
	QString oldName_;  // 旧值
	QString newName_;  // 新值

};

class ModifyVariableCommand : public QUndoCommand{
public:
	ModifyVariableCommand(ResourceHash key, int type, const QString& newValue, BlueprintResourceObject* scipt, QUndoCommand *parent = 0);
	~ModifyVariableCommand();

	enum{
		MVC_DATATYPE = 0,
		MVC_VALUE,
	};

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

private:
	BlueprintResourceObject* script_;
	QString name_;
	int type_;
	QString oldValue_;  // 旧值
	QString newValue_;  // 新值

};

///////////////////////////////////////////////////////////////////////////
// 模型
//////////////////////////////////////////////////////////////////////////

class AddBluePrintModelCommand : public QUndoCommand{
public:
	AddBluePrintModelCommand(BlueprintObject* obj, KismetScene* scene, QUndoCommand *parent = 0);
	~AddBluePrintModelCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

	void setEnable(bool flag){
		enabled_ = flag;
	}

private:
	KismetScene* scene_; 
	ResourceHash iid_;
	QByteArray data_;
	bool enabled_;
};


//////////////////////////////////////////////////////////////////////////
class CopyBluePrintModelCommand : public QUndoCommand{
public:
	CopyBluePrintModelCommand(BlueprintObject* obj, const QPointF& offset, KismetScene* scene, QUndoCommand *parent = 0);
	~CopyBluePrintModelCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

	ResourceHash newIID(){
		return iid_;
	}

private:
	KismetScene* scene_;
	ResourceHash iid_;
	QPointF offset_;
	QByteArray data_;
};


class DeleteBluePrintModelCommand : public QUndoCommand{
public:
	DeleteBluePrintModelCommand(BlueprintObject* obj,  KismetScene* scene, QUndoCommand *parent = 0);
	~DeleteBluePrintModelCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

private:
	KismetScene* scene_;
	ResourceHash groupid_;
	ResourceHash iid_;
	QByteArray data_;
	QList<QByteArray> links_;  // 连线
};


class MoveBluePrintModelCommand : public QUndoCommand{
public:
	MoveBluePrintModelCommand(BlueprintObject* obj, const QByteArray& oldData, KismetScene* scene, QUndoCommand *parent = 0);
	~MoveBluePrintModelCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

	void setEnable(bool flag){
		enabled_ = flag;
	}
private:
	KismetScene* scene_;
	ResourceHash iid_;
	QByteArray oldData_;
	QByteArray newData_;
	bool enabled_;
};


class ResizeBluePrintGroupCommand : public QUndoCommand{
public:
	ResizeBluePrintGroupCommand(BlueprintGroup* obj, const QByteArray& oldData, KismetScene* scene, QUndoCommand *parent = 0);
	~ResizeBluePrintGroupCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

	void setEnable(bool flag){
		enabled_ = flag;
	}
private:
	KismetScene* scene_;
	ResourceHash iid_;
	QSizeF oldSize_;
	QSizeF newSize_;
	bool enabled_;
};

//////////////////////////////////////////////////////////////////////////
// 端口
//////////////////////////////////////////////////////////////////////////

// 修改模型端口（增加，删除）
class AddBluePrintModelPortCommand : public QUndoCommand{
public:
	AddBluePrintModelPortCommand(BlueprintModelPort* port,  KismetScene* scene, QUndoCommand *parent = 0);
	~AddBluePrintModelPortCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

	void setEnable(bool flag){
		enabled_ = flag;
	}

private:
	KismetScene* scene_;
	ResourceHash iid_;
	QByteArray data_;
	bool enabled_;
};

class DeleteBluePrintModelPortCommand : public QUndoCommand{
public:
	DeleteBluePrintModelPortCommand(BlueprintModelPort* port, KismetScene* scene, QUndoCommand *parent = 0);
	~DeleteBluePrintModelPortCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

	// 应用数据
	void applyData(QByteArray& data);
private:
	KismetScene* scene_;
	ResourceHash iid_;
	QByteArray data_;
	QList<QByteArray> links_;  // 连线
};




///////////////////////////////////////////////////////////////////////////
// 组/注释
//////////////////////////////////////////////////////////////////////////

// 修改模型属性（组，注释）
class ModifyBluePrintModelCommand : public QUndoCommand{
public:
	ModifyBluePrintModelCommand(BlueprintObject* obj, const QByteArray& newData, KismetScene* scene, QUndoCommand *parent = 0);
	~ModifyBluePrintModelCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

	// 应用数据
	void applyData(QByteArray& data);
private:
	KismetScene* scene_;
	ResourceHash iid_;
	QByteArray oldData_;
	QByteArray newData_;

};

///////////////////////////////////////////////////////////////////////////
// 连线
//////////////////////////////////////////////////////////////////////////

class AddBluePrintLinkCommand : public QUndoCommand{
public:
	AddBluePrintLinkCommand(BlueprintLink* link, KismetScene* scene, QUndoCommand *parent = 0);
	AddBluePrintLinkCommand(const QByteArray& data, KismetScene* scene, QUndoCommand *parent = 0);
	~AddBluePrintLinkCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

	void setEnable(bool flag){
		enabled_ = flag;
	}

private:
	KismetScene* scene_;
	QByteArray data_;
	bool enabled_;
};


class DeleteBluePrintLinkCommand : public QUndoCommand{
public:
	DeleteBluePrintLinkCommand(const QList<BlueprintLink*>& links, KismetScene* scene, QUndoCommand *parent = 0);
	~DeleteBluePrintLinkCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

private:
	KismetScene* scene_;
	QList<QByteArray> links_;  // 连线
};

class ModifyBluePrintLinkCommand : public QUndoCommand{
public:
	ModifyBluePrintLinkCommand(BlueprintLink* link, QList<QPointF>& oldCtrls, KismetScene* scene, QUndoCommand *parent = 0);
	~ModifyBluePrintLinkCommand();

	void undo() Q_DECL_OVERRIDE;
	void redo() Q_DECL_OVERRIDE;

	void setEnable(bool flag){
		enabled_ = flag;
	}
private:
	KismetScene* scene_;
	QByteArray data_;
	QList<QPointF> oldCtrls_;
	QList<QPointF> newCtrls_;
	bool enabled_;
};



#endif // ORION_TILELISTWIDGET_H
