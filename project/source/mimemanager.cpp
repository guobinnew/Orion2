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

#include "mimemanager.h"
#include <QtGui>
#include <QtGlobal>
#include <QJsonDocument>
#include "configmanager.h"

//////////////////////////////////////////////////////////////////////////
// 临时函数
bool makeMimeJsonFile( const QString& in, const QString& out){
	QByteArray inData;
	QFile inFile(in);
	if (inFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		inData = inFile.readAll();
		inFile.close();
	}
	else{
		return false;
	}

	QJsonDocument document;
	QJsonArray arr;
	QStringList items = QString(inData).simplified().remove(QRegExp(UDQ_T("[{}\"]"))).split(UDQ_T(","));

	while (items.size() >= 2){
		QString type = items.takeLast().trimmed();
		while (type.isEmpty()){
			type = items.takeLast().trimmed();
		}

		QString id = items.takeLast().trimmed();
		while (id.isEmpty()){
			id = items.takeLast().trimmed();
		}

		QJsonObject obj;
		obj.insert(UDQ_T("id"), id);
		obj.insert(UDQ_T("type"), type);
		arr.push_front(obj);
	}

	document.setArray(arr);
	QByteArray outData = document.toJson();

	QFile outFile(out);
	if (outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)){
		outFile.write(outData);
		outFile.close();
		return true;
	}

	return false;
}

void MIMEManager::staticInit(){

}

// 准备JSON文件
void MIMEManager::prepareJson(){

	mimes_.clear();

	QJsonParseError error;

	QByteArray jsonData;
	QFile inFile(UDQ_T(":/json/") + gConfigManager->defaultMimeJson());
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
			mimes_.append(obj);
			int index = mimes_.size() - 1;
			baseIdMap_.insert(id, index);

			QString type = obj.value(UDQ_T("type")).toString();
			if (!type.isEmpty()){
				baseTypeMap_.insert(type, index);
			}
		}
	}

	// 读取扩展数据

	// 读取本地扩展MIME目录

	upateExtJson(QStringList());

}

// 更新扩展MIME
void MIMEManager::upateExtJson(const QStringList& extList){

}

QString MIMEManager::mimeType(const QString& suffix, bool dot){
	bool found = false;
	QString id = suffix.toLower();
	QString key;
	if (!dot){
		id.push_front(UDQ_T("."));
	}

	if (gConfigManager->useExtentedMime()){
		// 先检查扩展类型
		if (extIdMap_.contains(id)){
			found = true;
			Q_ASSERT(extIdMap_[id] < mimes_.size());
			key = mimes_.at(extIdMap_[id]).value(UDQ_T("type")).toString();
		}
	}

	if (!found){
		// 检查基础MIME
		if (baseIdMap_.contains(id)){
			found = true;		
			Q_ASSERT(baseIdMap_[id] < mimes_.size());
			key = mimes_.at(baseIdMap_[id]).value(UDQ_T("type")).toString();
		}
	}

	return key;
}

QString MIMEManager::mimeTypeByType(const QString& suffix, bool dot){
	bool found = false;
	QString key = suffix.toLower();
	QString id;
	if (gConfigManager->useExtentedMime()){
		// 先检查扩展类型
		if (extTypeMap_.contains(key)){
			found = true;
			Q_ASSERT(extTypeMap_[key] < mimes_.size());
			id = mimes_.at(extTypeMap_[key]).value(UDQ_T("id")).toString();
		}
	}

	if (!found){
		// 检查基础MIME
		if (baseTypeMap_.contains(key)){
			found = true;
			Q_ASSERT(baseTypeMap_[key] < mimes_.size());
			id = mimes_.at(baseTypeMap_[key]).value(UDQ_T("id")).toString();
		}
	}

	if (!dot && id.startsWith(UDQ_T("."))){
		id.remove(0,1);
	}

	return id;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

static MIMEManager OSSMIMEManager;
MIMEManager* gMIMEManager = &OSSMIMEManager;


