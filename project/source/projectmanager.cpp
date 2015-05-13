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

#include "projectmanager.h"
#include <QtGui>
#include <QtGlobal>
#include <QJsonDocument>
#include "configmanager.h"

const int PROJECT_BLANK = 0;  // 空白项目

const int PROJECT_USERTYPE = 100;

void ProjectManager::staticInit(){

}

// 准备JSON文件
void ProjectManager::prepareJson(){

	projectTypes_.clear();

	QJsonParseError error;

	QByteArray jsonData;
	QFile inFile(UDQ_T(":/json/project.json"));
	if (inFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		jsonData = inFile.readAll();
		inFile.close();
	}

	QJsonDocument document =  QJsonDocument::fromJson(jsonData, &error);
	Q_ASSERT(error.error == QJsonParseError::NoError && document.isArray() );

	QJsonArray arr = document.array();
	for (int i = 0; i < arr.size(); i++){
		QJsonObject obj = arr[i].toObject();
		QString id = obj.value(UDQ_T("id")).toString();

		if (!id.isEmpty()){
			projectTypes_.append(obj);
			int index = projectTypes_.size() - 1;
			baseIdMap_.insert(id.toInt(), index);
		}
	}

	// 读取扩展数据

}

bool ProjectManager::isNativeProject(unsigned int id){
	return id >= PROJECT_USERTYPE;
}

QList<unsigned int> ProjectManager::projectTypes(){
	QList<unsigned int> ids = baseIdMap_.uniqueKeys();
	qSort(ids.begin(), ids.end());
	return ids;
}

QStringList ProjectManager::projectInfo(unsigned int id){
	QStringList info;
		// 检查基础MIME
	if (baseIdMap_.contains(id)){
			Q_ASSERT(baseIdMap_[id] < projectTypes_.size());
			info.append(projectTypes_.at(baseIdMap_[id]).value(UDQ_T("name")).toString());
			info.append(projectTypes_.at(baseIdMap_[id]).value(UDQ_T("icon")).toString());
			info.append(projectTypes_.at(baseIdMap_[id]).value(UDQ_T("desc")).toString());
			info.append(QString::number(projectTypes_.at(baseIdMap_[id]).value(UDQ_T("width")).toInt()));
			info.append(QString::number(projectTypes_.at(baseIdMap_[id]).value(UDQ_T("height")).toInt()));
			info.append(QString::number(projectTypes_.at(baseIdMap_[id]).value(UDQ_T("aspect")).toInt()));
	}
	return info;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

static ProjectManager OrionProjectManager;
ProjectManager* gProjectManager = &OrionProjectManager;


