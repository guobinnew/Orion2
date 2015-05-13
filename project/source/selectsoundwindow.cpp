#include "selectsoundwindow.h"
#include "ui_selectsoundwindow.h"
#include "sounddefinewindow.h"
#include <QMessageBox>
#include "tooldefine.h"


SelectSoundWindow::SelectSoundWindow(ResourceHash key, QWidget *parent) :
QDialog(parent), soundKey_(0),
    ui(new Ui::SelectSoundWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	QTreeWidgetItem* item = NULL;
	// 加载列表
	for (TResourceObjectIterator<SoundResourceObject> it; it; ++it){
		SoundResourceObject* sound = (SoundResourceObject*)(*it);

		if (sound->flags() & URF_TagGarbage)
			continue;

		QString desc = sound->description();
		if (desc.isEmpty()){
			desc = sound->objectName();
			sound->setDescription(desc);
			sound->setFlags(URF_TagSave);
		}

		item = ui->treeWidget->addSoundItem(sound);
		if (sound->hashKey() == key){
			item->setSelected(true);
		}
	}
}

SelectSoundWindow::~SelectSoundWindow()
{
    delete ui;
}


void SelectSoundWindow::on_savepushButton_clicked(){
	QTreeWidgetItem* currentItem = ui->treeWidget->currentItem();
	if (!currentItem || currentItem->type() != ITEM_SOUND){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请选择一个声音"));
		return;
	}
	soundKey_ = currentItem->data(0, ITEM_HASHKEY).toUInt();
	accept();
}

void SelectSoundWindow::on_cancelpushButton_clicked(){
	reject();
}

void SelectSoundWindow::on_addpushButton_clicked(){
	// 新建声音
	SoundDefineWindow w(NULL);
	if (w.exec() == QDialog::Accepted){
		Q_ASSERT(w.sound_ != NULL);
		ui->treeWidget->addSoundItem(w.sound_);
	}
}