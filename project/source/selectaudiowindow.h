#ifndef SELECTAUDIOWINDOW_H
#define SELECTAUDIOWINDOW_H

#include <QDialog>
#include <QMediaPlayer>
#include <QListWidgetItem>
#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class SelectAudioWindow;
}

class SelectAudioWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SelectAudioWindow(QWidget *parent = 0);
    ~SelectAudioWindow();
	QMediaPlayer* player_;  // 播放器

	ResourceHash audioHash_;  // 选中的ID
	void initFromDb(void);

private:
    Ui::SelectAudioWindow *ui;

private slots:
	void  on_cancelpushButton_clicked();
	void  on_savepushButton_clicked();

	void on_playpushButton_clicked();
	void on_pausepushButton_clicked();
	void on_stoppushButton_clicked();
	void on_volumehorizontalSlider_valueChanged(int value);
	void on_listWidget_itemDoubleClicked(QListWidgetItem * item);

	void handleError(QMediaPlayer::Error error);
};

#endif // SELECTAUDIOWINDOW_H
