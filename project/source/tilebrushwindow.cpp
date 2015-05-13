#include "tilebrushwindow.h"
#include "ui_tilebrushwindow.h"

TileBrushWindow::TileBrushWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TileBrushWindow)
{
    ui->setupUi(this);
}

TileBrushWindow::~TileBrushWindow()
{
    delete ui;
}
