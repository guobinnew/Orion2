#ifndef KISMETEVENTWINDOW_H
#define KISMETEVENTWINDOW_H

#include <QWidget>
#include <QTreeWidget>

#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class KismetEventWindow;
}

class KismetEventWindow : public QWidget
{
    Q_OBJECT

public:
    explicit KismetEventWindow(QWidget *parent = 0);
    ~KismetEventWindow();

	// 准备事件清单
	void collectCommonEvent();
	// 显示系统插件
	void preparePluginList();
	void prepareEventList(PluginResourceObject* plugin);

	QTreeWidgetItem* findCateory(const QString& cate, QTreeWidgetItem* rootItem, bool create = true);

	// 添加公共接口
	void addCommonConditionEvent(PluginResourceObject* plugin, const QList<int>& attrs, QTreeWidgetItem* rootItem);
	void addConditionEvent(PluginResourceObject* plugin,  QTreeWidgetItem* rootItem);
	void addCommonActionEvent(PluginResourceObject* plugin, const QList<int>& attrs, QTreeWidgetItem* rootItem);
	void addActionEvent(PluginResourceObject* plugin, QTreeWidgetItem* rootItem);
	void addCommonExpressEvent(PluginResourceObject* plugin, const QList<int>& attrs, QTreeWidgetItem* rootItem);
	void addExpressEvent(PluginResourceObject* plugin, QTreeWidgetItem* rootItem);

	// 添加属性Item
	QTreeWidgetItem* addConditionItem(PluginInterface* inter, QTreeWidgetItem* rootItem);
	QTreeWidgetItem* addActionItem(PluginInterface* inter, QTreeWidgetItem* rootItem);
	QTreeWidgetItem* addExpressItem(PluginInterface* inter, QTreeWidgetItem* rootItem);

	QTreeWidgetItem* findPropertyItem(int type, const QString& id, QTreeWidgetItem* parent, bool create = true);

protected:
	virtual void resizeEvent(QResizeEvent * event);

private:
    Ui::KismetEventWindow *ui;
	QMap<QString, int> aces_;

	// 添加公共接口
	void addCommonEventItem(int type, PluginInterface* inter);
	QMultiHash<int, PluginInterface*> commonCondition_;  // 公共接口
	QMultiHash<int, PluginInterface*> commonAction_;  // 公共接口
	QMultiHash<int, PluginInterface*> commonExpress_;  // 公共接口

	QMap<ResourceHash, QTreeWidgetItem*>  PluginConditions_;
	QMap<ResourceHash, QTreeWidgetItem*>  PluginActions_;
	QMap<ResourceHash, QTreeWidgetItem*>  PluginExpresses_;

	void clear();
	QIcon folderIcon_;
	PluginResourceObject* currentPlugin_;   // 当前显示插件


	QList<QTreeWidgetItem*> interfaceItems_;  // 所有接口向
private slots:
	void on_refreshpushButton_clicked();
	void on_pluginpushButton_clicked();

	void on_plugintreeWidget_itemClicked(QTreeWidgetItem * item, int column);
	void on_filterlineEdit_textEdited(const QString & text);


};

#endif // KISMETEVENTWINDOW_H
