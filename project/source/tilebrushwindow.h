#ifndef TILEBRUSHWINDOW_H
#define TILEBRUSHWINDOW_H

#include <QWidget>

namespace Ui {
class TileBrushWindow;
}

class TileBrushWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TileBrushWindow(QWidget *parent = 0);
    ~TileBrushWindow();

private:
    Ui::TileBrushWindow *ui;
};

#endif // TILEBRUSHWINDOW_H
