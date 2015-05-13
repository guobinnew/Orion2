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
**  Orion2 is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/
#include <QtGui>
#include <QGraphicsRectItem>
#include <QGraphicsSceneDragDropEvent>
#include <QMenu>
#include <QMessageBox>

#include "spritescene.h"
#include <math.h>
#include "commondefine.h"
#include "spritedefine.h"
#include "menumanager.h"
#include "configmanager.h"
#include "spriteview.h"
#include "spritesequence.h"
using namespace ORION;


SpriteScene::SpriteScene(QObject *parent) : QGraphicsScene(parent), sprite_(NULL){

	setSceneRect( 0, 0, 12800,12800  );

	if( gConfigManager->showLogo() ){
		initLogo();
	}

	initAnimControl();

	// 边框
	QGraphicsRectItem* borderItem = new QGraphicsRectItem(QRect(0,0,12800,12800));
	borderItem->setPos(0,0);
	borderItem->setVisible( true );
	addItem( borderItem );

	animText_ = new QGraphicsSimpleTextItem();
	animText_->setVisible(false);
	animText_->setZValue(EDITOR_DEPTH_LOGO);
	animText_->setFlags(QGraphicsItem::ItemIgnoresTransformations);
	animText_->setBrush(Qt::blue);
	QFont font(UDQ_TR("微软雅黑"), 24);
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPixelSize(24);
	font.setBold(true);
	animText_->setFont(font);
	addItem(animText_);


	bgColor_ = Qt::lightGray;
	menuHost_ = NULL;
}

//////////////////////////////////////////////////////////////////////////

SpriteScene::~SpriteScene(){

}

// 初始化LOGO
bool SpriteScene::initLogo(){
	logoItem_ = EditorLogoPtr( new EditorLogo( UDQ_T(":/images/unique128.png")));
	logoItem_->setPos(0,0);
	addItem( logoItem_.get() );
	return true;
}

bool SpriteScene::initAnimControl(){
	animControlItem_ = SpriteAnimationControlPtr(new SpriteAnimationControl());
	animControlItem_->setPos(0, 0);
	animControlItem_->setVisible(false);
	addItem(animControlItem_.get());
	return true;
}



void SpriteScene::drawBackground(QPainter * painter, const QRectF & rect){
	// 绘制底色
	painter->fillRect( rect, bgColor_ );
	
	// 确定尺寸和位置
	QSize viewRect = views().at(0)->maximumViewportSize();
	if( logoItem_.get() != 0 ){
		logoItem_->setPos( views().at(0)->mapToScene( viewRect.width() - logoItem_->boundingRect().width(), viewRect.height() - logoItem_->boundingRect().height()  ) );
	}

	// 修改文字
	if (animText_ && animText_->isVisible()){
		QFontMetricsF fm(animText_->font());
		qreal pixelsWide = fm.width(animText_->text());
		qreal pixelsHigh = fm.height();
		animText_->setPos(views().at(0)->mapToScene((viewRect.width() - pixelsWide) / 2.0, pixelsHigh + 4));
	}
}
// 获取可见区域（场景坐标系）
QRectF SpriteScene::viewRegionRect(void){

	// 获取当前缩放比例
	QSize viewRect = views().at(0)->maximumViewportSize();

	QPointF viewLTPos = views().at(0)->mapToScene( 0, 0 );
	if( viewLTPos.rx() < 0 )
		viewLTPos.rx() = 0;
	if( viewLTPos.ry() < 0 )
		viewLTPos.ry() = 0;

	QRectF sRect = sceneRect();

	QPointF viewRBPos = views().at(0)->mapToScene( viewRect.width(), viewRect.height() );
	if( viewRBPos.rx() > sRect.width() )
		viewRBPos.rx() = sRect.width();
	if( viewRBPos.ry() > sRect.height() )
		viewRBPos.ry() = sRect.height();

	return QRectF(viewLTPos, viewRBPos );
}

//////////////////////////////////////////////////////////////////////////
// 拖放处理
//////////////////////////////////////////////////////////////////////////

void SpriteScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event){
	clearSelection();
	QGraphicsScene::dragEnterEvent(event);
}

void SpriteScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event){
	QGraphicsScene::dragLeaveEvent(event);
}

void SpriteScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event){
	QGraphicsScene::dragMoveEvent(event);
}

void SpriteScene::dropEvent(QGraphicsSceneDragDropEvent *event){
	QGraphicsScene::dropEvent(event);
}

void SpriteScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent){
	QGraphicsScene::mousePressEvent( mouseEvent );
}

void SpriteScene::removeSelectedObjects(){
	QList<QGraphicsItem*> items = selectedItems();

	// 获取可删除对象
	QList<QGraphicsItem*> objs;
	foreach(QGraphicsItem* item, items){
		if (item->type() == SPRITE_SEQUENCEFRAME && items.contains(item->parentItem())){
			continue;
		}
		objs.append(item);
	}

	// 删除对象
	foreach(QGraphicsItem* item, objs){
		if (item->type() == SPRITE_SEQUENCEFRAME){
			((SpriteSequence*)(item->parentItem()))->deleteFrame((SpriteSequenceFrame*)item);
		}
		else if (item->type() == SPRITE_SEQUENCE){
			deleteSequence((SpriteSequence*)item);
		}
	}

	sprite_->setFlags(URF_TagSave);
	ResourceObject::purgeGarbage();
}

void SpriteScene::keyPressEvent(QKeyEvent * keyEvent){
	 if (isPressDelete(keyEvent->key())){
		// 收集所有的序列

		 if (QMessageBox::question(views().at(0), UDQ_TR("删除"), UDQ_TR("确认删除选中的对象（不可恢复）?")) == QMessageBox::No){
			 return;
		 }

		 removeSelectedObjects();
	}
	QGraphicsScene::keyPressEvent( keyEvent );
}

void SpriteScene::keyReleaseEvent(QKeyEvent * keyEvent){

	QGraphicsScene::keyReleaseEvent( keyEvent );
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void  SpriteScene::load(SpriteResourceObject* sprite){
	clearSprite();
	sprite_ = sprite;

	if (!sprite_->viewData_.isEmpty()){
		QDataStream stream(sprite_->viewData_);
		int lod;
		QTransform trans;
		QPointF center;
		stream >> lod >> trans >> center;

		SpriteView* view = (SpriteView*)(views().at(0));
		view->setZoomStep(lod);
		view->setTransform(trans);
		view->centerOn(center);
	}

	// 添加序列对象
	foreach(SpriteSequenceResourceObject* seq, sprite_->sequences_){
		addSequenceItem(seq);
	}

	updateAnimText();

}

void  SpriteScene::save(SpriteResourceObject* sprite){
	if (sprite_ == NULL){
		return;
	}

	SpriteView* view = (SpriteView*)(views().at(0));
	sprite_->saveViewPort(view->zoomStep(), view->transform(), viewRegionRect().center());

	// 清空触发器
	foreach(SpriteSequence* obj, sequences_){
		// 更新数据到对象
		obj->updateInstance();
	}

	// 强制保存
	sprite_->setFlags(URF_TagSave);

}

void  SpriteScene::clearSprite(){
	// 执行清理工作
	clearSelection();

	// 清空触发器
	foreach(SpriteObject* obj, sequences_){
		removeItem(obj);
		delete obj;
	}
	sequences_.clear();
	sprite_ = NULL;
}


void SpriteScene::addSequenceItem(SpriteSequenceResourceObject* seq){

	SpriteSequence* seqItem = new SpriteSequence(seq);
	addItem(seqItem);

	sequences_.append(seqItem);

}

void  SpriteScene::bindAnimControl(SpriteSequence* seq){
	if (seq == NULL){
		animControlItem_->setParentItem(NULL);
		animControlItem_->setVisible(false);
	}
	else{
		QRectF bound = seq->boundingRect();
		animControlItem_->bind(seq);
		animControlItem_->setPos(bound.bottomLeft());
		animControlItem_->setVisible(true);
	}
}

SpriteSequenceResourceObject* SpriteScene::currentSequence(){
	SpriteSequenceResourceObject* seq = NULL;
	QList<QGraphicsItem*> items = selectedItems();
	foreach (QGraphicsItem* item, items)
	{
		if (item->type() == SPRITE_SEQUENCE){
			seq = ((SpriteSequence*)item)->resourceInstance();
			break;
		}
	}

	return seq;
}

void  SpriteScene::removeSelectedSequence(){

	// 提取所有序列对象
	QList<QGraphicsItem*> selItems = selectedItems();
	QList<SpriteSequence*> seqitems;
	foreach(QGraphicsItem* item, selItems)
	{
		if (item->type() == SPRITE_SEQUENCE){
			seqitems.append((SpriteSequence*)item);
		}
		// 清空选择
		item->setSelected(false);
	}

	bindAnimControl(NULL);
	
	QList<SpriteSequenceResourceObject*> seqs;
	foreach(SpriteSequence* seq, seqitems){
		seq->setVisible(false);
		deleteSequence(seq);
	}

	sprite_->setFlags(URF_TagSave);

	// 清除删除对象
	ResourceObject::purgeGarbage();
}

void SpriteScene::deleteSequence(SpriteSequence* seq){

	if (seq == NULL)
		return;

	removeItem(seq);

	seq->resourceInstance()->setFlags(URF_TagGarbage);
	sprite_->sequences_.removeOne(seq->resourceInstance());

	sequences_.removeOne(seq);
	delete seq;
}


void SpriteScene::applyAnchorToSequence(){
	if (menuHost_ == NULL )
		return;

	Q_ASSERT(menuHost_->type() == SPRITE_SEQUENCEFRAME && menuHost_->parentItem() );
	SpriteSequenceFrame* frame = qgraphicsitem_cast<SpriteSequenceFrame*>(menuHost_);
	SpriteSequence* seq = qgraphicsitem_cast<SpriteSequence*>(frame->parentItem());


}

// 刷新序列显示
void SpriteScene::refreshSequences(){

	clearSelection();

	foreach(SpriteSequence* seq, sequences_){
		seq->refreshFrames();
	}

}

void SpriteScene::updateAnimText(){
	if (sprite_){
		animText_->setText(sprite_->description());
		animText_->setVisible(true);
	}
	else{
		animText_->setVisible(false);
	}
	update();
}
