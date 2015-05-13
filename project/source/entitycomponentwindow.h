#ifndef ENTITYCOMPONENTWINDOW_H
#define ENTITYCOMPONENTWINDOW_H

#include <QWidget>
#include <QListWidget>

namespace Ui {
class EntityComponentWindow;
}

class EntityComponentWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EntityComponentWindow(QWidget *parent = 0);
    ~EntityComponentWindow();

	// 准备组件列表
	void refreshComponentList();

	// 清空
	void clear();

	void addListWidget(QListWidget* widget, int index);

protected:
	virtual void resizeEvent(QResizeEvent * event);

private:
    Ui::EntityComponentWindow *ui;
	QIcon defIcon_; 
	QListWidget* findToolBoxList(const QString& name, bool create = false);

private slots:
	void on_refreshpushButton_clicked();
};

#endif // ENTITYCOMPONENTWINDOW_H
