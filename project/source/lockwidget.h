/************************************************************************
**
**  Copyright (C) 2015  guobin  <guobin.gb@alibaba-inc.com>
**
**  This file is part of Orion2.
**
**  Orion2 is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation.
**
**  Orion2 is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#ifndef LOCKWIDGET_H
#define LOCKWIDGET_H

#include <QWidget>


class LockWidget : public QWidget{
	Q_OBJECT

public:
	LockWidget( QWidget *parent = 0);
	~LockWidget();

	// 重置
	void setLock(bool flag = false);

protected:
	virtual void	resizeEvent(QResizeEvent * event);
	virtual void   paintEvent ( QPaintEvent * event );
	virtual void	mouseMoveEvent(QMouseEvent * event);
	virtual void	mousePressEvent(QMouseEvent * event);
	virtual void	mouseReleaseEvent(QMouseEvent * event);

private:
	QColor bgColor_;  //  背景颜色
	QColor color_;  // 按钮颜色
	QColor highlightColor_;

	QString text_;  // 提示文字
	QColor textcolor_;  // 文字颜色
	
	bool disableMode_;  // 禁用模式
	bool lockMode_;  // 进入解锁模式
	QPoint currentMousePos_; //

	QList<QRect> buttonBounds_;  // 按钮中心位置

	QList<int> lockActions_; // 解锁动作
	void adjust();

	// 将动作转变为签名
	QByteArray toSignature();
	//  绘制按钮
	void drawButtons(QPainter& painter);
	void drawButton(QPainter& painter, const QRectF& rect );
	// 绘制连线
	void drawLines(QPainter& painter);

signals:
	void inputPwd(const QByteArray& code);

};

#endif // PICTUREWIDGET_H
