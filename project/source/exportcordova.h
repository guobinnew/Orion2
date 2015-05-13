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
**  along with Orion2.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#ifndef ORION_EXPORTCORDOVA_H
#define ORION_EXPORTCORDOVA_H


#include "exporter.h"
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;


namespace ORION{

	enum{
		FCT_CONFIG,
		FCT_XDKCONFIG,
	};

}

class CordovaExporter : public Exporter
{
	Q_OBJECT
public:
	CordovaExporter(ProjectResourceObject* proj);
	virtual ~CordovaExporter();

	virtual void write(int mode = 0);
	virtual QString name();

	enum{
		DEV_UNIVERSAL,
		DEV_HANDSETS,
		DEV_TABLETS,
	};

	enum{
		PRM_GEO = 1,
		PRM_VIBRATE = 2,
		PRM_CAMERA = 4,
		PRM_MEIDA = 8,
	};

	enum{
		AUDIO_M4A = 1,
		AUDIO_OGG = 2,
	};

	enum{
		IOS_ANY = 0,
		IOS_7,
		IOS_8,
	};

	enum{
		AND_ANY = 0,
		AND_4_0,
		AND_4_1,
		AND_4_2,
		AND_4_3,
		AND_4_4,
		AND_5_0,
	};

	void setDevice(int flags){
		device_ = flags;
	}

	void setFullscreen(bool flag){
		fullscreen_ = flag;
	}

	void setPermission(int flags){
		permission_ = flags;
	}

	void setAudioFormat(int flags){
		audio_ = flags;
	}

	void setiOSVersion(int flags){
		iosversion_ = flags;
	}

	void setAndriodVersion(int flags){
		andriodversion_ = flags;
	}

protected:
	virtual void makeTemplateDB(const QString& file, const QString& src, bool afterDelete = false);

private:

	//生成文件
	void generateFile(int type,  const QString& filePath, bool text = true, bool cache = true );
	QByteArray replaceTextContent(const QByteArray& code, int	encodeType);

	// 准备工作
	void prepare();

	QString makeIOSVersion(int ver);
	QString makeAndriodVersion(int ver);

	ProjectResourceObject* project_;  // 工程
	// 内部使用
	QString exportPath_;  // 导出工程路径
	QString dbPath_;  // 数据库文件路径

	int device_;  // 
	bool fullscreen_;  //  是否全屏显示
	int permission_;  //  
	int audio_;  //  音频格式
	int iosversion_;  //  
	int andriodversion_;  //  
};

#endif // ORION_EXPORTCORDOVA_H

