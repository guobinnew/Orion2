#include "selectobjecttypewindow.h"
#include "ui_selectobjecttypewindow.h"
#include "tooldefine.h"


SelectObjectTypeWindow::SelectObjectTypeWindow(QWidget *parent) :
QDialog(parent),
    ui(new Ui::SelectObjectTypeWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	// 构建对象类型树
	ui->treeWidget->setDragEnabled(false);
	ui->treeWidget->setAssociateItem(false);
	// 保存到文件
	WorkspaceResourceObject*  ws = ResourceObject::currentWorkspace();
	Q_ASSERT(ws != NULL);
	ProjectResourceObject* proj = ws->currentProject();
	if (proj){
		ui->treeWidget->init(proj);
	}
}

SelectObjectTypeWindow::~SelectObjectTypeWindow()
{
    delete ui;
}


void SelectObjectTypeWindow::on_treeWidget_itemClicked(QTreeWidgetItem * item, int column){
	// 检查是否选择合法对象
	if ( item->type() != ITEM_OBJECTTYPE &&  item->type() != ITEM_FAMILY)
		return;
	//
	typeHash_ = item->data(0, ITEM_HASHKEY).toUInt();
	accept();
}
