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

#include "resourcereferencecheckwindow.h"
#include <QtGui>
#include <QFileDialog>
#include "commondefine.h"
#include "configmanager.h"
#include "tooldefine.h"
#include "resourcelinker.h"


ResourceReferenceCheckWindow::ResourceReferenceCheckWindow(ResourceHash hashkey, QWidget *parent)
	: QDialog(parent), ui(new Ui::ResourceReferenceCheckWindow)
{
	ui->setupUi(this);
	setFixedSize(size());
	res_ = ResourceObject::findResource( hashkey );

	// 执行引用检查
	checkReference();
}

ResourceReferenceCheckWindow::~ResourceReferenceCheckWindow()
{

}

//  检查引用
void ResourceReferenceCheckWindow::checkReference(){

	references_.clear();
	if( res_ == NULL ){
		return;
	}

	ui->pathlineEdit->setText( res_->pathName());
	ui->typelineEdit->setText(resourceTypeToString( res_));

	// 判断资源类型
	if( UIsExactKindOf(SpriteResourceObject, res_) ){
		checkSpriteReference( res_ );
	}
	else if( UIsExactKindOf(ImageResourceObject, res_) ){
		checkImageReference( res_ );
	}

	//  显示数据
	foreach( ResourceObject* obj, references_){
		QListWidgetItem* item = new QListWidgetItem();
		item->setText( resourcetoString(obj) );
		item->setData( ITEM_HASHKEY, obj->hashKey());
		ui->loglistWidget->addItem( item );
	}
}

QString ResourceReferenceCheckWindow::resourceTypeToString(ResourceObject* obj){
	QString resTypeStr;

	if( UIsExactKindOf(SpriteResourceObject, obj) ){
		resTypeStr = UDQ_TR("精灵");
	}
	else if( UIsExactKindOf(ImageResourceObject, obj) ){
		resTypeStr = UDQ_TR("图片");
	}
	else{
		resTypeStr = UDQ_TR("未知类型资源");
	}
	return resTypeStr;
}


QString ResourceReferenceCheckWindow::resourcetoString(ResourceObject* obj){

	return QString(UDQ_TR("%1<%2>"))
		.arg( resourceTypeToString(obj))
		.arg( obj->pathName() );
}

void ResourceReferenceCheckWindow::checkSpriteReference( ResourceObject* sprite){
	Q_ASSERT( sprite != NULL );

}

void ResourceReferenceCheckWindow::checkImageReference( ResourceObject* img ){
	Q_ASSERT(img != NULL);

	// 检查态势模型( 仅当前工作空间 )
	for( TResourceObjectIterator<SpriteResourceObject> it; it; ++it ){
		SpriteResourceObject* sprite = UDynamicCast(SpriteResourceObject, *it);
		if (sprite->flags() & URF_TagGarbage)
			continue;

		// 遍历动画对象
		foreach(ResourceHash key, sprite->imgSources_.values()){
			if (key == img->hashKey()){
				references_.append(sprite);
				break;
			}
		}
	}

}


void ResourceReferenceCheckWindow::on_cancelpushButton_clicked(){
	reject();
}

void ResourceReferenceCheckWindow::on_savepushButton_clicked(){
	//  保存到TXT文件中
	QString filename = QFileDialog::getSaveFileName(this, UDQ_TR("保存TXT文件"), QDir::tempPath(), UDQ_TR("文本文件(*.txt)"));
	if( filename.isEmpty() )
		return;

	QFile file( filename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream out(&file);
	for( int i=0; i<ui->loglistWidget->count(); ++i){
		out<<ui->loglistWidget->item(i)->text();
		out<<UDQ_T("\n");
	}

	file.close();
}