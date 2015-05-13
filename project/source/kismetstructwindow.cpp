#include "kismetstructwindow.h"
#include "ui_kismetstructwindow.h"
#include "tooldefine.h"
#include "framemanager.h"
#include "mainkismetframe.h"

KismetStructWindow::KismetStructWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KismetStructWindow)
{
    ui->setupUi(this);

	 // 初始化对象列表
	updateWorkspace();
}

KismetStructWindow::~KismetStructWindow()
{
    delete ui;
}

void	KismetStructWindow::resizeEvent(QResizeEvent * event){
	QSize newSize = event->size() - QSize(WIDGET_MARGIN * 2, WIDGET_MARGIN * 2);
	ui->layoutWidget->setGeometry(QRect(QPoint(WIDGET_MARGIN, WIDGET_MARGIN), newSize));
}


void KismetStructWindow::on_scriptpushButton_clicked(){
	// 新建脚本
	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	if (ws == NULL){
		QMessageBox::warning(this, UDQ_TR("新建图层"), UDQ_TR("请先打开一个工程!"));
		return;
	}

	// 获取当前电子书
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != 0 && !(proj->flags()  & URF_TagGarbage));

	BlueprintResourceObject* script = proj->addNewScript();
	Q_ASSERT(script != 0);
	ui->scripttreeWidget->addScripttem(script);

	// 设置保存标记
	proj->setFlags(URF_TagSave);

}

void KismetStructWindow::updateScript(ProjectResourceObject* proj){
	ui->scripttreeWidget->init(proj);
}

// 改变工作空间
void KismetStructWindow::updateWorkspace(){

	// 更新态势树
	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	Q_ASSERT(ws != NULL);

	// 获取当前电子书
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != 0 && !(proj->flags()  & URF_TagGarbage));

	init(proj);

}

void KismetStructWindow::init(ProjectResourceObject* proj){
	if (proj == NULL){
		ui->scripttreeWidget->clear();
	}
	else{
		updateScript(proj);
	}

}


void KismetStructWindow::on_scripttreeWidget_itemDoubleClicked(QTreeWidgetItem * item, int column){
	MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
	Q_ASSERT(frame != NULL);

	ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
	DEFINE_RESOURCE_HASHKEY(BlueprintResourceObject, script, key);
	frame->loadScript(script);

}

void KismetStructWindow::on_scripttreeWidget_itemChanged(QTreeWidgetItem * item, int column){
	ResourceHash key = item->data(0, ITEM_HASHKEY).toUInt();
	DEFINE_RESOURCE_HASHKEY(BlueprintResourceObject, script, key);
	script->setDescription(item->text(0));

	MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
	Q_ASSERT(frame != NULL);
	frame->refreshBlueprint();
}

void KismetStructWindow::on_funcpushButton_clicked(){
	QMessageBox::warning(this, windowTitle(), UDQ_TR("此功能尚未实现，敬请等待"));
}