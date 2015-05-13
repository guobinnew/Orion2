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
#ifndef ORION_IMAGEVIEW_H
#define ORION_IMAGEVIEW_H

#include <QGraphicsView>

class ImageView : public QGraphicsView{
	Q_OBJECT

public:
	ImageView(QWidget * parent = 0);
	virtual ~ImageView();

protected:
	virtual void mouseMoveEvent( QMouseEvent *mouseEvent );
	virtual void wheelEvent ( QWheelEvent * event );

private:
	int zoomStep_;  // Ëõ·Å±ÈÀý
};

#endif
