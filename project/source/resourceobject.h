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
**  along with Orion2.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#ifndef ORION_RESOURCEOBJECT_H
#define ORION_RESOURCEOBJECT_H

#include <QString>
#include <QHash>
#include <QMultiHash>
#include <QLinkedList>
#include <QList>
#include <QMap>
#include <QPair>
#include <QVector>
#include <QDataStream>
#include <QByteArray>
#include <QVector4D>
#include <QDate>
#include <QDateTime>
#include <QFont>
#include <QPixmap>
#include <QPointF>
#include <QAudioBuffer>
#include <QGraphicsItem>
#include "commondefine.h"
#include "editordefine.h"
#include "resourcertti.h"
#include <QGraphicsItem>
#include <QTreeWidgetItem>

#include "resourceversion.h"
#include "resourcedata.h"

namespace ORION{
	//////////////////////////////////////////////////////////////////////////
	// 资源包说明：
	// 素材资源按包保存，每个包对应一个目录，目录下保存多个资源文件；
	// 电子书工程按工作空间保存，每个工作空间对应一个目录，目录下可以包含
	// 多个电子书；
	//////////////////////////////////////////////////////////////////////////

    #define  NATIVE_READONLY  0   // 系统资源只读

    #define  NULL_FILENAME UDQ_T("null")

    #define PROJECT_FILENAME UDQ_T("project")
    #define  GALLERY_IMAGE_FILENAME UDQ_T("images")
    #define  GALLERY_AUDIO_FILENAME UDQ_T("audio")
    #define  GALLERY_VIDEO_FILENAME UDQ_T("video")
    #define  GALLERY_SOUND_FILENAME UDQ_T("sound")
    #define  GALLERY_MOVIE_FILENAME UDQ_T("movie")

    #define  SPRITE_FILENAME UDQ_T("sprites")
    #define  PLUGIN_FILENAME UDQ_T("plugins")

	#define  DEFAULT_PROJECT UDQ_L8("新工程")
    #define  DEFAULT_AUTHOR UDQ_L8("狮鱼")

    #define  DEFAULT_PACKAGE UDQ_T("Base")
    #define  DEFAULT_WORKSPACE UDQ_T("TempWorkspace")

		// 文件类型定义
	#define DECLARE_RESOURCE_FILETYPE( t ) \
	public:\
		virtual QString fileType(){return staticFileType();} \
		static QString staticFileType(){ return t; }

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	#define 	URT_Object            0x0000000000000001   // 对象基类
	#define 	URT_Linker              0x0000000000000002   // 数据库接口
    #define 	URT_NullObject     0x0000000000000004   // 空对象

	#define 	URT_Package       0x0000000000000010  // 资源包
	#define 	URT_Field               0x0000000000000020   // 可分类对象基类
    #define URT_Struct              0x0000000000000040   // 带属性结构体

    #define 	URT_WorkSpace  0x0000000000000100  // 工作空间
    #define 	URT_Project          0x0000000000000200  // 工程
    #define 	URT_Level             0x0000000000000400  // 场景
    #define 	URT_Layer             0x0000000000000800  // 图层

    #define 	URT_Gallery          0x0000000000010000  // 素材
    #define 	URT_Sound            0x0000000000020000 // 声音（包含多种格式文件）
    #define 	URT_Movie           0x0000000000040000  // 电影（包含多种格式文件）

    #define 	URT_Image          0x0000000000100000  // 图片
	#define 	URT_Video           0x0000000000200000  // 视频
	#define 	URT_Audio           0x0000000000400000  // 音频
    #define 	URT_Sprite            0x0000000000800000  // 精灵

   #define   URT_Plugin              0x0000000001000000  // 插件
   #define   URT_ObjectType    0x0000000002000000  // 对象类型
   #define   URT_Blueprint         0x0000000004000000  //  脚本
   #define   URT_Family              0x0000000008000000  //  集合

   #define 	URT_SpriteSequence   0x0000001000000000  // 精灵序列


	enum ParameterType{   // 四种基本类型
		PAT_BOOL,
		PAT_INT,
		PAT_DOUBLE,
		PAT_STRING,
	};

	enum PluginType{
		PIT_ENTITY,
		PIT_BEHAVIOR,
		PIT_EFFECT,
		PIT_NATIVE,
	};

	enum FileType{
		FIT_RUNTIME,
		FIT_FX,

		FIT_JAVASCRIPT,
		FIT_CSS,
		FIT_HTML,
		FIT_TEXT,
		FIT_DATA,
		FIT_IMAGE,
		FIT_AUDIO,
		FIT_VIDEO,
		FIT_OTHER,
	};


	enum AttributeType{
		EF_NONE = 0,
		EF_WORLD,
		EF_ROTATABLE,
		EF_SINGLEGLOBAL,
		EF_TEXTURE,
		EF_TILING,
		EF_ANIMATIONS,
		EF_NOSIZE,
		EF_EFFECTS,
		EF_PREDRAW,
		EF_POSITION,
		EF_SIZE,
		EF_ANGLE,
		EF_ZORDER,
		EF_APPEARANCE,
		EF_ONCE,
		EF_BLENDBG,
		EF_CROSSSAMPLE,
		EF_ANIMATIONDRAW,
		EF_BEHAVIORS,

		FT_NONE = 100,
		FT_DEPRECATED,
		FT_TRIGGER,
		FT_FAKE_TRIGGER,
		FT_STATIC,
		FT_INVERTIBLE,
		FT_INCOMPATIBLE,
		FT_LOOPING,
		FT_FAST_TRIGGER,

		FT_RETURN_INT = 200,
		FT_RETURN_FLOAT,
		FT_RETURN_STRING,
		FT_RETURN_ANY,
		FT_VARIADIC
	};

	inline QString Value2String(int value){
		QString str;
#define TOSTRING(x) case x: {str = UDQ_T(#x); str = str.right(str.size() - 3).toLower();} break;\

		switch (value){
			TOSTRING(EF_WORLD);
			TOSTRING(EF_ROTATABLE);
			TOSTRING(EF_SINGLEGLOBAL);
			TOSTRING(EF_TEXTURE);
			TOSTRING(EF_TILING);
			TOSTRING(EF_ANIMATIONS);
			TOSTRING(EF_NOSIZE);
			TOSTRING(EF_EFFECTS);
			TOSTRING(EF_PREDRAW);
			TOSTRING(EF_POSITION);
			TOSTRING(EF_SIZE);
			TOSTRING(EF_ANGLE);
			TOSTRING(EF_ZORDER);
			TOSTRING(EF_APPEARANCE);
			TOSTRING(EF_ONCE);
			TOSTRING(EF_BLENDBG);
			TOSTRING(EF_CROSSSAMPLE);
			TOSTRING(EF_ANIMATIONDRAW);
			TOSTRING(EF_BEHAVIORS);

			TOSTRING(FT_NONE);
			TOSTRING(FT_DEPRECATED);
			TOSTRING(FT_TRIGGER);
			TOSTRING(FT_FAKE_TRIGGER);
			TOSTRING(FT_STATIC);
			TOSTRING(FT_INVERTIBLE);
			TOSTRING(FT_INCOMPATIBLE);
			TOSTRING(FT_LOOPING);
			TOSTRING(FT_FAST_TRIGGER);

			TOSTRING(FT_RETURN_INT);
			TOSTRING(FT_RETURN_FLOAT);
			TOSTRING(FT_RETURN_STRING);
			TOSTRING(FT_RETURN_ANY);
			TOSTRING(FT_VARIADIC);
		default: return QString::number(value);
		}
#undef TOSTRING
		return str;
	}

	enum PropertyType{
		PT_VARIABLE,
		PT_CONDITION,
		PT_ACTION,
		PT_EXPRESS
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	enum ResourceFlags{
		URF_Obsolete			      = 0x00000001,        // 过时
		URF_Public			              = 0x00000002,	     // 公共对象，包外可见
		URF_Local			              = 0x00000004,       // 本地
	
		URF_TagSave			      = 0x00000010,	// 保存标志
		URF_TagLoad			      = 0x00000020,	// 加载标志
		URF_TagGarbage		  = 0x00000040,	// 垃圾回收进行检查

		URF_Transient                  = 0x00000100,	  // 临时对象，不保存
		URF_Destroyed               = 0x00000200,    // 被销毁
		URF_Native			          = 0x00000400,    // 本地(一般只读，不保存)
		URF_NeedLoad              = 0x00000800,    // 需要加载（仅包括索引）

		URF_Error                          = 0x10000000,	 // 无效对象
	};

	//////////////////////////////////////////////////////////////////////////
	// 加载对象标识
	//////////////////////////////////////////////////////////////////////////
	enum ResourceLoadFlags{
		RLF_None			      = 0x0000,	// 无标识( 全部加载 )
		RLF_NoFail			  = 0x0001,	// 如果加载失败视为致命错误 
		RLF_Verify			      = 0x0002,	// 确认存在，不进行加载
		RLF_LoadIndex       = 0x0008,	// 加载索引
	};

#ifndef DEF_RESOURCEHASH
   #define DEF_RESOURCEHASH
	typedef unsigned int ResourceHash;  // 资源句柄
#endif

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	class PackageResourceObject;
	class FieldResourceObject;

	class WorkspaceResourceObject;
	class ProjectResourceObject;
	class GalleryResourceObject;
	class SoundResourceObject;
	class MovieResourceObject;

	class LevelResourceObject;
	class LayerResourceObject;

	class PluginResourceObject;
	class ObjectTypeResourceObject;
	class BlueprintResourceObject;
	class FamilyResourceObject;

	class SpriteResourceObject;
	class SpriteSequenceResourceObject;

	class ResourceLinker;

	// 资源根对象
	class ResourceObject{
		DeclareRootRTTI( ResourceObject )
		friend class ResourceObjectIterator;
	public:
		ResourceObject( unsigned int flags = URF_Public );
		ResourceObject( const QString& inName, unsigned int flags = URF_Public );

		virtual ~ResourceObject(){}

		ResourceObject* outer() const{
			return outer_;
		}

		void setOuter( ResourceObject* inOuter ){
			outer_ = inOuter;
		}

		// 添加孩子
		virtual ResourceObject* addChild( const QString& inName,  const ResourceRTTI* RTTI );
		virtual ResourceObject* createChild( const QString& inName,  RTTIType childType ){ return 0; }

		// 获取最外围对象
		ResourceObject* getOutermost() const{
			ResourceObject* top;
			for( top = outer() ; top && top->outer() ; top = top->outer() );
			return top;
		}

		// 获取指定的父对象
		bool hasOuter( ResourceObject* stop ) const{
			if (stop == NULL)
				return false;

			bool res = false;
			ResourceObject* top = outer(); 
			while (top){
				if (top == stop){
					res = true;
					break;
				}
				top = top->outer();
			}
			return res;
		}

		// “类名className@路径名pathName”
		QString fullName() const;

		// 获取实例路径名（包括外围对象,格式采用Parent.child）
		QString pathName( ResourceObject* stopOuter = NULL ) const;

		QString objectName() const{
			return name_;
		}

		void setObjectName( const QString& inName ){
			name_ = inName;
		}

		QString description() const{
			return description_;
		}

		void setDescription( const QString& desp ){
			description_ = desp;
		}

		virtual void rename( const QString& inName );

		ResourceHash hashKey() const{
			return hashKey_;
		}

		unsigned int flags() const{
			return objectFlags_;
		}

		void setFlags( unsigned int newFlags ){
			objectFlags_ |= newFlags;
		}

		void clearFlags( unsigned int newFlags ){
			objectFlags_ &= ~newFlags;
		}

		bool isNative(){
			return objectFlags_ & URF_Native;
		}

		// 获取连接器
		ResourceLinker* getLinker() const{
			return linker_;
		}

		void setLinker( ResourceLinker* inLinker );

		// 添加对象实例
		void addObject();
		void hashObject();
		void unhashObject();

		// 重新hash子对象
		void hashChildren();

		virtual void encode( QDataStream& stream );
		virtual bool decode( QDataStream& stream );
		// 初始化后处理
		virtual void postInit(){}

		unsigned int version( void ){
			return version_;
		};

		void updateVersion(unsigned int ver){
			version_ = ver;
		}

		virtual bool checkVersion( QDataStream& stream );

		// 时戳
		qint64 timeStamp(){
			return timestamp_;
		}

		// 解码内部模型
		template<class T>
		T* decodeChild(QDataStream& stream);

		// 清理
		virtual void clear(){}

		// 复制新对象
		virtual void copyObject(ResourceObject* obj);

		// 克隆新对象
		virtual ResourceObject* clone(const QString& inName){
			return NULL;
		}

	protected:
		ResourceObject* outer_;   // 父节点
		ResourceHash hashKey_;  // 哈希值
		QString name_;  // 名称
		QString description_;  // 描述
		unsigned int objectFlags_;  // 标志
		ResourceLinker* linker_;  // 连接器
		unsigned int version_;  // 当前版本(从数据库读取的版本)
		qint64 timestamp_;  // 时戳
	
	    //////////////////////////////////////////////////////////////////////////
		// 静态
		//////////////////////////////////////////////////////////////////////////
	public:
		// 静态初始化
		static void staticInit();
		// 静态退出
		static void staticExit();
		// 查找资源
		static ResourceObject* findResource( ResourceObject* inOuter, const QString& resPath, bool exactClass = true );
		static ResourceObject* findResource( ResourceHash hashkey );
		// 创建包对象
		static PackageResourceObject* createPackage(  const QString& pkgName, const QString& pkgPath, bool bNative = false );
		// 加载资源包
		static ResourceObject* loadPackage(const QString& pkgPath, unsigned int loadFlags = RLF_None, bool bNative = false);
		// 清空垃圾
		static void purgeGarbage();
		//  生成唯一对象名
		static QString makeUniqueObjectName( ResourceObject* inOuter, ResourceObject* cls );
		static QString makeUniqueObjectName( const QString& path,  const QString& baseName, ResourceObject* cls );

		// 获取包连接器
		static ResourceLinker* getPackageLinker(ResourceObject*	inOuter, const QString& inFilename );
		// 创建工作空间对象
		static WorkspaceResourceObject* createWorkspace(const QString& wsName, bool temp = false );
		// 加载工作空间对象
		static WorkspaceResourceObject* loadWorkspace(const QString& wsName, unsigned int loadFlags = RLF_None);
		static void loadWorkspace(  WorkspaceResourceObject* ws, unsigned int loadFlags = RLF_None );
		// 获取包连接器
		static ResourceLinker* getWorkspaceLinker(ResourceObject*	inOuter, const QString& inFilename );
		//清空工作空间
		static void resetWorkspace( WorkspaceResourceObject* inOuter );
		// 当前工作空间
		static WorkspaceResourceObject* currentWorkspace();
		//  临时工作空间
		static WorkspaceResourceObject* tempWorkspace();

	private:
		static QHash<ResourceHash, ResourceObject*> objectIndex_;   // 资源索引
		static QList<ResourceObject*> objects_; // 资源列表
		static QLinkedList<ResourceLinker*> linkers_;  // 连接器
		static QLinkedList<ResourceObject*> rootObjects_;  // 包对象

		static WorkspaceResourceObject* tmpWorkspace_;// 临时工作空间
	};


	//////////////////////////////////////////////////////////////////////////
	// 对象迭代器
	//////////////////////////////////////////////////////////////////////////

	class ResourceObjectIterator{
	public:
		ResourceObjectIterator(const ResourceRTTI* Rtti = NULL ): RTTI_(Rtti), index_(-1){
			++*this;
		}

		void operator++(){
			while( ++index_<ResourceObject::objects_.size() 
				&& ( !( ResourceObject::objects_[index_] ) ||  ( RTTI_ != NULL && !( ResourceObject::objects_[index_]->isKindOf( RTTI_ ) ) ) ) );
		}
		ResourceObject* operator*(){
			return ResourceObject::objects_[index_];
		}

		ResourceObject* operator->(){
			return ResourceObject::objects_[index_];
		}

		// 类型转换操作符
		operator bool(){
			return index_<ResourceObject::objects_.size();
		}

	protected:
		const ResourceRTTI* RTTI_;
		int index_; // 索引
	};

	template< class T > 
	class TResourceObjectIterator : public ResourceObjectIterator{
	public:
		TResourceObjectIterator():	ResourceObjectIterator( &T::RTTI_ ){}

		T* operator* (){
			return (T*)ResourceObjectIterator::operator*();
		}

		T* operator-> (){
			return (T*)ResourceObjectIterator::operator->();
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//  资源包对象
	//////////////////////////////////////////////////////////////////////////

	class PackageResourceObject : public ResourceObject{
		DeclareRTTI()
	public:
		PackageResourceObject(){}
		PackageResourceObject( const QString& inName, unsigned int flags =  URF_Transient | URF_Public );
		// 重新加载
		void reload();
		virtual ResourceObject* createChild( const QString& inName,  RTTIType childType );

		QString dirPath_; //   包目录路径

		// 配置信息读写
		bool saveConfig();
		// 读取包配置（config中依次包名，包描述，包路径）
		static bool loadConfig( const QString& path, QStringList& config );
	private: 
	};

	//////////////////////////////////////////////////////////////////////////
	//  模型资源父对象
	//////////////////////////////////////////////////////////////////////////

	class FieldResourceObject : public ResourceObject{
		DeclareRTTI( )
	public:
		FieldResourceObject(){}
		FieldResourceObject( const QString& inName, unsigned int flags = URF_Transient | URF_Public );

		virtual void encode( QDataStream& stream );
		virtual bool decode( QDataStream& stream );
		// 文件类型
		virtual QString fileType() = 0;

		bool isBelong(const QString& category);
		bool hasTag(const QString& tag, bool exactlyMatch = false);

		// 复制新对象
		virtual void copyObject(ResourceObject* obj);

		QStringList tags_;  //   分类标签
		QStringList categories_;  // 资源类型标签（字符串采用“parent.child”格式）

	};

	class NullResourceObject : public  FieldResourceObject{
		DeclareRTTI( )
		DECLARE_RESOURCE_FILETYPE( NULL_FILENAME )
	public:
		NullResourceObject(){}
	};

	//////////////////////////////////////////////////////////////////////////
	//  模板类
	//////////////////////////////////////////////////////////////////////////

	// 字符串哈希
	template<typename T>
	unsigned int THashString( const QString& inStr ){
		QString fullStr = QString(UDQ_T("%1@%2")).arg( T::RTTI_.getName()).arg( inStr);
		return hashString( fullStr );
	}

	// 根据哈希查找资源
	template<typename T>
	T* TFindResource( unsigned int inHashKey ){
		return  UDynamicCast(T, ResourceObject::findResource( inHashKey ));
	}


#define  FIND_RESOURCE_HASH( T, hashStr ) \
	UDynamicCast(T, ResourceObject::findResource(THashString<T>(hashStr)))

#define  DEFINE_RESOURCE_HASHKEY(T, var, hashValue) \
	T* var = UDynamicCast(T, ResourceObject::findResource(hashValue));

#define  DEFINE_RESOURCE_HASHKEY_VALID(T, var, hashValue) \
	T* var = UDynamicCast(T, ResourceObject::findResource(hashValue)); \
	Q_ASSERT(var != NULL);

	#define  DEFINE_RESOURCE_PATH( T, var, hashStr ) \
	    DEFINE_RESOURCE_HASHKEY( T, var, THashString<T>(hashStr) )

   #define SAVE_PACKAGE_RESOURCE(T, var) \
	    if( var->getLinker() == 0 ){ \
		    QString file =  var->getOutermost()->objectName() + UDQ_T(".") + var->fileType();\
		    ResourceLinker* linker = ResourceObject::getPackageLinker( var->getOutermost(), file );\
			var->setLinker( linker );}\
	    var->getLinker()->save<T>( var->hashKey() );

    #define SAVE_WORKSPACE_RESOURCE(T, var) \
		if( var->getLinker() == 0 ){ \
		QString file =  var->getOutermost()->objectName() + UDQ_T(".") + var->fileType();\
		ResourceLinker* linker = ResourceObject::getWorkspaceLinker( var->getOutermost(), file );\
		var->setLinker( linker );}\
		var->getLinker()->save<T>( var->hashKey() );


	template<typename T>
	T* TAddChildObject( ResourceObject* inOuter, const QString& inName ){
		return (T*)inOuter->addChild(inName, &T::RTTI_);
	}

	template<typename T>
	T*  TCreateResource(ResourceObject* inPkg, const QString& inName){
		return TAddChildObject<T>(inPkg, inName);
	}

	// 克隆新对象
	template<typename T>
	T* TCloneObject(T* prototype, const QString& inName){
		// 保存到数据库中
		T* newObj = TCreateResource<T>(prototype->getOutermost(), inName);
		Q_ASSERT(newObj != 0);

		prototype->copyObject(newObj);
		newObj->setFlags(URF_TagSave);
		return newObj;
	}


	template<typename T>
	T* TFindResource(const QString& inPkg,  const QString& inName, bool bCreate = false){
		QString path = QString(UDQ_T("%1.%3")).arg( inPkg).arg( inName );
		DEFINE_RESOURCE_PATH(T, res, path  );

		if (bCreate){
			if (!res){
				// 确保包必须存在
				DEFINE_RESOURCE_PATH(PackageResourceObject, pkg, inPkg);
				Q_ASSERT(pkg != NULL);
				res = TCreateResource<T>(pkg, inName);
			}
			else if (res && (res->flags() & URF_TagGarbage))  {
				res->clearFlags(URF_TagGarbage);
				res->setFlags(URF_TagSave);
			}
		}

		if (res && res->flags() & URF_TagGarbage){
			res = NULL;
		}

		return res;
	}


	template<typename T>
	T* TFindWSResource(const QString& inPkg, const QString inBook, bool bCreate = false){
		QString path = QString(UDQ_T("%1.%2")).arg(inPkg).arg(inBook);
		DEFINE_RESOURCE_PATH(T, res, path);

		if (bCreate){
			if (!res){
				// 如果没有，则创建
				WorkspaceResourceObject* ws = ResourceObject::createWorkspace(inPkg);
				Q_ASSERT(ws != NULL);
				res = TAddChildObject<T>(ws, inBook);
			}
			else if (res && (res->flags() & URF_TagGarbage))  {
				res->clearFlags(URF_TagGarbage);
				res->setFlags(URF_TagSave);
			}
		}

		if (res && res->flags() & URF_TagGarbage){
			res = NULL;
		}

		return res;
	}

	template<typename T>
	T* TFindWSResource( const QString& inPkg,  const QString inProject, const QString inName,  bool bCreate = false ){
		QString path = QString(UDQ_T("%1.%2.%3")).arg(inPkg).arg(inProject).arg(inName);
		DEFINE_RESOURCE_PATH(T, res, path  );

		if (bCreate){
			if ( !res ){
				// 确保中间对象（Project）都被创建
				ProjectResourceObject* proj = TFindWSResource<ProjectResourceObject>(inPkg, inProject, true);
				Q_ASSERT(proj != NULL);
				res = TAddChildObject<T>(proj, inName);
			}
			else if ( res->flags() & URF_TagGarbage ) {
				res->clearFlags(URF_TagGarbage);
				res->setFlags(URF_TagSave);
			}
		}

		if (res && res->flags() & URF_TagGarbage){
			res = NULL;
		}

		return res;
	}


	template<typename T>
	void  TClearResource(){
		for( TResourceObjectIterator<T> it; it; ++it ){
			it->setFlags( URF_TagGarbage );
		}
	}

	template<typename T>
	QStringList TResourceNames(){
		QStringList list;
		for( TResourceObjectIterator<T> it; it; ++it ){
			list<<it->objectName();
		}
		return list;
	}
			
	// 生成唯一实例名，如果提供refName，则优先使用refName；如果发现重名，则自动生成新的名称;
	// affix为自动生成的名称前缀
	template<typename T>
	QString makeInstanceName( ResourceObject* inOuter , ResourceObject* prototype, const QString& refName = QString(), const QString& affix = QString() ){
		Q_ASSERT( inOuter != 0 && inOuter->outer() != 0 );

		QString newPre =  inOuter->pathName();
		QString tmpStringName;
		QString tmpPath;

		//  如果提供参考名
		if( !refName.isEmpty() ){
			tmpPath = QString(UDQ_T("%1.%2")).arg( newPre ).arg(refName);
			if( !FIND_RESOURCE_HASH(T, tmpPath) )
				return refName;
		}

		// 如果没有参考名，或参考名已被使用
		do{
			tmpStringName = QString(UDQ_T("%3%1%2")).arg(prototype->objectName()).arg( prototype->objectId() ).arg(affix);
			tmpPath = QString(UDQ_T("%1.%2")).arg( newPre ).arg(tmpStringName);
		} while( FIND_RESOURCE_HASH(T, tmpPath) ); // 如果发现重复对象名，累加id

		return tmpStringName;
	}

	// 解码内部模型
	template<class T>
	T* ResourceObject::decodeChild(QDataStream& stream){
		// 记录起始位置
		qint64 pos = stream.device()->pos();
		unsigned int ver, hashkey;
		stream >> ver >> hashkey;
		// 根据哈希查找对象
		DEFINE_RESOURCE_HASHKEY(T, child, hashkey);
		if (child == NULL){
			child = new T();
		}
		else{
			child->clearFlags(URF_TagGarbage);
			child->setFlags(URF_TagSave);
		}
		// 回到起始位置
		stream.device()->seek(pos);

		if (!child->decode(stream)){
			delete child;
			// 提示错误
			return NULL;
		}

		child->clearFlags(URF_NeedLoad);

		if (child->outer() == NULL){
			child->setOuter(this);
			child->addObject();
		}
		else if (child->outer() != this){
			child->setOuter(this);
		}

		return child;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	class GalleryResourceObject : public FieldResourceObject{
		DeclareRTTI( )
	public:
		GalleryResourceObject(){}
		GalleryResourceObject(const QString& inName, unsigned int flags = URF_Transient | URF_Public);

		virtual void encode( QDataStream& stream );
		virtual bool decode( QDataStream& stream );

		// 复制新对象
		virtual void copyObject(ResourceObject* obj);

		void setContent(const QByteArray& data);

		QPixmap thumbnail_;   // 缩略图
		QByteArray content_;  // 二进制数据流
		QString format_;  // 原始格式
	
		// 生成缩略图
		virtual void  makeThumbnail(int size = 128);

		// 获取原始文件格式
		virtual QString  getFormat(){
			return format_;
		}

		// 获取用于文件对话框的过滤器
		static QString fileFilters();

		QByteArray hashsha1_;  // 内容哈希
	private:

	};

	class ImageResourceObject : public GalleryResourceObject{
		DeclareRTTI()
		DECLARE_RESOURCE_FILETYPE(GALLERY_IMAGE_FILENAME)
	public:
		ImageResourceObject(){}
		ImageResourceObject(const QString& inName, unsigned int flags = URF_Public);

		virtual void encode(QDataStream& stream);
		virtual bool decode(QDataStream& stream);

		// 复制新对象
		virtual void copyObject(ResourceObject* obj);

		bool isGroup_;  // 图片集合组
		QList<QRect> frames_;  // 图片帧位置

		// 仅用于显示（非持久化）
		QImage image_;  // 原始图像

		// 图片数目
		int imageCount();
		// 获取子图片
		QImage getImage(int index);
		// 获取原始文件格式
		virtual QString  getFormat();

		// 生成缩略图
		virtual void  makeThumbnail(int size = 128);

		// 设置新图片
		void setImage(QImage& newImage);

		// 获取用于文件对话框的过滤器
		static QString fileFilter();
		// 判断文件扩展名（以后考虑根据文件内容）
		static bool isValidFile( const QString& ext );
	private:
	};


	class AudioResourceObject : public GalleryResourceObject{
		DeclareRTTI()
		DECLARE_RESOURCE_FILETYPE(GALLERY_AUDIO_FILENAME)
	public:
		AudioResourceObject(){}
		AudioResourceObject(const QString& inName, unsigned int flags = URF_Public);

		virtual QString  getFormat();

		// 获取用于文件对话框的过滤器
		static QString fileFilter();
		// 判断文件扩展名（以后考虑根据文件内容）
		static bool isValidFile(const QString& ext);
	private:
	};

	class VideoResourceObject : public GalleryResourceObject{
		DeclareRTTI()
		DECLARE_RESOURCE_FILETYPE(GALLERY_VIDEO_FILENAME)
	public:
		VideoResourceObject(){}
		VideoResourceObject(const QString& inName, unsigned int flags = URF_Public);

		virtual QString  getFormat();

		// 获取用于文件对话框的过滤器
		static QString fileFilter();
		// 判断文件扩展名（以后考虑根据文件内容）
		static bool isValidFile(const QString& ext);
	private:
	};

	///////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	class SoundResourceObject : public FieldResourceObject{
		DeclareRTTI()
		DECLARE_RESOURCE_FILETYPE(GALLERY_SOUND_FILENAME)
	public:
		SoundResourceObject(){}
		SoundResourceObject(const QString& inName, unsigned int flags = URF_Public);

		virtual void encode(QDataStream& stream);
		virtual bool decode(QDataStream& stream);

		// 获取目前的文件格式
		QStringList formats();
		QList<ResourceHash>  audios_;  // 文件列表
	private:
	};

	class MovieResourceObject : public FieldResourceObject{
		DeclareRTTI()
		DECLARE_RESOURCE_FILETYPE(GALLERY_MOVIE_FILENAME)
	public:
		MovieResourceObject(){}
		MovieResourceObject(const QString& inName, unsigned int flags = URF_Public);


		virtual void encode(QDataStream& stream);
		virtual bool decode(QDataStream& stream);

		// 获取目前的文件格式
		QStringList formats();
		QList<ResourceHash>  videos_;  // 文件列表
	private:
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	class SpriteResourceObject : public FieldResourceObject{
		DeclareRTTI()
		DECLARE_RESOURCE_FILETYPE(SPRITE_FILENAME)
	public:
		SpriteResourceObject(){}
		SpriteResourceObject(const QString& inName, unsigned int flags = URF_Public);

		virtual void encode(QDataStream& stream);
		virtual bool decode(QDataStream& stream);
		// 复制新对象
		virtual void copyObject(ResourceObject* obj);
		// 克隆新对象
		virtual ResourceObject* clone(const QString& inName);

		virtual ResourceObject* createChild(const QString& inName, RTTIType childType);

		// 解码内部模型
		template<class T>
		bool decodeChild(QDataStream& stream){
			// 记录起始位置
			qint64 pos = stream.device()->pos();
			unsigned int ver, hashkey;
			stream >> ver >> hashkey;
			// 根据哈希查找对象
			DEFINE_RESOURCE_HASHKEY(T, child, hashkey);
			if (child == NULL){
				child = new T();
			}
			else{
				child->clearFlags(URF_TagGarbage);
				child->setFlags(URF_TagSave);
			}
			// 回到起始位置
			stream.device()->seek(pos);

			if (!child->decode(stream)){
				delete child;
				// 提示错误
				return false;
			}

			child->clearFlags(URF_NeedLoad);

			if (child->outer() == NULL){
				child->setOuter(this);
				child->addObject();
			}
			else if (child->outer() != this){
				child->setOuter(this);
			}

			return true;
		}

		void saveViewPort(int lod, const QTransform& trans, const QPointF& center);

		void addImageSource(ResourceHash id, ResourceHash key);
		void removeImageSource(ResourceHash id);

		// 获取动画序列
		SpriteSequenceResourceObject* sequence(const QString& name);

		// 获取帧区域位置
		QRect frameRect(ResourceHash key, int index);

		QByteArray viewData_;  // 视口信息
		QMap<ResourceHash, ResourceHash> imgSources_;  // 动画文件源

		QString defaultSeq_;  //  缺省序列帧
		// 非持久化
		QList<SpriteSequenceResourceObject*> sequences_;  // 包含的序列对象
	};


	struct SequenceFrame : public ResourceData
	{
		QPointF pos;
		ResourceHash imghost;  // 素材ID
		int imgindex;  // 素材中索引位置
		QMap<ResourceHash, QPointF> anchors;  // 锚点位置
		QList<QPointF> collisionRegion; // 碰撞区域
		QBitArray state;  // 翻转状态（0旋转颠倒，1翻转颠倒，  4水平翻转  5垂直翻转， 6 对角翻转 ）

		QImage buffer;  // 图像缓存（不保存）

		void save(QDataStream& stream){
			stream << pos << imghost << imgindex << anchors << collisionRegion << state;
		}
		void load(QDataStream& stream){
			stream >> pos >> imghost >> imgindex >> anchors >> collisionRegion >> state;
		}
	};


	const QString ANCHOR_CENTER = UDQ_T("Origin");

	class SpriteSequenceResourceObject : public ResourceObject{
		DeclareRTTI()
	public:
		SpriteSequenceResourceObject(){}
		SpriteSequenceResourceObject(const QString& inName, unsigned int flags = URF_Public);

		virtual void encode(QDataStream& stream);
		virtual bool decode(QDataStream& stream);
		// 复制新对象
		virtual void copyObject(ResourceObject* obj);

		// 获取动画帧图片（没有找到则返回空图像）
		void refreshImage(SequenceFrame* frame);

		// 获取帧图片
		QImage frameSourceImage(int index);
		QImage frameSourceImage(SequenceFrame* frame);

		QImage frameImage(int index);
		QPointF frameAnchor(int index, const QString& anchor);  // 0 表示中心锚点
		QPointF frameCenterAnchor(int index);
		// 碰撞顶点
		QList<QPointF> frameCollision(int index);

		QPointF pos_;  // 位置
		bool isLocking_;  //
		qreal speed_;  // 播放速度（0~1, 相对60FPS）
		bool playback_; // 是否倒放
		bool pingpong_;  // 往复播放
		bool repeat_;  // 重复播放
		int count_;  // 重复次数
		int repeatTo_;  // 播放结束停止帧数
		QStringList anchors_;  // 锚点列表
		TResourceList<SequenceFrame> frames_;  // 动画帧

	private:
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	enum{
		VAR_EXTEND = 1,  // 扩展变量
	};

	struct PluginVariable : public ResourceData
	{
		QString name;
		QString title;
		QString datatype;
		QStringList enums;
		QString desc;
		QString value;
		int flags;  // 变量标志

		PluginVariable() : flags(0){}

		void save(QDataStream& stream){
			stream << name << datatype << title << enums << value << desc << flags;
		}
		void load(QDataStream& stream){
			stream >> name >> datatype >> title >> enums >> value >> desc >> flags;
		}
	};


	struct PluginInterface : public ResourceData
	{
		int type;
		QString name;
		QString category;
		QStringList flags;
		QString desc;
		QString func;
		TResourceList<PluginVariable> params;  // 参数带次序

		PluginVariable* findParam(const QString& name){
			foreach(PluginVariable* var, params.values()){
				if (var->name == name){
					return var;
				}
			}
			return NULL;
		}

		bool hasFlag(int flag){
			return flags.contains(Value2String(flag));
		}
			
		void save(QDataStream& stream){
			stream << name << type <<  flags << category << func << desc;
			stream << params.toByteArray();
		}
		void load(QDataStream& stream){
			stream >> name >> type >> flags >> category >> func >> desc;
			QByteArray data;
			stream >> data;
			params.fromByteArray(data);
		}
	};


	class PluginResourceObject : public FieldResourceObject{
		DeclareRTTI()
		DECLARE_RESOURCE_FILETYPE(PLUGIN_FILENAME)
	public:
		PluginResourceObject() : commonPlugin_(NULL){}
		PluginResourceObject(const QString& inName, unsigned int flags = URF_Public);

		virtual void encode(QDataStream& stream);
		virtual bool decode(QDataStream& stream);

		// 初始化后处理
		void postInit();
		// 处理参数缺省值
		void processParameterDefault(const QString& prefix, PluginInterface* inter);
		QString getParameterDefault(PluginVariable* var);

		// 获取
		QString getVariableDefault(const QString& prop);

		QString getParameterDefault(unsigned int key);
		QString getConditionParameterDefault(const QString& prop, const QString& parent);
		QString getActionParameterDefault(const QString& prop, const QString& parent);
		QString getExpressParameterDefault(const QString& prop, const QString& parent);

		// 获取变量定义
		PluginVariable* getVariable(const QString& prop);

		PluginVariable* getConditionVariable(const QString& prop, const QString& parent);
		PluginVariable* getActionVariable(const QString& prop, const QString& parent);
		PluginVariable* getExpressVariable(const QString& prop, const QString& parent);

		void setVariableValue(const QString& name, const QString& value);
	
		void copyObject(ResourceObject* obj);
		int type_;  // 插件类型（0对象，1行为，2特效）
		bool devmode_;  // 系统保留插件（只能在开发者模式下编辑）

		QString title_;  // 显示名称
		QString modelVer_;  // 模型版本
		int eid_;  // 模型导出ID（每次导出时累加）
		QImage icon_;  // ICON

		QSize  extendBound_;  // 包围盒扩展
		QMap<int, int>  attributes_;  // 特性

		// 是否具有属性
		bool hasAttribute(int);

		QMultiHash<int, QByteArray>  filedata_;  // 关联文件数据（目前只有1个）
		QList<ResourceHash> varIndex_; //  变量顺序索引
		TResourceMap<PluginVariable>  variables_;
		TResourceMap<PluginInterface>  conditions_;  // 条件
		TResourceMap<PluginInterface>  actions_;    // 动作
		TResourceMap<PluginInterface>  expresses_;   // 表达式

		PluginInterface* getAction(ResourceHash key);
		PluginInterface* getCondition(ResourceHash key);
		PluginInterface* getExpress(ResourceHash key);

		static PluginResourceObject* acquireCommonPlugin();
		static PluginResourceObject* acquirePlugin(const QString& name );

		//  runtime字符串
		QString runtimeString(PluginInterface* inter = NULL);
		QString runtimeString(int type, ResourceHash key);

		//  非持久化
		QMap<ResourceHash, QString>  varDefaults_;  // 所有参数缺省值
		QMap<ResourceHash, QString>  paramDefaults_;  // 所有参数缺省值
		PluginVariable* getVariableDefine(const QString& prop, const QString& parent, TResourceMap<PluginInterface>& container);
		PluginResourceObject* commonPlugin_;  // 公共插件接口
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	class WorkspaceResourceObject : public ResourceObject{
		DeclareRTTI()
	public:
		WorkspaceResourceObject(){}
		WorkspaceResourceObject( const QString& inName,  unsigned int flags =  URF_Transient | URF_Public );

		virtual ResourceObject* createChild( const QString& inName,  RTTIType childType );

		void reset();

		// 返回当前活动态势（目前一个工作空间下仅包含一个游戏）
		ProjectResourceObject* currentProject();
		void activiteProject(unsigned int hash);
		ProjectResourceObject* findProject(const QString& inName, bool create = false);
		void deleteProject(unsigned int hash);

		QString dirPath_;    // 工作空间目录 
		int projectType_;  // 项目类型
		QString tmpName_; // 临时项目名

		// 配置信息读写
		bool saveConfig();
		bool loadConfig();

	protected:
		ResourceHash  activitedH_;  // 当前激活电子书

	};

	//  全屏模式
	enum{
		FULLSCR_NONE = 0,
		FULLSCR_CROP,
		FULLSCR_SCALE_INNER,
		FULLSCR_SCALE_OUTER,
		FULLSCR_LETTERBOX_SCALE,
		FULLSCR_LETTERBOX_INTSCALE,
	};

	// 物理引擎
	enum{
		PHYENG_BOX2D = 0,
	};

	// 媒体类型
	enum{
		MEDIA_IMAGE = 0,
		MEDIA_AUDIO,
		MEDIA_VIDEO,
		MEDIA_MISC,
		MEDIA_PROJECT,  // 工程名
	};

	enum{
		ORIENT_ANY,
		ORIENT_PORTRAIT,
		ORIENT_LANDSCAPE,
	};

	enum{
		QUALITY_LOW,
		QUALITY_HIGH,
	};

	enum{
		MIPMAP_NO,
		MIPMAP_DENSE,
		MIPMAP_SPARSE,
	};

	enum{
		SAMPLE_LINEAR,
		SAMPLE_POINT,
	};

	enum{
		LOADER_PROGLOGO,
		LOADER_PROGRESS,
		LOADER_PERCENT,
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	class ProjectResourceObject : public ResourceObject{
		DeclareRTTI()
		DECLARE_RESOURCE_FILETYPE( PROJECT_FILENAME )

	public:
		ProjectResourceObject(){}
		ProjectResourceObject(const QString& inName, unsigned int flags = URF_Public);
	
		virtual void encode( QDataStream& stream );
		virtual bool decode( QDataStream& stream );

		ResourceObject* createChild(const QString& inName, RTTIType childType);

		QString pubVersion_; // 发布版本
		QString email_;  // 邮箱
		QString website_;  // 项目的网站
		QString domainId_; // ID

		QDateTime createDate_;  // 创建日期
		QDateTime lastModifyDate_;  // 上一次修改日期
		QString author_;  // 作者
		QPixmap thumbnail_; // 缩略图（128*128）
		QSize resolution_;  // 分辨率
		bool aspect_;  // 是否固定长宽比
		QString startLevel_;  // 启动场景名

		int fullscreenMode_;  // 全屏模式
		int fullscreenQuality_;  // 画面质量
		bool useHighDPI_;
		int sampleType_;
		int orientation_;  // 方向
		int downScaling_;  // 缩小
		int physicalEngine_;  // 物理引擎
		int loaderStyle_;  // 加载页面样式
		bool useLoaderLayout_;  //
		bool useWebGL_;
		bool pixelRounding_;
		bool preloadSound_; //  预加载音乐
		bool pauseOnUnfocus_;  // 失去焦点则停止运行
		bool clearBackground_;  // 是否清除背景（关闭可提高效率）

		// 导出设置
		QString exportDir_;
		QMap<int, QString> mediaDirs_;
		bool minfyScript_; 

		QStringList objectGroups_;  // 对象分组

		QString getDefMediaDir(int type);
		QString getExportName();

		// 插入新场景
		LevelResourceObject* addNewLevel(const QString& inName, int index = -1);
		// 获取场景
		LevelResourceObject* getLevel(const QString& name);

		// 加入新对象类型
		ObjectTypeResourceObject* addNewType(PluginResourceObject* prototype, const QString& inName = QString() );
		// 查找对象类型
		ObjectTypeResourceObject* getObjectType(PluginResourceObject* prototype, const QString& inName = QString());
		ObjectTypeResourceObject* getObjectType(const QString& inName);


		// 加入新脚本
		BlueprintResourceObject* addNewScript(const QString& inName = QString());

		// 加入新集合
		FamilyResourceObject* addNewFamily( const QString& inName = QString());

		FamilyResourceObject* getFamily(const QString& inName);


		// 查找场景
		int indexOf(LevelResourceObject* level);
		int indexOf(ResourceHash hash);

		// 移动场景
		void moveLevel(int src, int dest);
		void moveLevel(LevelResourceObject* level, int index);
		// 删除场景
		void removeLevel(LevelResourceObject* level);

		// 脚本是否有效
		bool validScript();

		// 非持久数据
		QList<LevelResourceObject*> levels_;  // 场景列表（0为缺省起始场景，其余无次序）
		QList<ObjectTypeResourceObject*> objTypes_;  // 对象类型
		QList<BlueprintResourceObject*> blueprints_; // 脚本
		QList<FamilyResourceObject*> families_;  // 集合
		QList<SoundResourceObject*> sounds_;
		QList<MovieResourceObject*> movies_;

		QTreeWidgetItem* leveltreeItem_;  //

		LayerResourceObject* currentLayer_;  // 当前正在编辑的图层
		QString exportTitle_;  // 导出标题名
		int exportType_;  // 导出类型 0正式导出， 1预览， 2调试

		// 查找孩子对象
		template<class T>
		T* findChild(const QString& inName, bool create = false ){
			return TFindWSResource<T>(outer()->objectName(), objectName(), inName, create );
		}

		//  生成唯一名字
		template<class T>
		QString makeUniqueName(const QString& baseName){
			static int index = 0;
			QString newName = baseName;

			QString path;
			while (true){
				if (index == 0){
					newName = baseName;
					index++;
				}
				else{
					newName = QString(UDQ_T("%1%2")).arg(baseName).arg(index++);
				}

				T* t = TFindWSResource<T>(outer()->objectName(), objectName(), newName);
				if (t == NULL){
					break;
				}
			}
			return newName;
		}

	};

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	class LevelResourceObject : public ResourceObject{
		DeclareRTTI()
	public:
		LevelResourceObject(){}
		LevelResourceObject(const QString& inName, unsigned int flags = URF_Public);

		QPixmap thumbnail_; // 缩略图（128*128）

		QSize size_;  //  场景大小（不能移动）
		QSize margin_;  // 场景边框
		bool unboundScroll_;  // 无限滚动
		ResourceHash blueprint_;  // 关联事件模型

		QList<QString> effects_;  // 特效
		QMap<QString, PluginResourceObject*> attachments_;  //特效
		QMap<ResourceHash, QString>  defaults_;  // 参数缺省值（key为 pluginName.paramName）
		QList<QByteArray> instances_;  // 实例数据

		virtual void encode(QDataStream& stream);
		virtual bool decode(QDataStream& stream);

		// 清理
		virtual void clear();

		int indexOf(LayerResourceObject* layer);
		int indexOf(ResourceHash hash);

		// 添加现有页面
		int insertLayer(LayerResourceObject* layer, int index = -1);
		void removeLayer(LayerResourceObject* layer);

		// 移动分组
		void moveLayer(int src, int dest);
		// 交换分组
		void exchangeLayer(int src, int dest);

		// 拷贝对象
		void copyObject(ResourceObject* obj);

		// 插入新图层
		LayerResourceObject* addNewLayer(const QString& inName, int index = -1);

		QTransform matrix_;
		QPointF viewport_;
		int zoomStep_;

		LayerResourceObject* findLayer(const QString& inName);

		// 非持久数据
		QList<LayerResourceObject*> layers_;   // 页面列表
		QTreeWidgetItem* treeItem_;  // 目录树对应项

		// 更新缩略图
		void setThumb(QImage& img);
		void updateThumb();
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	class LayerResourceObject : public ResourceObject{
		DeclareRTTI()
	public:
		LayerResourceObject(){}
		LayerResourceObject(const QString& inName, unsigned int flags = URF_Public);

		virtual void encode(QDataStream& stream);
		virtual bool decode(QDataStream& stream);

		// 属性值备份
		QByteArray saveProperty();
		void restoreProperty(QByteArray& data);

		void copyObject(ResourceObject* obj);

		QPixmap thumbnail_; // 缩略图（128*128）
		bool isLocking_;  // 是否锁定
		bool isVisible_;    //是否可见
		bool isGlobal_;  //  是否全局

		bool initVisible_;  // 初始可见
		int scale_; // 相对缩放比例（0表示不缩放)
		QPoint parallax_;   // 滚动速度（百分比）
		QColor bgColor_;  // 背景颜色
		int opacity_;  // 透明度
		bool transparent_; // 是否背景透明
		bool ownTexture_;  // 纹理缓存
		bool useRenderCell_;  // 是否使用渲染网格
		QList<QByteArray> instances_;  // 图层实例数据

		int blend_; // 混合模式
		int fallback_;  // 回调类型

		QList<QString> effects_;  // 特效
		QMap<QString, PluginResourceObject*> attachments_;  //特效
		QMap<ResourceHash, QString>  defaults_;  // 参数缺省值（key为 pluginName.paramName）

		// 非持久数据
		QTreeWidgetItem* treeItem_;  // 目录树对应项
		LevelResourceObject* level_; // 图层所在场景
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	enum{
		FBT_NORMAL = 0,
		FBT_ADDITIVE,
		FBT_XOR,
		FBT_COPY,
		FBT_DEST_OVER,
		FBT_SRC_IN,
		FBT_DEST_IN,
		FBT_SRC_OUT,
		FBT_DEST_OUT,
		FBT_SRC_ATOP,
		FBT_DEST_ATOP,
		FBT_DESTORY,
	};


	class ObjectTypeResourceObject : public ResourceObject{
		DeclareRTTI()
	public:
		ObjectTypeResourceObject(){}
		ObjectTypeResourceObject(const QString& inName, unsigned int flags = URF_Public);

		virtual void encode(QDataStream& stream);
		virtual bool decode(QDataStream& stream);

		void copyObject(ResourceObject* obj);

		PluginResourceObject*  prototype_;   // 原型
		bool global_;  // 全局对象
		QList<QString> behaviors_;
		QList<QString> effects_;
	
		QMap<QString, PluginResourceObject*> attachments_;  // 行为和特效

		TResourceMap<PluginVariable> variables_;  // 实例变量定义（只有布尔，整数，浮点数和字符串4个类型）
		QMap<ResourceHash, QString>  defaults_;  // 参数缺省值（key为 pluginName.paramName）

		PluginResourceObject* getPlugin(const QString& pluginName);

		// 是否带指定类型的行为
		bool hasAttachment(ResourceHash key);
		bool hasAttachment(PluginResourceObject* plugin);

		// 获取变量定义
		PluginVariable* getVariableDefine(const QString& prop, const QString& pluginName = QString());

		// 读写参数
		QString getParameter(const QString& prop, const QString& pluginName = QString());
		void setParameter(const QString& prop, const QString& value, const QString& pluginName = QString());
	
		// 初始化后处理
		void postInit();

		// 清理
		void clear();

		// 获取缺省动画序列
		SpriteSequenceResourceObject* getDefaultAnimation();

		// 切换为静态图片模式
		bool setImageResource(ResourceHash key);
		bool hasImageResource();
		// 清除图像资源
		void clearImageResource();

		//缩略图
		QPixmap thumbnailImage(int size = 64);

		QByteArray defaultData_;  // 缺省对象数据
		QByteArray tilemapData_;  // TileMap关联数据，碰撞网格

		QString categroy_;  // 对象分组

		// 非持久数据
		QTreeWidgetItem* treeItem_;  // 目录树对应项
		SpriteResourceObject* animation_;  // 精灵动画
		ImageResourceObject* staticImage_;  // 静态图片
		ResourceHash staticSID_;  // 静态SID

	};

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	class FamilyResourceObject : public ResourceObject{
		DeclareRTTI()
	public:
		FamilyResourceObject(){}
		FamilyResourceObject(const QString& inName, unsigned int flags = URF_Public);

		virtual void encode(QDataStream& stream);
		virtual bool decode(QDataStream& stream);

		void copyObject(ResourceObject* obj);

		QList<QString> behaviors_;
		QList<QString> effects_;
		QMap<QString, PluginResourceObject*> attachments_;  // 行为和特效

		TResourceMap<PluginVariable> variables_;  // 实例变量定义（只有布尔，整数，浮点数和字符串4个类型）
		QMap<ResourceHash, QString>  defaults_;  // 参数缺省值（key为 pluginName.paramName）

		// 获取变量定义
		PluginVariable* getVariableDefine(const QString& prop, const QString& pluginName = QString());
		// 读写参数
		QString getParameter(const QString& prop, const QString& pluginName = QString());
		void setParameter(const QString& prop, const QString& value, const QString& pluginName = QString());
		// 初始化后处理
		void postInit();
		// 清理
		void clear();

		// 非持久数据
		QTreeWidgetItem* treeItem_;  // 目录树对应项
		QList<ObjectTypeResourceObject*> members_;  //  成员类型
	};


	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	struct ScriptVariable : public ResourceData
	{
		QString name;
		QString datatype;
		QString value;
		QString desc;

		void save(QDataStream& stream){
			stream << name << datatype << value << desc;
		}
		void load(QDataStream& stream){
			stream >> name >> datatype >> value >> desc;
		}
	};

	class BlueprintResourceObject : public ResourceObject{
		DeclareRTTI()
	public:
		BlueprintResourceObject(){}
		BlueprintResourceObject(const QString& inName, unsigned int flags = URF_Public);

		void clearAll();

		virtual void encode(QDataStream& stream);
		virtual bool decode(QDataStream& stream);

		void copyObject(ResourceObject* obj);

		// 添加新变量（缺省字符串类型）
		ScriptVariable* addNewVariable(const QString& name =QString());

		//  生成有效变量名
		QString makeVariableName();

		// 显示参数
		bool func_;  // 是否为函数
		QTransform matrix_;
		QPointF viewport_;
		int zoomStep_;

		TResourceMap<ScriptVariable> variables_;  // 变量（如果是根脚本，则为全局变量，否则为局部变量）
		// 参数列表
		QList<QByteArray> events_;  // 事件，变量
		QList<QByteArray> links_;  // 连线对象列表
		QList<QByteArray> groups_; // 组对象列表
		QList<QByteArray> assets_; // 其他对象列表
		QList<ResourceHash> includes_;  // 包含的其他Blueprint对象( 防止循环包含 )

		QByteArray scriptJson_;  // 脚本代码
		
		// 非持久数据
		BlueprintResourceObject* parent_;  // 父脚本（函数）
		QTreeWidgetItem* treeItem_;  // 目录树对应项
		bool hasError_;  // 有错误

	};

}

#endif
