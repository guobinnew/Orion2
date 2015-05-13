#ifndef SELECTSOUNDWINDOW_H
#define SELECTSOUNDWINDOW_H

#include <QDialog>
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class SelectSoundWindow;
}

class SelectSoundWindow : public QDialog
{
    Q_OBJECT

public:
	explicit SelectSoundWindow(ResourceHash key, QWidget *parent = 0);
    ~SelectSoundWindow();

	ResourceHash soundKey_;  // Ñ¡ÖÐµÄÉùÒô
	
private:
    Ui::SelectSoundWindow *ui;


private slots:
	void on_savepushButton_clicked();
	void on_cancelpushButton_clicked();

	void on_addpushButton_clicked();
};

#endif // SELECTSOUNDWINDOW_H
