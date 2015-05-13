#include "inputloginwindow.h"
#include "ui_inputloginwindow.h"
#include "configmanager.h"
#include "commondefine.h"
#include <QMessageBox>

InputLoginWindow::InputLoginWindow(QWidget *parent) :
QDialog(parent),
    ui(new Ui::InputLoginWindow)
{
    ui->setupUi(this);
	setFixedSize(size());
	connect(ui->loginwidget, SIGNAL(inputPwd(const QByteArray&)), this, SLOT(processAuthorization(const QByteArray&)));
	updateLabel();

}

InputLoginWindow::~InputLoginWindow()
{
    delete ui;
}

void InputLoginWindow::processAuthorization(const QByteArray& data){

	if (data.size() == 0){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("输入图案不符合要求，请重新输入"));
		firstCode_.clear();
		ui->loginwidget->setLock(true);
	}
	else{

		if (firstCode_.size() == 0){
			firstCode_ = data;
			ui->loginwidget->setLock(true);
		}
		else {
			if (firstCode_ != data){
				firstCode_.clear();
				ui->loginwidget->setLock(true);
			}
			else{
				// 设置code
				gConfigManager->setLoginCode(firstCode_);
				accept();
			}
		}

	}

	updateLabel();

}

void InputLoginWindow::updateLabel(){

	if (firstCode_.size() == 0){
		ui->infolabel->setText(UDQ_TR("首次使用, 请输入解锁图案（至少连接4个点）"));
	}
	else{
		ui->infolabel->setText(UDQ_TR("请再次输入解锁图案（至少连接4个点）"));
	}

}
