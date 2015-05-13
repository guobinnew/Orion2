#ifndef PUBLISHPROJECTWINDOW_H
#define PUBLISHPROJECTWINDOW_H

#include <QDialog>
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class PublishProjectWindow;
}

class PublishProjectWindow : public QDialog
{
    Q_OBJECT

public:
	explicit PublishProjectWindow(ProjectResourceObject* project, QWidget *parent = 0);
    ~PublishProjectWindow();

	bool canSave();

	int exportPlatform(){
		return platform_;
	}

private:
    Ui::PublishProjectWindow *ui;

	ProjectResourceObject* project_;
	qreal aspect_;  // 长宽比
	int platform_;  // 导出平台类型
	void init();
	void initPlatform();

private slots:
	void on_savepushButton_clicked();
	void on_cancelpushButton_clicked();

	void on_findpushButton_clicked();
	void on_widthspinBox_valueChanged(int i);
};

#endif // PUBLISHPROJECTWINDOW_H
