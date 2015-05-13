#include "colorselectwindow.h"
#include "ui_colorselectwindow.h"
#include <QResizeEvent>
#include <QColorDialog>

ColorSelectWindow::ColorSelectWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorSelectWindow)
{
    ui->setupUi(this);
	color_ = Qt::black;
}

ColorSelectWindow::~ColorSelectWindow()
{
    delete ui;
}

void	ColorSelectWindow::resizeEvent(QResizeEvent * event){
	QSize newSize = event->size();
	ui->horizontalLayoutWidget->setGeometry(QRect(QPoint(0, 0), newSize));
}

void  ColorSelectWindow::on_clrpushButton_clicked(){
	color_ = QColorDialog::getColor(color_, this);
	ui->colorlabel->setText(color_.name());
}

void ColorSelectWindow::setColor(const QString& clr){
	if (clr.isEmpty()){
		color_ = Qt::black;
	}
	else{
		color_.setNamedColor(clr);
	}
	ui->colorlabel->setText(color_.name());
}

QString ColorSelectWindow::getColorString(){
	return color_.name();
}