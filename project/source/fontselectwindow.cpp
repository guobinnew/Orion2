#include "fontselectwindow.h"
#include "ui_fontselectwindow.h"
#include <QFontDialog>
#include <QResizeEvent>

FontSelectWindow::FontSelectWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FontSelectWindow)
{
    ui->setupUi(this);
}

FontSelectWindow::~FontSelectWindow()
{
    delete ui;
}

void	FontSelectWindow::resizeEvent(QResizeEvent * event){
	QSize newSize = event->size();
	ui->horizontalLayoutWidget->setGeometry(QRect(QPoint(0, 0), newSize));
}

void  FontSelectWindow::on_fontpushButton_clicked(){
	QFontDialog c(font_, this);
	if (c.exec() == QDialog::Accepted){
		font_ = c.currentFont();
		ui->fontlabel->setText(font_.toString());
	}
}

void FontSelectWindow::setFontString(const QString& fnt){
	font_.fromString(fnt);
	ui->fontlabel->setText(font_.toString());
}

QString FontSelectWindow::getFontString(){
	return font_.toString();
}