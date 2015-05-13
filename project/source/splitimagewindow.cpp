#include "splitimagewindow.h"
#include "ui_splitimagewindow.h"

SplitImageWindow::SplitImageWindow(ResourceHash key, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SplitImageWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	DEFINE_RESOURCE_HASHKEY_VALID(ImageResourceObject, img, key);
	image_ = img;

	// 显示图像
	ui->previewwidget->showPicture(img->content_);
	ui->previewwidget->showFrames(image_->image_.size(), QSize(0,0));

	// 显示信息
	QSize imgsize = ui->previewwidget->getPicture().size();
	QString info = UDQ_TR("%1: 图像大小 %2x%3")
		.arg(img->objectName())
		.arg(imgsize.width())
		.arg(imgsize.height());
	ui->infolabel->setText(info);

	ui->widthspinBox->setMinimum(1);
	ui->heightspinBox->setMinimum(1);
	if (imgsize.width() < 8){
		ui->widthspinBox->setMaximum(1);
		ui->wspacespinBox->setEnabled(false);
	}
	else{
		ui->widthspinBox->setMaximum(imgsize.width() / 8);
	}
	if (imgsize.height() < 8){
		ui->heightspinBox->setMaximum(1);
		ui->hspacespinBox->setEnabled(false);
	}
	else{
		ui->heightspinBox->setMaximum(imgsize.height() / 8);
	}

}

SplitImageWindow::~SplitImageWindow()
{
    delete ui;
}


void SplitImageWindow::on_savepushButton_clicked(){

	int num = ui->numspinBox->value();
	image_->frames_.clear();
	if (num== 0){
		image_->isGroup_ = false;
	}
	else{
		image_->isGroup_= true;
		for (int i = 0; i < num; i++){
			image_->frames_.append(ui->previewwidget->getFrames().at(i));
		}
	}

	image_->clearFlags(URF_TagGarbage);
	image_->setFlags(URF_TagSave);
	// 保存到本地
	SAVE_PACKAGE_RESOURCE(ImageResourceObject, image_);

	accept();
}

void SplitImageWindow::on_cancelpushButton_clicked(){

	reject();
}

void SplitImageWindow::on_gridradioButton_clicked(bool checked ){
	if (checked){
		ui->widthspinBox->setMinimum(1);
		ui->heightspinBox->setMinimum(1);
		ui->wspacespinBox->setEnabled(false);
		ui->hspacespinBox->setEnabled(false);

		QSize imgsize = ui->previewwidget->getPicture().size();

		if (imgsize.width() < 8){
			ui->widthspinBox->setMaximum(1);
		}
		else{
			ui->widthspinBox->setMaximum(imgsize.width() / 8);
		}
		if (imgsize.height() < 8){
			ui->heightspinBox->setMaximum(1);
		}
		else{
			ui->heightspinBox->setMaximum(imgsize.height() / 8);
		}

		updatePreview();
	}
}

void SplitImageWindow::on_sizeradioButton_clicked(bool checked ){
	if (checked){

		ui->widthspinBox->setMinimum(8);
		ui->heightspinBox->setMinimum(8);
		ui->wspacespinBox->setEnabled(true);
		ui->hspacespinBox->setEnabled(true);

		QSize imgsize = ui->previewwidget->getPicture().size();

		if (imgsize.width() < 8){
			ui->widthspinBox->setMaximum(8);
			ui->wspacespinBox->setEnabled(false);
		}
		else{
			ui->widthspinBox->setMaximum(imgsize.width());
		}
		if (imgsize.height() < 8){
			ui->heightspinBox->setMaximum(8);
			ui->hspacespinBox->setEnabled(false);
		}
		else{
			ui->heightspinBox->setMaximum(imgsize.height());
		}

		updatePreview();
	}
}


void SplitImageWindow::updatePreview(){
	QSize imgsize = ui->previewwidget->getPicture().size();
	if (ui->gridradioButton->isChecked()){
		ui->previewwidget->showFrames(QSize(imgsize.width() / ui->widthspinBox->value(), imgsize.height() / ui->heightspinBox->value()),
			QSize(0,0));
	}
	else{
		ui->previewwidget->showFrames(QSize(ui->widthspinBox->value(), ui->heightspinBox->value()), 
			QSize(ui->wspacespinBox->value(), ui->hspacespinBox->value()));
	}

	int frameNum = ui->previewwidget->getFrames().size();
	if (frameNum == 0){
		ui->numspinBox->setMinimum(0);
		ui->numspinBox->setMaximum(0);
	}
	else{
		ui->numspinBox->setMinimum(1);
		ui->numspinBox->setMaximum(frameNum);
	}
	ui->numspinBox->setValue(frameNum);
}

void SplitImageWindow::on_widthspinBox_valueChanged(int i){
	// 刷新图像显示
	updatePreview();
}

void SplitImageWindow::on_wspacespinBox_valueChanged(int i){
	// 刷新图像显示
	updatePreview();
}

void SplitImageWindow::on_heightspinBox_valueChanged(int i){
	// 刷新图像显示
	updatePreview();
}

void SplitImageWindow::on_hspacespinBox_valueChanged(int i){
	// 刷新图像显示
	updatePreview();
}
