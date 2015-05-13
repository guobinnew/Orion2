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
#include "editortilemap.h"
#include <QtGui>
#include <QDialog>
#include <QVector2D>
#include <math.h>
#include "levelscene.h"
#include "commondefine.h"
#include "componentdefine.h"
#include "resourcelinker.h"
#include "spritedefine.h"
using namespace ORION;



EditorTileBrush::EditorTileBrush(QGraphicsItem * parent){
	boundingBox_.setRect(0, 0, 32, 32);
	setOpacity(0.5);
	setFlags(0);
	flip_.resize(4);
}

EditorTileBrush::~EditorTileBrush(){

}

void EditorTileBrush::setImage(const QImage& img){
	image_ = img;
	prepareGeometryChange();
	boundingBox_.setRect(0, 0, image_.width(), image_.height());
}

void EditorTileBrush::setRect(const QRect& rect){
	image_ = QImage();
	prepareGeometryChange();
	boundingBox_.setRect(0,0,rect.width(),rect.height());
}

//  重载函数
QRectF EditorTileBrush::boundingRect() const{
	return boundingBox_;
}

QPainterPath EditorTileBrush::shape() const{
	QPainterPath path;
	path.addRect(boundingBox_);
	return path;
}

void EditorTileBrush::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget){

	if (!image_.isNull()){
		QImage img= image_.mirrored(flip_[0], flip_[1]);
		// 对角翻转
		if (flip_[2]){
			QTransform trans;
			trans.rotate(90);
			img = img.transformed(trans);
			img = img.mirrored(false, true);
		}
		painter->drawImage(QPointF(0, 0), img);
	}
	else{
		painter->drawRect(boundingBox_);
	}

}

void EditorTileBrush::updateState(int state){
	flip_.setBit(0, state & 1);
	flip_.setBit(1, state & 2);
	flip_.setBit(2, state & 4);
	update();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

EditorTileMap::EditorTileMap(ObjectTypeResourceObject* prototype, QGraphicsItem * parent)
	:EditorRegion(prototype, parent){

	sizeFlags_ = ANCHORFLAG_RESIZE;  // 不能选择
	canflip_ = false;
	setAcceptDrops(true);
	setAcceptHoverEvents(true);
	// 必须是TileBG插件实例
	if (prototype){
		Q_ASSERT(hashStringLower(typePrototype_->prototype_->objectName()) == COMP_TILEMAP);
		postinit();
	}

	brushItem_ = new EditorTileBrush();
	brushItem_->setVisible(false);
	brushItem_->setParentItem(this);
	state_.resize(8);
	state_.setBit(0, false);

	state_.setBit(4, false);
	state_.setBit(5, false);
	state_.setBit(6, false);
	mode_ = -1;

	brushSize_ = QSize(0, 0);
}

EditorTileMap::~EditorTileMap(){
}

void EditorTileMap::drawShape(QPainter *painter){

	painter->setOpacity(opacity_);
	painter->fillRect(boundingBox_, SPRITE_FILLCOLOR);

	// 绘制Tile网格
	if (grid_.width() > 0 && grid_.height() > 0 && !tileImage_.isNull()){

		// 计算行列
		int col = tileImage_.width() / grid_.width();
		int row = tileImage_.height() / grid_.height();

		QRectF block = QRectF(0, 0, grid_.width(), grid_.width());

		painter->setClipRect(boundingBox_);
		QMapIterator<QPair<int, int>, QSize> i(mapData_);
		while (i.hasNext()){  // 绘制地图块
			i.next();
			if (i.key().first * grid_.width() >= boundingBox_.width() || i.key().second * grid_.height() >= boundingBox_.height()){
				continue;
			}

			QImage img;
			QPair<int, int> index = qMakePair(i.value().width(), i.value().height());
			if (bufferImage_.contains(index)){
				img = bufferImage_[index];
			}
			else{
				//  生成缓存图片
				QBitArray flip(4);
				flip.setBit(0, i.value().height() & 1);
				flip.setBit(1, i.value().height() & 2);
				flip.setBit(2, i.value().height() & 4);

				img = tileImage_.copy((i.value().width() % col) * grid_.width(), (i.value().width() / col) * grid_.height(), grid_.width(), grid_.height());
				img = img.mirrored(flip[0], flip[1]);
				// 对角翻转
				if (flip[2]){
					QTransform trans;
					trans.rotate(90);
					img = img.transformed(trans);
					img = img.mirrored(false, true);
				}
				bufferImage_.insert(index, img);
			}
			painter->drawImage(i.key().first * grid_.width(), i.key().second  * grid_.height(), img );
		}
	}

	// 如果是选择状态
	if (isSelected()){
		QPen pen(Qt::red);
		painter->setPen(pen);
		painter->drawRect(boundingBox_);
	}

}

void	EditorTileMap::dropEvent(QGraphicsSceneDragDropEvent * event){
	if (event->mimeData()->hasFormat(MIME_ORION_GALLERY)){
		QByteArray itemData = event->mimeData()->data(MIME_ORION_GALLERY);
		QDataStream stream(&itemData, QIODevice::ReadOnly);

		LevelScene* level = (LevelScene*)scene();
		// 获取索引号
		int index;
		stream >> index;
		Q_ASSERT(index == 1);
		ResourceHash key;
		stream>>key;

		if (typePrototype_->setImageResource(key)){
			postinit();
			update();
		}

		if (isSelected()){
			// 通知Tile窗口显示
			level->updateTileMap(this);
		}

		event->accept();
	}

	EditorRegion::dropEvent(event);
}

void EditorTileMap::postinit(bool resize){

	if (typePrototype_->staticImage_ != NULL){  // 静态图片的中心锚点始终为中心
		tileImage_ = typePrototype_->staticImage_->image_;
	}

	grid_.setWidth(getParameterInt(UDQ_T("TileWidth"), typePrototype_->prototype_->objectName()));
	grid_.setHeight(getParameterInt(UDQ_T("TileHeight"), typePrototype_->prototype_->objectName()));

	if (grid_.width() < 0){
		grid_.setWidth(0);
	}

	if (grid_.height() < 0){
		grid_.setHeight(0);
	}

	isAdjusting_ = true;
	updateAnchor();
	isAdjusting_ = false;

}

void EditorTileMap::dragEnterEvent(QGraphicsSceneDragDropEvent * event){

	if (event->mimeData()->hasFormat(MIME_ORION_GALLERY)){
		setSelected(true);
		update();
		event->accept();
		return;
	}

	EditorRegion::dragEnterEvent(event);
}

void EditorTileMap::dragLeaveEvent(QGraphicsSceneDragDropEvent * event){
	if (event->mimeData()->hasFormat(MIME_ORION_GALLERY)){
		setSelected(false);
		update();
		event->accept();
		return;
	}
	EditorRegion::dragLeaveEvent(event);
}

void EditorTileMap::dragMoveEvent(QGraphicsSceneDragDropEvent * event){
	EditorRegion::dragMoveEvent(event);
}

// 编码
void EditorTileMap::encodeData(QDataStream& stream){
	EditorRegion::encodeData(stream);
	
	QRect bound = boundingBox_.toRect();
	// 计算有效个数
	int gridW = bound.width() / grid_.width();
	int gridH = bound.height() / grid_.height();

	stream << gridW;
	stream << gridH;
	stream << mapData_;

}

void EditorTileMap::decodeData(QDataStream& stream){
	EditorRegion::decodeData(stream);
	int tmp;
	stream >> tmp >> tmp;
	stream >> mapData_;
}

void EditorTileMap::encodeDefault(QDataStream& stream){
	EditorRegion::encodeDefault(stream);
	stream << mapData_;
}

void EditorTileMap::decodeDefault(QDataStream& stream){
	EditorRegion::decodeDefault(stream);
	stream >> mapData_;
}

QVariant EditorTileMap::itemChange(GraphicsItemChange change, const QVariant & value){

	if (change == QGraphicsItem::ItemSelectedHasChanged && scene()){
		// 通知Tile窗口显示
		((LevelScene*)scene())->updateTileMap(this);
	}

	return EditorRegion::itemChange(change, value);
}

void EditorTileMap::updateBrush(const QRect& region, int mode){
	mode_ = mode;
	// 恢复缺省状态
	state_.setBit(0, 0);
	state_.setBit(4, 0);
	state_.setBit(5, 0);
	state_.setBit(6, 0);

	if (region.isNull()){
		isEditing_ = false;
		brushItem_->setImage(QImage());
		setFlag(QGraphicsItem::ItemIsMovable, true);
		isEditing_ = false;
		brushData_.clear();
	}
	else{
		if (mode_ == 0){
			// 生成刷子图片
			QImage brushImage = tileImage_.copy(region);
			brushItem_->setImage(brushImage);
		}
		else{
			brushItem_->setRect(region);
		}
		makeBrushData(region);
		setFlag(QGraphicsItem::ItemIsMovable, false);
		isEditing_ = true;
	}
}


void EditorTileMap::addTileBlock(const QPoint& pos){

	int col = qFloor(pos.x() / grid_.width());
	int row = qFloor(pos.y() / grid_.height());

	if ( brushData_.size() > 0){

		foreach(QRect r, brushData_){
			int state = (state_[4] ? 1 : 0);
			state += (state_[5] ? 1 : 0) * 2;
			state += (state_[6] ? 1 : 0) * 4;

			QSize data = QSize(r.right(), state);

			QPoint cur = r.topLeft();
			// 根据翻转重新定位左边
			if (state_[4]){
				cur.setX(brushSize_.width() - cur.x() - 1);
			}

			if (state_[5]){
				cur.setY(brushSize_.height() - cur.y() - 1);
			}

			if (state_[6]){ // 对角线
				// 顺时针旋转90
				int oldx = cur.x();
				cur.setX(brushSize_.height() - cur.y() - 1);
				cur.setY(oldx);
				// 垂直翻转
				cur.setY(brushSize_.width() - cur.y() - 1);
			}

			if (mode_ == 0){
				mapData_.insert(QPair<int, int>(col + cur.x(), row + cur.y()), data);
				bufferImage_.remove(qMakePair(data.width(), data.height()));
			}
			else{
				mapData_.remove(QPair<int, int>(col + cur.x(), row + cur.y()));
			}
		}
	}
}


void EditorTileMap::mousePressEvent(QGraphicsSceneMouseEvent * event){
	if (event->button() == Qt::RightButton){  // 如果按下右键，顺时针旋转方向
		if (brushItem_->isVisible()){

			if (event->modifiers() & Qt::ControlModifier){  // 如果是Ctrl+右键，则水平翻转
				state_.toggleBit(state_[6] ? 5: 4 );
			}
			else if (event->modifiers() & Qt::AltModifier){  // 如果是Alt+右键，则垂直翻转
				state_.toggleBit(state_[6] ? 4 : 5);
			}
			else{ // 顺时针旋转
				if (!state_[0]){
					state_.toggleBit(4);
				}
				else{
					state_.toggleBit(5);
				}
				state_.toggleBit(0);
				state_.toggleBit(6);
			}

			// 更新显示
			int state = (state_[4] ? 1 : 0);
			state += (state_[5] ? 1 : 0) * 2;
			state += (state_[6] ? 1 : 0) * 4;
			brushItem_->updateState(state);
		}
	}
	else if (event->button() == Qt::LeftButton){

		if (event->modifiers() & Qt::ControlModifier){
		}
		else{
			// 添加数据
			if (mode_ >= 0){
				addTileBlock(event->pos().toPoint());
				update();
				return;
			}
		}
	}

	EditorRegion::mousePressEvent(event);
}

void EditorTileMap::mouseMoveEvent(QGraphicsSceneMouseEvent * event){

	// 计算网格位置
	if ( brushItem_->isVisible() && event->buttons() & Qt::LeftButton ){
		QPoint pos = event->pos().toPoint();
		int posx = (pos.x() / grid_.width()) * grid_.width();
		int posy = (pos.y() / grid_.height()) * grid_.height();
		brushItem_->setPos(posx, posy);

		// 修改数据
		addTileBlock(event->pos().toPoint());
		update();
	}

	EditorRegion::mouseMoveEvent(event);
}

void EditorTileMap::hoverMoveEvent(QGraphicsSceneHoverEvent * event){
	// 计算网格位置
	if (mode_ >= 0 && brushItem_->isVisible()){
		QPoint pos = event->pos().toPoint();
		int posx = (pos.x() / grid_.width()) * grid_.width();
		int posy = (pos.y() / grid_.height()) * grid_.height();
		brushItem_->setPos(posx, posy);
	}
	EditorRegion::hoverMoveEvent(event);
}

void EditorTileMap::hoverEnterEvent(QGraphicsSceneHoverEvent * event){

	if ( isEditing_ )
		brushItem_->setVisible(true);

	EditorRegion::hoverEnterEvent(event);
}

void EditorTileMap::hoverLeaveEvent(QGraphicsSceneHoverEvent * event){
	brushItem_->setVisible(false);
	EditorRegion::hoverLeaveEvent(event);
}

void EditorTileMap::contextMenuEvent(QGraphicsSceneContextMenuEvent *event){
	if (mode_ >= 0)
		return;

	return EditorRegion::contextMenuEvent(event);
}


void EditorTileMap::makeBrushData(const QRect& r){
	brushData_.clear();
	brushSize_.setWidth(0);
	brushSize_.setHeight(0);

	// 生成Tile数据
	if (grid_.width() > 0 && grid_.height() > 0 ){

		// 计算行列
		int col = tileImage_.width() / grid_.width();
		int row = tileImage_.height() / grid_.height();

		// 计算行列上下限
		int col1 = r.left() / grid_.width();
		int col2 = r.right() / grid_.width();
		int row1 = r.top()  / grid_.height();
		int row2 = r.bottom() / grid_.height();

		brushSize_.setWidth(col2 - col1 + 1);
		brushSize_.setHeight(row2 - row1 + 1);

		QRect r;
		for (int i = row1; i <= row2; i++){
			for (int j = col1; j <= col2; j++){	
				r.setLeft(j - col1);
				r.setTop(i - row1);

				if (mode_ == 0){
					r.setRight(i*col + j);
				}
				else{
					r.setRight(0);
				}
				r.setBottom(0);
				brushData_.append(r);
			}
		}
	}
}
