#include "nwjssettingwindow.h"
#include "ui_nwjssettingwindow.h"

NWJSSettingWindow::NWJSSettingWindow(NWJSExporter* expoter, QWidget *parent) :
QDialog(parent), exporter_(expoter),
    ui(new Ui::NWJSSettingWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

}

NWJSSettingWindow::~NWJSSettingWindow()
{
    delete ui;
}


void NWJSSettingWindow::on_savepushButton_clicked(){
	int flags = 0;

	if (ui->singlecheckBox->isChecked()){
		flags |= NWJSExporter::WMD_SINGLE;
	}

	if (ui->framecheckBox->isChecked()){
		flags |= NWJSExporter::WMD_FRAME;
	}

	if (ui->resizecheckBox->isChecked()){
		flags |= NWJSExporter::WMD_RESIZE;
	}

	if (ui->kioskcheckBox->isChecked()){
		flags |= NWJSExporter::WMD_KIOSK;
	}
	exporter_->setWindowFlags(flags);


	if (ui->w32checkBox->isChecked()){
		exporter_->addPlatform(NWJSExporter::PLT_WIN32);
	}
	if (ui->w64checkBox->isChecked()){
		exporter_->addPlatform(NWJSExporter::PLT_WIN64);
	}

	if (ui->l32checkBox->isChecked()){
		exporter_->addPlatform(NWJSExporter::PLT_LINUX32);
	}
	if (ui->l64checkBox->isChecked()){
		exporter_->addPlatform(NWJSExporter::PLT_LINUX64);
	}

	if (ui->o32checkBox->isChecked()){
		exporter_->addPlatform(NWJSExporter::PLT_OSX32);
	}
	if (ui->o64checkBox->isChecked()){
		exporter_->addPlatform(NWJSExporter::PLT_OSX64);
	}

	accept();
}

void NWJSSettingWindow::on_cancelpushButton_clicked(){
	reject();
}