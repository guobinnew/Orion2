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

#ifndef ORION_FRAME_MANAGER_H
#define ORION_FRAME_MANAGER_H

#include <QObject>
#include <QMainWindow>
#include <QMap>


class FrameManager : public QObject{
    Q_OBJECT
public:
    FrameManager(){};

    enum{
        WID_LEVEL,   // 关卡编辑
		WID_SPRITE,   // 精灵编辑
        WID_ENTITY,   // 实体编辑
		WID_BLUEPRINT,  // 脚本编辑
    };

    void addWindow( int Id, QMainWindow* window );
    QMainWindow* getWindow( int Id );

    //////////////////////////////////////////////////////////////////////////
    // 实体缓冲

private:
    QMap<int, QMainWindow*> windows_;

};

extern FrameManager* gFrameManager;




#endif
