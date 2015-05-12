//
//  原作见codeproject.org，
//  地址：http://www.codeproject.com/Articles/822380/Shadow-Mapping-with-Android-OpenGL-ES
//  移植者：jiangcaiyang
//  博客地址：http://blog.csdn.net/gamesdev

#include <QMouseEvent>
#include <QOpenGLFramebufferObject>
#include "Cube.h"
#include "Plane.h"
#include "Cube.h"
#include "Plane.h"
#include "OpenGLWidget.h"

#undef near
#undef far

OpenGLWidget::OpenGLWidget( QOpenGLWidget* widget ):
    QOpenGLWidget( widget )
{
    m_shadowMapWidth = 1024;
    m_shadowMapHeight = 1024;
    m_lightPosition = QVector3D( 0.0f, 9.0f, 5.0f );
    m_rotationX = 0.0f;
    m_rotationY = 0.0f;
    m_depthOnly = false;
}

OpenGLWidget::~OpenGLWidget( void )
{

}

void OpenGLWidget::initializeGL( void )
{
    initializeOpenGLFunctions( );

    connect( context( ), SIGNAL( aboutToBeDestroyed( ) ),
             this, SLOT( release( ) ) );

    initialize( );
}

void OpenGLWidget::paintGL( void )
{
    // 临时测试的
    static bool runOnce = grubData( );
    Q_UNUSED( runOnce );

    if ( m_depthOnly )
    {
        renderShadow( );
    }
    else
    {
        renderToFBO( );
        render( );
    }
    animate( );
    update( );
}


void OpenGLWidget::resizeGL( int width, int height )
{
    // Adjust the viewport based on geometry changes,
    // such as screen rotation
    glViewport(0, 0, width, height );

    // Generate buffer where depth values are saved for shadow calculation

    float ratio = (float) width / height;

    m_projectionMatrix.setToIdentity( );
    m_lightProjectionMatrix.setToIdentity( );
    m_projectionMatrix.perspective(
                90.0f,
                ratio,
                1.0f,
                100.0f );
    m_lightProjectionMatrix.perspective(
                90.0f,
                ratio,
                1.0f,
                100.0f );
}

void OpenGLWidget::release( void )
{
    makeCurrent( );
    for ( int i = 0; i < CUBE_COUNT; ++i )
    {
        delete m_cubes[i];
    }
    delete m_plane;
}

void OpenGLWidget::createCube( int i,
                               const QVector3D& translation,
                               const QString& fileName,
                               qreal length )
{
    m_modelMatrices[i].setToIdentity( );
    m_modelMatrices[i].translate( translation );
    m_cubes[i] = new Cube( m_modelMatrices[i],
                             fileName,
                             Cube::SimpleShadow );
    m_cubes[i]->resize( length );
}

void OpenGLWidget::generateShadowFBO( void )
{
    QSize size( m_shadowMapWidth, m_shadowMapHeight );
    m_FBO = new QOpenGLFramebufferObject( size );
}

void OpenGLWidget::animate( void )
{
    // light rotates around Y axis in every 12 seconds
    long elapsedMilliSec = m_timer.elapsed();
    long rotationCounter = elapsedMilliSec % 12000L;

    float lightRotationDegree = (360.0f / 6000.0f) * ((int)rotationCounter);

    QMatrix4x4 rotationMatrix;

    rotationMatrix.setToIdentity( );

    rotationMatrix.rotate( lightRotationDegree, 0.0f, 1.0f, 0.0f );

    m_rotatedLightPosition = rotationMatrix * m_lightPosition;

    //Set view matrix from light source position
    m_lightViewMatrix.setToIdentity( );
    m_lightViewMatrix.lookAt(
                //lightX, lightY, lightZ,
                m_rotatedLightPosition,
                //lookX, lookY, lookZ,
                //look in direction -y
                QVector3D( 0, 0, 0 ),
                //mActualLightPosition[0], -mActualLightPosition[1], mActualLightPosition[2],
                //upX, upY, upZ
                //up vector in the direction of axisY
                QVector3D( 0, 1, 0 ) );

    // 最后一个矩形进行移动
    QMatrix4x4 cubeRotationX, cubeRotationY;
    cubeRotationX.rotate( m_rotationX, 0, 1.0, 0 );
    cubeRotationY.rotate( m_rotationY, 1.0, 0, 0 );
    m_modelMatrices[4] = cubeRotationX * cubeRotationY;
}

int OpenGLWidget::shadowTexture( void )
{
    return m_FBO->texture( );
}

void OpenGLWidget::mousePressEvent( QMouseEvent* event )
{
    if ( event->button( ) == Qt::RightButton )
    {
        m_depthOnly = !m_depthOnly;
        return;
    }

    m_lastRotationX = m_rotationX;
    m_lastRotationY = m_rotationY;
    m_lastPosition = event->pos( );
}

void OpenGLWidget::mouseMoveEvent( QMouseEvent* event )
{
    QPoint delta = event->pos( ) - m_lastPosition;
    m_rotationX = m_lastRotationX + delta.x( ) * 1.0f;
    m_rotationY = m_lastRotationY + delta.y( ) * 1.0f;
}

void OpenGLWidget::initialize( void )
{
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    m_viewMatrix.lookAt( //eyeX, eyeY, eyeZ,
                        QVector3D( 0, 4, -12 ),
                        //lookX, lookY, lookZ,
                        QVector3D( 0, 0, 0 ),
                        //upX, upY, upZ
                        QVector3D( 0, 1, 0 ) );

    // 创建深度的着色器
    m_depthProgram.addShaderFromSourceFile( QOpenGLShader::Vertex,
                                           ":/Depth.vert" );
    m_depthProgram.addShaderFromSourceFile( QOpenGLShader::Fragment,
                                           ":/Depth.frag" );
    m_depthProgram.link( );

    createCube( 0, QVector3D( -4.0f, -3.9f, 4.0f ), ":/biscuit.jpg", 2.0 );
    createCube( 1, QVector3D( 4.0f, -3.9f, 4.0f ), ":/wood.jpg", 2.0 );
    createCube( 2, QVector3D( 4.0f, -3.9f, -4.0f ), ":/spiral.jpg", 2.0 );
    createCube( 3, QVector3D( -4.0f, -3.9f, -4.0f ), ":/shining.jpg", 2.0 );
    createCube( 4, QVector3D( 0.0f, 0.0f, 0.0f ), ":/color_line.jpg", 3.0 );

    m_modelMatrix.setToIdentity( );
    m_modelMatrix.translate( 0.0f, -5.0f, 0.0f );
    m_plane = new Plane( m_modelMatrix, ":/color_line.jpg", Plane::SimpleShadow );
    m_plane->resize( 20.0f );

    generateShadowFBO( );
}

void OpenGLWidget::renderToFBO( void )
{
    // 首先渲染的是深度图
    m_FBO->bind( );
    glViewport( 0, 0, m_shadowMapWidth, m_shadowMapHeight );
    renderShadow( );
    m_FBO->bindDefault( );
    glViewport( 0, 0, width( ), height( ) );
}

void OpenGLWidget::renderShadow( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glCullFace( GL_FRONT );
    m_depthProgram.bind( );
    m_depthProgram.setUniformValue( "viewMatrix", m_lightViewMatrix );
    m_depthProgram.setUniformValue( "projectionMatrix", m_lightProjectionMatrix );

    for ( int i = 0; i < CUBE_COUNT; ++i )
    {
        m_cubes[i]->renderShadow( this );
    }
    m_plane->renderShadow( this );
    m_depthProgram.release( );
    glCullFace( GL_BACK );
}

void OpenGLWidget::render( void )
{
    // 接着渲染的是场景
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    for ( int i = 0; i < CUBE_COUNT; ++i )
    {
        m_cubes[i]->render( this );
    }
    m_plane->render( this );
}

#include <QDebug>
bool OpenGLWidget::grubData( void )// 截取数据的
{
    qDebug( ) << "plane's model matrix: ";
    qDebug( ) << m_modelMatrix;

    qDebug( ) << "camera view matrix: ";
    qDebug( ) << m_viewMatrix;

    qDebug( ) << "camera projection matrix: ";
    qDebug( ) << m_projectionMatrix;

    qDebug( ) << "light position: ";
    qDebug( ) << m_lightPosition;

    qDebug( ) << "light view matrix: ";
    qDebug( ) << m_lightViewMatrix;

    qDebug( ) << "light projection matrix: ";
    qDebug( ) << m_lightProjectionMatrix;

    return true;
}
