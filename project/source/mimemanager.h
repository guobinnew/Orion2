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

#ifndef  MIME_MANAGER_H
#define MIME_MANAGER_H

#include "commondefine.h"
#include <QString>
#include <QMap>
#include <QSettings>


class MIMEManager{
public:
    static void staticInit();

	// 准备JSON文件
	void prepareJson();
	// 更新扩展MIME
	void upateExtJson(const QStringList& extList);

	// 根据扩展名获取类型标志
	QString mimeType(const QString& suffix, bool dot = false);
	// 根据类型获取扩展名
	QString mimeTypeByType(const QString& suffix, bool dot = false);

private:
	QList<QJsonObject> mimes_;
	QHash<QString, unsigned int>  baseIdMap_;  // ID映射
	QHash<QString, unsigned int>  baseTypeMap_;  // Type映射

	QHash<QString, unsigned int>  extIdMap_;
	QHash<QString, unsigned int>  extTypeMap_;

};

extern MIMEManager* gMIMEManager;



#endif // OSS_MIME_MANAGER_H
