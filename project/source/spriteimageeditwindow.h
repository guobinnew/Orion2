#ifndef SPRITEIMAGEEDITWINDOW_H
#define SPRITEIMAGEEDITWINDOW_H

#include <QDialog>
#include <QListWidget>
#include "imagescene.h"

#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class SpriteImageEditWindow;
}

class SpriteImageEditWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SpriteImageEditWindow(SpriteSequenceResourceObject* seq, QWidget *parent = 0);
    ~SpriteImageEditWindow();

	QListWidgetItem* addFrame(SequenceFrame* frame);

protected:
	virtual void resizeEvent(QResizeEvent * event);

private slots:
    void on_listWidget_currentItemChanged(QListWidgetItem * current, QListWidgetItem * previous);
	void on_anchorlistWidget_itemSelectionChanged();
	void on_anchorlistWidget_itemChanged(QListWidgetItem * item);

	void on_addpushButton_clicked();
	void on_delpushButton_clicked();

	void on_savepushButton_clicked();
	void on_pointerpushButton_clicked();
	void on_anchorpushButton_clicked();
	void on_collisionpushButton_clicked();
	void on_applypushButton_clicked();

	void on_flippushButton_clicked();
	void on_revertpushButton_clicked();
	void on_ccwpushButton_clicked();
	void on_cwpushButton_clicked();

private:
    Ui::SpriteImageEditWindow *ui;
	ImageScene* scene_;
	SpriteSequenceResourceObject* sequence_;  // ∂Øª≠–Ú¡–

	QIcon anchorIcon_;  // 
};

#endif // SPRITEIMAGEEDITWINDOW_H
