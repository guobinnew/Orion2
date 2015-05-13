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

#ifndef  PROJECT_MANAGER_H
#define PROJECT_MANAGER_H

#include "commondefine.h"
#include <QString>
#include <QMap>
#include <QSettings>

class ProjectManager{
public:
    static void staticInit();

	// 准备JSON文件
	void prepareJson();

	// 是否为Native项目
	bool isNativeProject(unsigned int id);

	// 项目类型列表
	QList<unsigned int> projectTypes();

	// 获取项目信息
	QStringList projectInfo(unsigned int id);

private:
	QList<QJsonObject> projectTypes_;   // 项目类型
	QHash<unsigned int, unsigned int>  baseIdMap_;  // ID映射

};

extern ProjectManager* gProjectManager;



#endif // OSS_MIME_MANAGER_H
