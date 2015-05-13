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

#include "kismetinstancewindow.h"
#include "ui_kismetinstancewindow.h"
#include "tooldefine.h"
#include "framemanager.h"
#include "mainkismetframe.h"
#include "kismetundocommand.h"
#include "kismetinstancevar.h"
#include "kismetscene.h"
#include "kismetvariable.h"
#include "kismetlinkarray.h"

KismetInstanceWindow::KismetInstanceWindow(QWidget *parent) :
QWidget(parent), script_(NULL),
    ui(new Ui::KismetInstanceWindow)
{
    ui->setupUi(this);

	connect(ui->treeWidget, SIGNAL(dataTypeChanged(unsigned int, const QString&)), this, SLOT(dataTypeChanged(unsigned int, const QString&)));
	connect(ui->treeWidget, SIGNAL(valueChanged(unsigned int, const QString&)), this, SLOT(valueChanged(unsigned int, const QString&)));

}

KismetInstanceWindow::~KismetInstanceWindow()
{
    delete ui;
}

void	KismetInstanceWindow::resizeEvent(QResizeEvent * event){
	QSize newSize = event->size() - QSize(WIDGET_MARGIN * 2, WIDGET_MARGIN * 2);
	ui->layoutWidget->setGeometry(QRect(QPoint(WIDGET_MARGIN, WIDGET_MARGIN), newSize));
}

void KismetInstanceWindow::init(BlueprintResourceObject* script){
	script_ = script;
	ui->treeWidget->init(script_);
}

void KismetInstanceWindow::on_addpushButton_clicked(){
	if (script_ == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请先打开一个脚本"));
		return;
	}

	// 生成新变量名
	QString newName = script_->makeVariableName();
	MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
	Q_ASSERT(frame != NULL);
	QUndoCommand* command = new AddVariableCommand(newName, script_);
	frame->undoStack()->push(command);
	
}

void KismetInstanceWindow::addVariable(ScriptVariable* var){
	ui->treeWidget->addVariableItem(var);
}

void KismetInstanceWindow::deleteVariable(ScriptVariable* var){
	ui->treeWidget->deleteVariableItem(var);
}

void KismetInstanceWindow::refreshVariable(ScriptVariable* var){
	ui->treeWidget->refreshVariableItem(var);
}

void KismetInstanceWindow::on_delpushButton_clicked(){
	if (script_ == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请先打开一个脚本"));
		return;
	}

	QTreeWidgetItem* item = ui->treeWidget->currentItem();
	if (item){

		if (QMessageBox::question(this, windowTitle(), UDQ_TR("确认删除变量%1吗（脚本中的变量对象也全部被删除）?").arg(item->text(0)), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No){
			return;
		}

		// 删除编辑器中所有变量
		ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
		MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
		Q_ASSERT(frame != NULL);
		ScriptVariable* var = script_->variables_.value(key);
		DeleteVariableCommand* command = new DeleteVariableCommand(key, script_);
		frame->undoStack()->push(command);
	}
}

void KismetInstanceWindow::on_treeWidget_itemChanged(QTreeWidgetItem * item, int column){
	qDebug() << "on_treeWidget_itemChanged";

	if ( script_ && item && column == 0){
		ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
		ScriptVariable* var = script_->variables_.value(key);
		Q_ASSERT(var != NULL);

		QString newName = item->text(0);
		// 检查变量重名
		if (ui->treeWidget->containVariable(newName) > 1){
			QMessageBox::warning(this, windowTitle(), UDQ_TR("变量名<%1>已经存在!").arg(newName));
			item->setText(0,var->name);  // 恢复原来名字
		}

		if (var->name != newName){
			MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
			Q_ASSERT(frame != NULL);
			QUndoCommand* command = new RenameVariableCommand(key, newName, script_);
			frame->undoStack()->push(command);
		}
	}
}

void KismetInstanceWindow::dataTypeChanged(unsigned int key, const QString& dt){
	MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
	Q_ASSERT(frame != NULL);

	QUndoCommand* command = new ModifyVariableCommand(key, ModifyVariableCommand::MVC_DATATYPE, dt, script_);
	frame->undoStack()->push(command);

}
void KismetInstanceWindow::valueChanged(unsigned int key, const QString& v){

	MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
	Q_ASSERT(frame != NULL);

	QUndoCommand* command = new ModifyVariableCommand(key, ModifyVariableCommand::MVC_VALUE, v, script_);
	frame->undoStack()->push(command);
}
