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
#include "imagehelper.h"

#include <QColor>
#include <QPainter>

namespace ORION{

	// 颜色混合
	QImage  colorMask( unsigned int mask,  unsigned int hue,  QImage* source ){

		int width = source->width();
		int height = source->height();

		QImage imageMask( width, height, QImage::Format_ARGB32);
		uint newColor =  (mask & 0x00FFFFFF ) | ( hue <<24 );
		imageMask.fill(  newColor );

		QImage image( width, height, QImage::Format_ARGB32);
		QPainter painter(&image);
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		painter.fillRect(image.rect(), Qt::transparent);
		painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
		painter.drawImage(0, 0, imageMask);
		painter.setCompositionMode(QPainter::CompositionMode_DestinationAtop);
		painter.drawImage(0, 0, *source);
		painter.end();

		return image;
	}

	QImage channelMask(  unsigned int mask,  unsigned int hue, QImage* source ){

		int width = source->width();
		int height = source->height();

		QImage image( width, height, QImage::Format_ARGB32);
		image.fill(0);

		for( int i=0; i<width; i++)
			for( int j=0; j<height; j++){
				// 跳过透明色
				QRgb color = source->pixel(i,j);
				if( qAlpha(color) == 0 )
					continue;

				int gray = qGray( color );
				QRgb newColor = qRgba( gray,gray,gray, qAlpha(color) );
				newColor &= mask;

				if( hue != 100 ){
					QColor hueColor( newColor ) ;
					newColor = hueColor.lighter(hue).rgba();
				}

				image.setPixel(i, j, newColor );
			}
		return image;
	}

	// 改变图像Alpha通道
	QImage*  alphaMask( const QString& url,  qreal alpha ){
		QImage image( url );
		int width = image.width();
		int height = image.height();
		Q_ASSERT( width > 0 && height > 0 );

		QImage* result = new QImage( width, height, QImage::Format_ARGB32);
		result->fill(0);

		for( int i=0; i<width; i++)
			for( int j=0; j<height; j++){
				// 跳过透明色
				QRgb color = image.pixel(i,j);
				if( qAlpha(color) == 0 )
					continue;

				int newA = qAlpha( color ) * alpha;
                color = (color & 0xffffff) + ( newA << 24 );
				result->setPixel(i, j, color );
			}
			return result;
	}

	QImage  scaleMask( int width, int height, const  QImage& source ){
		Q_ASSERT( width > 0 && height > 0 );

		QImage result( width, height, QImage::Format_ARGB32);
		result.fill(0);

		QPainter painter( &result );

		// 居中显示
		qreal b1 = source.width() * 1.0 / width;
		qreal b2 = source.height() * 1.0 / height;

		int offx = 0;
		int offy = 0;

		if( b1 <= 1 && b2 <= 1 ){  // 图片
			offy = ( height - source.height() ) / 2;
			offx = ( width - source.width() ) / 2;
			painter.drawImage(offx, offy, source );
		}
		else{
			QImage imgtmp;
			if( b1 > b2 ){
				imgtmp = source.scaledToWidth( width, Qt::SmoothTransformation);
				offy = ( height- imgtmp.height() ) / 2;
			}
			else{
				imgtmp = source.scaledToHeight( height, Qt::SmoothTransformation );
				offx = ( width - imgtmp.width() ) / 2;
			}
			painter.drawImage(offx, offy, imgtmp);
		}
		return result;
	}

	// 生成图像
	QImage  makeImage(int width, int height, const QColor& color){
		Q_ASSERT(width > 0 && height > 0);

		QImage result(width, height, QImage::Format_ARGB32);
		result.fill(color);

		return result;
	}

	QImage  makeImage(int width, int height, const  QPixmap & tile){
		Q_ASSERT(width > 0 && height > 0);

		QImage result(width, height, QImage::Format_ARGB32);
		result.fill(0);

		{
			QPainter painter(&result);
			painter.drawTiledPixmap(0, 0, width, height, tile);
		}

		return result;
	}

	// 生成缩略图
	QImage thnumImage(int width, int height, QImage& source){
		Q_ASSERT(width > 0 && height > 0);

		QImage result(width, height, QImage::Format_ARGB32);
		result.fill(0);

		QPainter painter(&result);

		// 居中显示
		qreal b1 = source.width() * 1.0 / width;
		qreal b2 = source.height() * 1.0 / height;

		int offx = 0;
		int offy = 0;

		if (b1 <= 1 && b2 <= 1){  // 图片
			offy = (height - source.height()) / 2;
			offx = (width - source.width()) / 2;
			painter.drawImage(offx, offy, source);
		}
		else{
			QImage imgtmp;
			if (b1 > b2){
				imgtmp = source.scaledToWidth(width, Qt::SmoothTransformation);
				offy = (height - imgtmp.height()) / 2;
			}
			else{
				imgtmp = source.scaledToHeight(height, Qt::SmoothTransformation);
				offx = (width - imgtmp.width()) / 2;
			}
			painter.drawImage(offx, offy, imgtmp);
		}
		return result;
	}


	// 图片背景透明
	QImage transparentImage(QImage& img, const QColor& bgcolor){

		int width = img.width();
		int height = img.height();
		Q_ASSERT(width > 0 && height > 0);

		QRgb bg = bgcolor.rgb();
		QImage result(width, height, QImage::Format_ARGB32);
		result.fill(0);

		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++){
				// 跳过透明色
				QRgb color = img.pixel(i, j);				
				if (color == bg )  // 跳过与指定颜色相同的像素
					continue;
				result.setPixel(i, j, color);
			}

		return result;
	}



}
