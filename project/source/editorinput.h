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
#ifndef  ORION_EDITORINPUT_H
#define ORION_EDITORINPUT_H

#include <QGraphicsItem>
#include <QFont>
#include <QVector3D>
#include <QMap>
#include <QDataStream>
#include <QByteArray>
#include <QPainterPath>
#include "leveldefine.h"
#include "editorregion.h"
#include "resourceobject.h"
using namespace ORION;

class EditorInputButton : public EditorRegion{
public:
	// 实体类型
	ORION_TYPE_DECLARE(LEVEL_INPUT_BUTTON)
	EditorInputButton(ObjectTypeResourceObject* prototype, QGraphicsItem * parent = 0);
	virtual ~EditorInputButton();

	// 绘制触发区域形状
	virtual void drawShape(QPainter *painter);
	void postinit(bool resize = true);

private:
	int inputtype_;  // 类型
	QString text_;  //文字
	bool checked_;  // 勾选状态

};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

class EditorInputTextBox : public EditorRegion{
public:
	// 实体类型
	ORION_TYPE_DECLARE(LEVEL_INPUT_TEXTBOX)
	EditorInputTextBox(ObjectTypeResourceObject* prototype, QGraphicsItem * parent = 0);
	virtual ~EditorInputTextBox();

	// 绘制触发区域形状
	virtual void drawShape(QPainter *painter);
	void postinit(bool resize = true);

private:
	QString text_;
	QString placeholder_;
	int inputtype_;  // 类型
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

class EditorInputList : public EditorRegion{
public:
	// 实体类型
	ORION_TYPE_DECLARE(LEVEL_INPUT_LIST)
	EditorInputList(ObjectTypeResourceObject* prototype, QGraphicsItem * parent = 0);
	virtual ~EditorInputList();

	// 绘制触发区域形状
	virtual void drawShape(QPainter *painter);
	void postinit(bool resize = true);

private:
	QStringList items_;
	int inputtype_;  // 类型
	QImage icon_;  // 下拉箭头
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

class EditorInputFileChooser : public EditorRegion{
public:
	// 实体类型
	ORION_TYPE_DECLARE(LEVEL_INPUT_FILECHOOSER)
	EditorInputFileChooser(ObjectTypeResourceObject* prototype, QGraphicsItem * parent = 0);
	virtual ~EditorInputFileChooser();

	// 绘制触发区域形状
	virtual void drawShape(QPainter *painter);
	void postinit(bool resize = true);
};


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

class EditorInputProgressBar : public EditorRegion{
public:
	// 实体类型
	ORION_TYPE_DECLARE(LEVEL_INPUT_PROGRESSBAR)
	EditorInputProgressBar(ObjectTypeResourceObject* prototype, QGraphicsItem * parent = 0);
	virtual ~EditorInputProgressBar();

	// 绘制触发区域形状
	virtual void drawShape(QPainter *painter);
	void postinit(bool resize = true);

private:
	int percent_;  // 进度百分比
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

class EditorInputSliderBar : public EditorRegion{
public:
	// 实体类型
	ORION_TYPE_DECLARE(LEVEL_INPUT_SLIDERBAR)
	EditorInputSliderBar(ObjectTypeResourceObject* prototype, QGraphicsItem * parent = 0);
	virtual ~EditorInputSliderBar();

	// 绘制触发区域形状
	virtual void drawShape(QPainter *painter);
	void postinit(bool resize = true);

private:
	int value_;  // 进度百分比
	int gridnum_;
	QImage icon_;  // 下拉箭头
};


#endif // ORION_EDITORIMAGE_H
