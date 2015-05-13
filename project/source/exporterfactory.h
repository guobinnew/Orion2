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


#ifndef ORION_EXPORTERFACTORY_H
#define ORION_EXPORTERFACTORY_H

#include "exporter.h"

#include "resourceobject.h"
using namespace ORION;

class ExporterFactory
{
public:
	// ªÒ»°
	Exporter* getExporter(int type,  ProjectResourceObject* proj);

};


extern ExporterFactory* gExporterFactory;

#endif // EXPORTERFACTORY_H

