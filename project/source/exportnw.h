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

#ifndef ORION_EXPORTNWJS_H
#define ORION_EXPORTNWJS_H


#include "exporter.h"
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;


namespace ORION{

	enum{
		FCT_PACKAGE,
		FCT_ICUDTL,
		FCT_DXWEB,

		//win64
		FCT_WIN64_D3D = 10,
		FCT_WIN64_FFMPEG,
		FCT_WIN64_EGL,
		FCT_WIN64_GLES,
		FCT_WIN64_NWEXE,
		FCT_WIN64_NWPAK,

		//win32
		FCT_WIN32_D3D = 50,
		FCT_WIN32_FFMPEG,
		FCT_WIN32_EGL,
		FCT_WIN32_GLES,
		FCT_WIN32_NWEXE,
		FCT_WIN32_NWPAK,

		// linux64
		FCT_LINUX64_FFMPEG = 100,
		FCT_LINUX64_NWEXE,
		FCT_LINUX64_NWPAK,

		// linux32
		FCT_LINUX32_FFMPEG = 150,
		FCT_LINUX32_NWEXE,
		FCT_LINUX32_NWPAK,

		// osx64
		FCT_OSX64_FRAME = 200,
		FCT_OSX64_NWJS,
		FCT_OSX64_RESOURCE,
		FCT_OSX64_PLIST,
		FCT_OSX64_PKGINFO,

		// osx32
		FCT_OSX32_FRAME = 250,
		FCT_OSX32_NWJS,
		FCT_OSX32_RESOURCE,
		FCT_OSX32_PLIST,
		FCT_OSX32_PKGINFO,
	};

}

class NWJSExporter : public Exporter
{
	Q_OBJECT
public:
	NWJSExporter(ProjectResourceObject* proj);
	virtual ~NWJSExporter();

	virtual void write(int mode = 0);
	virtual QString name();

	enum{
		WMD_SINGLE= 1,
		WMD_RESIZE = 2,
		WMD_FRAME = 4,
		WMD_KIOSK = 8,
	};


	enum{
		PLT_WIN32 = 1,
		PLT_WIN64 = 2,
		PLT_LINUX32 = 4,
		PLT_LINUX64 = 8,
		PLT_OSX32 = 16,
		PLT_OSX64 = 32,
		PLT_ALL = 0x8F,
	};

	void setWindowFlags(int flags){
		flags_ = flags;
	}

	void addPlatform(int flag){
		platforms_ |= flag;
	}

	void removePlatform(int flag){
		platforms_ &= ~flag;
	}

protected:
	virtual void makeTemplateDB(const QString& file, const QString& src, bool afterDelete = false);

private:

	//生成文件
	void generateFile(int type,  const QString& filePath, bool text = true, bool cache = true );
	QByteArray replaceTextContent(const QByteArray& code, int	encodeType);

	// 准备工作
	void prepare();

	// 打包pkg
	bool saveToPackage(const QString &temppath, const QString &pkgpath);
	// 解包
	bool loadFromToPackage(const QString &temppath, const QString &pkgpath);

	ProjectResourceObject* project_;  // 工程
	// 内部使用
	QString exportPath_;  // 导出工程路径
	QString dbPath_;  // 数据库文件路径

	int flags_;  // 窗口标志
	int platforms_;  //  导出平台
};

#endif // ORION_EXPORTEPUB_H

