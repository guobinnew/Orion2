#ifndef KISMETGROUPEDITWINDOW_H
#define KISMETGROUPEDITWINDOW_H

#include <QDialog>
#include "kismetgroup.h"

namespace Ui {
class KismetGroupEditWindow;
}

class KismetGroupEditWindow : public QDialog
{
    Q_OBJECT

public:
	explicit KismetGroupEditWindow(BlueprintGroup* group, QWidget *parent = 0);
    ~KismetGroupEditWindow();

	bool canSave(void);

private:
    Ui::KismetGroupEditWindow *ui;
	BlueprintGroup* group_;

private slots:
	void on_cancelpushButton_clicked();
	void on_okpushButton_clicked();
	void on_sortpushButton_clicked();
	void on_colorhorizontalSlider_valueChanged(int value);
	void on_alphahorizontalSlider_valueChanged(int value);
};

#endif // KISMETGROUPEDITWINDOW_H
