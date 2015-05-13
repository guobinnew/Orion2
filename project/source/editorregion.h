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
#ifndef ORION_EDITORREGION_H
#define ORION_EDITORREGION_H

#include "editoritem.h"
#include <QGraphicsBlurEffect>
#include <QFont>
#include <QVector3D>
#include <QDataStream>
#include <QByteArray>
#include <QPainterPath>
#include "editordefine.h"
#include "resourceobject.h"
using namespace ORION;

// 基础区域单元（区域中可以显示一个触发器或素材图片，视频播放器、音频播放器或其他）
class EditorRegion : public EditorItem{
public:
	// 实体类型
	ORION_TYPE_DECLARE(EDITOR_REGION)
	EditorRegion(ObjectTypeResourceObject* prototype, QGraphicsItem * parent = 0);
	EditorRegion(const QRectF& rect,  const QSizeF& origin = QSizeF(0.5, 0.5), QGraphicsItem * parent = 0);
	virtual ~EditorRegion();

	// 绘制触发区域形状
	virtual void drawShape(QPainter *painter);
	// 编码
	virtual void encodeData(QDataStream& stream);
	virtual void decodeData(QDataStream& stream);
	// 缺省数据
	virtual void encodeDefault(QDataStream& stream);
	virtual void decodeDefault(QDataStream& stream);

	void saveDefault();
	void loadDefault();

	// 移动到图层
	void changeToLayer(const QString& layer);

	// 获取父透明度
	qreal getParentOpacity();

	void setParameter(const QString& prop, const QString& value, const QString& plugin = QString());

	QString getParameterString(const QString& name, const QString& plugin = QString());
	int getParameterInt(const QString& name, const QString& plugin = QString());
	double getParameterDouble(const QString& name, const QString& plugin = QString());
	bool getParameterBool(const QString& name, const QString& plugin = QString());
	int getParameterEnum( const QString& name, const QString& plugin );

	int blend_; // 混合模式
	int fallback_;  // 回调类型
	QColor borderColor_; // 边框颜色
	//
	ObjectTypeResourceObject* typePrototype_;  // 类型原型
	QMap<ResourceHash, QString> parameters_; // 实例参数（key为 pluginName.paramName）

protected:
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

	bool boundScene_;  // 是否限制在场景中
};


#endif // ORION_EDITORREGION_H
