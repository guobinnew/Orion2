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
**  Orion is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/


#include <QApplication>
#include <QTextCodec>
#include <QSplashScreen>
#include <QTranslator>
#include <QDir>
#include <QTextCodec>
#include <QDesktopWidget>
#include <QMap>
#include <QFile>

#include "commondefine.h"
#include "menumanager.h"
#include "mainmanagerframe.h"
#include "mainspriteframe.h"
#include "configmanager.h"
#include "framemanager.h"
#include "mimemanager.h"
#include "devicemanager.h"
#include "projectmanager.h"
#include "logmanager.h"
#include "loginwindow.h"
#include "inputloginwindow.h"
#include "resourceobject.h"
using namespace ORION;

int main(int argc, char *argv[])
{
	//
	Q_INIT_RESOURCE(orion2);

    QApplication app(argc, argv);

	//////////////////////////////////////////////////////////////////////////
	// 测试


	// windows
#if defined( Q_OS_WIN ) ||defined( Q_OS_MAC )
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
#else
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif

	// 基础信息
	QApplication::setOrganizationName(QStringLiteral("Unique"));
	QApplication::setApplicationName(QStringLiteral("Orion2"));
	QApplication::setApplicationVersion(QStringLiteral("2.0"));
	LogManager::staticInit();

	gLogManager->log(UDQ_TR("\n////////////////////////////////////////////////////////////////////////\n")); 
	gLogManager->log(UDQ_TR("Orion2 开始启动...."));

	ConfigManager::staticInit();
	MIMEManager::staticInit();
	DeviceManager::staticInit();
	ProjectManager::staticInit();

	gLogManager->log(UDQ_TR("开始解析JSON 数据...."));
	// 准备基础数据
	gMIMEManager->prepareJson();
	gDeviceManager->prepareJson();
	gProjectManager->prepareJson();

	// 设置界面样式
	QFile file(QString(UDQ_T(":/qss/%1")).arg(gConfigManager->defaultStyle()));
	QString test = QString(UDQ_T(":/qss/%1")).arg(gConfigManager->defaultStyle());
	file.open(QFile::ReadOnly);
	QString styleSheet = QString::fromLatin1(file.readAll());
	app.setStyleSheet(styleSheet);

	// 添加Qt翻译
	QTranslator qtTranslator;
	qtTranslator.load(UDQ_T("qt_") + QLocale::system().name(), UDQ_T(":/translations"));
	app.installTranslator(&qtTranslator);

	QTranslator qtpTranslator;
	qtpTranslator.load(QString("Qt5PropertyBrowser"), QString(":/translations"));
	app.installTranslator(&qtpTranslator);

    QApplication::setStartDragDistance(32);
    QApplication::setStartDragTime(500);

	if (gConfigManager->loginCode().size() == 0){  // 头一次使用
		InputLoginWindow w;
		if (w.exec() != QDialog::Accepted){
			return 0;   // 退出程序
		}
	}
	else{
		LoginWindow w;
		if (w.exec() != QDialog::Accepted){
			return 0;   // 退出程序
		}
	}

    QDesktopWidget destop;

    QMap<int,int> screenPos;
	screenPos[FrameManager::WID_LEVEL] = 0;
	screenPos[FrameManager::WID_SPRITE] = 0;
	screenPos[FrameManager::WID_ENTITY] = 0;
	screenPos[FrameManager::WID_BLUEPRINT] = 0;

    QApplication::setWindowIcon(QIcon(":images/app.ico"));

	gLogManager->log(UDQ_TR("显示闪屏"));

	QPixmap pixmap(":/images/splash.png");
	QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
	splash.show();

	splash.showMessage(UDQ_TR("Build(%1)\n初始化资源中...").arg(gConfigManager->buildVersion()), Qt::AlignBottom | Qt::AlignLeft, Qt::red);

	MIMEManager::staticInit();
	MenuManager::staticInit();
	ResourceObject::staticInit();


	app.processEvents();

	gLogManager->log(UDQ_TR("显示主界面"));

    MainManagerFrame mainManagerFrm;
	gFrameManager->addWindow(FrameManager::WID_LEVEL, &mainManagerFrm);
	mainManagerFrm.move(destop.screenGeometry(screenPos[FrameManager::WID_LEVEL]).center());
    mainManagerFrm.showMaximized();
	splash.finish(&mainManagerFrm);
	gLogManager->log(UDQ_TR("隐藏闪屏"));
 
    int result = app.exec();

	gLogManager->log(UDQ_TR("退出准备中..."));
	ResourceObject::staticExit();
	MenuManager::staticExit();

	gLogManager->log(UDQ_TR("Orion2 退出"));

    return result;
}
