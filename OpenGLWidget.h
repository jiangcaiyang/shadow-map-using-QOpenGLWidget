#ifndef OPENGLWIDGET
#define OPENGLWIDGET

#include <QPoint>
#include <QMatrix4x4>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QElapsedTimer>

#define CUBE_COUNT     5

QT_BEGIN_NAMESPACE
class QOpenGLFramebufferObject;
QT_END_NAMESPACE

class Cube;
class Plane;

class OpenGLWidget: public QOpenGLWidget,
        protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLWidget( QOpenGLWidget* widget = Q_NULLPTR );
    ~OpenGLWidget( void );
    void generateShadowFBO( void );
    void initialize( void );
    void render( void );
    void animate( void );
    void renderToFBO( void );
    void renderShadow( void );

    inline QVector3D& lightPosition( void ) { return m_rotatedLightPosition; }
    inline QMatrix4x4& viewMatrix( void ) { return m_viewMatrix; }
    inline QMatrix4x4& projectionMatrix( void ) { return m_projectionMatrix; }
    inline QMatrix4x4& lightViewMatrix( void ) { return m_lightViewMatrix; }
    inline QMatrix4x4& lightProjectionMatrix( void ) { return m_lightProjectionMatrix; }
    inline QOpenGLShaderProgram& depthProgram( void ) { return m_depthProgram; }
    int shadowTexture( void );
protected:
    void initializeGL( void );
    void resizeGL( int width, int height );
    void paintGL( void );

    void mousePressEvent( QMouseEvent* event );
    void mouseMoveEvent( QMouseEvent* event );
protected slots:
    void release( void );
protected:
    void createCube( int i,
                     const QVector3D& translation,
                     const QString& fileName,
                     qreal length );
    bool grubData( void );

    QOpenGLShaderProgram m_depthProgram;
    QOpenGLFramebufferObject* m_FBO;
    QVector3D           m_rotatedLightPosition, m_lightPosition;
    QMatrix4x4          m_cubeRotation;
    QMatrix4x4          m_viewMatrix, m_projectionMatrix;
    QMatrix4x4          m_lightViewMatrix, m_lightProjectionMatrix;
    Cube*               m_cubes[CUBE_COUNT];
    QMatrix4x4          m_modelMatrices[CUBE_COUNT];
    Plane*              m_plane;
    QMatrix4x4          m_modelMatrix;
    int                 m_shadowMapWidth, m_shadowMapHeight;
    float               m_rotationX, m_rotationY;
    float               m_lastRotationX, m_lastRotationY;
    QElapsedTimer       m_timer;
    QPoint              m_lastPosition;
    bool                m_depthOnly;
};

#endif // OPENGLWIDGET

