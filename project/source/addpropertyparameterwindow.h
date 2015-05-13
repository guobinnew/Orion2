#ifndef ADDPROPERTYPARAMETERWINDOW_H
#define ADDPROPERTYPARAMETERWINDOW_H

#include <QWidget>

namespace Ui {
class AddPropertyParameterWindow;
}

class AddPropertyParameterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AddPropertyParameterWindow(QWidget *parent = 0);
    ~AddPropertyParameterWindow();

private:
    Ui::AddPropertyParameterWindow *ui;
};

#endif // ADDPROPERTYPARAMETERWINDOW_H
