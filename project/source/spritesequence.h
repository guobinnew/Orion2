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
#ifndef  ORION_SPRITESEQUENCE_H
#define ORION_SPRITESEQUENCE_H

#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QFont>
#include <QTimeLine>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "editordefine.h"
#include "spritedefine.h"
#include "spriteobject.h"

#include "resourceobject.h"
using namespace ORION;


class SpriteSequenceFrame : public SpriteObject{
	Q_OBJECT
public:
	// 实体类型
	ORION_TYPE_DECLARE(SPRITE_SEQUENCEFRAME);

	SpriteSequenceFrame(SequenceFrame* frame, QGraphicsItem * parent = 0);
	virtual ~SpriteSequenceFrame();

	//  重载函数
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

	//  刷新图片
	void refreshImage();

	// 图片大小
	QSize imageSize();

	// 图片
	QImage& image();

	// 以中心锚点为原点的区域大小
	QRect anchorRegion();

	// 恢复拖放前状态
	bool restoreDrag();

	// 图片是否有效
	bool isValidImage(){
		return  !frame_->buffer.isNull();
	}

	// 更新数据
	void updateInstance();

	SequenceFrame* getFrame() const{
		return frame_;
	}
protected:
	virtual void	mouseMoveEvent(QGraphicsSceneMouseEvent * event);
	virtual void	mousePressEvent(QGraphicsSceneMouseEvent * event);
	virtual void	mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
	virtual  void  contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);

	QRectF imgBound_;  // 图片绘制区域
	SequenceFrame* frame_;  // 数据帧
	QImage errorimage_;  // 错误图片
	QGraphicsItem* oldParent_; // 旧序列（用于撤销拖放）
	int oldIndex_;  

	// 绘制锚点
	void drawAnchor(QPainter* painter);

};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

class SpriteSequence : public SpriteObject{
	Q_OBJECT
public:
	// 实体类型
	ORION_TYPE_DECLARE(SPRITE_SEQUENCE);

	SpriteSequence(SpriteSequenceResourceObject* instance, QGraphicsItem * parent = 0);
	virtual ~SpriteSequence();
	//  获取标题名
	virtual QString title();
	//  重载函数
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

	// 更新实例
	void  updateInstance();

	// 刷新动画帧（包括调整锚点和大小）
	void refreshFrames();

	// 获取最大的图像大小
	QSize acquireFrameMaxSize();

	// 调整孩子位置
	void adjust();

	// 移除当前帧（返回移除前索引）
	int removeFrame(SpriteSequenceFrame* frame);
	// 彻底删除
	void deleteFrame(SpriteSequenceFrame* frame);
	// 添加动画帧
	void addFrame(SpriteSequenceFrame* frame, int index = -1);

	// 动画帧数
	int frameCount(){
		return frames_.size();
	}
	// 设置播放速度
	void setPlaySpeed(qreal speed);
	void setPlayBack(bool flag = true);
	void setPingPong(bool flag = true);
	void setRepeat(bool flag = true);
	void setRepeatCount(int count = 1);
	void setRepeatTo(int index = 0);

	void playAnim( bool enable = true);
	void resetAnim();

	bool isPlaying();
	bool isPausing();
	bool isPlayBack();
	qreal playSpeed();

	bool isPingpong();
	bool isRepeat();
	int repeatCount();
	int repeatTo();


	SpriteSequenceResourceObject* resourceInstance(){
		return instance_;
	}

	int currentAnchor(){
		return currentAnchor_;
	}

	void setCurrentAnchor(int id){
		currentAnchor_ = id;
	}

protected:
	virtual void	dragEnterEvent(QGraphicsSceneDragDropEvent * event);
	virtual void	dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
	virtual void	dragMoveEvent(QGraphicsSceneDragDropEvent * event);
	virtual void	dropEvent(QGraphicsSceneDragDropEvent * event);
	virtual  void  contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);
	virtual void	mousePressEvent(QGraphicsSceneMouseEvent * event);
	virtual void	mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

private:
	SpriteSequenceResourceObject* instance_;
	QList<SpriteSequenceFrame*> frames_;  // 动画帧对象

	int insertAnchor_;  // 插入位置标记

	QRectF imgBound_;  // 图片绘制区域
	QPointF imgOrigin_;  // 动画绘制原点
	QTimeLine *timeLine_; // 动画
	int currrentFrame_;  // 当前播放动画帧

	// 锚点
	ResourceHash currentAnchor_;  // 当前显示的锚点
	QMap<ResourceHash, QRectF> anchorBounds_;  // 锚点绘制区域

	void initFrames();
	// 初始化动画
	void initAnimation();

	// 绘制锚点
	void drawAnchor(QPainter* painter);

private slots:
	void setValue(int value);
};


#endif
