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

#include "oriontreewidget.h"
#include "commondefine.h"

OrionTreeWidget::OrionTreeWidget(QWidget *parent) : QTreeWidget(parent){

	branchCloseImage_ = QImage((UDQ_T(":/images/add.png"))).scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	branchOpenImage_ = QImage((UDQ_T(":/images/delete.png"))).scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

OrionTreeWidget::~OrionTreeWidget(){

}

void OrionTreeWidget::drawBranches(QPainter* painter, const QRect& rect, const QModelIndex& index) const{
	QTreeWidgetItem* item = itemFromIndex(index);
	QImage img = item->isExpanded() ? branchOpenImage_ : branchCloseImage_;
	if (item->childCount() > 0){
		// 在右边居中绘制（不进行缩放）
		int offx = rect.right() - img.width();
		int offy = rect.top() + (rect.height() - img.height()) / 2;
		painter->drawImage(offx, offy, img);
	}

}

QTreeWidgetItem* OrionTreeWidget::findTopLevelItem(const QString& name,  int column) const{
	QTreeWidgetItem* result = NULL;
	for (int i = 0; i < topLevelItemCount(); i++){
		if (topLevelItem(i)->text(column) == name){
			result = topLevelItem(i);
			break;
		}
	}
	return result;
}