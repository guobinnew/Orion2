#include "addanimationanchorwindow.h"
#include "ui_addanimationanchorwindow.h"

AddAnimationAnchorWindow::AddAnimationAnchorWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddAnimationAnchorWindow)
{
    ui->setupUi(this);
}

AddAnimationAnchorWindow::~AddAnimationAnchorWindow()
{
    delete ui;
}
