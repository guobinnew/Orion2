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


#ifndef ORION_COMMON_DEFINE_H
#define ORION_COMMON_DEFINE_H

#include <QtGlobal>
#include <QtGui>
#include <QLatin1Char>
#include <QLatin1String>

#define UDQ_T(a)  QLatin1String(a)
#define UDQ_C(a)  QLatin1Char(a)

#define UDQ_ASCII(a) QString::fromLatin1(a)
#define UDQ_U8(a) QString::fromUtf8(a)
#define UDQ_L8(a) QString::fromLocal8Bit(a)

#define UDQ_TR UDQ_L8

#define REG_NAMEASCII    UDQ_T("^([a-z]|[A-Z])+([0-9]|[a-z]|[A-Z]){0,}$")   // 字母数字
#define REG_NAMEID       UDQ_T("^([a-z]|[A-Z])+([0-9]|[a-z]|[A-Z]|_ ){0,}$")   // 字母数字和下划线，不包含中文
#define REG_GALLERYID       UDQ_T("^([a-z]|[A-Z]|[0-9]|_)+([0-9]|[a-z]|[A-Z]|_|-){0,}$")   // 字母数字和-，_，不包含中文
#define REG_NAMEDT       UDQ_T("^([a-z]|[A-Z])+([0-9]|[a-z]|[A-Z]|*){0,}$")   // 数据类型
#define REG_NAMEFULL     UDQ_T("^\\S+$")   // 字符串（包含中文）不包含空格
#define REG_FLOAT        UDQ_T("^[+,-]{0,1}\\d+.{0,1}\\d*$")  // 整数或浮点数（带正负号）
#define REG_INTERGER     UDQ_T("^[+,-]{0,1}\\d+$")  // 整数（带正负号）
#define REG_IPADDR       UDQ_T("(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-4]|[01]?\\d\\d?")  // IP地址


#if defined(_MSC_VER)
typedef quint32 Pointer;
#elif defined(__linux__)
typedef quint64 Pointer;
#endif

namespace ORION{

	static const double Pi = 3.14159265358979323846264338327950288419717;
    static const double TwoPi = 2.0 * Pi;

	// 适用于ARGB格式
	QImage toGray( const QImage& image );
	QImage toAlpha( const QImage& image, double ratio );
	// 计算文字大小
	QSize computeText( const QString& text, const QFont& font );
	QSize computeText(int flags, const QString & text, const QFont& font);
	// 计算文字在指定区域的绘制位置（居中）
	QPointF computeTextPos( const QRectF& rect, const QString& txt, const QFont& font );

	// 删除目录（及其内部所有文件）
	bool deleteDir(const QString &dirName, bool self = true);

	// 是否按下Delete键
	bool isPressDelete(int key);

	// HMAC HSA1校验
	QString hmacSha1(QByteArray key, QByteArray baseString);

	// 简单异或加解密
	QString xorEncrypt(const QString& src, const QString& key = QString());
	QString xorDecrypt(const QString& src, const QString& key = QString());

	// 字符与UTF-8字节流转换
	QString unicodeByteToString(const QByteArray &byte);
	QByteArray stringToUnicodeByte(const QString &strInfo);

	// 字符串编码（%XX）
	QString unicodeStringEncode(const QString& str);

	// GMT格式日期
	QString currentGMTDateString();
	QString GMTDateString(const QDateTime& dt, QLocale::Language lang = QLocale::English);

	// 字符串hash
	unsigned int hashString(const QString& str);
	unsigned int hashStringLower(const QString& str);

	// 计算最接近的2幂次
	int twoPower(int i);

	// 是否为完全平方数
	bool isSqrt(int i);

	// 去除注释(// /* */ )
	QString removeNote(const QString& src);
	QString removeAllNote(const QString& src);

	qreal clampValuef(qreal v, qreal a, qreal b);
	int clampValue(int v, int a, int b);
}

#endif
