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
#include "exporter.h"
#include "commondefine.h"

const int BUFFER_SIZE = 100 * 1024 * 1024;   // 100M

void Exporter::loadTextFromDB(const QString& file, int type, QByteArray& code){

	char* buffer = new char[BUFFER_SIZE];  // 缓冲

	Db db_Code(NULL, 0);
	u_int32_t oFlags = DB_RDONLY;
	Dbt key, data;
	Dbc *db_Cursor = 0;

	try {
		QString datFile = file;
		std::string arrstr = datFile.toStdString();

		db_Code.open(NULL,
			arrstr.c_str(),
			NULL,
			DB_BTREE,
			oFlags,
			0);

		db_Code.cursor(NULL, &db_Cursor, 0);
		Dbt key((char*)(&type), sizeof(int));
		Dbt data;

		data.set_data(buffer);
		data.set_ulen(BUFFER_SIZE);
		data.set_flags(DB_DBT_USERMEM);

		int ret = db_Cursor->get(&key, &data, DB_SET);
		if (ret != DB_NOTFOUND) {
			// 读取文件模板
			code = QByteArray((const char*)data.get_data(), data.get_size());
		}

		db_Cursor->close();
		db_Code.close(0);

		delete buffer;

	}
	catch (DbException &e) {
	}
	catch (std::exception &e) {
	}
}


bool Exporter::saveTextToDB(Db& db, int type, const QString fileName, const QString& src){

	QString name = QString(UDQ_T("%1/%2")).arg(src).arg(fileName);
	QFile codeFile(name);

	if (codeFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		QByteArray  codeDat = codeFile.readAll();
		codeFile.close();

		Dbt key((char*)(&type), sizeof(int));
		char* cCode = codeDat.data();
		Dbt value(cCode, codeDat.size());

		db.put(NULL, &key, &value, 0);

		return true;
	}

	return false;
}


bool  Exporter::saveFile(Db& db, int type, const QString fileName, const QString& src, bool txt){

	QString name = QString(UDQ_T("%1/%2")).arg(src).arg(fileName);
	QFile file(name);

	QIODevice::OpenMode mode = QIODevice::ReadOnly;
	if (txt){
		mode |= QIODevice::Text;
	}

	if (file.open(mode)){
		QByteArray  dat = file.readAll();
		file.close();

		Dbt key((char*)(&type), sizeof(int));
		char* cCode = dat.data();
		Dbt value(cCode, dat.size());
		db.put(NULL, &key, &value, 0);

		return true;
	}

	return false;
}



void Exporter::saveTextToDisk(const QString& fileName, const QByteArray& text){
	//检查目录是否存在
	QFileInfo info(fileName);
	QDir dir = info.absoluteDir();

	if (!dir.exists()){
		dir.mkpath(info.absolutePath());
	}

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
		return;
	file.write(text);
	file.close();
}

void Exporter::saveDataToDisk(const QString& fileName, const QByteArray& data){
	//检查目录是否存在
	QFileInfo info(fileName);
	QDir dir = info.absoluteDir();

	if (!dir.exists()){
		dir.mkpath(info.absolutePath());
	}

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return;
	file.write(data);
	file.close();
}


