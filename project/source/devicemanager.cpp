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

#include "devicemanager.h"
#include <QtGui>
#include <QtGlobal>
#include <QJsonDocument>
#include "configmanager.h"
#include "commondefine.h"
using namespace ORION;

void DeviceManager::staticInit(){

}

// 准备JSON文件
void DeviceManager::prepareJson(){
	defaultIndex_ = -1;

	QJsonParseError error;

	QByteArray jsonData;
	QFile inFile(UDQ_T(":/json/device.json"));
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
			deviceIds_.append(id);

			QString w = obj.value(UDQ_T("width")).toString();
			QString h = obj.value(UDQ_T("height")).toString();
			baseIdMap_.insert(hashString(id), QSize(w.toInt(), h.toInt()));

			bool def = obj.value(UDQ_T("default")).toBool();
			if (def){
				defaultIndex_ = deviceIds_.size() - 1;
			}
		}
	}
}

QSize DeviceManager::deviceResolution(const QString& id){
	unsigned int key = hashString(id);
	QSize size(0, 0);
	if (baseIdMap_.contains(key)){
		size = baseIdMap_[key];
	}
	return size;
}

QString DeviceManager::defaultDevice(){

	if (defaultIndex_ >= deviceIds_.size()){
		defaultIndex_ = deviceIds_.size() - 1;
	}

	if (defaultIndex_ < 0){
		defaultIndex_ = 0;
	}

	if (deviceIds_.size() == 0){
		return QString();
	}

	return deviceIds_.at(defaultIndex_);

}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

static DeviceManager OrionDeviceManager;
DeviceManager* gDeviceManager = &OrionDeviceManager;
