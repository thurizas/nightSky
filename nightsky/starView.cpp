
#include "starView.h"
#include "nighsky.h"
#include "logger.h"

#include <QWidget>
#include <QGraphicsScene>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

#include <iostream>


/**********************************************************************************************************************
* function  : 
*
* abstract  : 
*
* parameters: 
*
* retuns    : 
*
* written   : GKHuber Dec 2021
**********************************************************************************************************************/
starView::starView(QWidget* parent) : QGraphicsView(parent), m_pParent(parent)
{ 
	m_pMainWnd = reinterpret_cast<nightSky*>(m_pParent->parentWidget());
}


/**********************************************************************************************************************
* function  : 
*
* abstract  :
*
* parameters:
*
* retuns    :
*
* written   : GKHuber Dec 2021
**********************************************************************************************************************/
starView::starView(QGraphicsScene* scene, QWidget* parent) : QGraphicsView(scene, parent), m_pParent(parent)
{  
	m_pMainWnd = reinterpret_cast<nightSky*>(m_pParent->parentWidget());
}


/**********************************************************************************************************************
* function  :
*
* abstract  :
*
* parameters:
*
* retuns    :
*
* written   : GKHuber Dec 2021
**********************************************************************************************************************/
starView::~starView()
{

}


/**********************************************************************************************************************
* function  :
*
* abstract  :
*
* parameters:
*
* retuns    :
*
* written   : GKHuber Dec 2021
**********************************************************************************************************************/
void starView::contextMenuEvent(QContextMenuEvent*)
{
	qDebug() << "in context menu handler";
}


/**********************************************************************************************************************
* function  : wheelEvent
*
* abstract  : This handles operations with the mouse wheel.  An unmodified wheel event causes the immage to scroll
*             veritcally (the default behavior).  If the control (CTRL) button is held down, then the image is zoomed
*             either in or out based on the direction of the scroll wheel turning.  Finally, if the alternate key (ALT)
*             is held down we scroll either left or right.
*
* parameters:
*
* retuns    :
*
* written   : GKHuber Dec 2021
**********************************************************************************************************************/
void starView::wheelEvent(QWheelEvent* evt)
{
	float zoomFactor = 1.15;
	int   pixelsperclick = 4;

	float deltaX = evt->angleDelta().x();
	float deltaY = evt->angleDelta().y();

	if (Qt::AltModifier == evt->modifiers())
	{
		if (deltaX > 0) scroll(-pixelsperclick, 0);
		else scroll(pixelsperclick, 0);
	}
	else if (Qt::ControlModifier == evt->modifiers())                      // zooming in/out ...
	{
		if (deltaY > 0) scale(zoomFactor, zoomFactor);
		else scale(1 / zoomFactor, 1 / zoomFactor);
	}
	else
	{
		if (deltaY > 0) scroll(0, pixelsperclick);
		else scroll(0, -pixelsperclick);
	}

	evt->accept();
}


/**********************************************************************************************************************
* function  : mouseMoveEvent
*
* abstract  : By default this gets the current cursor location and shows the location in the status bar.
*             if the left-mouse button is held down, then we drag the map image.
*
* parameters:
*
* retuns    :
*
* written   : GKHuber Dec 2021
*             GKHuber Jan 2026 - modified to support dragging with left mouse button depressed
**********************************************************************************************************************/
void starView::mouseMoveEvent(QMouseEvent* evt)
{
	static bool bMoving = false;
	int32_t   transX = 0, transY = 0;
	int32_t   startX = 0, startY = 0;
	int32_t   stopX = 0, stopY = 0;

	const int margin = 20;                                  // margin width in pixels

	//qDebug() << QString("in mouse move event, location is: (%1,%2)").arg(evt->pos().x()).arg(evt->pos().y());

	if (((evt->buttons() & Qt::LeftButton) == Qt::LeftButton) && !bMoving)         // initialize the move logic...
	{
		bMoving = true;

		startX = mapFromGlobal(evt->screenPos()).x();
		startY = mapFromGlobal(evt->screenPos()).y();
	}

	if (((evt->buttons() & Qt::LeftButton) != Qt::LeftButton) && bMoving)
	{
		bMoving = false;

		stopX = mapFromGlobal(evt->screenPos()).x();
		stopY = mapFromGlobal(evt->screenPos()).y();
		
		transX = stopX - startX;
		transY = stopY - stopY;

		if (transX != 0 || transY != 0)
		{
			QRectF rect = sceneRect();
			rect.translate(QPointF{ (qreal)transX, (qreal)transY });
			setSceneRect(rect);
		}
	}

	QPointF   sceneLoc = mapToScene(evt->pos());

	QString strStatus = QString("cursor at %1, %2").arg(sceneLoc.x()).arg(sceneLoc.y());
	m_pMainWnd->statusBar()->showMessage(strStatus);

}

void starView::mousePressEvent(QMouseEvent* evt)
{
	if (evt->button() == Qt::LeftButton)
	{
		qDebug() << "left mouse pressed";
	}
	else if (evt->button() == Qt::MiddleButton)
	{
		qDebug() << "middle mouse pressed";
	}
	else if (evt->button() == Qt::RightButton)
	{
		qDebug() << "right mouse pressed";
	}
	else
	{
		CLogger::getInstance()->outMsg(cmdLine, CLogger::level::ERR, "unknown mouse button");
	}

	QGraphicsView::mousePressEvent(evt);
}

void starView::mouseReleaseEvent(QMouseEvent* evt)
{
	if (evt->button() == Qt::LeftButton)
	{
		qDebug() << "left mouse released";
	}
	else if (evt->button() == Qt::MiddleButton)
	{
		qDebug() << "middle mouse released";
	}
	else if(evt->button() == Qt::RightButton)
	{
		qDebug() << "right mouse released";
	}
	else
	{
		CLogger::getInstance()->outMsg(cmdLine, CLogger::level::ERR, "unknown mouse button");
	}

	QGraphicsView::mouseReleaseEvent(evt);
}
