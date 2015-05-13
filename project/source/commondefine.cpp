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


#include "commondefine.h"
#include <QColor>
#include <QDir>
#include <boost/crc.hpp> 
#include <boost/cstdint.hpp>

namespace ORION{

	QSize computeText(const QString& text, const QFont& font){
		QFontMetrics fm(font);
		return QSize(fm.width(text), fm.height());
	}

	QSize computeText(int flags, const QString & text, const QFont& font){
		QFontMetrics fm(font);
		return fm.size(flags, text);
	}

	QPointF computeTextPos(const QRectF& rect, const QString& txt, const QFont& font){
		QSize txtSize = computeText(txt, font);
		return QPointF(rect.left() + (rect.width() - txtSize.width()) / 2,
			rect.top() + (rect.height() + txtSize.height()) / 2);
	}

	QImage toGray(const QImage& image){
		QSize colourImgSize = image.size();
		int width = colourImgSize.rwidth();
		int height = colourImgSize.rheight();

		QImage grayImg(colourImgSize, image.format());

		for (int i = 0; i < height; i++){
			for (int j = 0; j < width; j++){
				QRgb rgb = image.pixel(i, j);
				int gray = qGray(rgb);
				grayImg.setPixel(i, j, qRgba(gray, gray, gray, qAlpha(rgb)));
			}
		}

		return grayImg;
	}

	QImage toAlpha(const QImage& image, double ratio){
		QSize colourImgSize = image.size();
		int width = colourImgSize.rwidth();
		int height = colourImgSize.rheight();

		QImage alphaImg(colourImgSize, image.format());

		for (int i = 0; i < height; i++){
			for (int j = 0; j < width; j++){
				QRgb rgb = image.pixel(i, j);
				int alpha = qAlpha(rgb) * ratio;
				alphaImg.setPixel(i, j, qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), alpha));
			}
		}

		return alphaImg;
	}



	bool deleteDir(const QString &dirName, bool self){
		static QVector<QString> dirNames;

		QDir dir;
		QFileInfoList filst;
		QFileInfoList::iterator curFi;

		//初始化
		dirNames.clear();
		if (dir.exists()){
			dirNames << dirName;
		}
		else{
			return true;
		}

		//遍历各级文件夹，并将这些文件夹中的文件删除  
		for (int i = 0; i < dirNames.size(); ++i){
			dir.setPath(dirNames[i]);
			filst = dir.entryInfoList(QDir::Dirs | QDir::Files
				| QDir::Readable | QDir::Writable
				| QDir::Hidden | QDir::NoDotAndDotDot
				, QDir::Name);
			if (filst.size()>0){
				curFi = filst.begin();
				while (curFi != filst.end()){
					//遇到文件夹,则添加至文件夹列表dirs尾部  
					if (curFi->isDir()){
						dirNames.push_back(curFi->filePath());
					}
					else if (curFi->isFile()){
						//遇到文件,则删除之  
						dir.remove(curFi->fileName());
					}
					curFi++;
				}//end of while  
			}
		}

		if (!self){
			dirNames.pop_front();  // 去掉根目录
		}

		//删除文件夹  
		for (int i = dirNames.size() - 1; i >= 0; --i){
			dir.rmpath(dirNames[i]);
		}
		return true;
	}

	// 是否按下Delete键
	bool isPressDelete(int key){
#ifdef Q_OS_OSX
		return key == Qt::Key_Backspace;
#else
		return key == Qt::Key_Delete;
#endif
	}

	QString hmacSha1(QByteArray key, QByteArray baseString)
	{
		int blockSize = 64;
		if (key.length() > blockSize) {
			key = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
		}

		QByteArray innerPadding(blockSize, char(0x36)); // "6"
		QByteArray outerPadding(blockSize, char(0x5c)); //  "\"

		for (int i = 0; i < key.length(); i++) {
			innerPadding[i] = innerPadding[i] ^ key.at(i); // XOR 
			outerPadding[i] = outerPadding[i] ^ key.at(i); // XOR
		}

		QByteArray total = outerPadding;
		QByteArray part = innerPadding;
		part.append(baseString);
		total.append(QCryptographicHash::hash(part, QCryptographicHash::Sha1));
		QByteArray hashed = QCryptographicHash::hash(total, QCryptographicHash::Sha1);
		return hashed.toBase64();
	}

	QString unicodeByteToString(const QByteArray &byte){
		QString result;
		if (byte.size() > 0){
			QTextCodec *codec = QTextCodec::codecForName("UTF-8");
			result = codec->toUnicode(byte);
		}
		return result;
	}

	QByteArray stringToUnicodeByte(const QString &strInfo){
		QByteArray result;
		if (strInfo.length() > 0){
			QTextCodec *codec = QTextCodec::codecForName("UTF-8");
			result = codec->fromUnicode(strInfo);
		}
		return result;
	}

	QString xorEncrypt(const QString& src, const QString& key){
		QString result;
		QByteArray bs = stringToUnicodeByte(src);

		QByteArray ks;
		if (key.isEmpty()){
			ks = stringToUnicodeByte(UDQ_T("OSSSnake_AccessKey"));
		}
		else{
			ks = stringToUnicodeByte(key);
		}
		Q_ASSERT(ks.size() > 0);

		int ki = 0;
		for (int i = 0; i < bs.size(); i++){
			bs[i] = bs[i] ^ ks[ki++];
			if (ki == ks.size()){
				ki = 0;
			}
		}

		result = unicodeByteToString(bs.toBase64());
		return result;
	}

	QString xorDecrypt(const QString& src, const QString& key){
		QString result;
		QByteArray bs = QByteArray::fromBase64(stringToUnicodeByte(src));

		QByteArray ks;
		if (key.isEmpty()){
			ks = stringToUnicodeByte(UDQ_T("OSSSnake_AccessKey"));
		}
		else{
			ks = stringToUnicodeByte(key);
		}
		Q_ASSERT(ks.size() > 0);

		int ki = 0;
		for (int i = 0; i < bs.size(); i++){
			bs[i] = bs[i] ^ ks[ki++];
			if (ki == ks.size()){
				ki = 0;
			}
		}

		result = unicodeByteToString(bs);
		return result;
	}

	QString unicodeStringEncode(const QString& str){
		QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
		QByteArray encoded = utf8->fromUnicode(str);
		return QString(encoded.toPercentEncoding());
	}

	QString currentGMTDateString(){
		return GMTDateString(QDateTime::currentDateTime());
	}

	QString GMTDateString(const QDateTime& dt, QLocale::Language lang){
		// Wed, 11 May 2011 07:59:25 GMT
		QLocale locale(lang);
		QString datestr = locale.toString(dt.toTimeSpec(Qt::UTC), "ddd, dd MMM yyyy HH:mm:ss") + " GMT";
		return datestr;
	}

	unsigned int hashString(const QString& str){
		if (str.isEmpty())
			return 0;
		QByteArray arr = str.toLocal8Bit();
		boost::crc_basic<32>  crc_ccitt1(0x1021, 0xFFFF, 0, false, false);
		crc_ccitt1.process_bytes(arr.data(), arr.length());
		return  crc_ccitt1.checksum();
	}

	unsigned int hashStringLower(const QString& str){
		return hashString(str.toLower());
	}

	int twoPower(int i){
		int res = 1;
		while (res < i)
			res <<= 1;
		return res;
	}

	bool isSqrt(int n){
		for (int i = 1; n > 0; i += 2) n -= i;
		return 0 == n;
	}

	// 去除注释
	QString removeNote(const QString& src){
		const QString regex = UDQ_T("//");
		QString res;

		int start = 0;
		int index = src.indexOf(regex, start);

		while (index >= 0 && start < src.size()){
			// 确保//必须是行开头( 前面必须是回车)
			res.append(src.mid(start, index - start ));

			start = index + 2;
			Q_ASSERT(src.at(index) == '/');
			Q_ASSERT(src.at(index + 1) == '/');
			index = src.indexOf(UDQ_T("\n"), start);		
			if (index >= 0){
				start = index;
				index = src.indexOf(regex, start);
			}
			else{
				break;
			}
		}

		if (start < src.size()){
			res.append(src.mid(start));
		}

		return res;
	}

	// 去除注释
	QString removeAllNote(const QString& src){
		QRegExp regex = QRegExp(UDQ_T("(/*|//)"));
		QString res;

		int start = 0;
		int index = src.indexOf(regex, start);

		while (index >= 0 && start < src.size()){
			res.append(src.mid(start, index - start));
			start = index + 2;
			Q_ASSERT(src.at(index) == '/');

			if (src.at(index + 1) == '/'){  // 如果是//,则找到回车
				index = src.indexOf(UDQ_T("\n"), start);
				if (index >= 0){
					start = index;
					index = src.indexOf(regex, start);
				}
				else{
					break;
				}
			}
			else{
				Q_ASSERT(src.at(index + 1) == '*');
				index = src.indexOf(UDQ_T("*/"), start);
				if (index >= 0){
					start = index+2;
					index = src.indexOf(regex, start);
				}
				else{
					start = src.size();
					break;
				}
			}
		}

		if (start < src.size()){
			res.append(src.mid(start));
		}

		return res;
	}

	qreal clampValuef(qreal v, qreal a, qreal b){
		if (v < a){
			v = a;
		}

		if (v > b){
			v = b;
		}
		return v;
	}

	int clampValue(int v, int a, int b){
		if (v < a){
			v = a;
		}

		if (v > b){
			v = b;
		}
		return v;
	}
}
