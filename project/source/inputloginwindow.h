#ifndef INPUTLOGINWINDOW_H
#define INPUTLOGINWINDOW_H

#include <QDialog>

namespace Ui {
class InputLoginWindow;
}

class InputLoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit InputLoginWindow(QWidget *parent = 0);
    ~InputLoginWindow();

private:
    Ui::InputLoginWindow *ui;

	void updateLabel();

	QByteArray firstCode_;  // ÊäÈëµÄ´úÂë


private slots:
	void processAuthorization(const QByteArray& data);
};

#endif // INPUTLOGINWINDOW_H
