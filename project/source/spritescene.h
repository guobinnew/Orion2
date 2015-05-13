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


#ifndef  ORION_SPRTIESCENE_H
#define ORION_SPRTIESCENE_H

#include <QGraphicsScene>
#include <QMap>
#include <QPixmap>

#include "editorlogo.h"
#include "spriteobject.h"
#include "spritesequence.h"
#include "spriteanimationcontrol.h"
#include "resourceobject.h"
using namespace ORION;

class QDragEnterEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;


class SpriteScene : public QGraphicsScene{
	Q_OBJECT
public:

	SpriteScene(QObject *parent = 0);
	virtual ~SpriteScene();

	// 获取可见区域（场景坐标系）
	QRectF viewRegionRect(void);

	void load(SpriteResourceObject* sprite);
	void save(SpriteResourceObject* sprite = NULL);
	void clearSprite();

	SpriteResourceObject* spriteObject(){
		return sprite_;
	}

	// 获取当前的序列
	SpriteSequenceResourceObject* currentSequence();

	void addSequenceItem(SpriteSequenceResourceObject* seq);

	// 更新动画播放控件
	void bindAnimControl(SpriteSequence* seq);

	// 删除当前选中的序列
	void  removeSelectedSequence();
	// 删除当前选中的对象
	void removeSelectedObjects();

	// 刷新序列显示
	void refreshSequences();

	void setCurrentMenuHost(QGraphicsItem* host){
		menuHost_ = host;
	}

	// 删除序列
	void deleteSequence(SpriteSequence* seq);
	void updateAnimText();
protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
	virtual void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
	virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
	virtual void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
	virtual void dropEvent(QGraphicsSceneDragDropEvent * event);
	virtual void keyPressEvent(QKeyEvent * keyEvent);
	virtual void keyReleaseEvent(QKeyEvent * keyEvent);
	virtual void drawBackground(QPainter * painter, const QRectF & rect);

signals:
	void entityInserted(QGraphicsItem* entity);
	void entityDeleted(QGraphicsItem* entity);
	void pointerMode();
	void error(const QString& msg);

private:
	// 初始化LOGO
	bool initLogo();
	// 初始化动画控制器
	bool initAnimControl();

	QColor bgColor_;  // 背景颜色
	EditorLogoPtr logoItem_;
	SpriteAnimationControlPtr animControlItem_; // 动画控制
	QGraphicsSimpleTextItem*  animText_;  // 图层信息

	QList<SpriteSequence*> sequences_;   // 对象列表

	SpriteResourceObject* sprite_;  // 精灵对象
	QGraphicsItem* menuHost_;  

private slots:
    void applyAnchorToSequence();

};

#endif
