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

#ifndef ORION_EXPORTH5_H
#define ORION_EXPORTH5_H


#include "exporter.h"
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;


namespace ORION{

	// Native文件列表
	enum{
		FCT_INDEX,
		FCT_APPMANIFEST,
		FCT_APPCACHE,
		FCT_RUNTIME,
		FCT_JQUERY,

		FCT_ICON16,
		FCT_ICON32,
		FCT_ICON128,
		FCT_ICON256,
		FCT_ICONLOADING,

		FCT_INDEXNW,
		FCT_INDEXCORDOVA,

		FCT_INDEXDEBUG,
		FCT_RUNTIMEDEBUG,
		FCT_DEBUGGER,
		FCT_DEBUGGER_CSS,
		FCT_DEBUGGER_COLLAPSED,
		FCT_DEBUGGER_DEL,
		FCT_DEBUGGER_EXPANEDED,
		FCT_DEBUGGER_INSPECT,
		FCT_DEBUGGER_WATCH,

	};

}

class HTML5Exporter : public Exporter
{
	Q_OBJECT
public:
	HTML5Exporter(ProjectResourceObject* proj);
	virtual ~HTML5Exporter();

    virtual void write(int mode = 0);
	virtual QString name();

protected:
	virtual void makeTemplateDB(const QString& file, const QString& src, bool afterDelete = false);

private:

	// 保存图片( 返回图片文件名) usestyle 用于在文件名中标注v或h，说明图片方向
	QString saveImageFile(ResourceHash hash, const QString& desPath);
	QString saveEmptyImageFile(const QString& desPath);
	QString saveAudioFile(ResourceHash hash, const QString& desPath );
	QString saveVideoFile(ResourceHash hash, const QString& desPath);

	//生成文件
	void generateFile(int type,  const QString& filePath, bool text = true, bool cache = true );
	QByteArray replaceTextContent(const QByteArray& code, int	encodeType);


	// 准备工作
	void prepare();
	void processModel(const QByteArray& data);
	
	void prepareData();

	// 生成runtime.js文件
	void makeRuntimeFile();
	// 生成数据文件
	void makeDataFile();
	QJsonArray generatePlugins();	// 生成插件定义列表
	QJsonArray generateObjectTypes();	// 生成对象类型列表
	QJsonArray generateVariables( ObjectTypeResourceObject* obj);	// 生成实例列表
	QJsonArray generateAnimations(ObjectTypeResourceObject* obj);	// 生成动画列表
	QJsonArray generateBehaviors(ObjectTypeResourceObject* obj);	// 生成行为列表
	QJsonArray generateTexture(ObjectTypeResourceObject* obj);	// 生成纹理数据
	QJsonArray generateBehaviors(FamilyResourceObject* obj);	// 生成行为列表

	QJsonArray generateFamilies();  // 生成集合列表
	QJsonArray generateLevels(); // 生成场景列表
	QJsonArray generateLayer(LayerResourceObject* layer);  // 生成图层
	// 处理对象实例
	QJsonArray processInstance(const QByteArray& data);
	// 对象实例变量
	QJsonValue processInstanceVariable(PluginVariable* var, ObjectTypeResourceObject* objType, QMap<ResourceHash, QString>& parameters);
	// 插件属性
	QJsonArray processPluginProperty(const QString& name, ObjectTypeResourceObject* objType, QMap<ResourceHash, QString>& parameters);
	// Tilemap数据
	QJsonArray processTileMap(int w, int h, QMap<QPair<int, int>, QSize>& data);

	QString state2String(int state);
	QString color2String(const QColor& clr);
	QString font2String(const QFont& fnt);
	int pixelFormat(QImage& img);

	QJsonArray generateBlueprints(); // 生成脚本列表
	void replaceBlock(QJsonArray& block);
	void replaceParam(QJsonArray& param);
	void replaceExpNode(QJsonArray& param);
	void replaceInstanceIndex(QJsonArray& param);

	// 生成资源文件
	void makeResourceFile();
	// 生成图标文件
	void makeIconFile();
	// 生成公共JS文件
	void makeCommonScriptFile();

	// 生成特效插件代码
	QString generateEffectCode(PluginResourceObject* effect);
	QString processFxCode(QByteArray& data);

	// 生成插件代码
	QString generatePluginCode(PluginResourceObject* plugin);
	QString processScriptCode(QByteArray& data);

	ProjectResourceObject* project_;  // 工程

	// 内部使用
	QString exportPath_;  // 导出工程路径
	QString dbPath_;  // 数据库文件路径
	QStringList cacheFiles_;  // 缓存文件清单

	 int groupRef_;
	bool hasGroup_;  // 是否包含组
	bool hasElse_;   // 是否包含Else模块

	QList<PluginResourceObject*> entities_;
	QList<PluginResourceObject*> behaviors_;
	QList<PluginResourceObject*> effects_;

	QSet<ResourceHash>  galleries_;  // 基础资源
	QSet<ObjectTypeResourceObject*>  emptyGalleries_;  // 包含空精灵资源
	int emptysize_; 

	QSet<SpriteResourceObject*> sprites_;  // 精灵资源
	QSet<SoundResourceObject*> sounds;  // 声音资源
	QSet<MovieResourceObject*> movies_;  // 电影资源

	int nextUID_;  // 实例ID
	QStringList objectRefs_;
	QMap<ResourceHash, QString> resourceMap_;  // 资源对象映射（相对路径文件名）
	QMap<ResourceHash, int>  objectRefMap_;  // 对象引用映射
	QMap<ResourceHash, int>  objectTypeMap_;  // 对象类型引用映射
	QMap<ResourceHash, int> objectVariableMap_; // 对象实例变量索引映射


	//////////////////////////////////////////////////////////////////////////
	int mode_;   //  生成模式： 0表示H5， 1表示Cordova,  2表示nw
};

#endif // ORION_EXPORTEPUB_H

