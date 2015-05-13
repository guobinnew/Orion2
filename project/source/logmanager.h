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
**  along with Orion.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#ifndef  LOG_MANAGER_H
#define LOG_MANAGER_H

#include "commondefine.h"
#include <QString>
#include <QMap>
#include <QSettings>

class LogManager{
public:
    static void staticInit();

	void log(const QString& text);
	void logError(const QString& err);
	void logWarning(const QString& err);
};

extern LogManager* gLogManager;



#endif // LOG_MANAGER_H
