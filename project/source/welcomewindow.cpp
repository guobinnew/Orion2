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

#include "welcomewindow.h"
#include "ui_welcomewindow.h"
#include "tooldefine.h"
#include "configmanager.h"
#include "projectmanager.h"
#include "resourceobject.h"
#include "resourcelinker.h"
#include "framemanager.h"
#include "mainmanagerframe.h"
#include <QFileDialog>
using namespace ORION;

WelcomeWindow::WelcomeWindow(QWidget *parent) :
QDialog(parent),
    ui(new Ui::WelcomeWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	defaultIcon_ = QIcon(UDQ_T(":/images/default_project.png"));
	prepareNewProjectList();

	int num = prepareExistProjectList();
	if ( num == 0 ){
		ui->newpushButton->setChecked(true);
		ui->stackedWidget->setCurrentIndex(0);
	}
}

WelcomeWindow::~WelcomeWindow()
{
    delete ui;
}

void  WelcomeWindow::on_browserpushButton_clicked(){
	// 从目录打开项目
	QString pathName = QFileDialog::getExistingDirectory(this, UDQ_TR("选择项目目录"));
	if (pathName.isEmpty()){
		return;
	}

	// 打开项目
	if (openProjectFromDir(pathName)){
		emit openProject();
		accept();
	}

}

void WelcomeWindow::on_namelineEdit_textChanged(const QString & text){
	// 同步更新项目路径
	QString path = QString(UDQ_T("%1/%2")).arg(gConfigManager->baseWorkspacePath()).arg(hashString(text));
	ui->pathlineEdit->setText(path);
}

void  WelcomeWindow::on_refreshpushButton_clicked(){
	// 刷新当前目录，更新项目列表
	prepareExistProjectList();

}

void  WelcomeWindow::on_projectpushButton_clicked(){
	ui->stackedWidget->setCurrentIndex(1);
}

void  WelcomeWindow::on_newpushButton_clicked(){
	ui->stackedWidget->setCurrentIndex(0);
}

bool WelcomeWindow::openProjectFromDir(const QString& path, const QString& name){
	// 关闭当前工作空间
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);
	// 关闭当前工作空间
	frame->closeCurrentWorkspace();

	QString wsName = name;
	if (wsName.isEmpty()){
		QDir dir(path);  // 临时以目录名为空间名，在loadConfig中修改为正确空间名
		wsName = dir.dirName();
	}
	// 创建工作空间
	WorkspaceResourceObject* ws = ResourceObject::createWorkspace(wsName);
	Q_ASSERT(ws != 0);
	ws->dirPath_ = path;
	// 从配置文件中获取缩略图
	bool r = ws->loadConfig();

	if (!r){
		QMessageBox::warning(this, UDQ_TR("打开项目"), UDQ_TR("打开项目文件失败，请检查文件是否完整!"));
		ResourceObject::resetWorkspace(ws);
		return false;
	}

	//  加载空间（完成数据库初始化）
	ResourceObject::loadWorkspace(ws, RLF_LoadIndex);

	// 激活电子书，书名与工作空间相同
	ProjectResourceObject*  proj = ws->findProject(ws->objectName(), true);
	Q_ASSERT(proj != NULL);
	Q_ASSERT(proj->getLinker() != NULL);

	// 重新加载（完整数据）
	ResourceLinker* linker = proj->getLinker();
	Q_ASSERT(linker != 0);
	linker->load<ProjectResourceObject>(proj->hashKey(), RLF_None);

	ws->activiteProject(proj->hashKey());

	return true;
}

void WelcomeWindow::on_projectlistWidget_itemDoubleClicked(QListWidgetItem * item){
	if (item == NULL)
		return;

	QString path = item->data(ITEM_CONTEXT).toString();
	if (openProjectFromDir(path, item->text())){
		emit openProject();
		accept();
	}

}

// 准备新项目列表
void WelcomeWindow::prepareNewProjectList(){
	ui->typelistWidget->clear();

	foreach (unsigned int key, gProjectManager->projectTypes())
	{
		QStringList info = gProjectManager->projectInfo(key);
		if (info.size() >= 6){
			QListWidgetItem* project = new QListWidgetItem();
			project->setIcon(QIcon(UDQ_T(":/images/") + info[1]));
			project->setText(info[0]);
			project->setData(ITEM_HASHKEY, key);
			project->setToolTip(info[2]);

			QSize s;
			s.setWidth(info[3].toUInt());
			s.setHeight(info[4].toUInt());
			project->setData(ITEM_SCREEN, s);
			project->setData(ITEM_ASPECT, info[5].toUInt());
			ui->typelistWidget->addItem(project);
		}
	}
}

void WelcomeWindow::on_createpushButton_clicked(){

	if (!checkValid())
		return;

	QListWidgetItem* item = ui->typelistWidget->currentItem();
	Q_ASSERT(item != NULL);
	// 项目类型

	// 创建工作空间
	WorkspaceResourceObject* ws = ResourceObject::createWorkspace(ui->namelineEdit->text());
	Q_ASSERT(ws != 0);
	ws->dirPath_ = ui->pathlineEdit->text();
	ws->projectType_ = item->data(ITEM_HASHKEY).toUInt();

	// 生成配置文件（project.cfg）
	ws->saveConfig();

	//  加载空间（完成数据库初始化）
	ResourceObject::loadWorkspace(ws, RLF_None);

	// 创建新电子书，书名与工作空间相同
	ProjectResourceObject*  proj = ws->findProject(ws->objectName(), true);
	Q_ASSERT(proj != NULL);
	proj->setFlags(URF_TagSave);

	// 根据类型设置项目缺省配置
	// 项目尺寸
	proj->resolution_ = item->data(ITEM_SCREEN).toSize();
	// 是否固定长宽比
	proj->aspect_ = item->data(ITEM_ASPECT).toUInt() == 1;

	proj->createDate_ = QDateTime::currentDateTime();
	ws->activiteProject(proj->hashKey());
	SAVE_WORKSPACE_RESOURCE(ProjectResourceObject, proj);

	// 更新项目列表
	prepareExistProjectList();

	// 打开新项目
	emit openProject();
	accept();
}

bool WelcomeWindow::checkValid(){
	QListWidgetItem* item = ui->typelistWidget->currentItem();
	if (item == NULL){
		QMessageBox::warning(this, UDQ_TR("新建项目"), UDQ_TR("请选择一个项目类型!"));
		return false;
	}

	LINEEDIT_CHECK(ui->pathlineEdit, UDQ_TR("新建项目"), UDQ_TR("项目路径"));
	LINEEDIT_CHECK(ui->namelineEdit, UDQ_TR("新建项目"), UDQ_TR("项目名"));

	// 检查工作空间是否存在
	QString wsName = ui->namelineEdit->text();
	if (wsName.indexOf(UDQ_T("@")) >= 0){
		QMessageBox::warning(this, UDQ_TR("新建项目"), UDQ_TR("项目名中不能包含@字符!").arg(wsName));
		return false;
	}

	// 检查目录中是否为空
	QString path = ui->pathlineEdit->text();
	QDir dir(ui->pathlineEdit->text());

	if (dir.exists()){
		QStringList children = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
		if (children.size() > 0){
			if (QMessageBox::question(this, UDQ_TR("新建项目"), UDQ_TR("项目目录[ %1 ]不为空,是否继续(目录被清空)?").arg(dir.absolutePath())) == QMessageBox::No)
				return false;

			// 清空目录
			deleteDir(dir.absolutePath(), false);
		}
	}
	else{
		if (!dir.mkpath(path)){
			QMessageBox::warning(this, UDQ_TR("新建项目"), UDQ_TR("项目 目录[%1]创建失败!").arg(dir.absolutePath()));
			return false;
		}
	}

	return true;

}


int  WelcomeWindow::prepareExistProjectList(){
	ui->projectlistWidget->clear();

	QString wspath = gConfigManager->baseWorkspacePath();
	Q_ASSERT(!wspath.isEmpty());

	int projnum = 0;
	// 递归目录
	QDir dir(wspath);
	if (dir.exists()){
		// 从文件加载缩略图
		WorkspaceResourceObject* ws = ResourceObject::tempWorkspace();

		QStringList children = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs );
		if (children.size() > 0){

			QString projPath;
			foreach(QString name, children){

				projPath = wspath + UDQ_T("/") + name;
				ws->dirPath_ = projPath;
				if (ws->loadConfig()){
					QListWidgetItem* item = new QListWidgetItem();
					item->setText(ws->tmpName_);
					item->setIcon(defaultIcon_);
					item->setData(ITEM_DESC, ws->description());
					item->setData(ITEM_CONTEXT, projPath);
					ui->projectlistWidget->addItem(item);
					projnum++;
				}
			}
		
		}
	}

	return projnum;
}