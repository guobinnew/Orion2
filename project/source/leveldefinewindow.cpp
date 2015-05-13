#include "leveldefinewindow.h"
#include "ui_leveldefinewindow.h"
#include "tooldefine.h"

LevelDefineWindow::LevelDefineWindow(LevelResourceObject* level, QWidget *parent) :
QDialog(parent), level_(level),
    ui(new Ui::LevelDefineWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	ui->namelineEdit->setText(level_->description());
	ui->marginwspinBox->setValue(level_->margin_.width());
	ui->marginhspinBox->setValue(level_->margin_.height());

	ui->widthspinBox->setValue(level_->size_.width());
	ui->heightspinBox->setValue(level_->size_.height());

	ui->scrollcheckBox->setChecked(level_->unboundScroll_);

	// 准备脚本列表
	WorkspaceResourceObject* ws = ResourceObject::currentWorkspace();
	Q_ASSERT(ws != NULL);
	// 获取当前电子书
	ProjectResourceObject* proj = ws->currentProject();
	Q_ASSERT(proj != 0 && !(proj->flags()  & URF_TagGarbage));

	QStringList items;
	foreach(BlueprintResourceObject* blueprint, proj->blueprints_){
		if (blueprint->flags() & URF_TagGarbage)
			continue;

		items.append(blueprint->description());
		scriptMap_.insert( blueprint->hashKey(), blueprint->description() );
	}
	ui->scriptcomboBox->addItems(items);
	ui->scriptcomboBox->setCurrentText(scriptMap_.value(level_->blueprint_));

}

LevelDefineWindow::~LevelDefineWindow()
{
    delete ui;
}

bool LevelDefineWindow::canSave(){
	LINEEDIT_CHECK(ui->namelineEdit, UDQ_TR("场景属性设置"), UDQ_TR("名称"));
	return true;
}

void  LevelDefineWindow::on_savepushButton_clicked(){

	level_->setDescription(ui->namelineEdit->text());
	level_->margin_.setWidth(ui->marginwspinBox->value());
	level_->margin_.setHeight(ui->marginhspinBox->value());
	level_->size_.setWidth(ui->widthspinBox->value());
	level_->size_.setHeight(ui->heightspinBox->value());

	QString script = ui->scriptcomboBox->currentText();
	if (script.isEmpty()){
		level_->blueprint_ = 0;
	}
	else{
		QMapIterator<ResourceHash, QString> iter(scriptMap_);
		while (iter.hasNext()) {
			iter.next();
			if (iter.value() == script){
				level_->blueprint_ = iter.key();
				break;
			}
		}
	}

	level_->setFlags(URF_TagSave);
	accept();
}

void  LevelDefineWindow::on_cancelpushButton_clicked(){
	reject();
}