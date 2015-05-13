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

#ifndef ORION_KISMET_COMMENT_H
#define ORION_KISMET_COMMENT_H

#include <QFont>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "kismetdefine.h"
#include "kismetobject.h"
using namespace ORION;


class BlueprintComment  : public BlueprintObject{
public:
	// 实体类型
	ORION_TYPE_DECLARE( KISMET_COMMENT );

	BlueprintComment(const QString& text = QString(), QGraphicsItem * parent = 0);
	virtual ~BlueprintComment();

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

	// 编码
	virtual void encodeData(QDataStream& stream);
	virtual void decodeData(QDataStream& stream);

	//  获取标题名
	virtual QString title(){
		return comment_;
	}

	// 调整尺寸
	virtual void adjust();

	// 设置文本
	void setText( const QString& text ){
		comment_ = text;
	}

	void setTextColor( const QColor& color ){
		textColor_ = color;
	}

	QColor textColor(){
		return textColor_;
	}

	// 更改文本
	void changeText( const QString& text );
	// 追加文本
	void appendText( const QString& text );
		
	QPainterPath shape() const;

	virtual void backupState();
	virtual bool isStateChanged(int type);
	virtual void copyState(QDataStream& stream, int type);

protected:
	virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );

private:
	QString comment_;  // 注释文本
	QColor textColor_; // 文本颜色


	QColor oldColor_;
	QString oldComment_;
};


#endif
