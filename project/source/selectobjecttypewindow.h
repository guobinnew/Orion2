#ifndef SELECTOBJECTTYPEWINDOW_H
#define SELECTOBJECTTYPEWINDOW_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class SelectObjectTypeWindow;
}

class SelectObjectTypeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SelectObjectTypeWindow(QWidget *parent = 0);
    ~SelectObjectTypeWindow();

	ResourceHash typeHash_;  // ¿‡–Õπ˛œ£
private:
    Ui::SelectObjectTypeWindow *ui;

private slots:
	void on_treeWidget_itemClicked(QTreeWidgetItem * item, int column);

};

#endif // SELECTOBJECTTYPEWINDOW_H
