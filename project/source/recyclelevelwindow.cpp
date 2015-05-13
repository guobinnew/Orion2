#include "recyclelevelwindow.h"
#include "ui_recyclelevelwindow.h"

#include "tooldefine.h"

RecycleLevelWindow::RecycleLevelWindow(ProjectResourceObject* project, QWidget *parent) :
QDialog(parent),
    ui(new Ui::RecycleLevelWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	QListWidgetItem* item = NULL;
	foreach(LevelResourceObject* level, project->levels_){
		if (level->flags() & URF_TagGarbage){
			item = new QListWidgetItem();
			item->setText(level->description());
			item->setData(ITEM_HASHKEY, level->hashKey());
			ui->listWidget->addItem(item);
		}
	}

}

RecycleLevelWindow::~RecycleLevelWindow()
{
    delete ui;
}

void RecycleLevelWindow::on_listWidget_itemDoubleClicked(QListWidgetItem * item){
	if (item == NULL)
		return;

	recycleLevel(item);
	accept();
}


void RecycleLevelWindow::on_okpushButton_clicked(){

	QListWidgetItem* item = ui->listWidget->currentItem();
	if (item == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请选择一个场景"));
		return;
    }

	recycleLevel(item);
	accept();
}

void RecycleLevelWindow::recycleLevel(QListWidgetItem* item){
	ResourceHash key = item->data(ITEM_HASHKEY).toUInt();
	DEFINE_RESOURCE_HASHKEY_VALID(LevelResourceObject, l, key);
	level_ = l;

	level_->clearFlags(URF_TagGarbage);
	level_->setFlags(URF_TagSave);
}
