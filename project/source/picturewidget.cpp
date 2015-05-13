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
#include "picturewidget.h"
#include <QtGui>
#include "commondefine.h"

const int TEXT_MARGIN = 4;

PictureWidget::PictureWidget(QWidget *parent): QWidget(parent){
	textcolor_ = QColor( Qt::white );
	zoom_ = true;
	showFrames_ = false;
}

PictureWidget::~PictureWidget(){

}

// 获取图像信息
QString PictureWidget::acquireInfo(){

	QString grayscaleColor = image_.isGrayscale() ? UDQ_TR("灰度") : UDQ_TR("彩色");

	QString bpp;
	if (image_.depth() == 32) {
		bpp = QString(UDQ_TR(" %1bpp")).arg(image_.bitPlaneCount());
	}
	else if (image_.depth() > 0) {
		bpp = QString(UDQ_TR(" %1bpp (%2 %3)")).arg(image_.bitPlaneCount()).arg(image_.colorCount()).arg(grayscaleColor);
	}

	qreal size = image_.byteCount() / 1024.0;
	return QString(UDQ_TR("数据大小: %1KB |  尺寸: %2x%3px  |  颜色: %4")).arg(QLocale().toString(size, 'f', 2)).arg(image_.width()).arg(image_.height()).arg(bpp);
}


void PictureWidget::showPicture( QImage& img ){
	image_ = img;

	QBuffer buffer(&data_);
	buffer.open(QIODevice::WriteOnly);
	img.save(&buffer, "PNG");
	
	update();
}

void  PictureWidget::showPicture( const QString& img ){

	QFile imageFile( img );
	if ( imageFile.open(QIODevice::ReadOnly) ){
		data_ = imageFile.readAll();
		imageFile.close();
	}
	image_ = QImage::fromData( data_ );
	update();
}

void PictureWidget::showPicture( QByteArray& data ){
	image_ = QImage::fromData( data );
	data_ = data;
	update();
}

void PictureWidget::clear(){
	image_ = QImage();
	data_.clear();
	update();
}


void PictureWidget::paintEvent ( QPaintEvent * event ){
	QPainter painter( this );

	painter.setRenderHint( QPainter::SmoothPixmapTransform, true );
	painter.fillRect(rect(), color_);
	// 计算大小
	QRect box = rect().adjusted(4,4,-4,-4);
	QSize imgbox = image_.size();
	if( imgbox.width() == 0 || imgbox.height() == 0 )
		return;

	qreal b1 = imgbox.width() * 1.0 / box.width();
	qreal b2 = imgbox.height() * 1.0 / box.height();

	qreal zoom = 1.0f;
	QImage imgtmp;
	int offx = 0;
	int offy = 0;
	if( b1 <= 1 && b2 <= 1 ){
		offy = box.top() + (box.height() - image_.height()) / 2;
		offx = box.left() + (box.width() - image_.width()) / 2;
		imgtmp = image_;
	}
	else{
		if( b1 > b2 ){
			imgtmp = image_.scaledToWidth( box.width(), Qt::SmoothTransformation );
			offy = box.top() + (box.height() - imgtmp.height() ) / 2;
			offx = box.left();
			zoom = box.width() * 1.0f / imgbox.width();
		}
		else{
			imgtmp = image_.scaledToHeight( box.height(), Qt::SmoothTransformation );
			offx = box.left() +  ( box.width() - imgtmp.width() ) / 2;
			offy = box.top();
			zoom = box.height() * 1.0f / imgbox.height();
		}
	}
	// 居中
	painter.drawImage(offx, offy, imgtmp);

	painter.setPen(Qt::red);
	QPainterPath path;

	// 绘制frames
	if (showFrames_){
		painter.save();
		QTransform ts;
		ts.translate(offx, offy);
		ts.scale(zoom, zoom);
		painter.setTransform(ts);

		foreach(QRect r, frames_){
			painter.drawRect(r);
		}
	
		painter.restore();
	}

	// 绘制文本
	if( !text_.isEmpty() ){
		QFont font =  textFont();
		painter.setFont( font );
		painter.setPen( textcolor_ );

		QFontMetrics fm( font );
		int textW = fm.width(text_);

		// 计算位置
		QPoint pos;
		if( textpos_ & TPOS_LEFT )
			pos.setX( box.left() + TEXT_MARGIN  );
		else if( textpos_ & TPOS_RIGHT )
			pos.setX( box.right() - textW - TEXT_MARGIN );
		else if( textpos_ & TPOS_HCENTER )
			pos.setX( box.center().rx() - textW / 2 );

		if( textpos_ & TPOS_TOP )
			pos.setY( box.top() + TEXT_MARGIN + font.pixelSize() );
		else if( textpos_ & TPOS_BOTTOM )
			pos.setY( box.bottom() - TEXT_MARGIN );
		else if( textpos_ & TPOS_VCENTER )
			pos.setY( box.center().ry() + font.pixelSize() / 2 );

		painter.drawText( pos, text_ );
	}

}

// 设置显示文本()
void PictureWidget::setText( const QString& text, int pos ){
	text_ = text;
	textpos_ = pos;
	update();
}


QFont PictureWidget::textFont(){
	QFont font;
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPixelSize(12);
	font.setFamily(UDQ_L8("微软雅黑"));
	font.setBold( false );
	font.setItalic( false );
	return font;
}

void PictureWidget::mouseDoubleClickEvent ( QMouseEvent * event ){
	if (event->button() == Qt::LeftButton){
		QPoint center = rect().center();
		QPoint pos = event->pos();

		// 计算角度
		QLineF line1(center, QPointF(center.rx(), center.ry() - 50));
		QLineF line2(center, pos);
		int d = qRound(line2.angleTo(line1));
		if (d < 0)
			d += 360;

		emit angle(d);
	}
}

void PictureWidget::showFrames(const QSize& gridsize, const QSize& space){
	showFrames_ = true;
	frames_.clear();
	// 计算有效frames
	if (gridsize.width() == 0 || gridsize.height() == 0){
		return;
	}

	QSize intersize = gridsize + space;

	int offx = 0;
	int offy = 0;
	while (offy < image_.height()){
		while (offx < image_.width()){
			frames_.append(QRect(offx, offy, gridsize.width(), gridsize.height()));
			offx += intersize.width();

			if (offx + intersize.width() > image_.width()){
				break;
			}
		}
		offy += intersize.height();
		offx = 0;

		if (offy + intersize.height() > image_.height()){
			break;
		}
	}

	update();
}


void PictureWidget::showFrames(const QList<QRect>& frames){
	showFrames_ = true;
	frames_ = frames;
	update();
}

void PictureWidget::hideFrames(){
	showFrames_ = false;
	update();
}