#ifndef KISMETCOMMENTEDITWINDOW_H
#define KISMETCOMMENTEDITWINDOW_H

#include <QDialog>
#include "kismetcomment.h"

namespace Ui {
class KismetCommentEditWindow;
}

class KismetCommentEditWindow : public QDialog
{
    Q_OBJECT

public:
	explicit KismetCommentEditWindow(BlueprintComment* comment, QWidget *parent = 0);
    ~KismetCommentEditWindow();
	bool canSave(void);
private:
    Ui::KismetCommentEditWindow *ui;
	BlueprintComment* comment_;

private slots:
	void on_cancelpushButton_clicked();
	void on_okpushButton_clicked();
	void on_colorhorizontalSlider_valueChanged(int value);

};

#endif // KISMETCOMMENTEDITWINDOW_H
