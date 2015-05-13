#include "kismetgroupeditwindow.h"
#include "ui_kismetgroupeditwindow.h"
#include "kismetgroupsortwindow.h"
#include "mainkismetframe.h"
#include "framemanager.h"
#include "kismetundocommand.h"
#include "kismetscene.h"

KismetGroupEditWindow::KismetGroupEditWindow(BlueprintGroup* group, QWidget *parent) :
QDialog(parent), group_(group),
    ui(new Ui::KismetGroupEditWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	ui->renamelineEdit->setText(group_->title());

	QColor oldColor = group_->fillColor();

	ui->alphahorizontalSlider->setValue(oldColor.alpha());
	ui->colorhorizontalSlider->setValue(oldColor.hsvHue());

	ui->alphalabel->setText(QString::number(oldColor.alpha()));
	ui->previewwidget->setFillColor(oldColor);
	ui->previewwidget->update();

	ui->activecheckBox->setChecked( group_->isActiveOnStart());
}

KismetGroupEditWindow::~KismetGroupEditWindow()
{
    delete ui;
}

bool KismetGroupEditWindow::canSave(void){
	return true;
}


void KismetGroupEditWindow::on_okpushButton_clicked(){
	if (!canSave())
		return;

	MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
	Q_ASSERT(frame != NULL);

	QByteArray newData;
	{
		QDataStream stream(&newData, QIODevice::WriteOnly);
		stream << ui->renamelineEdit->text();
		stream << ui->previewwidget->fillColor();
		stream << ui->activecheckBox->isChecked();
	}

	ModifyBluePrintModelCommand* command = new ModifyBluePrintModelCommand(group_, newData, (KismetScene*)(group_->scene()));
	frame->undoStack()->push(command);

	accept();
}


void KismetGroupEditWindow::on_cancelpushButton_clicked(){

	reject();
}

void	KismetGroupEditWindow::on_colorhorizontalSlider_valueChanged(int value){
	ui->previewwidget->setFillColor(value, ui->alphahorizontalSlider->value());
	ui->previewwidget->update();
}

void	KismetGroupEditWindow::on_alphahorizontalSlider_valueChanged(int value){
	ui->alphalabel->setText(QString::number(value));
	ui->previewwidget->setFillColor(ui->colorhorizontalSlider->value(), value);
	ui->previewwidget->update();
}

void KismetGroupEditWindow::on_sortpushButton_clicked(){
	// ±£´æÎÄ×Ö
	if (!canSave())
		return;
	group_->setTitle(ui->renamelineEdit->text());

	KismetGroupSortWindow w( (KismetScene*)group_->scene() );
	w.exec();
}