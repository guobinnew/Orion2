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
**  Orion2 is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion2.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#ifndef ORION_VERSION_DEFINE_H
#define ORION_VERSION_DEFINE_H

namespace ORION{
//////////////////////////////////////////////////////////////////////////

#define PACKAGE_RESOURCE_VERSON_BASIC                   200        // 基础版本
#define PACKAGE_RESOURCE_ADD_TIMESTAMP                 201       //  加入保存时戳
#define PACKAGE_RESOURCE_ADD_VIEWPORT                  202       //  场景中加入编辑器视口信息
#define PACKAGE_RESOURCE_ADD_FAMILY                        203       //  加入Family对象
#define PACKAGE_RESOURCE_ADD_EXPORTSETUP             204       //  给Project加入导出设置信息
#define PACKAGE_RESOURCE_ADD_ANIMSETUP                205       //  给动画帧加入配置信息
#define PACKAGE_RESOURCE_ADD_BLEND                         206       //  给对象类型添加特效混合模式
#define PACKAGE_RESOURCE_ADD_LAYERSETUP                207       //  给图层对象添加属性
#define PACKAGE_RESOURCE_ADD_VARINDEX                  208        //  插件属性加上顺序索引
#define PACKAGE_RESOURCE_ADD_SCRIPTJSON               209        // 脚本对象加入预生成JSON
#define PACKAGE_RESOURCE_ADD_RENDERCELL              210        // 图层加入渲染网格属性
#define PACKAGE_RESOURCE_ADD_DEFAULTOBJECT        211        // 对象类型加入缺省实例初始化数据
#define PACKAGE_RESOURCE_ADD_TILEMAP                     212        // 对象类型加入TileMap数据
#define PACKAGE_RESOURCE_ADD_LAYERGLOBAL           213        // 图层加入全局属性
#define PACKAGE_RESOURCE_ADD_PROJECTUPDATE       214        // 增加工程项目属性
#define PACKAGE_RESOURCE_ADD_SOUNDMOVIE          215        // 增加声音和电影
#define PACKAGE_RESOURCE_ADD_IMAGEGROUP          216        // 增加图像组
#define PACKAGE_RESOURCE_ADD_LEVELEFFECT               217        // 增加场景图层特效
#define PACKAGE_RESOURCE_ADD_OBJECTGROUP          218        // 增加对象类型分组
#define PACKAGE_RESOURCE_ADD_SCRIPTFUNCTION       219        // 增加脚本函数
#define PACKAGE_RESOURCE_ADD_DEFAULTSEQUENCE    220        // 增加缺省动画序列属性
#define PACKAGE_RESOURCE_ADD_ENTITYOBJECT               221        // 增加场景的公共对象


//////////////////////////////////////////////////////////////////////////

#define PACKAGE_RESOURCE_VERSON        PACKAGE_RESOURCE_ADD_ENTITYOBJECT 

}

#endif
