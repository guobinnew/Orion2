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
#include "exporterfactory.h"
#include "exporth5.h"
#include "exportnw.h"
#include "exportcordova.h"

Exporter* ExporterFactory::getExporter(int type,ProjectResourceObject* proj )
{
	switch (type)
	{
	case Exporter::TYPE_HTML5:
	{
		return  new HTML5Exporter(proj);
	}
	break;
	case Exporter::TYPE_NWJS:
	{
		return  new NWJSExporter(proj);
	}
	break;
	case Exporter::TYPE_CORDOVA:
	{
		return  new CordovaExporter(proj);
	}
	break;
	}

	return NULL;

}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

static ExporterFactory OrionExporterFactory;
ExporterFactory* gExporterFactory = &OrionExporterFactory;



