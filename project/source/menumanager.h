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

#ifndef ORION_MENU_ORION_H
#define ORION_MENU_ORION_H

#include <QHash>
#include <QAction>

//////////////////////////////////////////////////////////////////////////
//
static const char* STR_MENU_PROJECT = QT_TR_NOOP("项目[&P]");


//////////////////////////////////////////////////////////////////////////

enum{

    //////////////////////////////////////////////////////////////////////////
    //  
    //////////////////////////////////////////////////////////////////////////
	ORION_PROJECT_NEW,
	ORION_PROJECT_SAVE,
	ORION_PROJECT_SAVEAS,
	ORION_PROJECT_EXPORT,
	ORION_PROJECT_IMPORT,
	ORION_PROJECT_DELETE,
	ORION_PROJECT_OPEN,
	ORION_PROJECT_CLOSE,
	ORION_PROJECT_PUBLISH,
	ORION_PROJECT_EXIT,

	//////////////////////////////////////////////////////////////////////////
	//  编辑器
	//////////////////////////////////////////////////////////////////////////
	ORION_EDIT_POINTER,
	ORION_EDIT_IMAGE,

	ORION_EDIT_DELETE,
	ORION_EDIT_CLONE,
	ORION_EDIT_COPY,
	ORION_EDIT_CUT,
	ORION_EDIT_PASTE,

	ORION_EDIT_MOVEFIRST,
	ORION_EDIT_MOVELAST,

	ORION_EDIT_ADDEFFECT,

	ORION_EDIT_AUDIO,
	ORION_EDIT_CHANGEIMAGE,
	ORION_EDIT_TRIGGERPROP,
	ORION_EDIT_PANELOPTION,
	ORION_EDIT_CANVASCOMPONENT,

	ORION_EDIT_LOCK,  

	ORION_EDIT_TODEFAULTOBJ,
	ORION_EDIT_FROMDEFAULTOBJ,

	ORION_EDIT_SAVETMX,
	ORION_EDIT_LOADEMX,

	ORION_EDIT_SOUND,
	ORION_EDIT_MOVIE,

	ORION_EDIT_OBJECTTYPE,

	//////////////////////////////////////////////////////////////////////////
	// 场景
	//////////////////////////////////////////////////////////////////////////

	ORION_OBJECTTYPE_DELETE,
	ORION_OBJECTTYPE_ADDCATEGORY,

	//////////////////////////////////////////////////////////////////////////
	//  工具
	//////////////////////////////////////////////////////////////////////////
	ORION_GALLERY_EDIT,
	ORION_GALLERY_ADDCATEGORY,
	ORION_GALLERY_DELETECATEGORY,

	ORION_GALLERY_MERGE,
	ORION_GALLERY_TRANSPARENT,
	ORION_GALLERY_COPY,
	ORION_GALLERY_SPLIT,

	ORION_GALLERY_SPRITE,
	ORION_GALLERY_DEVICE,
	ORION_GALLERY_BLUEPRINT,
	ORION_GALLERY_PLUGIN,

	ORION_GALLERY_SOUND,
	ORION_GALLERY_MOVIE,

	//////////////////////////////////////////////////////////////////////////
	//  预览管理
	//////////////////////////////////////////////////////////////////////////
	ORION_TOOL_DEVICE,
	ORION_TOOL_SETUP,
	ORION_TOOL_PREVIEW,
	ORION_TOOL_DEBUG,

	//////////////////////////////////////////////////////////////////////////
	//  精灵
	//////////////////////////////////////////////////////////////////////////
	ORION_SPRTIE_SAVE,
	ORION_SPRTIE_SAVEALL,

	ORION_SPRTIE_REFRESH,

	ORION_SPRTIE_EXPORT,
	ORION_SPRTIE_COPY,
	ORION_SPRTIE_DELETE,

	ORION_SPRTIE_ADDSEQ,
	ORION_SPRTIE_DELSEQ,
	ORION_SPRTIE_RENAMESEQ,
	ORION_SPRTIE_LOCKSEQ,

	ORION_SPRTIE_ADDANCHOR,
	ORION_SPRTIE_DELANCHOR,
	ORION_SPRTIE_COPYANCHOR,
	ORION_SPRTIE_DEFAULTANCHOR,

	ORION_SPRITE_DEFAULTSEQ,


	//////////////////////////////////////////////////////////////////////////
	//  事件脚本
	//////////////////////////////////////////////////////////////////////////
	ORION_KISMET_SAVE,
	ORION_KISMET_DELETE,
	ORION_KISMET_CLEAR,

	ORION_KISMET_POINTER,
	ORION_KISMET_ADDGROUP,
	ORION_KISMET_ADDCOMMENT,
	ORION_KISMET_ADDEVENT,

	ORION_KISMET_ADDBRANCH,
	ORION_KISMET_ADDSEQUENCE,
	ORION_KISMET_ADDMATH,

	ORION_KISMET_ADDINSTANCEVAR,
	ORION_KISMET_ADDINSTANCEINDEX,
	
	ORION_KISMET_DELETEPORT,
	ORION_KISMET_CLEARPORTLINK,

	ORION_KISMET_VALIDATE,

	//////////////////////////////////////////////////////////////////////////
	//  帮助
	//////////////////////////////////////////////////////////////////////////
	ORION_HELP_ABOUT,
	ORION_HELP_WELCOME,

    MENU_MAX_COUNT,
};

enum{
	EDITOR_LEVEL,
	EDITOR_SPRITE,
	EDITOR_KISMET,
	EDITOR_PARTICLE,
};

struct MenuItemDefine{
    int                   id;
    int                   checkable;       // 是否可选
    int                   enabled;         // 是否置灰
    int                   scope;             // 所属界面
	const char*     icon;               // 图标
	const char*     shortCut;        // 快捷键
	const char*     text;               // 文本
	const char*     tooltip;          // 工具条提示
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


class MenuManager : public QObject{
    Q_OBJECT
public:
    static void staticInit();
    static void staticExit();

    void createActions( int scope, QWidget* parent );
    QAction* getAction( int id );

private:
    static QHash<int, MenuItemDefine*> menuItems_;
    QHash<int, QAction*> actions_;
};

extern MenuManager* gMenuManager;

#define BEGIN_ACTION() \
	QAction* act;

#define CONNECT_ACTION( id, dest, func)  \
    act=gMenuManager->getAction(id); \
    connect(act,SIGNAL(triggered()),dest,SLOT(func()));

#define DISCONNECT_ACTION(id) \
	act = gMenuManager->getAction(id); \
	disconnect(act, SIGNAL(triggered()),0,0);

#define  END_ACTION()


#endif // ORION_MENU_ORION_H
