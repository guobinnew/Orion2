#ifndef KISMETSTRUCTWINDOW_H
#define KISMETSTRUCTWINDOW_H

#include <QWidget>
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;

namespace Ui {
class KismetStructWindow;
}

class KismetStructWindow : public QWidget
{
    Q_OBJECT

public:
	explicit KismetStructWindow(QWidget *parent = 0);
    ~KismetStructWindow();

	void updateWorkspace();

	void init(ProjectResourceObject* script);
	void updateScript(ProjectResourceObject* proj);

protected:
	virtual void resizeEvent(QResizeEvent * event);

private:
    Ui::KismetStructWindow *ui;

private slots:
    void on_scriptpushButton_clicked();
	void on_funcpushButton_clicked();
	void on_scripttreeWidget_itemDoubleClicked(QTreeWidgetItem * item, int column);
	void on_scripttreeWidget_itemChanged(QTreeWidgetItem * item, int column);
};

#endif // KISMETSTRUCTWINDOW_H
