#pragma once


#include <QGraphicsView>

class QWidget;
class QGraphicsScene;
class QContextMenuEvent;
class QMouseEvent;
class QWheelEvent;
class nightSky;

class starView : public QGraphicsView
{
	Q_OBJECT

public:
	starView(QWidget* parent = nullptr);
	starView(QGraphicsScene* scene, QWidget* parent = nullptr);
	~starView();

protected:
	void contextMenuEvent(QContextMenuEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);

private:
	QWidget*       m_pParent;
	nightSky*      m_pMainWnd;
};
