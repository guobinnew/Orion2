#ifndef DEBUGSETUPWINDOW_H
#define DEBUGSETUPWINDOW_H

#include <QDialog>

namespace Ui {
class DebugSetupWindow;
}

class DebugSetupWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DebugSetupWindow(QWidget *parent = 0);
    ~DebugSetupWindow();

	bool canSave();

private:
    Ui::DebugSetupWindow *ui;

private slots:
	void on_savepushButton_clicked();
	void on_cancelpushButton_clicked();

	void on_pathpushButton_clicked();
	void on_findpushButton_clicked();
	void on_iecomboBox_currentIndexChanged(int index);
};

#endif // DEBUGSETUPWINDOW_H
