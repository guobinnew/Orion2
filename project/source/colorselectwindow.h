#ifndef COLORSELECTWINDOW_H
#define COLORSELECTWINDOW_H

#include <QWidget>

namespace Ui {
class ColorSelectWindow;
}

class ColorSelectWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ColorSelectWindow(QWidget *parent = 0);
    ~ColorSelectWindow();

	void setColor(const QString& clr);
	QString getColorString();

private:
    Ui::ColorSelectWindow *ui;
	QColor color_;  // 当前颜色值

protected:
	virtual void resizeEvent(QResizeEvent * event);

private slots:
    void  on_clrpushButton_clicked();

};

#endif // COLORSELECTWINDOW_H
