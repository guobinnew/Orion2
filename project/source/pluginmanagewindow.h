#ifndef PLUGINMANAGEWINDOW_H
#define PLUGINMANAGEWINDOW_H

#include <QDialog>
#include <QMultiMap>
#include <QListWidgetItem>
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;

namespace Ui {
class PluginManageWindow;
}

class PluginManageWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PluginManageWindow(QWidget *parent = 0);
    ~PluginManageWindow();

	void initFromDb(void);

	// 有效性检查
	bool checkValid();

private:
    Ui::PluginManageWindow *ui;

	QTreeWidgetItem* nativeRootItem_;
	QTreeWidgetItem* entityRootItem_;
	QTreeWidgetItem* behaviorRootItem_;
	QTreeWidgetItem* effectRootItem_;

	QTreeWidgetItem* variableRootItem_;
	QTreeWidgetItem* conditionRootItem_;
	QTreeWidgetItem* actionRootItem_;
	QTreeWidgetItem* expressRootItem_;

	QMultiMap<int, QListWidgetItem*> attrItemsGroup_;  // 特性Item
	QMap<int, QListWidgetItem*> attrItems_;  // 特性Item
	QMap<QString, QListWidgetItem*> attrStringItems_;  // 特性Item
	void prepareAttrItems(int type, const QMap<int, QString>& items);

	// 查找类目
	QTreeWidgetItem* findCateory(int type, const QString& cate, bool create = true );

	// 查找属性Item
	QTreeWidgetItem* findPropertyItem(int type, const QString& id, bool create = true);
	QTreeWidgetItem* findInterfaceItem(int type, const QString& id, bool create = true);

	// 添加属性Item
	QTreeWidgetItem* addVariableItem(PluginVariable* var);
	QTreeWidgetItem* addVariableItem( const QJsonObject& data );

	QTreeWidgetItem* addInterfaceItem(PluginInterface* inter);
	QTreeWidgetItem* addInterfaceItem(int type, const QJsonObject& data);


	QByteArray tmpFiledat_;  // 临时文件内容
	QByteArray tmpJsonDat_; // 属性数据

	QMultiHash<int, QByteArray> assosiateFiles_;   // 关联文件列表

	void clearChildItem(QTreeWidgetItem* parent);



private slots:
	void on_savepushButton_clicked();
	void on_cancelpushButton_clicked();
	void on_deletepushButton_clicked();
	void on_filepushButton_clicked();


	void on_plugintreeWidget_itemSelectionChanged();
	void on_plugintreeWidget_itemDoubleClicked(QTreeWidgetItem * item, int column);

	void on_typecomboBox_currentIndexChanged(int index);
	void on_iconpushButton_clicked();

	void on_importpushButton_clicked();
	void on_exportpushButton_clicked();
	void on_clearpushButton_clicked();

	void on_addproppushButton_clicked();
	void on_delproppushButton_clicked();

	void on_proptreeWidget_itemDoubleClicked(QTreeWidgetItem * item, int column);

	void on_jsonimppushButton_clicked();
	void on_jsonexppushButton_clicked();
};

#endif // PLUGINMANAGEWINDOW_H
