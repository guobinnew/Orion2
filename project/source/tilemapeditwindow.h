#ifndef TILEMAPEDITWINDOW_H
#define TILEMAPEDITWINDOW_H

#include <QDialog>
#include <QListWidgetItem>
#include "imagescene.h"

#include "resourceobject.h"
using namespace ORION;

namespace Ui {
class TileMapEditWindow;
}

class TileMapEditWindow : public QDialog
{
    Q_OBJECT

public:
	explicit TileMapEditWindow(ObjectTypeResourceObject* obj, const QSize& grid, int index =-1, QWidget *parent = 0);
    ~TileMapEditWindow();

	void initTileBlocks(const QSize& size);

private:
    Ui::TileMapEditWindow *ui;
	ImageScene* scene_;
	ObjectTypeResourceObject* objType_;
	QSize gridsize_;
	QMap<int, QList<QPointF> > collision_; // Åö×²Íø¸ñ

	void setDefaultCollison();

private slots:
    void on_listWidget_currentItemChanged(QListWidgetItem * current, QListWidgetItem * previous);

    void on_savepushButton_clicked();
	void on_applypushButton_clicked();
	void on_defaultpushButton_clicked();
	void on_clearpushButton_clicked();
};

#endif // TILEMAPEDITWINDOW_H
