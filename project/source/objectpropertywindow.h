#ifndef OBJECTPROPERTYWINDOW_H
#define OBJECTPROPERTYWINDOW_H

#include <QWidget>
#include <QtAbstractPropertyManager>
#include <QtTreePropertyBrowser>
#include "resourceobject.h"
#include "editoritem.h"
#include "editorregion.h"
#include "editortilemap.h"
using namespace ORION;

namespace Ui {
class ObjectPropertyWindow;
}

class ObjectPropertyWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectPropertyWindow(QWidget *parent = 0);
    ~ObjectPropertyWindow();

	// 初始化
	void init();
	// 清除
	void clear();
	void clearUndo();

	QTreeWidgetItem* currentItem(){
		return currentItem_;
	}

	EditorItem* currentSceneItem(){
		return currentSceneItem_;
	}

	// 加载属性
	void loadProperty(QTreeWidgetItem* obj);
	void loadProperty(EditorItem* obj);

	// 更新属性值
	void updateProperty(EditorItem* obj, bool isAll = false );

	// 添加属性
	void addProperty(QtProperty *property, ResourceHash id, QtProperty *parent = NULL);
	
	// 准备属性
	void prepareLayerProperty(LayerResourceObject* layer);
	void prepareLevelProperty(LevelResourceObject* level);
	void prepareProjectProperty(ProjectResourceObject* proj);

	void prepareObjectTypeProperty(ObjectTypeResourceObject* obj);
	void prepareFamilyProperty(FamilyResourceObject* family);

	QtProperty* addInstance(PluginVariable* var, const QString& value = QString(), const QString& prefix = QString());
	void updateInstance(QtProperty* prop, PluginVariable* var, const QString& value);

	// 准备对象属性
	void prepareRegionProperty(EditorRegion* region );
	//  更新属性
	void updateFamilyProperty(FamilyResourceObject* family, const QString& varid, const QString& value);
	void updateObjectTypeProperty(ObjectTypeResourceObject* obj, const QString& varid, const QString& value);
	void updateRegionProperty(EditorRegion* region, const QString& varid, const QString& value);

	void updateExpandState();
protected:
	virtual void resizeEvent(QResizeEvent * event);

private:
    Ui::ObjectPropertyWindow *ui;

	enum{
		MT_VARIANT = 0,
		MT_BOOL,
		MT_CHAR,
		MT_INT,
		MT_DOUBLE,
		MT_STRING,
		MT_COLOR,
		MT_SIZE,
		MT_SIZEF,
		MT_POINT,
		MT_POINTF,
		MT_RECT,
		MT_RECTF,
		MT_MARGINS,
		MT_MARGINSF,
		MT_ENUM,
		MT_GROUP,
		MT_DATE,
		MT_TIME,
		MT_DATETIME,
		MT_KEYSEQUNCE,
		MT_FONT,
		MT_CURSOR,
	};

	QMap<int, QtAbstractPropertyManager*>  managers_;  // 属性管理器
	QtTreePropertyBrowser *propertyEditor_;

	QTreeWidgetItem* currentItem_;
	EditorItem* currentSceneItem_;

	QList<QtProperty *> subproperties_;  // 子属性
	QMap<QtProperty *, ResourceHash> propertyToId_;
	QMap<ResourceHash, bool> idToExpanded_;

	QMap<QString, ResourceHash> scriptToId_;   //  脚本
	bool isUpdating_;

	QByteArray oldValues_;  // 旧属性值备份

	// 处理undo
	void processUndo(QTreeWidgetItem* obj);
	void processUndo(EditorItem* obj);



private slots:
	void valueChanged(QtProperty *property, int value);
	void valueChanged(QtProperty *property, double value);
	void valueChanged(QtProperty *property, const QString &value);
	void valueChanged(QtProperty *property, const QColor &value);
	void valueChanged(QtProperty *property, const QSize &value);
	void valueChanged(QtProperty *property, const QPoint &value);
	void valueChanged(QtProperty *property, const QRect &value);
	void valueChanged(QtProperty *property, const QDateTime &value);
	void valueChanged(QtProperty *property, const QFont &value);
	void propertySelected(QtBrowserItem *);
};

#endif // OBJECTPROPERTYWINDOW_H
