/************************************************************************
**
**  Copyright (C) 2015  guobin  <guobin.gb@alibaba-inc.com>
**
**  This file is part of Orion2.
**
**  Orion2 is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation.
**
**  Orion is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#include "instanceeditwindow.h"
#include "ui_instanceeditwindow.h"
#include "tooldefine.h"
#include "leveldefine.h"
#include "componentdefine.h"
#include "plugintypeeditwindow.h"
#include "editorlayer.h"
#include "mainmanagerframe.h"
#include "framemanager.h"

enum{
	PRT_LAYER,
	PRT_ANGLE,
	PRT_POSX,
	PRT_POSY,
	PRT_SIZEX,
	PRT_SIZEY,
	PRT_INSTANCE,
	PRT_PLUGIN,
	PRT_BEHAVIOR,
	PRT_EFFECT,
	PRT_BLEND,
	PRT_FALLBACK,
	PRT_OPACITY,
};

InstanceEditWindow::InstanceEditWindow(EditorRegion* region, const  QList<ResourceHash>& layers, QWidget *parent) :
QDialog(parent), host_(region), 
    ui(new Ui::InstanceEditWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	foreach( ResourceHash key, layers)
	{
		DEFINE_RESOURCE_HASHKEY_VALID(LayerResourceObject, layer, key);
		layerMap_.insert(layer->description(), key);
	}

	blendMode_.append(UDQ_TR("Normal"));
	blendMode_.append(UDQ_TR("Additive"));
	blendMode_.append(UDQ_TR("XOR"));
	blendMode_.append(UDQ_TR("Copy"));
	blendMode_.append(UDQ_TR("Destination Over"));
	blendMode_.append(UDQ_TR("Source In"));
	blendMode_.append(UDQ_TR("Destination In"));
	blendMode_.append(UDQ_TR("Source Out"));
	blendMode_.append(UDQ_TR("Destination Out"));
	blendMode_.append(UDQ_TR("Source Atop"));
	blendMode_.append(UDQ_TR("Destination Atop"));

	// 初始化属性树
	preparePropertyTree();
}

InstanceEditWindow::~InstanceEditWindow()
{
    delete ui;
}

void  InstanceEditWindow::on_cancelpushButton_clicked(){

	reject();
}

void  InstanceEditWindow::on_savepushButton_clicked(){
	QTreeWidgetItem* item = NULL;
	// 保存基础属性
	// 图层
	item = attrMap_.value(PRT_LAYER);
	if (item){
		// 改变图层
		QString newlayer = ui->treeWidget->getItemValue(item);
		host_->changeToLayer(newlayer);
	}

	item = attrMap_.value(PRT_POSX);
	if (item){
		QPointF newPos;
		newPos.setX(ui->treeWidget->getItemValue(item).toInt());
		item = attrMap_.value(PRT_POSY);
		Q_ASSERT(item != NULL );
		newPos.setY(ui->treeWidget->getItemValue(item).toInt());
		host_->setPos(newPos);
	}

	item = attrMap_.value(PRT_ANGLE);
	if (item){
		int angle = ui->treeWidget->getItemValue(item).toInt();
		host_->setRotation(angle);
	}


	item = attrMap_.value(PRT_SIZEX);
	if (item){
		QSizeF newSize;
		newSize.setWidth(ui->treeWidget->getItemValue(item).toInt());
		item = attrMap_.value(PRT_SIZEY);
		Q_ASSERT(item != NULL);
		newSize.setHeight(ui->treeWidget->getItemValue(item).toInt());
		host_->resize(newSize);
		host_->updateAnchor();
	}

	item = attrMap_.value(PRT_OPACITY);
	if (item){
		qreal opacity = ui->treeWidget->getItemValue(item).toDouble();
		host_->setAlpha(opacity);
	}

	host_->parameters_.clear();

	// 保存属性
	for (int i = 0; i < varRoot_->childCount(); i++){
		item = varRoot_->child(i);
		host_->parameters_.insert(hashString(item->text(0)), ui->treeWidget->getItemValue(item));
	}

	//  保存插件变量值
	QString plugName = host_->typePrototype_->prototype_->objectName();
	for (int i = 0; i < pluginRoot_->childCount(); i++){
		item = pluginRoot_->child(i);
		host_->parameters_.insert(hashString(host_->typePrototype_->prototype_->objectName() + UDQ_T(".") + item->text(0)), ui->treeWidget->getItemValue(item));
	}

	// 保存行为变量
	QTreeWidgetItem* child = NULL;
	for (int i = 0; i < behaviorRoot_->childCount(); i++){
		item = behaviorRoot_->child(i);
		for (int m = 0; m < item->childCount(); m++){
			child = item->child(m);
			host_->parameters_.insert(hashString(item->text(0) + UDQ_T(".") + child->text(0)), ui->treeWidget->getItemValue(child));
		}
	}

	item = attrMap_.value(PRT_BLEND);
	host_->blend_ = ui->treeWidget->getItemIndex(item);
	if (host_->blend_ < 0 ){
		host_->blend_ = 0;
	}

	item = attrMap_.value(PRT_FALLBACK);
	host_->fallback_ = ui->treeWidget->getItemIndex(item);
	if (host_->fallback_ < 0){
		host_->fallback_ = 0;
	}

	// 保存特效变量
	for (int i = 0; i <effectRoot_->childCount(); i++){
		item = effectRoot_->child(i);
		for (int m = 0; m < item->childCount(); m++){
			child = item->child(m);
			host_->parameters_.insert(hashString(item->text(0) + UDQ_T(".") + child->text(0)), ui->treeWidget->getItemValue(child));
		}
	}

	// 更新对象类型列表
	MainManagerFrame* frame = (MainManagerFrame*)gFrameManager->getWindow(FrameManager::WID_LEVEL);
	Q_ASSERT(frame != NULL);
	frame->refreshObjectTypes();

	accept();
}

QTreeWidgetItem* InstanceEditWindow::findItem(const QString& name, QTreeWidgetItem* parent ){

	QTreeWidgetItem* item = NULL;
	for (int i = 0; i < parent->childCount(); i++){
		if (parent->child(i)->text(0) == name){
			item = parent->child(i);
			break;
		}
	}
	return item;
}


void InstanceEditWindow::on_editpushButton_clicked(){
	if (host_ == NULL)
		return;

	Q_ASSERT(host_->typePrototype_ != NULL);
	bool hasImage = host_->typePrototype_->hasImageResource();


	PluginTypeEditWindow w(host_->typePrototype_);
	if (w.exec() == QDialog::Accepted){
		// 对象名
		ui->typelineEdit->setText(host_->typePrototype_->description());

		// 更改大小
		if (!hasImage && host_->typePrototype_->hasImageResource()){  // 第一次设置图像资源

		}

		// 更新属性列表
		QList<QString> oldvars;
		for (int i = 0; i < varRoot_->childCount(); i++){
			oldvars.append(varRoot_->child(i)->text(0));
		}

		QTreeWidgetItem* item = NULL;
		QString value;
		foreach(PluginVariable* def, host_->typePrototype_->variables_.values())
		{
			value = host_->typePrototype_->getParameter(def->name);
			oldvars.removeOne(def->name);

			item = findItem(def->name, varRoot_);
			if (item){
				QString oldtype = item->data(0, ITEM_CONTEXT).toString();
				if (oldtype != def->datatype){
					// 替换控件
					varRoot_->removeChild(item);
					item = NULL;
				}
			}

			if ( item == NULL ){
				item = addPropertyItem(def, value, varRoot_);
			}

			item->setData(0, ITEM_CONTEXT, def->datatype);
			item->setData(0, ITEM_VALUE, PRT_INSTANCE);
		}

		foreach (QString name, oldvars){
			item = findItem(name, varRoot_);
			varRoot_->removeChild(item);
		}

		PluginResourceObject* pluginatt = NULL;
		// 更新行为变量
		oldvars.clear();
		for (int i = 0; i < behaviorRoot_->childCount(); i++){
			oldvars.append(behaviorRoot_->child(i)->text(0));
		}
		foreach(QString name, host_->typePrototype_->behaviors_){
			pluginatt = host_->typePrototype_->attachments_.value(name);
			oldvars.removeOne(name);

			QTreeWidgetItem* behItem = findItem(name, behaviorRoot_);
			if (behItem){
				// 检查类型是否变化
				ResourceHash oldkey = behItem->data(0, ITEM_HASHKEY).toUInt();
				if (oldkey != pluginatt->hashKey()){
					// 删除原有属性
					foreach(QTreeWidgetItem* child, behItem->takeChildren()){
						delete child;
					}
				}
			}
			else{
				behItem = new QTreeWidgetItem(behaviorRoot_, ITEM_BEHAVIOR);
				behItem->setText(0, name);
			}

			behItem->setText(1, pluginatt->title_);
			behItem->setData(0, ITEM_HASHKEY, pluginatt->hashKey());

			if (behItem->childCount() == 0){
				foreach(PluginVariable* def, pluginatt->variables_.values())
				{
					value = host_->typePrototype_->getParameter(def->name, pluginatt->objectName());
					item = addPropertyItem(def, value, behItem);
					if (item){
						item->setData(0, ITEM_VALUE, PRT_BEHAVIOR);
					}
				}
			}
		}

		foreach(QString name, oldvars){
			item = findItem(name, behaviorRoot_);
			behaviorRoot_->removeChild(item);
		}


		// 更新特效变量
		oldvars.clear();
		for (int i = 0; i < effectRoot_->childCount(); i++){
			if (effectRoot_->child(i)->type() == ITEM_EFFECT){
				oldvars.append(effectRoot_->child(i)->text(0));
			}
		}

		foreach(QString name, host_->typePrototype_->effects_){
			pluginatt = host_->typePrototype_->attachments_.value(name);
			oldvars.removeOne(name);

			QTreeWidgetItem* effItem = findItem(name, effectRoot_);
			if (effItem){
				// 检查类型是否变化
				ResourceHash oldkey = effItem->data(0, ITEM_HASHKEY).toUInt();
				if (oldkey != pluginatt->hashKey()){
					// 删除原有属性
					foreach(QTreeWidgetItem* child, effItem->takeChildren()){
						delete child;
					}
				}
			}
			else{
				effItem = new QTreeWidgetItem(effectRoot_, ITEM_EFFECT);
				effItem->setText(0, name);
			}

			effItem->setText(1, pluginatt->title_);
			effItem->setData(0, ITEM_HASHKEY, pluginatt->hashKey());

			if (effItem->childCount() == 0){
				foreach(PluginVariable* def, pluginatt->variables_.values())
				{
					value = host_->typePrototype_->getParameter(def->name, pluginatt->objectName());
					item = addPropertyItem(def, value, effItem);
					if (item){
						item->setData(0, ITEM_VALUE, PRT_EFFECT);
					}
				}
			}
		}

		foreach(QString name, oldvars){
			item = findItem(name, effectRoot_);
			effectRoot_->removeChild(item);
		}

		ui->treeWidget->expandAll();
	}

}

void InstanceEditWindow::preparePropertyTree(){
	if (host_ == NULL)
		return;

	attrMap_.clear();

	Q_ASSERT(host_->typePrototype_ != NULL);
	// 根据标志判断是否具备公共属性
	ui->typelineEdit->setText(host_->typePrototype_->description());

	PluginResourceObject* plugin = host_->typePrototype_->prototype_;  // 插件原型
	Q_ASSERT(plugin != NULL);
	// 公共属性
	QTreeWidgetItem* commonRoot = ui->treeWidget->addGategory(UDQ_TR("通用"));
	QTreeWidgetItem* item = NULL;
	if (plugin->hasAttribute(EF_ZORDER)){
		// 查找所在图层
		QList<QString> layers = layerMap_.keys();
		EditorLayer* layer = (EditorLayer*)host_->parentItem();
		item = ui->treeWidget->addItemCombox(commonRoot, UDQ_TR("图层"), UDQ_TR("所在的图层"), layers, layers.indexOf(layer->layerName()));
		item->setData(0, ITEM_VALUE, PRT_LAYER);
		attrMap_.insert(PRT_LAYER, item);
	}

	if (plugin->hasAttribute(EF_ANGLE)){
		int angle = qRound(host_->rotation());
		item = ui->treeWidget->addItemIntEdit(commonRoot, UDQ_TR("角度"), UDQ_TR("旋转角度（水平向右为0度，顺时针旋转）"), angle);
		item->setData(0, ITEM_VALUE, PRT_ANGLE);
		attrMap_.insert(PRT_ANGLE, item);
	}

	if (plugin->hasAttribute(EF_POSITION)){
		QPoint pos = host_->pos().toPoint();
		item = ui->treeWidget->addItemIntEdit(commonRoot, UDQ_TR("X位置"), UDQ_TR("在图层中的X坐标"), pos.x());
		item->setData(0, ITEM_VALUE, PRT_POSX);
		attrMap_.insert(PRT_POSX, item);
		item = ui->treeWidget->addItemIntEdit(commonRoot, UDQ_TR("Y位置"), UDQ_TR("在图层中的Y坐标"), pos.y());
		item->setData(0, ITEM_VALUE, PRT_POSY);
		attrMap_.insert(PRT_POSY, item);
	}

	if (plugin->hasAttribute(EF_SIZE)){
		QSize size = host_->boundingRect().size().toSize();
		item = ui->treeWidget->addItemIntEdit(commonRoot, UDQ_TR("宽度"), UDQ_TR("在图层中的宽度"), size.width());
		item->setData(0, ITEM_VALUE, PRT_SIZEX);
		attrMap_.insert(PRT_SIZEX, item);
		item = ui->treeWidget->addItemIntEdit(commonRoot, UDQ_TR("高度"), UDQ_TR("在图层中的高度"), size.height());
		item->setData(0, ITEM_VALUE, PRT_SIZEY);
		attrMap_.insert(PRT_SIZEY, item);
	}

	if (plugin->hasAttribute(EF_APPEARANCE)){
		QSize size = host_->boundingRect().size().toSize();
		item = ui->treeWidget->addItemDoubleEdit(commonRoot, UDQ_TR("透明度"), UDQ_TR("在图层中的透明度，取值（0.1-1.0）"), host_->getAlpha());
		item->setData(0, ITEM_VALUE, PRT_OPACITY);
		attrMap_.insert(PRT_OPACITY, item);
	}

	// 实例变量
	varRoot_ = ui->treeWidget->addGategory(UDQ_TR("实例变量"));
	QString value;
	foreach (PluginVariable* def, host_->typePrototype_->variables_.values())
	{
		value = host_->getParameterString(def->name);
		item = addPropertyItem(def, value, varRoot_);

		if (item){
			item->setData(0, ITEM_CONTEXT, def->datatype);  // 记录原来的数据类型
			item->setData(0, ITEM_VALUE, PRT_INSTANCE);
		}
	}

	pluginRoot_ = ui->treeWidget->addGategory(UDQ_TR("原型属性"));
	pluginRoot_->setText(1, host_->typePrototype_->prototype_->title_);
	foreach(PluginVariable* def, host_->typePrototype_->prototype_->variables_.values())
	{
		value = host_->getParameterString(def->name, host_->typePrototype_->prototype_->objectName());
		item = addPropertyItem(def, value, pluginRoot_);
		if (item){
			item->setData(0, ITEM_VALUE, PRT_PLUGIN);
		}
	}

	PluginResourceObject* pluginatt = NULL;
	behaviorRoot_ = ui->treeWidget->addGategory(UDQ_TR("行为"));
	foreach(QString name, host_->typePrototype_->behaviors_){
		pluginatt = host_->typePrototype_->attachments_.value(name);

		QTreeWidgetItem* behItem = new QTreeWidgetItem(behaviorRoot_, ITEM_BEHAVIOR);
		behItem->setText(0, name);
		behItem->setText(1, pluginatt->title_);
		behItem->setData(0, ITEM_HASHKEY, pluginatt->hashKey());

		foreach(PluginVariable* def, pluginatt->variables_.values())
		{
			value = host_->getParameterString(def->name, pluginatt->objectName());
			item = addPropertyItem(def, value, behItem);
			if (item){
				item->setData(0, ITEM_VALUE, PRT_BEHAVIOR);
			}
		}
	}

	effectRoot_ = ui->treeWidget->addGategory(UDQ_TR("特效"));

	// 混合模式
	item = ui->treeWidget->addItemCombox(effectRoot_, UDQ_TR("混合模式"), UDQ_TR("特效混合渲染模式"), blendMode_, host_->blend_);
	item->setData(0, ITEM_VALUE, PRT_BLEND);
	attrMap_.insert(PRT_BLEND, item);

	// 回调函数
	QStringList fbs = blendMode_;
	fbs.append(UDQ_TR("Destroy"));
	item = ui->treeWidget->addItemCombox(effectRoot_, UDQ_TR("回调函数"), UDQ_TR("如果特效不存在，则采取的代替模式"), fbs, host_->fallback_);
	item->setData(0, ITEM_VALUE, PRT_FALLBACK);
	attrMap_.insert(PRT_FALLBACK, item);

	foreach(QString name, host_->typePrototype_->effects_){
		pluginatt = host_->typePrototype_->attachments_.value(name);
		QTreeWidgetItem* effItem = new QTreeWidgetItem(effectRoot_, ITEM_EFFECT);
		effItem->setText(0, name);
		effItem->setText(1, pluginatt->title_);
		effItem->setData(0, ITEM_HASHKEY, pluginatt->hashKey());

		foreach(PluginVariable* def, pluginatt->variables_.values())
		{
			ResourceHash key = hashString(name + UDQ_T(".") + def->name);
			value = host_->getParameterString(def->name, pluginatt->objectName());
			item = addPropertyItem(def, value, effItem);
			if (item){
				item->setData(0, ITEM_VALUE, PRT_EFFECT);
			}
		}
	}

	ui->treeWidget->expandAll();
}


QTreeWidgetItem* InstanceEditWindow::addPropertyItem(PluginVariable* def, const QString& value, QTreeWidgetItem* parentItem){
	QTreeWidgetItem* item = NULL;
	if (def->datatype == UDQ_T("bool")){
		int index = value == UDQ_T("true") ? 1 : 0;
		QStringList items;
		items << UDQ_TR("否") << UDQ_TR("是");
		item = ui->treeWidget->addItemCombox(parentItem, def->name, def->desc, items, index);
	}
	else if (def->datatype == UDQ_T("int")){
		item = ui->treeWidget->addItemIntEdit(parentItem, def->name, def->desc, value.toInt());
	}
	else if (def->datatype == UDQ_T("float")){
		item = ui->treeWidget->addItemDoubleEdit(parentItem, def->name, def->desc, value.toDouble());
	}
	else if (def->datatype == UDQ_T("string")){
		item = ui->treeWidget->addItemLineEdit(parentItem, def->name, def->desc, value);
	}
	else if (def->datatype == UDQ_T("enum")){
		int index = def->enums.indexOf(value);
		item = ui->treeWidget->addItemCombox(parentItem, def->name, def->desc, def->enums, index);
	}
	else if (def->datatype == UDQ_T("percent")){
		item = ui->treeWidget->addItemDoubleEdit(parentItem, def->name, def->desc, value.toDouble());
	}
	else if (def->datatype == UDQ_T("font")){
		item = ui->treeWidget->addItemFont(parentItem, def->name, def->desc, value);
	}
	else if (def->datatype == UDQ_T("color")){
		item = ui->treeWidget->addItemColorPicker(parentItem, def->name, def->desc, value);
	}

	if (item){  // 扩展属性只读
		ui->treeWidget->itemWidget(item, 1)->setEnabled(def->flags == 0 );
	}

	return item;
}

void InstanceEditWindow::on_treeWidget_itemSelectionChanged(){
	QTreeWidgetItem* item = ui->treeWidget->currentItem();

	if (item == NULL){
		ui->desclabel->clear();
	}
	else{
		ui->desclabel->setText(item->toolTip(0));
	}
}