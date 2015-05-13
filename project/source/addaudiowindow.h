#ifndef ADDAUDIOWINDOW_H
#define ADDAUDIOWINDOW_H

#include <QDialog>
#include <QMediaPlayer>
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;

namespace Ui {
class AddAudioWindow;
}

class AddAudioWindow : public QDialog
{
    Q_OBJECT

public:
	explicit AddAudioWindow(const QString& filePath, const QStringList& categories, QWidget *parent = 0);
    ~AddAudioWindow();


	AudioResourceObject* newAudio_;

private:
    Ui::AddAudioWindow *ui;
	QMediaPlayer* player_;  // 播放器

	QByteArray  data_;  // 原始数据
	QString format_;  // 文件格式

	bool canSave(void);


private slots:
	void  on_cancelpushButton_clicked();
	void  on_savepushButton_clicked();

	void on_playpushButton_clicked();
	void on_pausepushButton_clicked();
	void on_stoppushButton_clicked();
	void on_volumehorizontalSlider_valueChanged(int value);

	void handleError(QMediaPlayer::Error error);

};

#endif // ADDAUDIOWINDOW_H
