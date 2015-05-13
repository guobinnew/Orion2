#ifndef NWJSSETTINGWINDOW_H
#define NWJSSETTINGWINDOW_H

#include <QDialog>
#include "exportnw.h"

namespace Ui {
class NWJSSettingWindow;
}

class NWJSSettingWindow : public QDialog
{
    Q_OBJECT

public:
	explicit NWJSSettingWindow(NWJSExporter* expoter, QWidget *parent = 0);
    ~NWJSSettingWindow();

private:
    Ui::NWJSSettingWindow *ui;
	NWJSExporter* exporter_; //
private slots:
    void on_savepushButton_clicked();
	void on_cancelpushButton_clicked();
};

#endif // NWJSSETTINGWINDOW_H
