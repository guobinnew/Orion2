#ifndef CORDOVASETTINGWINDOW_H
#define CORDOVASETTINGWINDOW_H

#include <QDialog>
#include "exportcordova.h"

namespace Ui {
class CordovaSettingWindow;
}

class CordovaSettingWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CordovaSettingWindow(CordovaExporter* exporter, QWidget *parent = 0);
    ~CordovaSettingWindow();

private:
    Ui::CordovaSettingWindow *ui;
	CordovaExporter* exporter_; //
private slots:
	void on_savepushButton_clicked();
	void on_cancelpushButton_clicked();
};

#endif // CORDOVASETTINGWINDOW_H
