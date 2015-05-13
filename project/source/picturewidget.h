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
**  Orion is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/
#ifndef PICTUREWIDGET_H
#define PICTUREWIDGET_H

#include <QWidget>

class PictureWidget : public QWidget{
	Q_OBJECT

public:
	PictureWidget(QWidget *parent = 0);
	~PictureWidget();

	enum{
		TPOS_LEFT = 1,
		TPOS_RIGHT = 2,
		TPOS_TOP = 4,
		TPOS_BOTTOM= 8,
		TPOS_HCENTER = 16,
		TPOS_VCENTER = 32,
	};

	// 设置显示文本()
	void setText( const QString& text, int pos = TPOS_LEFT | TPOS_TOP );

	void showPicture( QImage& img );
	void showPicture( const QString& img );
	void showPicture( QByteArray& data );

	// 显示网格
	void showFrames(const QSize& gridsize, const QSize& space);
	void showFrames(const QList<QRect>& frames);

	void hideFrames();

	QByteArray& getPictureData( void ){
		return data_;
	}

	QList<QRect>& getFrames(){
		return frames_;
	}

	void clear();

	void setFillColor( const QColor& color ){
		color_ = color;
	}

	void setFillColor( int hue, int alpha = 255 ){
		color_ = QColor::fromHsvF( hue / 360.0, 1, 1,  alpha / 255.0 );
	}

	QColor fillColor(){
		return color_;
	}
	
	void setTextColor( const QColor& color ){
		textcolor_ = color;
	}

	// 获取图像信息
	QString acquireInfo();

	QImage& getPicture(){
		return image_;
	}

	void setAutoZoom(bool enable){
		zoom_ = enable;
	}

protected:
	virtual void paintEvent ( QPaintEvent * event );
	virtual void mouseDoubleClickEvent ( QMouseEvent * event );

private:
	QImage image_;
	QByteArray data_;
	QColor color_;
	QString text_;
	QColor textcolor_;
	int textpos_;
	bool zoom_;  // 自动缩放

	bool showFrames_;
	QList<QRect> frames_;  // 边框

	static QFont textFont();

signals:
	void angle( int degree );  // 双击指定图片起始角，用于动画帧自动生成

};

#endif // PICTUREWIDGET_H
