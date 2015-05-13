#ifndef FONTSELECTWINDOW_H
#define FONTSELECTWINDOW_H

#include <QWidget>

namespace Ui {
class FontSelectWindow;
}

class FontSelectWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FontSelectWindow(QWidget *parent = 0);
    ~FontSelectWindow();
	void setFontString(const QString& fnt);
	QString getFontString();
private:
    Ui::FontSelectWindow *ui;
	QFont font_;
protected:
	virtual void resizeEvent(QResizeEvent * event);

private slots:
	void  on_fontpushButton_clicked();
};

#endif // FONTSELECTWINDOW_H
