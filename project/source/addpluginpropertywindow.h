#ifndef ADDPLUGINPROPERTYWINDOW_H
#define ADDPLUGINPROPERTYWINDOW_H

#include <QDialog>
#include <QMultiHash>
#include <QListWidgetItem>
#include "commondefine.h"
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class AddPluginPropertyWindow;
}

class AddPluginPropertyWindow : public QDialog
{
    Q_OBJECT

public:
	explicit AddPluginPropertyWindow(int pluginType, const QMultiHash<int, QString>& existIds, QWidget *parent = 0);
	explicit AddPluginPropertyWindow(int pluginType, const QMultiHash<int, QString>& existIds, const QByteArray& editprop, QWidget *parent = 0);  // 编辑模式

    ~AddPluginPropertyWindow();

	bool checkValid();

	// 加载属性数据
	void loadProperty(const QByteArray& editprop);

	QMap<QString, QByteArray> newVar_;  // 新添加的属性
	QMap<QString, QByteArray> newCnd_;  // 新添加的属性
	QMap<QString, QByteArray> newAct_;  // 新添加的属性
	QMap<QString, QByteArray> newExp_;  // 新添加的属性

	QMultiHash<int, QString> existIds_;  // 现有属性ID索引
	QMultiHash<int, QString> newIds_;  // 新加的属性ID索引

	QMap<int, QStringList> flags_;  // 类型标志

private:
    Ui::AddPluginPropertyWindow *ui;

	bool editmode_;
	void init( int type);
	// 检查重复ID
	bool duplicatedId(int type, const QString& id);
	QMultiMap<int, QListWidgetItem*> flagItemsGroup_;  // 特性Item
	QMap<int, QListWidgetItem*> flagItems_;  // 特性Item
	void prepareFlagItems(int type, const QMap<int, QString>& items);
	QListWidgetItem* addEnum(const QString& value);
private slots:
	void on_addpushButton_clicked();
	void on_cancelpushButton_clicked();

	void on_propcomboBox_currentIndexChanged(int index);

	void on_datatypecomboBox_currentIndexChanged(const QString & text);
	void on_addenumpushButton_clicked();
	void on_delenumpushButton_clicked();
	void on_enumlistWidget_itemDoubleClicked(QListWidgetItem * item);

	void on_addparampushButton_clicked();
	void on_delparampushButton_clicked();
};

#endif // ADDPLUGINPROPERTYWINDOW_H
