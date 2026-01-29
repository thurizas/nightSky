
#include "starView.h"
#include "nighsky.h"

#include <QWidget>
#include <QGraphicsScene>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>


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

	float delta = evt->angleDelta().y();

	if (Qt::AltModifier == evt->modifiers())
	{
		if (delta > 0) scroll(-pixelsperclick, 0);
		else scroll(pixelsperclick, 0);
	}
	else if (Qt::ControlModifier == evt->modifiers())                      // zooming in/out ...
	{
		if (delta > 0) scale(zoomFactor, zoomFactor);
		else scale(1 / zoomFactor, 1 / zoomFactor);
	}
	else
	{
		if (delta > 0) scroll(0, pixelsperclick);
		else scroll(0, -pixelsperclick);
	}

	evt->accept();
}



void starView::mouseMoveEvent(QMouseEvent* evt)
{
	qDebug() << QString("in mouse move event, location is: (%1,%2)").arg(evt->pos().x()).arg(evt->pos().y());

	QPointF   sceneLoc = mapToScene(evt->pos());

	QString strStatus = QString("cursor at %1, %2").arg(sceneLoc.x()).arg(sceneLoc.y());
	m_pMainWnd->statusBar()->showMessage(strStatus);

}
