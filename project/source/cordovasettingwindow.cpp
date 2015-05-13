#include "cordovasettingwindow.h"
#include "ui_cordovasettingwindow.h"

CordovaSettingWindow::CordovaSettingWindow(CordovaExporter* exporter, QWidget *parent) :
QDialog(parent), exporter_(exporter),
    ui(new Ui::CordovaSettingWindow)
{
    ui->setupUi(this);
	setFixedSize(size());


}

CordovaSettingWindow::~CordovaSettingWindow()
{
    delete ui;
}


void CordovaSettingWindow::on_savepushButton_clicked(){

	exporter_->setDevice(ui->devcomboBox->currentIndex());

	exporter_->setFullscreen(ui->fscheckBox->isChecked());

	int flags = 0;
	if (ui->geocheckBox->isChecked()){
		flags |= CordovaExporter::PRM_GEO;
	}
	if (ui->vibratecheckBox->isChecked()){
		flags |= CordovaExporter::PRM_VIBRATE;
	}
	if (ui->cameracheckBox->isChecked()){
		flags |= CordovaExporter::PRM_CAMERA;
	}
	if (ui->mediacheckBox->isChecked()){
		flags |= CordovaExporter::PRM_MEIDA;
	}
	exporter_->setPermission(flags);

	flags = 0;
	if (ui->m4acheckBox->isChecked()){
		flags |= CordovaExporter::AUDIO_M4A;
	}
	if (ui->oggcheckBox->isChecked()){
		flags |= CordovaExporter::AUDIO_OGG;
	}
	exporter_->setAudioFormat(flags);

	exporter_->setiOSVersion(ui->ioscomboBox->currentIndex());
	exporter_->setAndriodVersion(ui->andcomboBox->currentIndex());

	accept();
}

void CordovaSettingWindow::on_cancelpushButton_clicked(){
	reject();
}