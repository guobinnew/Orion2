#ifndef MERGEIMAGEWINDOW_H
#define MERGEIMAGEWINDOW_H

#include <QDialog>
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;

namespace Ui {
class MergeImageWindow;
}

class MergeImageWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MergeImageWindow(QList<ResourceHash>& keys, QWidget *parent = 0);
    ~MergeImageWindow();

private:
    Ui::MergeImageWindow *ui;

	QStringList categories_;  // 类目

	bool canSave(void);
	// 合并图像
	QImage mergeImage(const QSize& maxsize, bool fixsize, bool twom);

	QList<QRect> frames_;
private slots:
void on_savepushButton_clicked();
void on_cancelpushButton_clicked();
void on_mergepushButton_clicked();
void on_sortpushButton_clicked();
void on_squarecheckBox_toggled(bool checked);
void on_widthspinBox_valueChanged(int i);
};

#endif // MERGEIMAGEWINDOW_H
