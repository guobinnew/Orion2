#include "moviedefinewindow.h"
#include "ui_moviedefinewindow.h"

MovieDefineWindow::MovieDefineWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MovieDefineWindow)
{
    ui->setupUi(this);
}

MovieDefineWindow::~MovieDefineWindow()
{
    delete ui;
}
