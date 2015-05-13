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

#include <QtGui>
#include "macdockpanel.h"
#include <math.h>
#include "commondefine.h"
#include "configmanager.h"
#include "imagehelper.h"

#define MAC_ICON_TEXTBG_COLOR QColor(255,255,75,255)
#define MAC_PANEL_BUTTON_COLOR QColor(255,255,255,200)
#define MAC_PANEL_FOCUS_COLOR QColor(255,255,0,255)

const float MAC_ICON_SHADOW_ALPHA = 0.5;
const int MAC_TEXT_MAXWIDTH = 256;

MacDockPanel::MacDockPanel(){
	hoverPanel_ = false;
	hoverIndex_ = -1;
	centerIndex_ = 0;
	leftMove_ = false;
	rightMove_ = false;
    upperText_ = true;
	showShadow_ = true;
}

MacDockPanel::~MacDockPanel(){
	clearIcon();
}

// 清空
void MacDockPanel::clearIcon( void ){
	foreach( DOCKICONITEM* item, icons_){
		delete item;
	}
	icons_.clear();
	centerIndex_ = 0;
} 


qreal MacDockPanel::dockRealHeight( void ){
	qreal height = iconCount() > 0 ? MAC_PANEL_FULL_HEIGHT : MAC_PANEL_WIDTH ;

	if( hoverPanel_  && iconCount() > 0  ){
			height = MAC_PANEL_FULL_HEIGHT+ ( hoverIndex_>= 0 ? MAC_ICON_ZOOM : 0 );
	}
	return  height;
}

void MacDockPanel::drawPanel( QPainter * painter, const QRectF & rect, const QColor& color ){
	QPolygonF panel;
	panel<<rect.bottomLeft()<<QPointF( rect.left() + MAC_PANEL_WIDTH, rect.top() )<<QPointF( rect.right() - MAC_PANEL_WIDTH, rect.top() )<<rect.bottomRight();

	QPainterPath path;
	path.addPolygon( panel );
	painter->fillPath( path, QBrush( color ) );

	if( hoverPanel_ ){
		painter->setPen( QPen( MAC_PANEL_FOCUS_COLOR, 2 ) );
		painter->drawPath( path );
	}

	if( leftMove_ ){
		leftButton_.clear();
		leftButton_<<rect.bottomLeft()<<rect.topLeft()<<QPointF( rect.left() + MAC_PANEL_WIDTH, rect.top() )<<rect.bottomLeft();

		QPainterPath pathLB;
		pathLB.addPolygon( leftButton_ );
		painter->fillPath( pathLB, QBrush( MAC_PANEL_BUTTON_COLOR ) );

		if( hoverPanel_ )
			painter->drawPath( pathLB );
	}

	if( rightMove_ ){
		rightButton_.clear();
		rightButton_<<rect.topRight()<<rect.bottomRight()<<QPointF( rect.right() - MAC_PANEL_WIDTH, rect.top() )<<rect.topRight();

		QPainterPath pathRB;
		pathRB.addPolygon( rightButton_ );
		painter->fillPath( pathRB, QBrush( MAC_PANEL_BUTTON_COLOR ) );

		if( hoverPanel_ )
			painter->drawPath( pathRB );
	}

	// 
	drawDockIcon( painter, rect );
}

// 根据鼠标运动更新状态
void MacDockPanel::mouseMoveAt( const QPointF & mouse ){

	// 检查是否选择了ICON
	hoverIndex_ = -1;
	int index = 0;
	foreach( DOCKICONITEM* item, icons_ ){

		if( item->display ){
			QRectF region = QRectF( 0, 0, item->bound, item->bound );
			region.moveCenter( QPointF( item->center, centerPoint_.ry()  ) );
			region.moveBottom( centerPoint_.ry() );

			if( region.contains( mouse ) ){
				hoverIndex_= index;
				break;
			}
		}
		index++;
	}
}

// 根据鼠标点击更新状态
bool MacDockPanel::mousePressAt(const QPointF & mouse){

	if( !leftButton_.empty() && leftButton_.containsPoint( mouse, Qt::OddEvenFill) ){
		// 向左移动
		if (leftMove_ && centerIndex_ < (icons_.size() - 1) / 2){
			centerIndex_++;
		}
		return true;
	}
	else if( !rightButton_.empty() && rightButton_.containsPoint( mouse, Qt::OddEvenFill)){
		// 向右移动
		if( rightMove_ && centerIndex_ > (( icons_.size() - 1 ) / 2 * -1) ){
			centerIndex_--;
		}
		return true;
	}

	return false;
}

qreal computeZoom( qreal x ){
	qreal zoom = MAC_ICON_ZOOM - x / 8.0;
		return zoom < 0 ? 0 : zoom;
}

void MacDockPanel::computeRegion( const QRectF & rect ){

	QPointF center = rect.center();
	qreal basex;
	qreal width= 0;

	if( hoverIndex_ >=0 ){
		basex = icons_[hoverIndex_]->origon;
		icons_[hoverIndex_]->bound = MAC_ICON_WIDTH + computeZoom( qAbs( basex - lastMousePos_.rx() ) ); 
		width += icons_[hoverIndex_]->bound ;
		icons_[hoverIndex_]->center = basex;

		// 计算左边
		if( hoverIndex_ > 0 ){
			for( int i = hoverIndex_-1; i>=0; i--){
				qreal zoom = ( hoverIndex_ - i ) < 4 ?  computeZoom( qAbs( icons_[i]->origon - lastMousePos_.rx() ) ) : 0;
				if( zoom < 0 )zoom = 0;
				icons_[i]->bound = MAC_ICON_WIDTH + zoom;
				width += icons_[i]->bound ;
			}
		}

		// 计算右边
		if( hoverIndex_ < icons_.size() - 1 ){
			qreal centerx = basex;
			for( int i = hoverIndex_+1; i<icons_.size(); i++){
				qreal zoom = ( i - hoverIndex_ ) < 4 ? computeZoom( qAbs( icons_[i]->origon - lastMousePos_.rx() ) ) : 0;
				if( zoom < 0 )zoom = 0;
				icons_[i]->bound = MAC_ICON_WIDTH + zoom;
				width += icons_[i]->bound ;
			}
		}

		// 重新定位中心
		basex = center.rx() - width / 2 + MAC_ICON_WIDTH * centerIndex_;
		foreach(DOCKICONITEM* item, icons_){
			item->center = basex + item->bound / 2;
			basex += item->bound;
		}
	}
	else{

		qreal offset = MAC_PANEL_PROJECT * MAC_ICON_WIDTH;
		basex = center.rx() - ( offset * ( icons_.size() - 1 - centerIndex_ * 2  ) );
		int index = 0;

		leftMove_ = false;
		rightMove_ = false;

		foreach( DOCKICONITEM* item, icons_ ){
			// 中心位置
			item->center = basex + index * MAC_ICON_WIDTH;
			item->origon = basex + index * MAC_ICON_WIDTH;
			// 计算边界矩阵
			item->bound = MAC_ICON_WIDTH;

			if( item->center < MAC_PANEL_WIDTH + MAC_ICON_WIDTH*2  ){
				leftMove_ = true;
				item->display = false;
			}
			else if( item->center > rect.right() - MAC_PANEL_WIDTH - MAC_ICON_WIDTH*2 ){
				rightMove_ = true;
				item->display = false;
			}
			else{
				item->display = true;
			}

			index++;
		}
	}
}

QFont MacDockPanel::textFont( void ){
	QFont font;
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPixelSize(14);
    font.setFamily(gConfigManager->baseFont());
	font.setBold( true );
	return font;
}

void MacDockPanel::drawIconText( QPainter * painter, const QString& text, const QPointF & center ){
	// 计算文本长度
	QFont textfont = textFont();
	// 去掉回车换行
	QString newText = text;
	newText.replace(UDQ_C('\n'), UDQ_T(""));

	QFontMetrics metric( textfont );
	QSize size = metric.size(Qt::TextSingleLine, newText);
	if( size.width() < MAC_ICON_WIDTH )
		size.setWidth( MAC_ICON_WIDTH );

	if( size.width() >= MAC_TEXT_MAXWIDTH ){  //  改为多行显示
		size.setHeight( size.height()  * ( size.width() / MAC_TEXT_MAXWIDTH  + 1 ) );
		size.setWidth( MAC_TEXT_MAXWIDTH ); 
	}

	QPointF c = center;

	QRectF textBound;
	textBound.setLeft( c.rx() - ( size.width()/2.0 + MAC_ICON_SPACE * 2 ) );
	textBound.setRight( c.rx() + ( size.width()/2.0 + MAC_ICON_SPACE * 2 ) );

    if( upperText_ ){
		textBound.setTop( c.ry() - ( size.height() + MAC_ICON_SPACE * 2 ) );
		textBound.setBottom( c.ry() - MAC_ICON_SPACE * 2 );
	}
	else{
		textBound.setTop( c.ry() + MAC_ICON_SPACE * 2 );
		textBound.setBottom( c.ry()+ ( size.height() + MAC_ICON_SPACE * 2 ) );
	}

	// 绘制边框
	QPainterPath path;
	path.moveTo( c );
    if( upperText_ )
	{
		path.lineTo( c.rx() + MAC_ICON_SPACE, c.ry()  - MAC_ICON_SPACE*2   );
		path.lineTo( textBound.right(), textBound.bottom() );
		path.arcTo( textBound.right() - textBound.height() / 2 , textBound.top(), textBound.height(), textBound.height(), 270, 180 );
		path.lineTo( textBound.left()  , textBound.top() );
		path.arcTo( textBound.left() - textBound.height() / 2 , textBound.top(), textBound.height(), textBound.height(), 90, 180 );
		path.lineTo( c.rx() - MAC_ICON_SPACE,  c.ry()  - MAC_ICON_SPACE*2 );
	}
	else{
		path.lineTo( c.rx() - MAC_ICON_SPACE, c.ry()  + MAC_ICON_SPACE*2   );
		path.lineTo( textBound.left(), textBound.top() );
		path.arcTo( textBound.left() - textBound.height() / 2 , textBound.top(), textBound.height(), textBound.height(), 90, 180 );
		path.lineTo( textBound.right()  , textBound.bottom() );
		path.arcTo( textBound.right() - textBound.height() / 2 , textBound.top(), textBound.height(), textBound.height(), 270, 180 );
		path.lineTo( c.rx() + MAC_ICON_SPACE, c.ry()  + MAC_ICON_SPACE*2 );
	}

	path.lineTo( c );

	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true );
	painter->setRenderHint(QPainter::TextAntialiasing, true );

	painter->fillPath( path, MAC_ICON_TEXTBG_COLOR );

	painter->setPen( QPen(MAC_PANEL_FOCUS_COLOR, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin) );
	painter->drawPath( path );
	
	// 绘制文本
	painter->setPen( QPen(Qt::black, 2 )  );
	painter->setFont( textfont );
	painter->drawText( textBound, Qt::AlignCenter | Qt::TextWrapAnywhere, newText );

	painter->restore();
}

void MacDockPanel::drawDockIcon( QPainter * painter, const QRectF & rect ){
	
	QPointF center = rect.center();
	computeRegion( rect );
	int index = 0;

	foreach( DOCKICONITEM* item, icons_ ){

		if( item->display ){
			QRectF region = QRectF( 0,0,item->bound - MAC_ICON_SPACE, item->bound - MAC_ICON_SPACE );
			region.moveCenter( QPointF(item->center, center.ry()) );
			region.moveBottom( center.ry() );
			painter->drawImage( region, item->image );

			QRectF textRegion = region;

			if( showShadow_ ){
				// 绘制倒影
				painter->save();
				painter->setOpacity( MAC_ICON_SHADOW_ALPHA );
				region.moveTop( center.ry() );
				painter->drawImage(region, item->imageMirror );
				painter->restore();
			}
	
			// 绘制名字
			if( hoverIndex_ == index ){
                drawIconText( painter, item->name, QPointF( item->center, upperText_ ? textRegion.top() : textRegion.bottom()  )  );
			}

		}
		index++;
	}
}

DOCKICONITEM* MacDockPanel::appendIcon( const QString& name, const QString& image, void* context  ){
	return appendIcon( name, QImage(image), context );
}

DOCKICONITEM* MacDockPanel::appendIcon( const QString& name, const QImage& image, void* context  ){

	DOCKICONITEM* item = new DOCKICONITEM;
	item->name = name;
    item->image = ORION::scaleMask( MAC_ICON_DWIDTH, MAC_ICON_DWIDTH, image );
	item->imageMirror = item->image.mirrored();

	item->context = context;
	icons_.append( item );
	return item;

}

int MacDockPanel::appendIcon( DOCKICONITEM* item ){
	icons_.append( item );
	return icons_.size() - 1;
}


DOCKICONITEM* MacDockPanel::insertIcon( int index, const QString& name, const QImage& image, void* context ){
	DOCKICONITEM* item = new DOCKICONITEM;
	item->name = name;
    item->image = ORION::scaleMask( MAC_ICON_DWIDTH, MAC_ICON_DWIDTH, image );
	item->imageMirror = image.mirrored();
	item->context = context;
	insertIcon( index, item );
	return item;
}

int MacDockPanel::insertIcon( int index, DOCKICONITEM* item ){

	if( index > icons_.size() ){
		index = icons_.size();
	}
	else if( index < 0 ){
		index = 0;
	}

	icons_.insert( index, item );
	return index;

}


int MacDockPanel::getIconIndex( DOCKICONITEM* icon ){
	return icons_.indexOf( icon );
}

DOCKICONITEM* MacDockPanel::getIcon( int index ){
	if( index < 0 || icons_.size() <= index ){
		return 0;
	}
	return icons_[index];
}

DOCKICONITEM* MacDockPanel::swapIcon( int index, DOCKICONITEM* newItem ){
	DOCKICONITEM* item = getIcon( index );
	if( item != 0 )
		replaceIcon( index, newItem );
	return item;
}

void MacDockPanel::replaceIcon( int index, DOCKICONITEM* newItem ){
	if( index < 0 || index >= icons_.size() )
		return;

	deleteIcon( index );
	icons_.insert( index, newItem );
}

// 删除图标
DOCKICONITEM* MacDockPanel::removeIcon( int index ){
	DOCKICONITEM* item = getIcon( index );
	icons_.removeAt( index );
	return item;
}

void MacDockPanel::deleteIcon( int index ){
	DOCKICONITEM* item = removeIcon( index );
	delete item;
}
