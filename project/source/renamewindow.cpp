#include "renamewindow.h"
#include "ui_renamewindow.h"
#include <QMessageBox>

RenameWindow::RenameWindow(ResourceObject* host, bool createNew , QWidget *parent) :
QDialog(parent), host_(host), createNew_(createNew), newObj_(NULL),
    ui(new Ui::RenameWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	if (createNew){
			setWindowTitle(UDQ_TR("复制"));
	}

	QRegExp rx(REG_NAMEASCII);
	QValidator *validator = new QRegExpValidator(rx, this);
	ui->renamelineEdit->setValidator(validator);
	ui->renamelineEdit->setText(host->objectName());
}

RenameWindow::~RenameWindow()
{
    delete ui;
}

void RenameWindow::on_okpushButton_clicked()
{
	QString newName = ui->renamelineEdit->text();

	// 检查合法性
	if (newName.isEmpty()){
		QMessageBox::warning(this, windowTitle(), tr("名称不能为空.\n"));
		return;
	}


	// 检查是否相同
	if (newName == host_->objectName()){
		accept();
		return;
	}

	// 检查唯一性
	QString path = host_->outer() ? host_->outer()->pathName() : UDQ_T("");
	QString tmpPath = QString(UDQ_T("%1@%2.%3")).arg(host_->getRTTI()->getName()).arg(path).arg(newName);

	if (ResourceObject::findResource(hashString(tmpPath))){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("名称有重名，请重新输入.\n"));
		return;
	}

	if (createNew_){
		// 创建新对象
		newObj_ = host_->clone(newName);
	}
	else{
		// 执行修改
		host_->rename(newName);
		host_->setFlags(URF_TagSave);
	}

	accept();
}