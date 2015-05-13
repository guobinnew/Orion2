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
#include "exportcordova.h"
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


CordovaExporter::CordovaExporter(ProjectResourceObject* proj)
	:project_(proj)
{
	dbPath_ = QString(UDQ_T("%1/Exporter/%2.db")).arg(QApplication::applicationDirPath()).arg(name());
	exportPath_ = project_->exportDir_ + UDQ_T("/") + project_->objectName();
}

CordovaExporter::~CordovaExporter()
{
}

QString CordovaExporter::name(){
	return UDQ_T("cordova");
}

void CordovaExporter::write(int mode)
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

	//  生成config文件
	generateFile(FCT_CONFIG, exportPath_ + UDQ_T("/config.xml"));
	generateFile(FCT_XDKCONFIG, exportPath_ + UDQ_T("/intelxdk.config.additions.xml"));

	emit progress(90);
}

void  CordovaExporter::generateFile(int type, const QString& desPath, bool text, bool cache){

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
QByteArray CordovaExporter::replaceTextContent(const QByteArray& code, int	encodeType){

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

	// %ORION_DESC% 
	QString desc = project_->description();
	if (desc.isEmpty()){
		desc = name;
	}
	codeTxt.replace(UDQ_T("%ORION_DESC%"), desc);

	// %ORION_AUTHOR%
	codeTxt.replace(UDQ_T("%ORION_AUTHOR%"), project_->author_);

	// %ORION_ID%
	codeTxt.replace(UDQ_T("%ORION_ID%"), project_->domainId_);


	// %ORION_HIDDENBAR%
	QString fullscreen = fullscreen_ ? UDQ_T("true") : UDQ_T("false");
	codeTxt.replace(UDQ_T("%ORION_HIDDENBAR%"), fullscreen);
	// %ORION_STATUSBAR%
	QString statusbar;
	if (fullscreen_){
		statusbar.append(UDQ_T("<gap:config - file platform = \"ios\" parent = \"UIStatusBarHidden\"><true / >< / gap:config - file>\n"));
		statusbar.append(UDQ_T("<gap:config - file platform = \"ios\" parent = \"UIViewControllerBasedStatusBarAppearance\"><false/ > < / gap:config - file >\n"));	
	}
	codeTxt.replace(UDQ_T("%ORION_STATUSBAR%"), statusbar);

	// %ORION_GEOLOCATON%
	QString perm;
	if (permission_ & PRM_GEO){
		perm=UDQ_T("<feature name=\"http://api.phonegap.com/1.0/geolocation\"/>");
	}
	codeTxt.replace(UDQ_T("%ORION_GEOLOCATON%"), perm);

	// %ORION_CAMERA%
	if (permission_ & PRM_CAMERA){
		perm = UDQ_T("<feature name=\"http://api.phonegap.com/1.0/camera\"/>");
	}
	else{
		perm.clear();
	}
	codeTxt.replace(UDQ_T("%ORION_CAMERA%"), perm);

	// %ORION_NOTIFY%
	if (permission_ & PRM_VIBRATE){
		perm = UDQ_T("<feature name=\"http://api.phonegap.com/1.0/notification\"/>");
	}
	else{
		perm.clear();
	}
	codeTxt.replace(UDQ_T("%ORION_NOTIFY%"), perm);

	// %ORION_MEDIA%
	if (permission_ & PRM_MEIDA){
		perm = UDQ_T("<gap:plugin name=\"org.apache.cordova.media\" />");
	}
	else{
		perm.clear();
	}
	codeTxt.replace(UDQ_T("%ORION_MEDIA%"), perm);

	// %ORION_DEVICE%
	QString device;
	if (device_ == 1){
		device = UDQ_T("handset");
	}
	if (device_ == 2){
		device = UDQ_T("tablet");
	}
	else{
		device = UDQ_T("universal");
	}
	codeTxt.replace(UDQ_T("%ORION_DEVICE%"), device);

	// %ORION_IOSVERSION%
	codeTxt.replace(UDQ_T("%ORION_IOSVERSION%"), makeIOSVersion(iosversion_));

	// %ORION_ANDRIODVERSION%
	codeTxt.replace(UDQ_T("%ORION_ANDRIODVERSION%"), makeAndriodVersion(andriodversion_));

	// %ORION_WIDTH%
	QString width = QString::number(project_->resolution_.width());
	codeTxt.replace(UDQ_T("%ORION_WIDTH%"), width);

	// %ORION_HEIGHT%
	QString height = QString::number(project_->resolution_.height());
	codeTxt.replace(UDQ_T("%ORION_HEIGHT%"), height);

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

void CordovaExporter::prepare(){

	// 先导出H5文件
	Exporter* exporter = gExporterFactory->getExporter(Exporter::TYPE_HTML5, project_);
	Q_ASSERT(exporter != NULL);
	exporter->write(1);
	delete exporter;

}


QString CordovaExporter::makeIOSVersion(int ver){
	static QString tmpl = UDQ_T("<preference name = \"deployment - target\" value = \"%1\" / >");
	QString str;
	switch (ver)
	{
	case IOS_7:
	{
		str = tmpl.arg(UDQ_T("7.0"));
	}
	break;
	case IOS_8:
	{
		str = tmpl.arg(UDQ_T("8.0"));
	}
	break;
	}
	return str;
}

QString CordovaExporter::makeAndriodVersion(int ver){
	static QString tmpl = UDQ_T("<preference name = \"android - minSdkVersion\" value = \"%1\" / >");
	QString str;
	switch (ver)
	{
	case AND_4_0:
	{
		str = tmpl.arg(UDQ_T("14"));
	}
	break;
	case AND_4_1:
	{
		str = tmpl.arg(UDQ_T("16"));
	}
	break;
	case AND_4_2:
	{
		str = tmpl.arg(UDQ_T("17"));
	}
	break;
	case AND_4_3:
	{
		str = tmpl.arg(UDQ_T("18"));
	}
	break;
	case AND_4_4:
	{
		str = tmpl.arg(UDQ_T("19"));
	}
	break;
	case AND_5_0:
	{
		str = tmpl.arg(UDQ_T("21"));
	}
	break;
	}
	return str;
}

////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void CordovaExporter::makeTemplateDB(const QString& file, const QString& src, bool afterDelete){

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

			saveFile(db, FCT_CONFIG, UDQ_T("config.xml"), src);
			saveFile(db, FCT_XDKCONFIG, UDQ_T("intelxdk.config.additions.xml"), src);
		
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
