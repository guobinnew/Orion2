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


#ifndef ORION_CONFIG_MANAGER_H
#define ORION_CONFIG_MANAGER_H

#include <QString>
#include <QMap>
#include <QSettings>

class ConfigManager{
public:
    static void staticInit();

	static void setGlobalSettings(const QString& id, bool value);
	static bool globalSettings(const QString& id);

    //  是否显示LOGO
    static void setShowLogo( bool show );
	// 是否使用本地Base包
	static void setUseLocalBase(bool use);
	// 是否使用本地Base包
	static void setDevMode(bool use);

	// 是否管理员模式（可以编辑Base包）
	bool isBaseAdmin();

   static bool showLogo();
   static bool useLocalBase();
   static bool devMode();

    QString buildVersion();
    QString buildId();

	// 获取包搜索路径
	QString basePackagePath();

	// 获取包搜索路径
	QStringList packagePaths();
	void setPackagePaths(const QStringList& paths);

	// 工作空间
	QString baseWorkspacePath();
	void setWorkspaceConfig(const QString& path);

    QString baseFont();
    void setBaseFont(const QString& ft);

	// 样式名
	QString defaultStyle();
	void setDefaultStyle(const QString& style);

	QString defaultMimeJson();
	void setMimeJson(const QString& json);

	// 是否启用扩展MIME
	bool useExtentedMime();
	void enableExtentedMime(bool flag);

	// 最近项目列表
	QStringList projectLRU();
	void setProjectLRU(const QStringList& lru);
	// 添加项目
	void addProject(const QString& item);

	// 登录解锁设置
	QByteArray loginCode();
	void setLoginCode(const QByteArray& code);

	// 按键列表
	QStringList& keyboardMap();
	int keyboardValue(const QString& key);

	// 调试配置
	QString serverRootPath();
	void setServerRootPath(const QString& path);

	// 浏览器
	int serverPort();
	void setServerPort(int port);

	// 浏览器命令行
	QString explorerCmd();
	void setExplorerCmd(const QString& cmd);

	// 窗口设置
	void saveWindowState(const QByteArray& data);
	QByteArray windowState();

	void saveWindowGeometry(const QByteArray& data);
	QByteArray windowGeometry();

};

extern ConfigManager* gConfigManager;



#endif // ORION_CONFIG_MANAGER_H
