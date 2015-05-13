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

#include "editorviewport.h"
#include <QtGui>
#include <math.h>
#include "levelscene.h"
#include "commondefine.h"
#include "configmanager.h"
#include "devicemanager.h"
#include "imagehelper.h"
using namespace ORION;


const int VIEWPORT_FONTSIZE = 12;
const int VIEWPORT_MARGIN = 8;
EditorViewportText::EditorViewportText(const QString& text, QPointF pos, QGraphicsItem *parent) : QGraphicsSimpleTextItem(parent){

	setZValue(LEVEL_DEPTH_VIEWPORT);
	setFlags(ItemIgnoresTransformations);
	setText(text);
	setPos(pos);
	setBrush(Qt::black);

	QFont font;
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPixelSize(VIEWPORT_FONTSIZE);
	setFont(font);
	setVisible(false);

}

void EditorViewportText::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget){
	QGraphicsSimpleTextItem::paint(painter, item, widget);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

EditorViewport::EditorViewport(const QString& device, qreal zoom, qreal rot, QGraphicsItem * parent)
	:QGraphicsItem(parent), zoom_(zoom), rotation_(rot){

	if (device.isEmpty()){
		device_ = gDeviceManager->defaultDevice();
	}
	else{
		device_ = device;
	}

	baseSize_ = gDeviceManager->deviceResolution(device_);
	boundingBox_.setRect(-baseSize_.width() / 2.0f, -baseSize_.height() / 2.0f, baseSize_.width(), baseSize_.height());

	// 设置深度值
	setZValue(LEVEL_DEPTH_VIEWPORT);

	// 设置标志
	setFlag(ItemIsMovable, true);
	setFlag(ItemIsSelectable, true);
	setFlag(ItemIsFocusable, true);
	setFlag(ItemSendsGeometryChanges, true);

	setTransformOriginPoint(QPointF(0, 0));
	setRotation(rotation_);
	setScale(zoom_);

	text_ = new EditorViewportText(QString(), QPointF(VIEWPORT_MARGIN, VIEWPORT_MARGIN), this);
	minimized_ = false;
	image_.load(UDQ_T(":/images/viewport.png"));

	// 接受鼠标经过事件
	setAcceptHoverEvents(true);

	// 不接受拖放
	setAcceptDrops(false);
	setVisible(true);

}

EditorViewport::~EditorViewport(){

}

void	EditorViewport::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event){
	setMinimized(!minimized_);
	update();
}

QPainterPath EditorViewport::shape() const{
	QPainterPath path;
	path.addRect(boundingBox_);
	return path;
}

QRectF EditorViewport::boundingRect() const{
	return boundingBox_;
}

void EditorViewport::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	Q_UNUSED(widget);

	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->setRenderHint(QPainter::HighQualityAntialiasing, true);


	painter->fillRect(boundingBox_, VIEWPORT_FILLCOLOR);
	QPen pen(Qt::black);
	pen.setWidth(minimized_? 1.0 : 2.0);
	// 如果是选择状态
	if (isSelected()){
		pen.setColor(Qt::yellow);
	}
	painter->setPen(pen);
	painter->drawRect(boundingBox_);

	if (minimized_){
		painter->drawImage(boundingBox_.adjusted(8, 8, -8, -8), image_);
	}

}


// 进入最小化状态
void EditorViewport::setMinimized(bool enable){
	minimized_ = enable;

	prepareGeometryChange();

	if (enable){
		setFlag(QGraphicsItem::ItemIsMovable, false);
		setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
		setRotation(0);
		setScale(1.0);
		oldPos_ = pos();
		boundingBox_.setRect(0, 0, 64, 64);
		text_->setVisible(false);
	}
	else{
		setFlag(QGraphicsItem::ItemIsMovable, true);
		setFlag(QGraphicsItem::ItemIgnoresTransformations, false);
		setRotation(rotation_);
		setScale(zoom_);
		boundingBox_.setRect(-baseSize_.width() / 2.0f, -baseSize_.height() / 2.0f, baseSize_.width(), baseSize_.height());
		setPos(oldPos_);
		text_->setVisible(true);
		adjustText();
	}

	setSelected(false);

}

QVariant EditorViewport::itemChange(GraphicsItemChange change, const QVariant &value){

	if (change == QGraphicsItem::ItemSelectedHasChanged  && scene() && !minimized_) {
		text_->setVisible(value.toBool());
		if (text_->isVisible()){
			text_->setText(currentTitleString());
			adjustText();
		}
	}
	else if (change == QGraphicsItem::ItemPositionHasChanged  && scene() && !minimized_) {
		if (text_->isVisible()){
			text_->setText(currentTitleString());
			adjustText();
		}
	}

	return QGraphicsItem::itemChange(change, value);
}

QString  EditorViewport::currentTitleString(){
	return QString(UDQ_TR("设备: %6 \n大小: %1x%2  \n比例: %3倍  \n旋转: %7度\n位置: %4 , %5"))
		.arg(baseSize_.width())
		.arg(baseSize_.height())
		.arg(QString::number(zoom_, 'f', 2))
		.arg(QString::number(pos().x(), 'f', 2))
		.arg(QString::number(pos().y(), 'f', 2))
		.arg(device_)
		.arg(rotation_);
}

void EditorViewport::adjustText(){

	//  根据旋转角度找到最右边
	QList<QPointF> pts;
	pts.append(boundingBox_.topLeft());
	pts.append(boundingBox_.topRight());
	pts.append(boundingBox_.bottomLeft());
	pts.append(boundingBox_.bottomRight());

	QPointF topright;
	for (int i = 0; i < pts.size(); i++) {
		QPointF mappt = mapToScene(pts[i]);
		if (i == 0 || mappt.x() > topright.x()){
			topright.setX(mappt.x());
		}

		if (i == 0 || mappt.y() < topright.y()){
			topright.setY(mappt.y());
		}
	}

	topright.setX(topright.x() + 4);
	text_->setPos(mapFromScene(topright));

}