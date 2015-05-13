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
#include "editoritem.h"
#include <QtGui>
#include <QDialog>
#include <QVector2D>
#include <math.h>
#include "levelscene.h"
#include "commondefine.h"
using namespace ORION;

const int ANCHOR_SIZE = 2;
const float ROTATE_RATE = 0.05;

EditorAnchor::EditorAnchor(EditorItem* host, int index, QGraphicsItem* parent) : QGraphicsItem( parent), host_(host),anchorIndex_(index){
	setZValue(EDITOR_DEPTH_ANCHOR);
	GraphicsItemFlags flags = ItemIgnoresTransformations;
	if (anchorIndex_ != ANCHOR_CE){
		flags |= ItemIsMovable;
	}
	setFlags(flags);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	setVisible(false);
	setAcceptHoverEvents(true);
	boundingBox_ = QRectF(-ANCHOR_SIZE, -ANCHOR_SIZE, ANCHOR_SIZE * 2, ANCHOR_SIZE * 2);
	underMouse_ = false;
}

EditorAnchor::~EditorAnchor(){

}

void EditorAnchor::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget){

	QPen borderPen;
	if (isSelected() || underMouse_){
		// 绘制选择框颜色
		borderPen.setColor(Qt::yellow);
	}
	else{
		borderPen.setColor(Qt::white);
	}
	painter->setPen(borderPen);

	// 填充色
	QPainterPath path;
	if (anchorIndex_ != ANCHOR_CE){
		path.addRect(boundingBox_.adjusted(-2, -2, 2, 2));
	}
	else{
		path.addRect(boundingBox_.adjusted(-1, -1, 1, 1));
	}
	painter->fillPath(path, anchorIndex_ == ANCHOR_RO ? Qt::red : Qt::blue);

	painter->drawPath(path);
}


QRectF EditorAnchor::boundingRect() const{
	return boundingBox_;
}

QPainterPath EditorAnchor::shape() const{
	QPainterPath path;
	path.addRect(boundingBox_);
	return path;
}

void EditorAnchor::hoverEnterEvent(QGraphicsSceneHoverEvent * event){
	underMouse_ = true;
	setCursor(Qt::ArrowCursor);
	if (host_){
		hostIsLock_ = host_->isLocking();
		host_->cacheAspect();
		host_->setFlag(QGraphicsItem::ItemIsMovable, false);


	}
	QGraphicsItem::hoverEnterEvent(event);
	update();
}

void EditorAnchor::hoverLeaveEvent(QGraphicsSceneHoverEvent * event){
	underMouse_ = false;
	if (host_){
		host_->setFlag(QGraphicsItem::ItemIsMovable, !hostIsLock_);
	}
	QGraphicsItem::hoverLeaveEvent(event);
	update();
}


QVariant EditorAnchor::itemChange(GraphicsItemChange change, const QVariant & value){

	if (change == ItemPositionChange && scene()){
		// 如果位置发生改变，修改轨迹
		Q_ASSERT(host_ != NULL);
		if (!host_->isAdjusting_){
			QPointF newPos = host_->updateAnchor(anchorIndex_, value.toPointF());
			return newPos;
		}
	}

	
	return QGraphicsItem::itemChange(change, value);
}

void EditorAnchor::mousePressEvent(QGraphicsSceneMouseEvent * event){
	// 记录当前host的旧值
	if (host_){
		oldValues_ = host_->encode();
	}

	QGraphicsItem::mousePressEvent(event);
}

void EditorAnchor::mouseReleaseEvent(QGraphicsSceneMouseEvent * event){
	// 提交一次属性修改undo请求
	if (host_ && scene()){
		((LevelScene*)scene())->notifyInstanceModified(host_, oldValues_);
	}

	QGraphicsItem::mouseReleaseEvent(event);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

EditorItem::EditorItem(QGraphicsItem * parent)
	:QGraphicsItem(parent), isLocking_(false), sizeFlags_(ANCHORFLAG_RESIZE_ALL), opacity_(1.0), hotspot_(0,0), canflip_(true){
	iid_ = makeUniqueId();
	boundingBox_.setRect(0, 0, 32, 32);
	init();

}

EditorItem::EditorItem(const QRectF& rect, const QSizeF& origin, QGraphicsItem * parent)
	:QGraphicsItem(parent), isLocking_(false), opacity_(1.0), sizeFlags_(ANCHORFLAG_RESIZE_ALL), hotspot_(0,0), canflip_(true){
	iid_ = makeUniqueId();
	boundingBox_ = rect;
	init();
}

EditorItem::~EditorItem(){

}


void EditorItem::init(){
	isAdjusting_ = false;

	// 设置深度值
	setZValue(EDITOR_DEPTH_ITEM);

	// 设置标志
	setFlag(ItemIsMovable, true);
	setFlag(ItemIsSelectable, true);
	setFlag(ItemIsFocusable, true);
	setFlag(ItemSendsGeometryChanges, true);

	// 接受鼠标经过事件
	setAcceptHoverEvents(true);

	// 不接受拖放
	setAcceptDrops(false);
	setVisible(true);
}

QPainterPath EditorItem::shape() const{
	QPainterPath path;
	path.addRect(boundingBox_);
	return path;
}

QRectF EditorItem::boundingRect() const{
	return boundingBox_;
}

QRectF EditorItem::selectBound(){
	return boundingBox_.adjusted(-ANCHOR_SIZE, -ANCHOR_SIZE, ANCHOR_SIZE, ANCHOR_SIZE);
}

QPointF EditorItem::rotateAnchor(){
	return mapToScene(anchorPos(ANCHOR_CE, boundingBox_));
}

void EditorItem::drawShape(QPainter *painter){

	QPen borderPen;
	// 绘制边框颜色
	borderPen.setColor(BORDER_COLOR);
	borderPen.setStyle(Qt::DashDotLine);
	painter->setPen(borderPen);

	// 绘制矩形
	painter->drawRect(boundingBox_);

	// 绘制选择框
	if (isSelected()){
		painter->drawRect(selectBound());
	}

}

void EditorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	Q_UNUSED(widget);

	//painter->setRenderHint(QPainter::Antialiasing, true);
	//painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

	// 绘制基本形状
	drawShape(painter );

}

// 编码
QByteArray EditorItem::encode(bool def ){
	QByteArray arr;
	{
		QDataStream stream(&arr, QIODevice::WriteOnly);
		if (def){
			encodeDefault(stream);
		}
		else{
			encodeData(stream);
		}

	}
	return arr;
}

// 编码
void EditorItem::encodeData(QDataStream& stream){
	stream << type();
	unsigned int ver = LEVEL_DATA_VERSON;
	stream << ver;
	stream << iid_;
	stream << boundingBox_;
	stream << pos();
	stream << rotation();
	stream << hotspot_;
	stream << isLocking_;
	stream << opacity_;

}

void EditorItem::decode(QByteArray& data, bool def ){
	QDataStream stream(data);
	if (def){
		decodeDefault(stream);
	}
	else{
		decodeData(stream);
	}
	postinit(false);
}

void EditorItem::decodeData(QDataStream& stream){
	int t;
	stream >> t;
	Q_ASSERT(t == type());

	stream >> version_;  // 当前数据版本
	stream >> iid_;

	prepareGeometryChange();
	stream >> boundingBox_;
	
	QPointF pos;
	stream >> pos;
	setPos(pos);
	
	qreal angle;
	stream >> angle;
	setRotation(angle);

	stream >> hotspot_;
	stream >> isLocking_;
	setFlag(QGraphicsItem::ItemIsMovable, !isLocking_);

	stream >> opacity_;

}


QVariant EditorItem::itemChange(GraphicsItemChange change, const QVariant & value){

	if (change == QGraphicsItem::ItemSelectedHasChanged && scene()){
		// 通知场景显示隐藏锚点
		if (isSelected()){
			showAnchor(true);
		}
		else{
			hideAnchor();
		}
	}
	else if (change == QGraphicsItem::ItemPositionHasChanged && scene()){
		if (!isAdjusting_){
			isAdjusting_ = true;
			updateAnchor();
			((LevelScene*)scene())->notifyUpdateProperty(this);
			isAdjusting_ = false;
		}
	}
	return QGraphicsItem::itemChange(change, value);
}

void EditorItem::keyPressEvent(QKeyEvent * event){

	if (!isLocking_){
		if (event->key() == Qt::Key_Up){
			setPos(pos() - QPointF(0, 1));
		}
		else if (event->key() == Qt::Key_Down){
			setPos(pos() + QPointF(0, 1));
		}
		else if (event->key() == Qt::Key_Left){
			setPos(pos() - QPointF(1, 0));
		}
		else if (event->key() == Qt::Key_Right){
			setPos(pos() + QPointF(1, 0));
		}
	}
}

bool EditorItem::isLocking(){
	return isLocking_;
}

void EditorItem::toggleLock(){
	isLocking_ = !isLocking_;
	setFlag(QGraphicsItem::ItemIsMovable, !isLocking_);
}

// 返回支持的锚点ID
QList<int> EditorItem::anchors(){
	QList<int> ids;
	for (int i = 0, mask =1; i < ANCHOR_NUM; ++i, mask <<=1 ){  // 最多16个点
		if (sizeFlags_ & mask){
			ids.append(i);
		}
	}
	return ids;
}

void EditorItem::showAnchor(bool visible){
	initAnchor();

	isAdjusting_ = true;
	foreach(int id, anchors()){
		Q_ASSERT(anchors_.size() > id);
		anchors_[id]->setPos(anchorPos(id, selectBound()));
		anchors_[id]->setVisible(true);
	}
	isAdjusting_ = false;
}

// 返回锚点位置
QPointF EditorItem::anchorPos(int id, const QRectF& bound ){
	QPointF pos;
	switch (id)
	{
	case ANCHOR_LT:
	{
		pos = bound.topLeft();
	}
	break;
	case ANCHOR_RT:
	{
		pos = bound.topRight();
	}
	break;
	case ANCHOR_LB:
	{
		pos = bound.bottomLeft();
	}
	break;
	case ANCHOR_RB:
	{
		pos = bound.bottomRight();
	}
	break;
	case ANCHOR_LM:
	{
		pos = QPointF(bound.left(), bound.center().y());
	}
	break;
	case ANCHOR_RM:
	{
		pos = QPointF(bound.right(), bound.center().y());
	}
	break;
	case ANCHOR_TM:
	{
		pos = QPointF(bound.center().x(), bound.top());
	}
	break;
	case ANCHOR_BM:
	{
		pos = QPointF(bound.center().x(), bound.bottom());
	}
	break;
	case ANCHOR_CE:
	{
		pos = QPointF(boundingBox_.left() + boundingBox_.width() * hotspot_.x(), 
			boundingBox_.top() + boundingBox_.height() * hotspot_.y());
	}
	break;
	case ANCHOR_RO:
	{
		pos = QPointF(boundingBox_.left() + boundingBox_.width() * hotspot_.x() + 30,
			boundingBox_.top() + boundingBox_.height() * hotspot_.y());
	}
	break;
	default:
	{
		Q_UNREACHABLE();
	}
	break;
	}

	return mapToScene(pos);
}


QPointF EditorItem::updateAnchor(int id, const QPointF& pos){
	// 修改控制点数据
	anchorChanged(id, pos);
	update();

	// 返回新的锚点位置
	return anchorPos(id, selectBound());
}

qreal computeDelta(const QSize& size, const QSizeF& oldSize){
	qreal dx = size.width() / oldSize.width();
	qreal dy = size.height() / oldSize.height();
	return qAbs(dx) > qAbs(dy) ? dy : dx;
}

void EditorItem::adjustSize(QSize& newSize, bool keepAspect){

	if (keepAspect){
		QSizeF oldSize = oldAspect_;
		bool aspect = (oldSize.width() != 0 && oldSize.height() != 0);

		// 比较x,y变化最大
		if (!aspect && (oldSize.width() != 0 || oldSize.height() != 0)){
			if (oldSize.width() == 0){
				newSize.setWidth(0);
			}
			else{
				newSize.setHeight(0);
			}
		}
		else{
			qreal zoom = computeDelta(newSize, oldSize);
			newSize = (oldSize  * zoom).toSize();
		}
	}
	
	if (!canflip_){
		if (newSize.width() < 0)
			newSize.setWidth(0);

		if (newSize.height() < 0)
			newSize.setHeight(0);
	}

}

void EditorItem::anchorChanged(int id, const QPointF& newPos){

	isAdjusting_ = true;

	QPointF itemPos;
	if (id != ANCHOR_RO){
		itemPos = mapFromScene(anchorPos(id, boundingBox_) + newPos - anchorPos(id, selectBound()));   // 锚点移动的位移
	}
	else{
		itemPos = mapFromScene(newPos);
	}
	QSize newSize = boundingBox_.size().toSize();

	QPointF spotPos(0, 0);
	spotPos .setX( boundingBox_.left() + boundingBox_.width() * hotspot_.x() );
	spotPos.setY( boundingBox_.top() + boundingBox_.height() * hotspot_.y());

	// 等比缩放
	switch (id){
	case ANCHOR_LT:
	{
		newSize.setWidth( boundingBox_.right() - itemPos.x());
		newSize.setHeight(boundingBox_.bottom() -itemPos.y());

		// 按长宽比调整大小
		adjustSize(newSize);
	
		spotPos.setX(boundingBox_.right() - newSize.width() *( 1 - hotspot_.x()));
		spotPos.setY(boundingBox_.bottom() - newSize.height() *( 1- hotspot_.y()));

	}
	break;
	case ANCHOR_RB:
	{
		newSize.setWidth( itemPos.x() - boundingBox_.left() );
		newSize.setHeight(itemPos.y() - boundingBox_.top());

		// 按长宽比调整大小
		adjustSize(newSize);

		spotPos.setX(boundingBox_.left() + newSize.width() *hotspot_.x());
		spotPos.setY(boundingBox_.top() + newSize.height() *hotspot_.y());

	}
	break;
	case ANCHOR_RT:
	{
		newSize.setWidth(itemPos.x() - boundingBox_.left());
		newSize.setHeight(boundingBox_.bottom() - itemPos.y());

		// 按长宽比调整大小
		adjustSize(newSize);

		spotPos.setX(boundingBox_.left() + newSize.width() * hotspot_.x());
		spotPos.setY(boundingBox_.bottom() - newSize.height() *(1-hotspot_.y()));

	}
	break;
	case ANCHOR_LB:
	{
		newSize.setWidth(boundingBox_.right() - itemPos.x());
		newSize.setHeight(itemPos.y() - boundingBox_.top());

		// 按长宽比调整大小
		adjustSize(newSize);
	
		spotPos.setX(boundingBox_.right() - newSize.width() * (1-hotspot_.x()));
		spotPos.setY(boundingBox_.top() + newSize.height()  * hotspot_.y());
	
	}
	case ANCHOR_LM:
	{
		newSize.setWidth( boundingBox_.right() - itemPos.x());
		adjustSize(newSize, false);
		spotPos.setX(boundingBox_.right() - newSize.width() * (1-hotspot_.x()));
	
	}
	break;
	case ANCHOR_RM:
	{
		newSize.setWidth(itemPos.x() - boundingBox_.left());
		adjustSize(newSize, false);

		spotPos.setX(boundingBox_.left() +  newSize.width() * hotspot_.x());

	}
	break;
	case ANCHOR_TM:
	{
		newSize.setHeight(boundingBox_.bottom() - itemPos.y());
		adjustSize(newSize, false);
		spotPos.setY(boundingBox_.bottom() - newSize.height() * (1-hotspot_.y()));
	}
	break;
	case ANCHOR_BM:
	{
		newSize.setHeight( itemPos.y() - boundingBox_.top());
		adjustSize(newSize, false);
		spotPos.setY(boundingBox_.top() + newSize.height() * hotspot_.y());
	}
	break;
	case ANCHOR_RO:
	{
		// 获取原点
		QPointF origin = QPointF(boundingBox_.left() + boundingBox_.width() * hotspot_.x(),
			boundingBox_.top() + boundingBox_.height() * hotspot_.y());

		// 计算原点的夹角
		QLineF l1(origin, QPointF(origin.x() + 30, origin.y()));
		QLineF l2(origin, itemPos);
		qreal angle = l2.angleTo(l1);
		qreal na = rotation() + angle;
		if (na > 360){
			na -= 360;
		}

		if (na < -360){
			na += 360;
		}
		setRotation(na);
	}
	break;
	}

	setPos(mapToParent(spotPos));

	prepareGeometryChange();
	boundingBox_.setRect(-newSize.width()* hotspot_.x(), -newSize.height()*hotspot_.y(), newSize.width(), newSize.height());

	// 更新其他锚点
	updateAnchor(QList<int>()<<id);
	updateChildren();

	//////////////////////////////////////////////////////////////////////////
	// 通知界面属性更新
	if (scene()){
		((LevelScene*)scene())->notifyUpdateProperty(this);
	}
	isAdjusting_ = false;
	update();
}
	

bool EditorItem::initAnchor(){
	for (int i = 0; i < ANCHOR_NUM; ++i){
		EditorAnchor* anchor = new EditorAnchor(this, i );
		anchors_.insert(i,anchor);
		scene()->addItem(anchor);
	}
	return true;
}


void EditorItem::clearAnchor(){

	foreach(EditorAnchor* ar, anchors_.values()){
		delete ar;
	}
	anchors_.clear();
}

void EditorItem::hideAnchor(){
	foreach(EditorAnchor* anchor, anchors_.values()){
		anchor->setVisible(false);
	}
}

void EditorItem::updateAnchor(const QList<int>& except){
	if (anchors_.size() == 0){
		return;
	}

	foreach(int id, anchors()){
		Q_ASSERT(anchors_.size() > id);
		if (except.contains(id))
			continue;
		anchors_[id]->setPos(anchorPos(id,selectBound()));
	}
}

void EditorItem::resize(const QSizeF& newSize, bool flag){
	prepareGeometryChange();
	boundingBox_.setRect(-newSize.width()* hotspot_.x(), -newSize.height()*hotspot_.y(), newSize.width(), newSize.height());

	isAdjusting_ = true;
	updateAnchor();
	isAdjusting_ = false;

	if ( flag)
		update();
}

void EditorItem::setAlpha(qreal a){
	opacity_ = clampValue(a, 0.1, 1.0);
}


ResourceHash EditorItem::makeUniqueId(){
	QString uid = QUuid::createUuid().toString();
	return hashString(uid);
}

void EditorItem::postinit(bool resize){
	setFlag(ItemIsMovable, !isLocking_);
}


// 缺省数据
void EditorItem::encodeDefault(QDataStream& stream){
	stream << type();
	unsigned int ver = LEVEL_DATA_VERSON;
	stream << ver;

	stream << boundingBox_;
	stream << hotspot_;
	stream << opacity_;
}

void EditorItem::decodeDefault(QDataStream& stream){
	int t;
	stream >> t;
	Q_ASSERT(t == type());

	stream >> version_;  // 当前数据版本

	prepareGeometryChange();
	stream >> boundingBox_;
	stream >> hotspot_;
	stream >> opacity_;
}

void EditorItem::mousePressEvent(QGraphicsSceneMouseEvent * event){
	// 记录当前host的旧值
	oldValues_ = encode();

	QGraphicsItem::mousePressEvent(event);
}

void EditorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event){
	// 提交一次属性修改undo请求
	if (scene()){
		((LevelScene*)scene())->notifyInstanceModified(this, oldValues_);
	}


	QGraphicsItem::mouseReleaseEvent(event);
}