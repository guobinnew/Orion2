#include "entitycomponentwindow.h"
#include "ui_entitycomponentwindow.h"
#include "tooldefine.h"
#include <QListWidget>
#include <QVBoxLayout>
#include <QJsonDocument>
#include "configmanager.h"
#include "componentlistwidget.h"
#include "logmanager.h"

EntityComponentWindow::EntityComponentWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EntityComponentWindow)
{
    ui->setupUi(this);

	defIcon_ = QIcon(UDQ_T(":/images/component.png"));

	while (ui->toolBox->count() > 0 )
		ui->toolBox->removeItem(0);

	gLogManager->log(UDQ_TR("开始准备组件列表..."));
	refreshComponentList();
	gLogManager->log(UDQ_TR("组件列表准备完成"));
}

EntityComponentWindow::~EntityComponentWindow()
{
    delete ui;
}

void	EntityComponentWindow::resizeEvent(QResizeEvent * event){
	QSize newSize = event->size() - QSize(WIDGET_MARGIN * 2, WIDGET_MARGIN * 2);
	ui->layoutWidget->setGeometry(QRect(QPoint(WIDGET_MARGIN, WIDGET_MARGIN), newSize));
}

void EntityComponentWindow::addListWidget(QListWidget* widget, int index){

	Q_ASSERT(ui->toolBox->count() > index);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(widget);
	ui->toolBox->widget(index)->setLayout(layout);
}

void EntityComponentWindow::clear(){

	QListWidget* w = NULL;
	for (int i = 0; i < ui->toolBox->count(); i++){
		w = (QListWidget*)ui->toolBox->widget(i);
		w->clear();
	}
}

// 准备组件列表
void EntityComponentWindow::refreshComponentList(){
	clear();

	// 加载列表
	for (TResourceObjectIterator<PluginResourceObject> it; it; ++it){
		PluginResourceObject* plugin = (PluginResourceObject*)(*it);

		if (plugin->flags() & URF_TagGarbage || plugin->type_ == PIT_NATIVE || plugin->type_ == PIT_EFFECT || plugin->type_ == PIT_BEHAVIOR )
			continue;

		QString cate;
		Q_ASSERT(plugin->categories_.size() > 0);
		cate = plugin->categories_[0];
		Q_ASSERT(cate != NULL);

		//  查找
		QListWidget* w = findToolBoxList(cate, true);

		QListWidgetItem* item = new QListWidgetItem();
		item->setText(plugin->title_);

		if (plugin->icon_.isNull()){
			item->setIcon(defIcon_);
		}
		else{
			item->setIcon(QPixmap::fromImage(plugin->icon_));
		}
		item->setData(ITEM_HASHKEY, plugin->hashKey());
		item->setToolTip(plugin->description());
		w->addItem(item);
	}

}

QListWidget* EntityComponentWindow::findToolBoxList(const QString& name, bool create){
	QListWidget* w = NULL;
	for (int i = 0; i < ui->toolBox->count(); i++){
		if (ui->toolBox->itemText(i) == name){
			w = (QListWidget*)ui->toolBox->widget(i);
			break;
		}
	}

	if (w == NULL && create ){
		w = new ComponentListWidget();
		ui->toolBox->addItem(w, name);
	}

	return w;
	
}


void EntityComponentWindow::on_refreshpushButton_clicked(){
	refreshComponentList();
}