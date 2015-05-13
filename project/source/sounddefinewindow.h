#ifndef SOUNDDEFINEWINDOW_H
#define SOUNDDEFINEWINDOW_H

#include <QDialog>
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;

namespace Ui {
class SoundDefineWindow;
}

class SoundDefineWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SoundDefineWindow(SoundResourceObject* sound, QWidget *parent = 0);
    ~SoundDefineWindow();
	SoundResourceObject* sound_;


	void addAudioItem(AudioResourceObject* audio);

private:
    Ui::SoundDefineWindow *ui;

private slots:
    void on_savepushButton_clicked();
	void on_cancelpushButton_clicked();

	void on_filepushButton_clicked();
	void on_gallerypushButton_clicked();
};

#endif // SOUNDDEFINEWINDOW_H
