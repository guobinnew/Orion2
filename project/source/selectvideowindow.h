#ifndef SELECTVIDEOWINDOW_H
#define SELECTVIDEOWINDOW_H

#include <QDialog>
#include <QMediaPlayer>
#include <QListWidgetItem>
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class SelectVideoWindow;
}

class SelectVideoWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SelectVideoWindow(QWidget *parent = 0);
    ~SelectVideoWindow();
	QMediaPlayer* player_;  // 播放器

	ResourceHash videoHash_;  // 选中的ID
	void initFromDb(void);
private:
    Ui::SelectVideoWindow *ui;

private slots:
	void  on_cancelpushButton_clicked();
	void  on_savepushButton_clicked();
	void handleError(QMediaPlayer::Error error);

	void on_playpushButton_clicked();
	void on_pausepushButton_clicked();
	void on_stoppushButton_clicked();
	void on_volumehorizontalSlider_valueChanged(int value);
	void on_listWidget_itemDoubleClicked(QListWidgetItem * item);

};

#endif // SELECTVIDEOWINDOW_H
