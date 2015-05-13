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

#ifndef  ORION_KISMET_VIEW_H
#define  ORION_KISMET_VIEW_H

#include <QGraphicsView>

class KismetView : public QGraphicsView{
	Q_OBJECT

public:
	KismetView(QWidget * parent = 0);
	virtual ~KismetView();

	void setZoomStep( int step ){
		zoomStep_ = step;
	}

	int zoomStep(){
		return zoomStep_;
	}



	void zoomUP(int step = 1);
	void zoomDown(int step = 1);

protected:
	virtual void mouseMoveEvent( QMouseEvent *mouseEvent );
	virtual void wheelEvent ( QWheelEvent * event );
	virtual void keyPressEvent(QKeyEvent * event);
private:
	int zoomStep_;  // Ëõ·Å±ÈÀý
};

#endif
