#ifndef PLUGINFILELINKWINDOW_H
#define PLUGINFILELINKWINDOW_H

#include <QDialog>

namespace Ui {
class PluginFileLinkWindow;
}

class PluginFileLinkWindow : public QDialog
{
    Q_OBJECT

public:
	explicit PluginFileLinkWindow(QMultiHash<int, QByteArray>& files, QWidget *parent = 0);
    ~PluginFileLinkWindow();

private:
    Ui::PluginFileLinkWindow *ui;
	QMultiHash<int,QByteArray>& files_;
	QStringList types_;
	QHash<int, QStringList> fileexts_;

	int acquireFileType(const QString& ext);
	//  Ìí¼ÓÎÄ¼þ
	void addFileItem(const QString& name, int type,  const QString& date, QByteArray& data);

private slots:
	void on_addpushButton_clicked();
	void on_cancelpushButton_clicked();
	void on_deletepushButton_clicked();
	void on_emptypushButton_clicked();

};

#endif // PLUGINFILELINKWINDOW_H
