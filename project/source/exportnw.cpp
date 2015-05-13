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
#include <QApplication>
#include <QDateTime>

#include <string.h>
#include <zip.h>
#include <unzip.h>
#ifdef _WIN32
#include <iowin32.h>
#endif


#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QTextStream>
#include "exportnw.h"
#include "exporterfactory.h"
#include "exporth5.h"
#include "tooldefine.h"
#include "leveldefine.h"
#include "componentdefine.h"
#include "mimemanager.h"
#include "kismetdefine.h"
#include "logmanager.h"
#include "kismetobject.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
using namespace ORION;

enum{
	ECT_LATIN1,
	ECT_UTF8,
	ECT_LOCAL8BIT,
};


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


NWJSExporter::NWJSExporter(ProjectResourceObject* proj)
	:project_(proj), platforms_(0), flags_(0)
{
	dbPath_ = QString(UDQ_T("%1/Exporter/%2.db")).arg(QApplication::applicationDirPath()).arg(name());
	exportPath_ = project_->exportDir_ + UDQ_T("/") + project_->objectName();
}

NWJSExporter::~NWJSExporter()
{
}

QString NWJSExporter::name(){
	return UDQ_T("nwjs");
}

void NWJSExporter::write(int mode )
{

	// 确保导出目录存在
	QDir dir(exportPath_);
	if (!dir.exists()){
		dir.mkpath(exportPath_);
	}
	else{
		// 清空目录
		deleteDir(exportPath_, false);
	}

	// 生成前准备工作
	prepare();
	emit progress(10);

	QString tmppath = exportPath_ + UDQ_T("/") + project_->objectName();
	// 生成json文件
	generateFile(FCT_PACKAGE, tmppath + UDQ_T("/package.json"));
	// 生成pkg打包
	saveToPackage(tmppath, exportPath_);
	QString srcpackage = exportPath_ + UDQ_T("/package.nw");
	emit progress(20);

	QString desPath = exportPath_ + UDQ_T("/");
	if (platforms_ & PLT_WIN32){
		//  生成win32文件
		generateFile(FCT_WIN32_D3D, desPath + UDQ_T("win32/d3dcompiler_47.dll"), false);
		generateFile(FCT_WIN32_FFMPEG, desPath + UDQ_T("win32/ffmpegsumo.dll"), false);
		generateFile(FCT_DXWEB, desPath + UDQ_T("win32/dxwebsetup.exe"), false);
		generateFile(FCT_ICUDTL, desPath + UDQ_T("win32/icudtl.dat"), false);
		generateFile(FCT_WIN32_EGL, desPath + UDQ_T("win32/libEGL.dll"), false);
		generateFile(FCT_WIN32_GLES, desPath + UDQ_T("win32/libGLESv2.dll"), false);
		generateFile(FCT_WIN32_NWEXE, desPath + UDQ_TR("win32/%1.exe").arg(project_->getExportName()), false);
		generateFile(FCT_WIN32_NWPAK, desPath + UDQ_T("win32/nw.pak"), false);

		//  拷贝package.nw
		QFile::copy(srcpackage, desPath + UDQ_T("win32/package.nw"));
	}
	emit progress(30);
	if (platforms_ & PLT_WIN64){
		//  生成win64文件
		generateFile(FCT_WIN64_D3D, desPath + UDQ_T("win64/d3dcompiler_47.dll"), false);
		generateFile(FCT_WIN64_FFMPEG, desPath + UDQ_T("win64/ffmpegsumo.dll"), false);
		generateFile(FCT_DXWEB, desPath + UDQ_T("win64/dxwebsetup.exe"), false);
		generateFile(FCT_ICUDTL, desPath + UDQ_T("win64/icudtl.dat"), false);
		generateFile(FCT_WIN64_EGL, desPath + UDQ_T("win64/libEGL.dll"), false);
		generateFile(FCT_WIN64_GLES, desPath + UDQ_T("win64/libGLESv2.dll"), false);
		generateFile(FCT_WIN64_NWEXE, desPath + UDQ_TR("win64/%1.exe").arg(project_->getExportName()), false);
		generateFile(FCT_WIN64_NWPAK, desPath + UDQ_T("win64/nw.pak"), false);

		//  拷贝package.nw
		QFile::copy(srcpackage, desPath + UDQ_T("win64/package.nw"));
	}
	emit progress(40);

	if (platforms_ & PLT_LINUX32){
		//  生成linux32文件
		generateFile(FCT_LINUX32_FFMPEG, desPath + UDQ_T("linux32/libffmpegsumo.so"), false);
		generateFile(FCT_LINUX32_NWEXE, desPath + UDQ_TR("linux32/%1").arg(project_->getExportName()), false);
		generateFile(FCT_LINUX32_NWPAK, desPath + UDQ_T("linux32/nw.pak"), false);
		generateFile(FCT_ICUDTL, desPath + UDQ_T("linux32/icudtl.dat"), false);

		//  拷贝package.nw
		QFile::copy(srcpackage, desPath + UDQ_T("linux32/package.nw"));
	}
	emit progress(50);

	if (platforms_ & PLT_LINUX64){
		//  生成linux64文件
		generateFile(FCT_LINUX64_FFMPEG, desPath + UDQ_T("linux64/libffmpegsumo.so"), false);
		generateFile(FCT_LINUX64_NWEXE, desPath + UDQ_TR("linux64/%1").arg(project_->getExportName()), false);
		generateFile(FCT_LINUX64_NWPAK, desPath + UDQ_T("linux64/nw.pak"), false);
		generateFile(FCT_ICUDTL, desPath + UDQ_T("linux64/icudtl.dat"), false);
		//  拷贝package.nw
		QFile::copy(srcpackage, desPath + UDQ_T("linux64/package.nw"));
	}
	emit progress(60);
	if (platforms_ & PLT_OSX32){

		// app.Contents目录
		QString appContents = desPath + UDQ_TR("osx32/%1.app/Contents").arg(project_->objectName()) + UDQ_T("/");

		//  生成osx32文件
		generateFile(FCT_OSX32_PLIST, appContents + UDQ_T("Info.plist"));
		generateFile(FCT_OSX32_PKGINFO, appContents + UDQ_T("PkgInfo"), false);
		generateFile(FCT_OSX32_NWJS, appContents + UDQ_T("MacOS/nwjs"), false);

		// 解压Framework
		generateFile(FCT_OSX32_FRAME, appContents + UDQ_T("Frameworks.zip"), false);
		// 解压包
		loadFromToPackage(appContents, appContents + UDQ_T("Frameworks.zip"));
		generateFile(FCT_ICUDTL, appContents + UDQ_T("Frameworks/nwjs Framework.framework/Resources/icudtl.dat"), false);
		// 删除文件
		QFile::remove(appContents + UDQ_T("Frameworks.zip"));
		// 解压Resource目录
		generateFile(FCT_OSX32_RESOURCE, appContents + UDQ_T("Resources.zip"), false);
		// 解压包
		loadFromToPackage(appContents, appContents + UDQ_T("Resources.zip"));
		// 删除文件
		QFile::remove(appContents + UDQ_T("Resources.zip"));
		// 解压
		loadFromToPackage(appContents+UDQ_T("Resources/app.nw/"), srcpackage);
	}
	emit progress(70);
	if (platforms_ & PLT_OSX64){
		//  生成osx64文件
		// app.Contents目录
		QString appContents = desPath + UDQ_TR("osx64/%1.app/Contents").arg(project_->objectName()) + UDQ_T("/");

		generateFile(FCT_OSX64_PLIST, appContents + UDQ_T("Info.plist"));
		generateFile(FCT_OSX64_PKGINFO, appContents + UDQ_T("PkgInfo"), false);
		generateFile(FCT_OSX64_NWJS, appContents + UDQ_T("MacOS/nwjs"), false);

		// 解压Framework
		generateFile(FCT_OSX64_FRAME, appContents + UDQ_T("Frameworks.zip"), false);
		// 解压包
		loadFromToPackage(appContents, appContents + UDQ_T("Frameworks.zip"));
		generateFile(FCT_ICUDTL, appContents + UDQ_T("Frameworks/nwjs Framework.framework/Resources/icudtl.dat"), false);
		// 删除文件
		QFile::remove(appContents + UDQ_T("Frameworks.zip"));
		// 解压Resource目录
		generateFile(FCT_OSX64_RESOURCE, appContents + UDQ_T("Resources.zip"), false);
		// 解压包
		loadFromToPackage(appContents, appContents + UDQ_T("Resources.zip"));
		// 删除文件
		QFile::remove(appContents + UDQ_T("Resources.zip"));
		// 解压
		loadFromToPackage(appContents + UDQ_T("Resources/app.nw/"), srcpackage);
	}
	emit progress(80);

	// 删除package文件
	QFile::remove(srcpackage);
	emit progress(90);
}

void  NWJSExporter::generateFile(int type, const QString& desPath, bool text, bool cache){

	QByteArray code;
	// 从DB中加载模板
	loadTextFromDB(dbPath_, type, code);

	// 内容替换
	if (text){
		code = replaceTextContent(code, ECT_UTF8);
		saveTextToDisk(desPath, code);
	}
	else{
		saveDataToDisk(desPath, code);
	}

	if (cache){
		QDir dir(exportPath_);
		QString relpath = dir.relativeFilePath(desPath);
		if (relpath.startsWith(UDQ_T("/")) || relpath.startsWith(UDQ_T("\\"))){
			relpath = relpath.mid(1);
		}
	}

}

// 替换内容
QByteArray NWJSExporter::replaceTextContent(const QByteArray& code, int	encodeType){

	QString codeTxt;
	QString tmp;
	switch (encodeType){
	case ECT_LATIN1:
		codeTxt = QString::fromLatin1(code.data(), code.size());
		break;
	case ECT_UTF8:
		codeTxt = QString::fromUtf8(code.data(), code.size());
		break;
	case ECT_LOCAL8BIT:
		codeTxt = QString::fromLocal8Bit(code.data(), code.size());
		break;
	}

	//替换内容
	// %ORION_NAME% 
	QString name = project_->getExportName();
	codeTxt.replace(UDQ_T("%ORION_NAME%"), name);

	// %ORION_VERSION% 
	codeTxt.replace(UDQ_T("%ORION_VERSION%"), project_->pubVersion_);

	// %ORION_VERSION% 
	codeTxt.replace(UDQ_T("%ORION_DESC%"), project_->description());


	// %ORION_WIDTH%
	QString width = QString::number(project_->resolution_.width());
	codeTxt.replace(UDQ_T("%ORION_WIDTH%"), width);

	// %ORION_HEIGHT%
	QString height = QString::number(project_->resolution_.height());
	codeTxt.replace(UDQ_T("%ORION_HEIGHT%"), height);

	// %ORION_SINGLE% 
	QString single = (flags_ & WMD_SINGLE) ? UDQ_T("true") : UDQ_T("false");
	codeTxt.replace(UDQ_T("%ORION_SINGLE%"), single);

	// %ORION_RESIZE% 
	QString resize = (flags_ & WMD_RESIZE) ? UDQ_T("true") : UDQ_T("false");
	codeTxt.replace(UDQ_T("%ORION_RESIZE%"), resize);

	// %ORION_FRAME% 
	QString frame = (flags_ & WMD_FRAME) ? UDQ_T("true") : UDQ_T("false");
	codeTxt.replace(UDQ_T("%ORION_FRAME%"), frame);

	// %ORION_KIOSK% 
	QString kiosk = (flags_ & WMD_KIOSK) ? UDQ_T("true") : UDQ_T("false");
	codeTxt.replace(UDQ_T("%ORION_KIOSK%"), kiosk);

	//  %ORION_TIMESTAMP% 
	QString timestamp = QDateTime::currentDateTime().toString(UDQ_T("yyyyMMddhhmmss"));
	codeTxt.replace(UDQ_T("%ORION_TIMESTAMP%"), timestamp);


	if (encodeType == ECT_LOCAL8BIT){
		return codeTxt.toLocal8Bit();
	}
	else if (encodeType == ECT_LATIN1){
		return codeTxt.toLatin1();
	}

	return codeTxt.toUtf8();

}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void NWJSExporter::prepare(){

	// 先导出H5文件
	QString oldexpdir = project_->exportDir_;
	project_->exportDir_ = exportPath_;

	Exporter* exporter = gExporterFactory->getExporter(Exporter::TYPE_HTML5, project_);
	Q_ASSERT(exporter != NULL);
	exporter->write(2);
	delete exporter;

	// 恢复原来目录
	project_->exportDir_ = oldexpdir;

}

////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void NWJSExporter::makeTemplateDB(const QString& file, const QString& src, bool afterDelete){

	// 创建目标目录
	QFileInfo fileInfo(file);
	QDir dir;
	if (!dir.exists(fileInfo.absolutePath()))
		dir.mkpath(fileInfo.absolutePath());

	Db db(NULL, 0);
	u_int32_t oFlags = DB_CREATE;
	try {

		QString datFile = file;
		std::string arrstr = datFile.toStdString();

		db.open(NULL,
			arrstr.c_str(),
			NULL,
			DB_BTREE,
			oFlags,
			0);

		// 循环每个目录保存文件到数据库
		QString dirPath;
		if (dir.exists(src)){

			saveFile(db, FCT_PACKAGE, UDQ_T("package.json"), src);
			saveFile(db, FCT_ICUDTL, UDQ_T("icudtl.dat"), src, false);
			saveFile(db, FCT_DXWEB, UDQ_T("dxwebsetup.exe"), src, false);
			// win64
			saveFile(db, FCT_WIN64_D3D, UDQ_T("win64/d3dcompiler_47.dll"), src, false);
			saveFile(db, FCT_WIN64_FFMPEG, UDQ_T("win64/ffmpegsumo.dll"), src, false);
			saveFile(db, FCT_WIN64_EGL, UDQ_T("win64/libEGL.dll"), src, false);
			saveFile(db, FCT_WIN64_GLES, UDQ_T("win64/libGLESv2.dll"), src, false);
			saveFile(db, FCT_WIN64_NWEXE, UDQ_T("win64/nw.exe"), src, false);
			saveFile(db, FCT_WIN64_NWPAK, UDQ_T("win64/nw.pak"), src, false);

			// win32
			saveFile(db, FCT_WIN32_D3D, UDQ_T("win32/d3dcompiler_47.dll"), src, false);
			saveFile(db, FCT_WIN32_FFMPEG, UDQ_T("win32/ffmpegsumo.dll"), src, false);
			saveFile(db, FCT_WIN32_EGL, UDQ_T("win32/libEGL.dll"), src, false);
			saveFile(db, FCT_WIN32_GLES, UDQ_T("win32/libGLESv2.dll"), src, false);
			saveFile(db, FCT_WIN32_NWEXE, UDQ_T("win32/nw.exe"), src, false);
			saveFile(db, FCT_WIN32_NWPAK, UDQ_T("win32/nw.pak"), src, false);

			// linux64
			saveFile(db, FCT_LINUX64_FFMPEG, UDQ_T("linux64/libffmpegsumo.so"), src, false);
			saveFile(db, FCT_LINUX64_NWEXE, UDQ_T("linux64/nw"), src, false);
		    saveFile(db, FCT_LINUX64_NWPAK, UDQ_T("linux64/nw.pak"), src, false);

			// linux32
			 saveFile(db, FCT_LINUX32_FFMPEG, UDQ_T("linux32/libffmpegsumo.so"), src, false);
			 saveFile(db, FCT_LINUX32_NWEXE, UDQ_T("linux32/nw"), src, false);
			 saveFile(db, FCT_LINUX32_NWPAK, UDQ_T("linux32/nw.pak"), src, false);

			 // osx64
			 saveFile(db, FCT_OSX64_FRAME, UDQ_T("osx64/Frameworks.zip"), src, false);
			 saveFile(db, FCT_OSX64_NWJS, UDQ_T("osx64/nwjs"), src, false);
			 saveFile(db, FCT_OSX64_RESOURCE, UDQ_T("osx64/Resources.zip"), src, false);
			 saveFile(db, FCT_OSX64_PLIST, UDQ_T("osx64/Info.plist"), src);
			 saveFile(db, FCT_OSX64_PKGINFO, UDQ_T("osx64/PkgInfo"), src, false);

			 // osx32
			 saveFile(db, FCT_OSX32_FRAME, UDQ_T("osx32/Frameworks.zip"), src, false);
			 saveFile(db, FCT_OSX32_NWJS, UDQ_T("osx32/nwjs"), src, false);
			 saveFile(db, FCT_OSX32_RESOURCE, UDQ_T("osx32/Resources.zip"), src, false);
			 saveFile(db, FCT_OSX32_PLIST, UDQ_T("osx32/Info.plist"), src);
			 saveFile(db, FCT_OSX32_PKGINFO, UDQ_T("osx32/PkgInfo"), src, false);

			if (afterDelete){
				// 清空当前目录
				QDir d(dirPath);
				QStringList files = d.entryList(QDir::Files | QDir::Hidden);
				foreach(QString f, files){
					d.remove(f);
				}
			}
		}
		db.close(0);
	}
	catch (DbException &e) {
	}
	catch (std::exception &e) {
	}
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
const int ZIP_BUFFER_SIZE = 8 * 1024;   // 8K
bool NWJSExporter::saveToPackage(const QString &temppath, const QString &pkgpath)
{

	// 压缩包pkg临时文件
	QString tempFile = QString(UDQ_T("%1/package_temp.nw")).arg(pkgpath);
	QDateTime timeNow = QDateTime::currentDateTime();

	zip_fileinfo fileInfo;
#ifdef Q_OS_WIN32
	zlib_filefunc64_def ffunc;
	fill_win32_filefunc64W(&ffunc);
	zipFile zfile = zipOpen2_64(QDir::toNativeSeparators(tempFile).toStdWString().c_str(), APPEND_STATUS_CREATE, NULL, &ffunc);
#else
	zipFile zfile = zipOpen64(QDir::toNativeSeparators(tempFile).toUtf8().constData(), APPEND_STATUS_CREATE);
#endif

	if (zfile == NULL) {
		return false;
	}

	memset(&fileInfo, 0, sizeof(fileInfo));
	fileInfo.tmz_date.tm_sec = timeNow.time().second();
	fileInfo.tmz_date.tm_min = timeNow.time().minute();
	fileInfo.tmz_date.tm_hour = timeNow.time().hour();
	fileInfo.tmz_date.tm_mday = timeNow.date().day();
	fileInfo.tmz_date.tm_mon = timeNow.date().month() - 1;
	fileInfo.tmz_date.tm_year = timeNow.date().year();

	// 写入临时目录中所有文件
	QDirIterator it(temppath, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable | QDir::Hidden, QDirIterator::Subdirectories);

	while (it.hasNext()) {
		it.next();
		QString relpath = it.filePath().remove(temppath);  // 相对路径

		// 去掉相对路径开始的“/"
		while (relpath.startsWith(UDQ_T("/")) || relpath.startsWith(UDQ_T("\\"))) {
			relpath = relpath.remove(0, 1);
		}

		// Add the file entry to the archive.
		// We should check the uncompressed file size. If it's over >= 0xffffffff the last parameter (zip64) should be 1.
		if (zipOpenNewFileInZip4_64(zfile, relpath.toUtf8().constData(), &fileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, 8, 0, 15, 8, Z_DEFAULT_STRATEGY, NULL, 0, 0x0b00, 0, 0) != Z_OK) {
			zipClose(zfile, NULL);
			QFile::remove(tempFile);
			return false;
		}

		QFile dfile(it.filePath());
		if (!dfile.open(QIODevice::ReadOnly)) {
			zipCloseFileInZip(zfile);
			zipClose(zfile, NULL);
			QFile::remove(tempFile);
			return false;
		}

		// Write the data from the file on disk into the archive.
		char buff[ZIP_BUFFER_SIZE] = { 0 };
		qint64 read = 0;

		while ((read = dfile.read(buff, ZIP_BUFFER_SIZE)) > 0) {
			if (zipWriteInFileInZip(zfile, buff, read) != Z_OK) {
				dfile.close();
				zipCloseFileInZip(zfile);
				zipClose(zfile, NULL);
				QFile::remove(tempFile);
				return false;
			}
		}

		dfile.close();

		// There was an error reading the file on disk.
		if (read < 0) {
			zipCloseFileInZip(zfile);
			zipClose(zfile, NULL);
			QFile::remove(tempFile);
			return false;
		}

		if (zipCloseFileInZip(zfile) != Z_OK) {
			zipClose(zfile, NULL);
			QFile::remove(tempFile);
			return false;
		}
	}

	zipClose(zfile, NULL);
	// Overwrite the contents of the real file with the contents from the temp
	// file we saved the data do. We do this instead of simply copying the file
	// because a file copy will lose extended attributes such as labels on OS X.
	QFile temp_pkg(tempFile);

	if (!temp_pkg.open(QFile::ReadOnly)) {
		return false;
	}

	// 写入真正pkg文件
	QFile real_pkg(pkgpath + UDQ_T("/package.nw"));

	if (!real_pkg.open(QFile::WriteOnly | QFile::Truncate)) {
		temp_pkg.close();
		return false;
	}

	// 从临时文件拷贝内容
	char buff[ZIP_BUFFER_SIZE] = { 0 };
	qint64 read = 0;
	qint64 written = 0;

	while ((read = temp_pkg.read(buff, ZIP_BUFFER_SIZE)) > 0) {
		written = real_pkg.write(buff, read);

		if (written != read) {
			temp_pkg.close();
			real_pkg.close();
			QFile::remove(tempFile);
			return false;
		}
	}

	if (read == -1) {
		temp_pkg.close();
		real_pkg.close();
		QFile::remove(tempFile);
		return false;
	}

	temp_pkg.close();
	real_pkg.close();
	QFile::remove(tempFile);

	// 删除临时目录
	deleteDir(temppath);

	return true;
}


bool NWJSExporter::loadFromToPackage(const QString &temppath, const QString &pkgpath){
	
	// 解压
	QString tempFile = pkgpath;

	unz_file_info64 FileInfo;
#ifdef Q_OS_WIN32
	zlib_filefunc64_def ffunc;
	fill_win32_filefunc64W(&ffunc);
	zipFile zFile = unzOpen2_64(QDir::toNativeSeparators(tempFile).toStdWString().c_str(), &ffunc);
#else
	zipFile zFile = unzOpen64(QDir::toNativeSeparators(tempFile).toUtf8().constData());
#endif

	if (NULL == zFile)
		return false;

	unz_global_info64 gi;
	if (unzGetGlobalInfo64(zFile, &gi) != UNZ_OK)
	{
		unzClose(zFile);
		return false;
	}

	int result;
	char data[ZIP_BUFFER_SIZE] = { 0 };
	for (int i = 0; i < gi.number_entry; ++i)
	{
		char file[256] = { 0 };
		char ext[256] = { 0 };
		char com[1024] = { 0 };

		if (unzGetCurrentFileInfo64(zFile, &FileInfo, file, sizeof(file), ext, 256, com, 1024) != UNZ_OK){
			unzClose(zFile);
			return false;
		}
		
		if (!(FileInfo.external_fa & FILE_ATTRIBUTE_DIRECTORY)){ //文件，否则为目录
			//打开文件
			result = unzOpenCurrentFile(zFile);

		   int size;
		   //读取内容
		   QString path = temppath;
		   path.append(file);

		   // 确保目录被创建
		   QFileInfo info(path);
		   QDir dir = info.absoluteDir();
		   if (!dir.exists()){
			   dir.mkpath(info.absolutePath());
		   }

		   QFile f(path);
		   if (f.open(QFile::WriteOnly)){
			   while (true) {
				   size = unzReadCurrentFile(zFile, data, sizeof(data));
				   if (size <= 0)
					   break;
				   f.write(data, size);
			   }
			   f.close();
		   }
		}
		else{  // 目录
		}

		//关闭当前文件
		unzCloseCurrentFile(zFile);

		//出错
		if (i < gi.number_entry - 1 && unzGoToNextFile(zFile) != UNZ_OK){
			unzClose(zFile);
			return false;
		}
	}

	unzClose(zFile);
	return true;
}
