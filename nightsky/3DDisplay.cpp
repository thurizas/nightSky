
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QKeyEvent>


#include "OGLWidget.h"
#include "3DDisplay.h"



C3DDisplay::C3DDisplay(QWidget* parent) : QWidget(parent)
{
    setupUi();

    connect(m_pDisplay, SIGNAL(xRotationChanged(int)), this->m_rotXSlider, SLOT(setValue(int)));
    connect(m_pDisplay, SIGNAL(yRotationChanged(int)), this->m_rotYSlider, SLOT(setValue(int)));
    connect(m_pDisplay, SIGNAL(zRotationChanged(int)), this->m_rotZSlider, SLOT(setValue(int)));

}



C3DDisplay::~C3DDisplay()
{

}



void C3DDisplay::keyPressEvent(QKeyEvent* evt)
{
    if(evt->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(evt);
}



void C3DDisplay::setupUi()
{

    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QSlider *rotZSlider;


    if(this->objectName().isEmpty())
        this->setObjectName(QStringLiteral("3DDisplay"));

    this->resize(400, 500);

    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(6);
    verticalLayout->setContentsMargins(11, 11, 11, 11);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

    m_pDisplay = new COGLWidget(this);
    m_pDisplay->setObjectName(QStringLiteral("oglDisplay"));
    m_pDisplay->setMinimumSize(QSize(350, 350));

    verticalLayout->addWidget(m_pDisplay);

    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
    
    QLabel* label = new QLabel(this);
    label->setObjectName(QStringLiteral("label"));
    label->setText("xRot:");


    horizontalLayout->addWidget(label);

    m_rotXSlider = new QSlider(this);
    m_rotXSlider->setObjectName(QStringLiteral("rotXSlider"));
    m_rotXSlider->setMaximum(360);
    m_rotXSlider->setSingleStep(18);
    m_rotXSlider->setPageStep(18);
    m_rotXSlider->setOrientation(Qt::Horizontal);
    m_rotXSlider->setTickPosition(QSlider::TicksAbove);
    m_rotXSlider->setTickInterval(18);

    horizontalLayout->addWidget(m_rotXSlider);
    verticalLayout->addLayout(horizontalLayout);

    QHBoxLayout* horizontalLayout2 = new QHBoxLayout();
    horizontalLayout2->setSpacing(6);
    horizontalLayout2->setObjectName(QStringLiteral("horizontalLayout_2"));
        
    QLabel* label2 = new QLabel(this);
    label2->setObjectName(QStringLiteral("label_2"));
    label2->setText("YRot:");

    horizontalLayout2->addWidget(label2);

    m_rotYSlider = new QSlider(this);
    m_rotYSlider->setObjectName(QStringLiteral("rotYSlider"));
    m_rotYSlider->setMaximum(360);
    m_rotYSlider->setSingleStep(18);
    m_rotYSlider->setPageStep(18);
    m_rotYSlider->setOrientation(Qt::Horizontal);
    m_rotYSlider->setTickPosition(QSlider::TicksAbove);
    m_rotYSlider->setTickInterval(18);

    horizontalLayout2->addWidget(m_rotYSlider);
    verticalLayout->addLayout(horizontalLayout2);

    QHBoxLayout* horizontalLayout3 = new QHBoxLayout();
    horizontalLayout3->setSpacing(6);
    horizontalLayout3->setObjectName(QStringLiteral("horizontalLayout_3"));
    QLabel* label3 = new QLabel(this);
    label3->setObjectName(QStringLiteral("label_3"));
    label3->setText("ZRot:");

    horizontalLayout3->addWidget(label3);

    m_rotZSlider = new QSlider(this);
    m_rotZSlider->setObjectName(QStringLiteral("rotZSlider"));
    m_rotZSlider->setMaximum(360);
    m_rotZSlider->setSingleStep(18);
    m_rotZSlider->setPageStep(18);
    m_rotZSlider->setOrientation(Qt::Horizontal);
    m_rotZSlider->setTickPosition(QSlider::TicksAbove);
    m_rotZSlider->setTickInterval(18);

    horizontalLayout3->addWidget(m_rotZSlider);
    verticalLayout->addLayout(horizontalLayout3);

    QObject::connect(m_rotXSlider, SIGNAL(valueChanged(int)), m_pDisplay, SLOT(setXRotation(int)));
    QObject::connect(m_rotYSlider, SIGNAL(valueChanged(int)), m_pDisplay, SLOT(setYRotation(int)));
    QObject::connect(m_rotZSlider, SIGNAL(valueChanged(int)), m_pDisplay, SLOT(setZRotation(int)));

}