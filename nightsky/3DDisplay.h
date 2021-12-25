#ifndef _3DDisplay_h_
#define _3DDisplay_h_


#include <QWidget>

class COGLWidget;
class QSlider;



class C3DDisplay : public QWidget
{
    Q_OBJECT

public:
    C3DDisplay(QWidget* parent = 0);
    ~C3DDisplay();

protected:
    void keyPressEvent(QKeyEvent* event);

private:
    COGLWidget*    m_pDisplay;
    QSlider*       m_rotXSlider;
    QSlider*       m_rotYSlider;
    QSlider*       m_rotZSlider;

    void setupUi();

  

};

#endif