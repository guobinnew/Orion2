/************************************************************************
**
**  Copyright (C) 2015  guobin  <guobin.gb@alibaba-inc.com>
**
**  This file is part of Orion2.
**
**  Orion is free software: you can redistribute it and/or modify
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

#ifndef ORION_TOOLDEFINE_H
#define ORION_TOOLDEFINE_H

#include <QMessageBox>
#include <QRegExp>
#include <QSize>
#include <QTreeWidget>
#include "commondefine.h"

#define ITEM_HASHKEY          Qt::UserRole + 1
#define ITEM_GROUPKEY      Qt::UserRole + 2
#define ITEM_VALUE              Qt::UserRole + 3
#define ITEM_DESC                 Qt::UserRole + 4

#define ITEM_THUMBNAIL       Qt::UserRole + 5
#define ITEM_ASPECT               Qt::UserRole + 6
#define ITEM_SCREEN               Qt::UserRole + 7

#define ITEM_SIZE                   Qt::UserRole + 100

#define ITEM_CONTEXT          Qt::UserRole + 255

enum{
	ITEM_USERTYPE =   1000
};

#define ITEM_GROUP               ITEM_USERTYPE + 1
#define ITEM_PROJECT             ITEM_USERTYPE + 2

#define ITEM_ENTITYGROUP      ITEM_USERTYPE + 11
#define ITEM_OBJECTTYPE         ITEM_USERTYPE + 12
#define ITEM_INSTANCE             ITEM_USERTYPE + 13

#define ITEM_FAMILYGROUP       ITEM_USERTYPE + 15
#define ITEM_FAMILY                    ITEM_USERTYPE + 16
#define ITEM_FAMILYMEMBER     ITEM_USERTYPE + 17

#define ITEM_LEVELGROUP     ITEM_USERTYPE + 20
#define ITEM_LEVEL                  ITEM_USERTYPE + 21
#define ITEM_LAYER                  ITEM_USERTYPE + 22

#define ITEM_UNCLASS         ITEM_USERTYPE + 40 
#define ITEM_CATEGORY      ITEM_USERTYPE + 41
#define ITEM_GALLERY          ITEM_USERTYPE + 42

#define ITEM_DATATYPE        ITEM_USERTYPE + 50
#define ITEM_PACKAGE      ITEM_USERTYPE + 51
#define ITEM_PLATFORM      ITEM_USERTYPE + 52

#define ITEM_PLUGINGROUP  ITEM_USERTYPE + 60
#define ITEM_ENTITY                    ITEM_USERTYPE + 61
#define ITEM_BEHAVIOR           ITEM_USERTYPE + 62
#define ITEM_EFFECT                  ITEM_USERTYPE + 63
#define ITEM_NATIVE                 ITEM_USERTYPE + 64

#define  ITEM_VARIABLE        ITEM_USERTYPE + 70
#define  ITEM_CONDITION    ITEM_USERTYPE + 71
#define  ITEM_ACTION            ITEM_USERTYPE + 72
#define  ITEM_EXPRESS            ITEM_USERTYPE + 73

#define ITEM_BLUEPRINT       ITEM_USERTYPE + 80

#define ITEM_SPRITE       ITEM_USERTYPE + 90

#define ITEM_SOUNDGROUP       ITEM_USERTYPE + 100
#define ITEM_SOUND                     ITEM_USERTYPE + 101

#define ITEM_MOVIEGROUP       ITEM_USERTYPE + 110
#define ITEM_MOVIE                     ITEM_USERTYPE + 111

#define ITEM_ATTRIBUTE         ITEM_USERTYPE + 150




enum{
	FILTER_IMAGE = 1,
	FILTER_AUDIO = 2,
	FILTER_VIDEO = 4,

	FILTER_ALL = 0x7F,
};

// 缩略图缺省大小
const int THUMBNAIL_SIZE = 128;

// 窗口边框大小，用于Resize时自动调整控件位置
const int WIDGET_MARGIN = 2;

const QSize IPAD_SIZE_V = QSize(768, 1024);
const QSize NEWIPAD_SIZE_V = QSize(1536, 2048);

#define IMAGE2BYTEARRAY( img ) \
	QByteArray imgdata; \
{\
	QDataStream stream(&imgdata, QIODevice::WriteOnly | QIODevice::Truncate); \
	stream << img; \
}

#define BYTEARRAY2IMAGE( imgdata ) \
	QPixmap img;\
	{\
	QDataStream stream(imgdata); \
		stream >> img;\
	}

#define LINEEDIT_CHECK(a, b, c) \
	if( a->text().isEmpty() ){ \
	QMessageBox::critical(this, b, UDQ_TR("%1不能为空!").arg(c)); \
		return false; }

#define LINEEDIT_VALID(a, b, c, d) \
	{ QRegExp r(d); \
       if( !r.exactMatch( a->text()) ){ \
	   QMessageBox::critical(this, b, UDQ_TR("%1不合法!").arg(c)); \
	      return false; } }

#define TEXTEDIT_CHECK(a, b, c) \
	if( a->toPlainText().isEmpty() ){ \
	QMessageBox::critical(this, b, UDQ_TR("%1不能为空!").arg(c)); \
	return false; }

#define COMBOX_CHECK(a, b, c) \
	if( a->currentText().isEmpty() ){ \
	QMessageBox::critical(this, b, UDQ_TR("%1不能为空!").arg(c)); \
	return false; }

#define LISTWIDGET_CHECK(a, b, c) \
	if( a->count() == 0 ){ \
	QMessageBox::critical(this, b, UDQ_TR("%1不能为空!").arg(c)); \
	return false; }

#define PICTURE_CHECK(a, b, c) \
	if( a->getPictureData().isEmpty() ){ \
	QMessageBox::critical(this, b, UDQ_TR("%1不能为空!").arg(c)); \
	return false; }

#define PACKAGE_CHECK(a, b) \
	COMBOX_CHECK(a, b, UDQ_TR("资源包")); \
	if( a->currentText() == gConfigManager->basepkgConfigs() && !gConfigManager->isBaseAdmin() ){ \
	QMessageBox::critical(this, b, UDQ_TR("不能修改基础资源包[%1]!").arg(gConfigManager->basepkgConfigs())); \
		return false; } 


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

#define DELETE_TREEITEM( parent ) \
	while (parent && parent->childCount() > 0 ){ \
		QTreeWidgetItem* child = parent->takeChild(0); \
		delete child; \
	}




////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
// JSON 格式
const QString JSON_ID = UDQ_T("id");
const QString JSON_NAME = UDQ_T("name");
const QString JSON_DESC = UDQ_T("description");
const QString JSON_VER = UDQ_T("version");
const QString JSON_TYPE = UDQ_T("type");
const QString JSON_DEF = UDQ_T("default");
const QString JSON_FLAGS = UDQ_T("flags");
const QString JSON_CATEGORY = UDQ_T("category");
const QString JSON_FUNC = UDQ_T("function");
const QString JSON_ENUM = UDQ_T("enums");
const QString JSON_PARAM = UDQ_T("parameters");
const QString JSON_EXTEND = UDQ_T("extend");
const QString JSON_TITLE = UDQ_T("title");

#endif
