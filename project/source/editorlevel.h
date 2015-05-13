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
#ifndef ORION_EDITORLEVEL_H
#define ORION_EDITORLEVEL_H

#include "editoritem.h"
#include <QGraphicsBlurEffect>
#include <QGraphicsSimpleTextItem>
#include <QFont>
#include <QVector3D>
#include <QDataStream>
#include <QByteArray>
#include <QPainterPath>
#include "leveldefine.h"
#include "resourceobject.h"
using namespace ORION;

class EditorLevelText : public QGraphicsSimpleTextItem{

public:
	ORION_TYPE_DECLARE(LEVEL_VIEWPORT_TEXT);
	EditorLevelText(const QString& text, QPointF pos, QGraphicsItem *parent = 0);
};


// 基础区域单元（区域中可以显示一个触发器或素材图片，视频播放器、音频播放器或其他）
class EditorLevel : public EditorItem{
public:
	// 实体类型
	ORION_TYPE_DECLARE(LEVEL_PROFILE)
	EditorLevel(LevelResourceObject* level, QGraphicsItem * parent = 0);
	virtual ~EditorLevel();

	// 绘制触发区域形状
	virtual void drawShape(QPainter *painter);
	// 绘制场景外边界
	void drawMargin(QPainter *painter);
	virtual void resize(const QSizeF& newSize, bool update = true);

	// 更新孩子
	virtual void updateChildren();
	void updateText();

	void enableEdit(bool flag){
		setSizeFlag(flag ? ANCHORFLAG_RESIZE_R : ANCHORFLAG_NONE);
	}

	// 切换选择模式
	bool toggleSelectable();

	QColor fillColor_; // 填充色
	QColor marginfillColor_; // 边界填充色
	int flags_;  // 标记

	LevelResourceObject* level_;  
	EditorLevelText* text_;  // 文本

	// 获取文本位置
	QPointF textPos();

	// 刷新数据
	void refresh();

protected:
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

protected:
	// 获取当前字符串
	QString currentTitleString();
	void adjustText();
};


#endif // ORION_EDITORREGION_H
