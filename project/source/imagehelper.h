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
#ifndef ORION_IMAGEHELPER_H
#define ORION_IMAGEHELPER_H

#include <QImage>
#include <QRgb>

namespace ORION{

	const unsigned int RedChannel = 0xFFFF0000;
	const unsigned int GreenChannel = 0xFF00FF00;
	const unsigned int BlueChannel = 0xFF0000FF;
	const unsigned int GrayChannel = 0xFFFFFFFF;

	// 根据通道掩码生成新的图像，保留Alpha通道
	QImage channelMask( unsigned int mask,  unsigned int hue, QImage* source );

	// 改变图像Alpha通道
	QImage*  alphaMask( const QString& url ,  qreal alpha );

	// 颜色混合
	QImage  colorMask( unsigned int mask,  unsigned int hue, QImage* source );

	// 图像缩放
	QImage  scaleMask( int width, int height, const QImage& source );

	// 生成图像
	QImage  makeImage(int width, int height, const QColor& color );
	QImage  makeImage(int width, int height, const  QPixmap & tile );

	// 生成缩略图
	QImage thnumImage(int width, int height, QImage& img);

	// 图片背景透明
	QImage transparentImage(QImage& img, const QColor& color );

	// 图像合并


}


#endif
