#ifndef MOVIEDEFINEWINDOW_H
#define MOVIEDEFINEWINDOW_H

#include <QWidget>

namespace Ui {
class MovieDefineWindow;
}

class MovieDefineWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MovieDefineWindow(QWidget *parent = 0);
    ~MovieDefineWindow();

private:
    Ui::MovieDefineWindow *ui;
};

#endif // MOVIEDEFINEWINDOW_H
