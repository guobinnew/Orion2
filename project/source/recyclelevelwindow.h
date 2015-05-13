#ifndef RECYCLELEVELWINDOW_H
#define RECYCLELEVELWINDOW_H

#include <QDialog>
#include <QListWidgetItem>
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class RecycleLevelWindow;
}

class RecycleLevelWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RecycleLevelWindow(ProjectResourceObject* project, QWidget *parent = 0);
    ~RecycleLevelWindow();

	LevelResourceObject* level_;
private:
    Ui::RecycleLevelWindow *ui;
	void recycleLevel(QListWidgetItem* item);

private slots:
    void on_okpushButton_clicked();
	void on_listWidget_itemDoubleClicked(QListWidgetItem * item);

};

#endif // RECYCLELEVELWINDOW_H
