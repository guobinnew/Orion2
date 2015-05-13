#ifndef ADDANIMATIONANCHORWINDOW_H
#define ADDANIMATIONANCHORWINDOW_H

#include <QWidget>

namespace Ui {
class AddAnimationAnchorWindow;
}

class AddAnimationAnchorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AddAnimationAnchorWindow(QWidget *parent = 0);
    ~AddAnimationAnchorWindow();

private:
    Ui::AddAnimationAnchorWindow *ui;
};

#endif // ADDANIMATIONANCHORWINDOW_H
