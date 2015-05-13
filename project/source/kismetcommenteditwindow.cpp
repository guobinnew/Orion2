#include "kismetcommenteditwindow.h"
#include "ui_kismetcommenteditwindow.h"
#include "mainkismetframe.h"
#include "framemanager.h"
#include "kismetundocommand.h"
#include "kismetscene.h"

KismetCommentEditWindow::KismetCommentEditWindow(BlueprintComment* comment, QWidget *parent) :
QDialog(parent), comment_(comment),
    ui(new Ui::KismetCommentEditWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	ui->plainTextEdit->setPlainText(comment_->title());
	QColor oldColor = comment_->textColor();
	ui->colorhorizontalSlider->setValue(oldColor.hsvHue());
}

KismetCommentEditWindow::~KismetCommentEditWindow()
{
    delete ui;
}

bool KismetCommentEditWindow::canSave(void){
	return true;
}

void KismetCommentEditWindow::on_okpushButton_clicked(){
	if (!canSave())
		return;

	MainKismetFrame* frame = (MainKismetFrame*)gFrameManager->getWindow(FrameManager::WID_BLUEPRINT);
	Q_ASSERT(frame != NULL);

	QByteArray newData;
	{
		QDataStream stream(&newData, QIODevice::WriteOnly);
		stream << ui->plainTextEdit->toPlainText();
		stream << ui->previewwidget->fillColor();
	}

	ModifyBluePrintModelCommand* command = new ModifyBluePrintModelCommand(comment_, newData, (KismetScene*)(comment_->scene()));
	frame->undoStack()->push(command);

	accept();
}

void KismetCommentEditWindow::on_cancelpushButton_clicked(){

	reject();
}

void KismetCommentEditWindow::on_colorhorizontalSlider_valueChanged(int value){
	ui->previewwidget->setFillColor(value, 255);
	ui->previewwidget->update();
}
