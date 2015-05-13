/************************************************************************
**
**  Copyright (C) 2015  guobin  <guobin.gb@alibaba-inc.com>
**
**  This file is part of Orion2.
**
**  Orion is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation.
**
**  Orion2 is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion2.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/


#ifndef ORION_EXPORTER_H
#define ORION_EXPORTER_H

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <db_cxx.h>

class Exporter : public QObject
{
	Q_OBJECT
public:

	enum{
		TYPE_HTML5 = 1,
		TYPE_NWJS,
		TYPE_CORDOVA,
	};

    virtual ~Exporter() {}

	virtual void write(int mode = 0) = 0;
	virtual QString name() = 0;
	virtual void makeTemplateDB(const QString& file, const QString& src, bool afterDelete = false) = 0;

protected:
	void saveTextToDisk(const QString& fileName, const QByteArray& text);
	void saveDataToDisk(const QString& fileName, const QByteArray& data);

	void loadTextFromDB(const QString& file, int type, QByteArray& code);
	bool saveTextToDB(Db& db, int type, const QString fileName, const QString& src);

	bool saveFile(Db& db, int type, const QString fileName, const QString& src, bool txt = true);

signals:
	void progress(int);

};

#endif // ORION_EXPORTER_H


