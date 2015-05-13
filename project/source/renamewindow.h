#ifndef RENAMEWINDOW_H
#define RENAMEWINDOW_H

#include <QDialog>
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class RenameWindow;
}

class RenameWindow : public QDialog
{
    Q_OBJECT

public:
	explicit RenameWindow(ResourceObject* host, bool createNew = true,  QWidget *parent = 0);
    ~RenameWindow();

	ResourceObject* newObject(){
		return newObj_;
	}

private:
    Ui::RenameWindow *ui;
	ResourceObject* host_;
	bool createNew_; // 创建新对象

	ResourceObject* newObj_;

private slots:
	void on_okpushButton_clicked();
};

#endif // RENAMEWINDOW_H
