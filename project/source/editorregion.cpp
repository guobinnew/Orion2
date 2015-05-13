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
#include "editorregion.h"
#include <QtGui>
#include <QDialog>
#include <QVector2D>
#include <QMenu>
#include <math.h>
#include "levelscene.h"
#include "commondefine.h"
#include "instanceeditwindow.h"
#include "menumanager.h"
using namespace ORION;


EditorRegion::EditorRegion(ObjectTypeResourceObject* prototype, QGraphicsItem * parent)
	:EditorItem(parent), borderColor_(BORDER_COLOR), typePrototype_(prototype), blend_(0),fallback_(0){
	setZValue(EDITOR_DEPTH_DEFAULT);
}


EditorRegion::EditorRegion(const QRectF& rect, const QSizeF& origin,  QGraphicsItem * parent)
	:EditorItem(rect, origin, parent), borderColor_(BORDER_COLOR), blend_(0), fallback_(0){
}

EditorRegion::~EditorRegion(){

}

void EditorRegion::drawShape(QPainter *painter){

	QPen borderPen;
	// 绘制边框颜色
	borderPen.setColor(BORDER_COLOR);
	borderPen.setStyle(Qt::DashDotLine);
	painter->setPen(borderPen);

	// 获取所在图层
	painter->setOpacity(getParentOpacity());
	// 绘制矩形
	painter->drawRect(boundingBox_);
}

// 编码
void EditorRegion::encodeData(QDataStream& stream){
	EditorItem::encodeData(stream);
	stream << typePrototype_->hashKey();
	stream << parameters_;
	stream << blend_;
	stream << fallback_;

}

void EditorRegion::decodeData(QDataStream& stream){

	EditorItem::decodeData(stream);
	ResourceHash key;
	stream >> key;
	typePrototype_ = UDynamicCast(ObjectTypeResourceObject, (ResourceObject::findResource(key)));
	Q_ASSERT(typePrototype_ != NULL);

	stream >> parameters_;
	stream >> blend_;
	stream >> fallback_;

}

// 缺省数据
void EditorRegion::encodeDefault(QDataStream& stream){
	EditorItem::encodeDefault(stream);

	stream << parameters_;
	stream << blend_;
	stream << fallback_;
}

void EditorRegion::decodeDefault(QDataStream& stream){
	EditorItem::decodeDefault(stream);

	stream >> parameters_;
	stream >> blend_;
	stream >> fallback_;
}

void EditorRegion::changeToLayer(const QString& layer){
	if (layer.isEmpty() || scene() == NULL ){
		return;
	}

	LevelScene* level = (LevelScene*)scene();

	// 检查当前所在图层
	EditorLayer* oldLayer = (EditorLayer*)parentItem();
	if ( oldLayer == NULL ||  oldLayer->layer_->description() != layer){
		// 改变图层
		EditorLayer* newLayer = level->getLayer(layer);
		Q_ASSERT(newLayer != NULL);
		oldLayer->removeInstance(this);
		newLayer->addInstance(this);
	}
}

void EditorRegion::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event){
	Q_ASSERT(scene());
	//// 启动编辑窗口
	//QList<ResourceHash> layers = ((LevelScene*)scene())->layerIds();
	//InstanceEditWindow w(this, layers);
	//if (w.exec() == QDialog::Accepted){
	//	// 更新属性设置
	//	postinit(false);
	//	update();
	//}
}

void EditorRegion::setParameter(const QString& prop, const QString& value, const QString& plugin){
	ResourceHash key = 0;
	if (plugin.isEmpty()){
		key = hashString(prop);
	}
	else{
		key = hashString(plugin + UDQ_T(".") + prop);
	}
	parameters_.insert(key, value);
}

QString EditorRegion::getParameterString(const QString& name, const QString& plugin){
	QString v;
	QString path = plugin.isEmpty() ? name : plugin + UDQ_T(".") + name;
	ResourceHash key = hashString(path);
	// 检查当前属性值
	if (parameters_.contains(key)){
		v = parameters_.value(key);
	}
	else{
		v = typePrototype_->getParameter(name, plugin);
	}
	return v;
}

int EditorRegion::getParameterInt(const QString& name, const QString& plugin){
	QString v = getParameterString(name, plugin);
	return v.toInt();
}

double EditorRegion::getParameterDouble(const QString& name, const QString& plugin){
	QString v = getParameterString(name, plugin);
	return v.toDouble();
}

bool EditorRegion::getParameterBool(const QString& name, const QString& plugin){
	QString v = getParameterString(name, plugin);
	return v.toLower() == UDQ_T("true");
}

int EditorRegion::getParameterEnum(const QString& name, const QString& plugin ){
	QString v = getParameterString(name, plugin);
	// 根据枚举索引
	// 找到插件
	PluginResourceObject* p = typePrototype_->getPlugin(plugin);
	if (p == NULL)
		return -1;

	PluginVariable* var = p->getVariable(name);
	Q_ASSERT(var != NULL);

	return var->enums.indexOf(v);
}


void EditorRegion::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	((LevelScene*)scene())->setMenuHost(this);

	QMenu menu;
	QAction* act= gMenuManager->getAction(ORION_EDIT_LOCK);
	act->setChecked(isLocking());
	menu.addAction(act);

	menu.addSeparator();
	act = gMenuManager->getAction(ORION_EDIT_CLONE);
	act->setChecked(isLocking());
	menu.addAction(act);

	menu.addSeparator();
	act = gMenuManager->getAction(ORION_EDIT_COPY);
	act->setChecked(isLocking());
	menu.addAction(act);

	menu.addSeparator();
	act = gMenuManager->getAction(ORION_EDIT_TODEFAULTOBJ);
	act->setChecked(isLocking());
	menu.addAction(act);

	act = gMenuManager->getAction(ORION_EDIT_FROMDEFAULTOBJ);
	act->setChecked(isLocking());
	menu.addAction(act);

	menu.addSeparator();
	act = gMenuManager->getAction(ORION_EDIT_MOVEFIRST);
	menu.addAction(act);

	act = gMenuManager->getAction(ORION_EDIT_MOVELAST);
	menu.addAction(act);

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	menu.exec(event->screenPos());
}

void EditorRegion::saveDefault(){
	Q_ASSERT(typePrototype_ != NULL);
	typePrototype_->defaultData_ = encode(true);

}

void EditorRegion::loadDefault(){
	Q_ASSERT(typePrototype_ != NULL);
	if (typePrototype_->defaultData_.size() > 0){
		decode(typePrototype_->defaultData_, true);
	}
}

qreal EditorRegion::getParentOpacity(){
	QGraphicsItem* parent = parentItem();
	if (parent && parent->type() == LEVEL_LAYER){
		return ((EditorLayer*)parent)->layer_->opacity_ / 100.0;
	}
	return 1.0;
}
