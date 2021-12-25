
#include <QtWidgets>
#include <QtOpenGL>
#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>

#include <GL/include/glut.h>
#include "OGLWidget.h"
#include <math.h>

COGLWidget::COGLWidget(QWidget *parent)  : QOpenGLWidget(parent), m_xRot(0),  m_yRot(0), m_zRot(0), m_program(0)
{
    int ndx = 0;

    m_core = QCoreApplication::arguments().contains(QStringLiteral("--coreprofile"));
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.
    m_transparent = QCoreApplication::arguments().contains(QStringLiteral("--transparent"));
    if (m_transparent) 
    {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }
}

COGLWidget::~COGLWidget()
{
    cleanup();
}

QSize COGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize COGLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    qDebug() <<"angle is: " << angle;
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void COGLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        emit xRotationChanged(angle);
        update();
    }
}

void COGLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        emit yRotationChanged(angle);
        update();
    }
}

void COGLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        emit zRotationChanged(angle);
        update();
    }
}

void COGLWidget::cleanup()
{
    makeCurrent();
    //m_logoVbo.destroy();
    delete m_program;
    m_program = 0;
    doneCurrent();
}

static const char *vertexShaderSourceCore =
    "#version 150\n"
    "in vec4 vertex;\n"
    "in vec3 normal;\n"
    "out vec3 vert;\n"
    "out vec3 vertNormal;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vert = vertex.xyz;\n"
    "   vertNormal = normalMatrix * normal;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSourceCore =
    "#version 150\n"
    "in highp vec3 vert;\n"
    "in highp vec3 vertNormal;\n"
    "out highp vec4 fragColor;\n"
    "uniform highp vec3 lightPos;\n"
    "void main() {\n"
    "   highp vec3 L = normalize(lightPos - vert);\n"
    "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
    "   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
    "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
    "   fragColor = vec4(col, 1.0);\n"
    "}\n";

static const char *vertexShaderSource =
    "attribute vec4 vertex;\n"
    "attribute vec3 normal;\n"
    "varying vec3 vert;\n"
    "varying vec3 vertNormal;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vert = vertex.xyz;\n"
    "   vertNormal = normalMatrix * normal;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying highp vec3 vert;\n"
    "varying highp vec3 vertNormal;\n"
    "uniform highp vec3 lightPos;\n"
    "void main() {\n"
    "   highp vec3 L = normalize(lightPos - vert);\n"
    "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
    "   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
    "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
    "   gl_FragColor = vec4(col, 1.0);\n"
    "}\n";

void COGLWidget::initializeGL()
{
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &COGLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, m_transparent ? 0 : 1);

    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, m_core ? vertexShaderSourceCore : vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, m_core ? fragmentShaderSourceCore : fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");

    // Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
    // implementations this is optional and support may not be present
    // at all. Nonetheless the below code works in all cases and makes
    // sure there is a VAO when one is needed.
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Setup our vertex buffer object.
    //m_logoVbo.create();
    //m_logoVbo.bind();
    //m_logoVbo.allocate(m_logo.constData(), m_logo.count() * sizeof(GLfloat));

    // Store the vertex attribute bindings for the program.
    setupVertexAttribs();

    // Our camera never changes in this example.
    m_camera.setToIdentity();
    m_camera.translate(0, 0, -1);

    // Light position is fixed.
    m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 0, 70));

    m_program->release();
}



void COGLWidget::setupVertexAttribs()
{
    //m_logoVbo.bind();
    //QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    //f->glEnableVertexAttribArray(0);
    //f->glEnableVertexAttribArray(1);
    //f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    //f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    //m_logoVbo.release();
}



void COGLWidget::paintGL()
{
    int lats = 18;
    int longs = 36;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);

    m_world.setToIdentity();

    m_world.rotate(180.0f - (m_xRot), 1, 0, 0);
    m_world.rotate(m_yRot , 0, 1, 0);
    m_world.rotate(m_zRot , 0, 0, 1);


    drawWireFrameSphere(5, 17, 24);

    ////QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    ////m_program->bind();
    ////m_program->setUniformValue(m_projMatrixLoc, m_proj);
    ////m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
    ////QMatrix3x3 normalMatrix = m_world.normalMatrix();
    ////m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

    ////glDrawArrays(GL_TRIANGLES, 0, m_logo.vertexCount());
    ////m_program->release();
    //int i, j;
    //for(i = 0; i <= lats; i++) 
    //{
    //    double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
    //    double z0  = sin(lat0);
    //    double zr0 =  cos(lat0);
    //
    //    double lat1 = M_PI * (-0.5 + (double) i / lats);
    //    double z1 = sin(lat1);
    //    double zr1 = cos(lat1);
    //
    //    glBegin(GL_QUAD_STRIP);
    //    for(j = 0; j <= longs; j++) 
    //    {
    //        double lng = 2 * M_PI * (double) (j - 1) / longs;
    //        double x = cos(lng);
    //        double y = sin(lng);
    //
    //        glNormal3f(x * zr0, y * zr0, z0);
    //        glVertex3f(x * zr0, y * zr0, z0);
    //        glNormal3f(x * zr1, y * zr1, z1);
    //        glVertex3f(x * zr1, y * zr1, z1);
    //    }
    //    glEnd();
    //} 
 
}



void COGLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}



void COGLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}



void COGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) 
    {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) 
    {
        setXRotation(m_xRot + 8 * dy);
        setZRotation(m_zRot + 8 * dx);
    }
    m_lastPos = event->pos();
}



void COGLWidget::drawWireFrameSphere(int radius, int rings, int sectors)
{
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texcoords;
    std::vector<GLushort> indices;
    std::vector <GLushort> indexElements;


    //this->attachShader(_shader);
    //this->attachMaterial(_material);

    /** Implementation from:
    https://stackoverflow.com/questions/5988686/how-do-i-create-a-3d-sphere-in-opengl-using-visual-c?lq=1
    **/

    float const R = 1./(float)(rings-1);
    float const S = 1./(float)(sectors-1);
    int r, s;

    vertices.resize(rings * sectors * 3);
    texcoords.resize(rings * sectors * 2);
    std::vector<GLfloat>::iterator v = vertices.begin();
    std::vector<GLfloat>::iterator t = texcoords.begin();
    for(r = 0; r < rings; r++) 
    {
        for(s = 0; s < sectors; s++) 
        {
            float const y = sin( -M_PI/2 + M_PI * r * R );
            float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
            float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

            *t++ = s*S;
            *t++ = r*R;

            *v++ = x * radius;
            *v++ = y * radius;
            *v++ = z * radius;
        }
    }

    indices.resize(rings * sectors * 4);
    std::vector<GLushort>::iterator i = indices.begin();
    for(r = 0; r < rings-1; r++) 
    {
        for(s = 0; s < sectors-1; s++) 
        {
            *i++ = r * sectors + s;
            *i++ = r * sectors + (s+1);
            *i++ = (r+1) * sectors + (s+1);
            *i++ = (r+1) * sectors + s;
        }
    }


    //std::vector<GLfloat> vertices;

    glBegin (GL_TRIANGLE_STRIP);
    int cntPoints = vertices.size();
    for (int b = 0; b < cntPoints; b=b+3)
    {
        //Assigning the texture coordinates of the current vertex
        glTexCoord2f (vertices.at(b), vertices.at(b+1));
        //And the drawing the specified vertex with the Z coordinate inverted. Because our creation code only draws half a sphere, which is why I am also doing this loop again for the other half below.
        glVertex3f (vertices.at(b), vertices.at(b+1), -vertices.at(b+2));
    }
    glEnd();
}



//void COGLWidget::draw()
//{
    //GLfloat   myVar = 3.0;

    //qglColor(Qt::red);
    //
    //float color1[3] = {1.0,0.0,0.0};
    //float shininess = 64.0f;
    //float specularColor[] = {1.0, 1.0f, 1.0f, 1.0f};
   
    //glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess); // range 0 ~ 128
    //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);

    //glPushMatrix();
    //glTranslatef(1,1,1);
    //glColor3fv(color1);

    //GLUquadricObj* quadric;
    //quadric = gluNewQuadric();

    //gluQuadricDrawStyle(quadric, GLU_LINE);

    //gluSphere(quadric, 5, 30, 10);

 
    //glMatrixMode(GL_MODELVIEW);
    //glClear(GL_COLOR_BUFFER_BIT);
    //glLoadIdentity();
    //glTranslatef(0.0,0.0,-5.0);
    //glColor3f(0.9, 0.3, 0.2); 
    //glRotatef(/*xRotated*/0.2, 1.0, 0.0, 0.0);
    //glRotatef(/*yRotated*/ 0.0, 0.0, 1.0, 0.0);
    //glRotatef(/*zRotated*/0.0, 0.0, 0.0, 1.0);
    //glScalef(1.0,1.0,1.0);
    //glutWireSphere(/*radius*/3,20,20);
    //glFlush();    
//}
