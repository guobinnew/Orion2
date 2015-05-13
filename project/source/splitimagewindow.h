#ifndef SPLITIMAGEWINDOW_H
#define SPLITIMAGEWINDOW_H

#include <QDialog>
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;

namespace Ui {
class SplitImageWindow;
}

class SplitImageWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SplitImageWindow(ResourceHash key, QWidget *parent = 0);
    ~SplitImageWindow();

private:
    Ui::SplitImageWindow *ui;
	ImageResourceObject* image_;

	void updatePreview();

private slots:
    void on_gridradioButton_clicked(bool checked );
	void on_sizeradioButton_clicked(bool checked );

	void on_widthspinBox_valueChanged(int i);
	void on_wspacespinBox_valueChanged(int i);
	void on_heightspinBox_valueChanged(int i);
	void on_hspacespinBox_valueChanged(int i);

	void on_savepushButton_clicked();
	void on_cancelpushButton_clicked();
};

#endif // SPLITIMAGEWINDOW_H
