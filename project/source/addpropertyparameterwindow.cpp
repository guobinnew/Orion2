#include "addpropertyparameterwindow.h"
#include "ui_addpropertyparameterwindow.h"

AddPropertyParameterWindow::AddPropertyParameterWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddPropertyParameterWindow)
{
    ui->setupUi(this);
}

AddPropertyParameterWindow::~AddPropertyParameterWindow()
{
    delete ui;
}
