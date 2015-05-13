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

#include "commondefine.h"
#include "framemanager.h"


void FrameManager::addWindow(int Id, QMainWindow* window ){
    windows_.insert( Id, window );
}

QMainWindow* FrameManager::getWindow( int Id ){
    if( windows_.contains(Id))
        return windows_[Id];

    return NULL;
}

static FrameManager UH5FrameManager;
FrameManager* gFrameManager = &UH5FrameManager;



