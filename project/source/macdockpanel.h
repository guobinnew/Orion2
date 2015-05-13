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

#ifndef ORION_MACDOCKPANEL_H
#define ORION_MACDOCKPANEL_H

#include <QList>

// 图标信息结构体
typedef struct sDockIconItem{
	QString name; // 名称
	QImage image; // 图像
	QImage imageMirror; // 图像
	qreal bound; // 边界大小
	qreal center; // 中心位置
	qreal origon;  //  原始原点
	void* context; // 相关上下文
	int display; // 是否可见
}DOCKICONITEM;

const int MAC_PANEL_WIDTH = 48;    // DOCK宽度
const double MAC_PANEL_PROJECT = 0.5; // 投影位置比例
const int MAC_ICON_WIDTH = 64;  // 图标宽度
const int MAC_PANEL_HEIGHT = 96;  // DOCK高度
const int MAC_ICON_DWIDTH = MAC_ICON_WIDTH * 2;  // 图标宽度
const int MAC_ICON_SPACE = 4; // 图标间隔
const int MAC_ICON_ZOOM = 32; // 图标缩放大小
const int MAC_PANEL_FULL_HEIGHT = MAC_PANEL_WIDTH * MAC_PANEL_PROJECT + MAC_ICON_WIDTH;  // 有图标时图标高度

// 与配合使用
class MacDockPanel {
public:
	MacDockPanel();
	~MacDockPanel();

	void setCenterPoint( const QPointF& pt ){
		centerPoint_ = pt;
	}

	void setLastMousePos( const QPointF& pt ){
		lastMousePos_ = pt;
	}

	void setHoverFlag( bool flag ){
		hoverPanel_ = flag;
	}

	void showShadow( bool flag ){
		showShadow_ = flag;
	}

	bool hoverFlag( void ){
		return hoverPanel_;
	}

	void setHoverIndex( int id ){
		hoverIndex_ = id;
	}

	int hoverIndex( void ){
		return hoverIndex_;
	}

	// 绘制面板
	void drawPanel( QPainter * painter, const QRectF & rect, const QColor& color );
	// 绘制浮动图标
	void drawDockIcon( QPainter * painter, const QRectF & rect );
	// 绘制图表文本
	void drawIconText( QPainter * painter, const QString& text, const QPointF & center );

	// 添加图标
	DOCKICONITEM* appendIcon( const QString& name, const QString& image, void* context = 0 ); 
	DOCKICONITEM* appendIcon( const QString& name, const QImage& image, void* context = 0 ); 
	// 添加图标(返回索引)
	int appendIcon( DOCKICONITEM* item );

	// 插入
	DOCKICONITEM* insertIcon( int index, const QString& name, const QImage& image, void* context = 0 ); 
	// 插入(返回索引)
	int insertIcon( int index, DOCKICONITEM* item );

	// 获取索引
	int getIconIndex( DOCKICONITEM* icon );
	DOCKICONITEM* getIcon( int index );

	// 修改图标(返回旧Item)
	DOCKICONITEM* swapIcon( int index, DOCKICONITEM* newItem ); 
	void replaceIcon( int index, DOCKICONITEM* newItem ); 

	// 删除图标
	DOCKICONITEM* removeIcon( int index ); 
	void deleteIcon( int index ); 

	// 清空
	void clearIcon( void );

	// 根据鼠标位置检查碰撞
	void mouseMoveAt( const QPointF & mouse );
	// 如果在功能区，则返回true
	bool mousePressAt( const QPointF & mouse );

	// 计算DOCK当前高度
	qreal dockRealHeight( void );

	// 图标数目
	int iconCount( void ){
		return icons_.size();
	}

	void setTextUpper( bool flag ){
        upperText_ = flag;
	}

	static QFont textFont( void );

private:
	bool hoverPanel_;   // 鼠标是否进入
	bool leftMove_;  // 向左移动
	bool rightMove_; // 向右移动
	bool showShadow_;  // 倒影

	int hoverIndex_; // 当前图标索引（鼠标进入）
	int centerIndex_;  // 中心索引

	QPointF centerPoint_; // 中心点
	QPointF lastMousePos_; // 上一次鼠标位置
	QPolygonF leftButton_; // 左按钮
	QPolygonF rightButton_; // 左按钮
    bool upperText_;  // 文本显示在上面

	// 计算每个图标位置区域
	void computeRegion( const QRectF & rect );

	QList<DOCKICONITEM*> icons_;  // 图标列表

};

#endif
