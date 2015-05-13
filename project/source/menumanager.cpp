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

#include "menumanager.h"
#include <QtGui>
#include "commondefine.h"

#include <QtGlobal>
#include <QLatin1Char>
#include <QLatin1String>


#define ORION_MENUITEM_DEF( id, checkable, enable, img, shortcut, text, tooltip, type) \
	static MenuItemDefine MID_##id = { id, checkable, enable, type, img, shortcut, \
	QT_TR_NOOP(text), QT_TR_NOOP(tooltip) }; \
	menuItems_.insert(id, &MID_##id);

#define EDITOR_MENUITEM_DEF( id, checkable, enable, img, shortcut, text, tooltip) \
	ORION_MENUITEM_DEF(id, checkable, enable, img, shortcut, text, tooltip, EDITOR_LEVEL)

#define SPRITE_MENUITEM_DEF( id, checkable, enable, img, shortcut, text, tooltip) \
	ORION_MENUITEM_DEF(id, checkable, enable, img, shortcut, text, tooltip, EDITOR_SPRITE)

#define KISMET_MENUITEM_DEF( id, checkable, enable, img, shortcut, text, tooltip) \
	ORION_MENUITEM_DEF(id, checkable, enable, img, shortcut, text, tooltip, EDITOR_KISMET)

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

QHash<int, MenuItemDefine*> MenuManager::menuItems_;

void MenuManager::staticInit(){

	EDITOR_MENUITEM_DEF(ORION_PROJECT_NEW, 0, 1, ":/images/project_new.png", "", "新建", "新建一个工程");
	EDITOR_MENUITEM_DEF(ORION_PROJECT_SAVE, 0, 1, ":/images/project_save.png", "Ctrl+S", "保存", "保存当前工程");
	EDITOR_MENUITEM_DEF(ORION_PROJECT_SAVEAS, 0, 1, "", "", "另存为...", "另存当前工程");
	EDITOR_MENUITEM_DEF(ORION_PROJECT_EXPORT, 0, 1, ":/images/export.png", "", "导出", "将工程打包到本地文件");
	EDITOR_MENUITEM_DEF(ORION_PROJECT_IMPORT, 0, 1, ":/images/import.png", "", "导入", "从本地打包文件导入");

	EDITOR_MENUITEM_DEF(ORION_PROJECT_DELETE, 0, 1, ":/images/delete.png", "", "删除", "删除当前");
	EDITOR_MENUITEM_DEF(ORION_PROJECT_OPEN, 0, 1, ":/images/project_open.png", "", "打开", "打开一个已有工程");
	EDITOR_MENUITEM_DEF(ORION_PROJECT_CLOSE, 0, 1, "", "Ctrl+E", "关闭", "关闭当前打开的工程");
	EDITOR_MENUITEM_DEF(ORION_PROJECT_PUBLISH, 0, 1, "", "", "发布", "发布当前打开的工程");
	EDITOR_MENUITEM_DEF(ORION_PROJECT_EXIT, 0, 1, "", "Alt+F4", "退出", "退出应用程序");

	//////////////////////////////////////////////////////////////////////////
	
	EDITOR_MENUITEM_DEF(ORION_EDIT_POINTER, 0, 1, ":/images/pointer.png", "", "选择模式", "编辑器处于选择模式");
	EDITOR_MENUITEM_DEF(ORION_EDIT_IMAGE, 0, 1, ":/images/image.png", "", "图片", "添加静态图片");

	EDITOR_MENUITEM_DEF(ORION_EDIT_LOCK, 1, 1, ":/images/locky.png", "", "锁定", "锁定选中的对象（无法移动）");

	EDITOR_MENUITEM_DEF(ORION_EDIT_DELETE, 0, 1, ":/images/crash.png", "", "删除", "删除选中的对象");
	EDITOR_MENUITEM_DEF(ORION_EDIT_CLONE, 0, 1, ":/images/clone.png", "", "克隆新类型", "恢复为缺省对象数据");
	EDITOR_MENUITEM_DEF(ORION_EDIT_COPY, 0, 1, ":/images/copy.png", "", "拷贝", "拷贝");
	EDITOR_MENUITEM_DEF(ORION_EDIT_CUT, 0, 1, ":/images/cut.png", "", "剪切", "剪切");
	EDITOR_MENUITEM_DEF(ORION_EDIT_PASTE, 0, 1, ":/images/paste.png", "", "粘贴", "粘贴");

	EDITOR_MENUITEM_DEF(ORION_EDIT_MOVEFIRST, 0, 1, ":/images/movefirst.png", "", "移动到顶层", "将当前对象移动到所在图层的最上面");
	EDITOR_MENUITEM_DEF(ORION_EDIT_MOVELAST, 0, 1, ":/images/movelast.png", "", "移动到底层", "将当前对象移动到所在图层的最下面");

	EDITOR_MENUITEM_DEF(ORION_EDIT_PANELOPTION,1, 1, ":/images/objects.png", "", "素材面板", "显示/隐藏素材面板");

	EDITOR_MENUITEM_DEF(ORION_EDIT_ADDEFFECT, 0, 1, "", "", "添加特效", "向选中的对象添加特效");

	EDITOR_MENUITEM_DEF(ORION_EDIT_TODEFAULTOBJ, 0, 1, "", "", "保存为缺省对象", "保存为缺省对象");
	EDITOR_MENUITEM_DEF(ORION_EDIT_FROMDEFAULTOBJ, 0, 1, "", "", "恢复缺省值", "恢复为缺省对象数据");

	EDITOR_MENUITEM_DEF(ORION_EDIT_SOUND, 0, 1, ":/images/unique32.png", "", "编辑", "编辑声音定义");
	EDITOR_MENUITEM_DEF(ORION_EDIT_MOVIE, 0, 1, ":/images/unique32.png", "", "编辑", "编辑电影定义");

	EDITOR_MENUITEM_DEF(ORION_EDIT_OBJECTTYPE, 0, 1, ":/images/locky.png", "", "编辑", "编辑对象类型");

	//////////////////////////////////////////////////////////////////////////
	//
	EDITOR_MENUITEM_DEF(ORION_OBJECTTYPE_DELETE, 0, 1, "", "", "删除", "删除选中的对象类型");
	EDITOR_MENUITEM_DEF(ORION_OBJECTTYPE_ADDCATEGORY, 0, 1, "", "", "添加分类", "添加对象类型分类");

	//////////////////////////////////////////////////////////////////////////

	EDITOR_MENUITEM_DEF(ORION_GALLERY_EDIT, 0, 1, ":/images/gallery.png", "", "素材库", "打开素材库管理工具");
	EDITOR_MENUITEM_DEF(ORION_GALLERY_SPRITE, 0, 1, ":/images/macpanel.png", "", "动画编辑", "打开精灵动画编辑器");
	EDITOR_MENUITEM_DEF(ORION_GALLERY_PLUGIN, 0, 1, ":/images/macpanel.png", "", "插件管理", "打开插件管理工具");
	EDITOR_MENUITEM_DEF(ORION_GALLERY_BLUEPRINT, 0, 1, ":/images/edit_blueprint.png", "", "脚本编辑", "打开事件脚本编辑器");

	EDITOR_MENUITEM_DEF(ORION_GALLERY_ADDCATEGORY, 0, 1, ":/images/unique32.png", "", "添加类目", "在当前位置添加新的类目");
	EDITOR_MENUITEM_DEF(ORION_GALLERY_DELETECATEGORY, 0, 1, ":/images/unique32.png", "", "删除", "删除当前选中的类目");

	EDITOR_MENUITEM_DEF(ORION_GALLERY_MERGE, 0, 1, ":/images/unique32.png", "", "合并", "将多个图片合并成一个图片");
	EDITOR_MENUITEM_DEF(ORION_GALLERY_TRANSPARENT, 0, 1, ":/images/unique32.png", "", "背景透明", "将图片的背景色变为透明色");
	EDITOR_MENUITEM_DEF(ORION_GALLERY_COPY, 0, 1, ":/images/unique32.png", "", "复制", "复制选中的资源（重新命名）");
	EDITOR_MENUITEM_DEF(ORION_GALLERY_SPLIT, 0, 1, ":/images/unique32.png", "", "分割", "分割选中的图像");
	
	EDITOR_MENUITEM_DEF(ORION_HELP_ABOUT, 0, 1, "", "", "关于 Orion Editor", "显示关于对话框");
	EDITOR_MENUITEM_DEF(ORION_HELP_WELCOME, 0, 1, ":/images/welcome.png", "", " 欢迎页面", "显示欢迎页面");

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	SPRITE_MENUITEM_DEF(ORION_SPRTIE_SAVE, 0, 1, ":/images/save.png", "", "保存", "保存当前正在编辑的精灵");
	SPRITE_MENUITEM_DEF(ORION_SPRTIE_SAVEALL, 0, 1, ":/images/saveall.png", "", "保存全部", "保存所有编辑过的精灵");
	SPRITE_MENUITEM_DEF(ORION_SPRTIE_REFRESH, 0, 1, ":/images/refresh.png", "", "刷新", "刷新当前选择的精灵");

	SPRITE_MENUITEM_DEF(ORION_SPRTIE_EXPORT, 0, 1, ":/images/unique32.png", "", "导出", "导出动画序列到文件");
	SPRITE_MENUITEM_DEF(ORION_SPRTIE_COPY, 0, 1, ":/images/unique32.png", "", "复制", "复制当前选择的对象");
	SPRITE_MENUITEM_DEF(ORION_SPRTIE_DELETE, 0, 1, ":/images/unique32.png", "", "删除", "删除当前选择的对象");
	
	EDITOR_MENUITEM_DEF(ORION_SPRTIE_LOCKSEQ, 1, 1, ":/images/locky.png", "", "锁定", "锁定选中的动画序列");
	SPRITE_MENUITEM_DEF(ORION_SPRTIE_ADDSEQ, 0, 1, ":/images/animation.png", "", "添加动画序列", "添加动画序列");
	SPRITE_MENUITEM_DEF(ORION_SPRTIE_DELSEQ, 0, 1, ":/images/delete.png", "", "删除", "删除当前选择的动画序列");
	SPRITE_MENUITEM_DEF(ORION_SPRTIE_RENAMESEQ, 0, 1, "", "", "重命名", "重命名动画序列");

	SPRITE_MENUITEM_DEF(ORION_SPRTIE_ADDANCHOR, 0, 1, ":/images/unique32.png", "", "添加锚点", "添加新的锚点");
	SPRITE_MENUITEM_DEF(ORION_SPRTIE_DELANCHOR, 0, 1, ":/images/unique32.png", "", "删除锚点", "删除当前选择的锚点");
	SPRITE_MENUITEM_DEF(ORION_SPRTIE_COPYANCHOR, 0, 1, ":/images/unique32.png", "", "应用到所有帧", "将当前帧的锚点位置应用到其他所有帧");
	SPRITE_MENUITEM_DEF(ORION_SPRTIE_DEFAULTANCHOR, 0, 1, ":/images/unique32.png", "", "恢复默认", "将当前帧的锚点位置设置为图片中心");

	SPRITE_MENUITEM_DEF(ORION_SPRITE_DEFAULTSEQ, 0, 1, ":/images/unique32.png", "", "设置默认序列", "将当前动画序列设为默认序列");

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	KISMET_MENUITEM_DEF(ORION_KISMET_SAVE, 0, 1, ":/images/save.png", "", "保存", "保存所有脚本");
	KISMET_MENUITEM_DEF(ORION_KISMET_DELETE, 0, 1, ":/images/delete.png", "", "删除", "删除选中的脚本");
	KISMET_MENUITEM_DEF(ORION_KISMET_CLEAR, 0, 1, ":/images/crash.png", "", "清空", "清空当前编辑的脚本");
	
	KISMET_MENUITEM_DEF(ORION_KISMET_POINTER, 1, 1, ":/images/pointer.png", "", "指针", "切换到选择模式");
	KISMET_MENUITEM_DEF(ORION_KISMET_ADDGROUP,1, 1, ":/images/group.png", "", "事件组", "向当前编辑器添加一个事件组");
	KISMET_MENUITEM_DEF(ORION_KISMET_ADDCOMMENT, 1, 1, ":/images/notes.png", "", "注释", "向当前编辑器添加一个注释");

	KISMET_MENUITEM_DEF(ORION_KISMET_ADDBRANCH, 0, 1, ":/images/flowcontrol.png", "", "分支", "向当前编辑器添加一个分支控制");
	KISMET_MENUITEM_DEF(ORION_KISMET_ADDSEQUENCE, 0, 1, ":/images/sequence.png", "", "队列", "向当前编辑器添加一个顺序执行队列");
	KISMET_MENUITEM_DEF(ORION_KISMET_ADDMATH, 0, 1, ":/images/math.png", "", "算术运算", "向当前编辑器添加一个算术运算");

	KISMET_MENUITEM_DEF(ORION_KISMET_ADDINSTANCEVAR, 0, 1, ":/images/unique32.png", "", "成员变量", "访问对象的成员变量");
	KISMET_MENUITEM_DEF(ORION_KISMET_ADDINSTANCEINDEX, 0, 1, ":/images/unique32.png", "", "实例索引", "获取指定索引的对象实例");

	KISMET_MENUITEM_DEF(ORION_KISMET_VALIDATE, 0, 1, ":/images/check_script.png", "", "合法性检查", "检查脚本模型的合法性");

	KISMET_MENUITEM_DEF(ORION_KISMET_DELETEPORT, 0, 1, ":/images/unique32.png", "", " 删除", "删除端口");
	KISMET_MENUITEM_DEF(ORION_KISMET_CLEARPORTLINK, 0, 1, ":/images/unique32.png", "", "清空连线", "删除端口的所有连线");

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////
	EDITOR_MENUITEM_DEF(ORION_TOOL_DEVICE, 0, 1, "", "", "设备", "设备");
	EDITOR_MENUITEM_DEF(ORION_TOOL_SETUP, 0, 1, ":/images/", "", "设置", "设置预览/调试信息");
	EDITOR_MENUITEM_DEF(ORION_TOOL_PREVIEW, 0, 1, ":/images/preview.png", "", "预览", "通过浏览器预览当前工程");
	EDITOR_MENUITEM_DEF(ORION_TOOL_DEBUG, 0, 1, ":/images/debugger.png", "", "调试", "使用浏览器调试当前工程");

}

void MenuManager::staticExit(){
	menuItems_.clear();
}

void MenuManager::createActions(int scope, QWidget* parent){
	QHashIterator<int, MenuItemDefine*> i(menuItems_);
	while (i.hasNext()) {
		i.next();
		MenuItemDefine* def = i.value();
		if (def->scope == scope){
			QAction* action = new QAction(QIcon(UDQ_ASCII(def->icon)),
				UDQ_TR(def->text), parent);

			action->setCheckable((bool)def->checkable);
			action->setEnabled((bool)def->enabled);

			// 设置快捷键
			QString shortCut = QString::fromLatin1(def->shortCut);
			if (!shortCut.isEmpty())
				action->setShortcut(shortCut);
			// 提示
			action->setToolTip(UDQ_TR(def->tooltip));

			actions_.insert(i.key(), action);
		}
	}
}

QAction* MenuManager::getAction(int id){
	if (actions_.contains(id)){
		return actions_[id];
	}
	return NULL;
}


static MenuManager UH5MenuManager;
MenuManager* gMenuManager = &UH5MenuManager;

