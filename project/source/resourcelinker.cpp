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

#include "resourcelinker.h"
#include <QDir>

namespace ORION{

	ImplementRTTI(ResourceLinker, URT_Linker, ResourceObject, 0);

	ResourceLinker::ResourceLinker(ResourceObject* inRoot, const QString& inFilename, unsigned int flags)
		: ResourceObject(flags)
		, linkerRoot_(inRoot)
		, filePath_(inFilename)
		, contextFlags_(0){
	}

	// Çå¿Õ
	void ResourceLinker::clear(){
		// É¾³ýÎÄ¼þ
		QDir dir;
		dir.remove(filePath_);
		resourceMap_.clear();
	}
}
