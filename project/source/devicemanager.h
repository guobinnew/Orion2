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

#ifndef  DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include "commondefine.h"
#include <QString>
#include <QMap>
#include <QSettings>


class DeviceManager{
public:
    static void staticInit();

	// 准备JSON文件
	void prepareJson();

	// 设备ID列表
	QStringList  deviceIds(){
		return deviceIds_;
	}

	// 获取缺省设备名
	QString defaultDevice();

	// 设备大小
	QSize deviceResolution(const QString& id);

private:
	int defaultIndex_;  // 缺省设备索引
	QStringList deviceIds_; // 设备IDs
	QHash<unsigned int, QSize>  baseIdMap_;  // ID映射
	
};

extern DeviceManager* gDeviceManager;



#endif // OSS_MIME_MANAGER_H
