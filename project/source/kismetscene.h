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

#ifndef  ORION_KISTMET_SCENE_H
#define ORION_KISTMET_SCENE_H

#include <QGraphicsScene>
#include <QMap>
#include <QPixmap>

#include "kismetgroup.h"
#include "kismetcomment.h"
#include "kismetobject.h"
#include "kismetmodel.h"
#include "kismetvariable.h"
#include "kismetinstance.h"
#include "kismetflowcontrol.h"
#include "kismetarithmetic.h"
#include "kismetcollection.h"
#include "kismetinstancevar.h"
#include "selectobjecttypewindow.h"

#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;

class QDragEnterEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;

class KismetScene : public QGraphicsScene{
	Q_OBJECT

public:

	enum KISMET_MODE{
		DOM_POINTER = 0,        // 初始状态（选择, 移动）
		DOM_ADDGROUP,         // 添加Group
		DOM_ADDCOMMENT,   // 添加注释
		DOM_ADDLINK,   // 添加连线
	};

	KismetScene(QObject *parent = 0);
	virtual ~KismetScene();

	KISMET_MODE getMode(void){
		return mapMode_;
	}
	
	// 获取可见区域（场景坐标系）
	QRectF viewRegionRect( void );
	// 刷新脚本
	void refreshScript();
	// 重设
	void clearScene();
	// 保存
	bool saveCurrentScript();
	// 加载
	void loadScript(BlueprintResourceObject* blueprint);

	void enterInsertGroup();
	void enterInsertComment();
	void exitInsertState();

	// 更新脚本名
	void updateScriptText();
	void updateVariableItem(ScriptVariable* var);
	void removeVariableItem(ScriptVariable* var);

	QList<BlueprintVariable*> variables(const QString& name);
	QList<BlueprintVariable*> variableItems(ScriptVariable* var);

	// 新建对象
	BlueprintGroup* addGroupItem( const QString& text, const QPointF& pos);
	BlueprintComment* addCommentItem(const QString& text, const QPointF& pos);
	BlueprintLink* addLinkItem(BlueprintObject* source, BlueprintObject* dest, QList<QPointF>& ctrlPts );

	BlueprintModel* addEventItem(const QPointF& pos, const QByteArray& data);
	BlueprintVariable* addVariableItem(const QPointF& pos, ResourceHash key, const QString& name);
	BlueprintInstance* addInstanceItem(const QPointF& pos, ResourceHash key);
	BlueprintCollection* addCollectionItem(const QPointF& pos, ResourceHash key);
	// 添加分支
	BlueprintBranch* addBranchItem();
	BlueprintArithmetic* addMathItem();
	BlueprintSequence* addSequenceItem();
	BlueprintInstanceVariable* addInstanceVarItem();

	QStringList groupNames();

	//  清空所有连接
	void clearLink();

	BlueprintObject* menuHost(){
		return menuHostItem_;
	}

	void setMenuHost(BlueprintObject* host){
		menuHostItem_ = host;
	}

	BlueprintResourceObject* parentModelInstance(){
		return parentInstance_;
	}

	// 添加事件端口( 从模型中解析添加)
	BlueprintGroup* addGroupItem( QByteArray& data, ResourceHash newid = 0);
	BlueprintComment* addCommentItem(QByteArray& data, ResourceHash newid = 0);
	BlueprintObject* addModelItem(QByteArray& data, ResourceHash newid = 0);

	BlueprintLink* addLinkItem(QByteArray& data);
	BlueprintObject* decodePortFromData(QByteArray& data);

	// 根据IID获取对象
	BlueprintObject* getObjectItem(ResourceHash iid);
	void addObjectItem(BlueprintObject*);
	void removeObjectItem(BlueprintObject*);
	void deleteObjectItem(BlueprintObject*);

	//////////////////////////////////////////////////////////////////////////
	// 脚本检查，返回错误提示信息
	bool validateScript();
	// 检查上下游模型合法性
	bool checkModelSequence(BlueprintModel* up, BlueprintModel* down);
	void saveToJson();
	// 生成事件块代码
	QJsonArray generateBlockCode(BlueprintModel* head);
	QJsonArray generateBlockElseCode(BlueprintModel* head);
	QJsonArray generateActionsCode(BlueprintModel* head, bool isElse = false);
	QJsonArray generateSubsCode(BlueprintModel* head, bool isElse = false );

	QStringList& scriptErrors(){
		return scriptErrors_;
	}

	QList<BlueprintGroup*>& groupObjects(){
		return grpObjects_;
	}

	int groupIndex(BlueprintGroup* grp){
		return grpObjects_.indexOf(grp);
	}

	QString newGroupName();

	// 选择对象类型
	void selectObjectType(const QPointF& pos);

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	void notifyObjectInsert(QGraphicsItem* item);
	void notifyLinkChanged(BlueprintLink* link);

protected:
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
	virtual void dragEnterEvent ( QGraphicsSceneDragDropEvent * event );
	virtual void dragLeaveEvent ( QGraphicsSceneDragDropEvent * event );
	virtual void dragMoveEvent ( QGraphicsSceneDragDropEvent * event );
	virtual void dropEvent ( QGraphicsSceneDragDropEvent * event );
	virtual void keyPressEvent ( QKeyEvent * keyEvent );
	virtual void keyReleaseEvent ( QKeyEvent * keyEvent );
	virtual void drawBackground ( QPainter * painter, const QRectF & rect );
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);

public slots:
    void setMode(KISMET_MODE mode);
	void addModelPort();
	void addInstanceVariable();
	void addInstanceIndex();
	void deleteModelPort();
	void clearModelPortLink();

	void removeVariable(const QString& name);
	void updateVariable(const QString& name);

signals:
	void entityInserted( QGraphicsItem* entity );
	void entityResized(QGraphicsItem* entity);
	void entityDeleted(const QList<QGraphicsItem*>& items);
	void entityMoved(const QList<QGraphicsItem*>& items);
	void entityCopy(const QList<QGraphicsItem*>& items, const QPointF& pos);
	void entityModified(QGraphicsItem* entity, const QByteArray& oldData);

	void entityExpanded( unsigned int hashkey );
	void entityCollapsed( unsigned int hashkey );

	void pointerMode();

private:
	KISMET_MODE   mapMode_;     // 地图模式
	bool insertInputEvent_; // 是否插入输入事件

	QColor bgColor_;  // 背景颜色
	QGraphicsSimpleTextItem*  scriptText_;  // 图层信息

	BlueprintObject* menuHostItem_;
	BlueprintResourceObject* parentInstance_;   // 模型父实例

	QList<BlueprintModel*> modelObjects_;   // ( 模型 )
	QList<BlueprintVariable*>  varObjects_;  // 变量
	QList<BlueprintInstance*>  instObjects_;  // 实例变量
	QList<BlueprintCollection*> aggrObjects_;  // 集合对象
	QList<BlueprintObject*> assetObjects_;   // ( 注释 等其他物体 )
	QList<BlueprintGroup*> grpObjects_;   // ( 组)

	QHash<ResourceHash, BlueprintObject*>  iidObjects_;  // 对象IID映射

	QStringList scriptErrors_; // 错误信息


	// 计算当前图像的有效区域
	QRect validRegion();
	QRect validRegionGroup(BlueprintGroup* grp);
	QRect validRegionViewPort();
	QList<BlueprintGroup*> selectedGroups();
	void screenShotGroup(BlueprintGroup* grp);

	//////////////////////////////////////////////////////////////////////////
	QMap<ResourceHash, QByteArray>   oldData_;  // 当前选中对象旧数据

};


#endif
