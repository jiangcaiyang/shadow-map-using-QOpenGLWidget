#include <math.h>
#include "OpenGLWidget.h"
#include "Plane.h"

#define VERTEX_COUNT    6
#define CUBE_LENGTH     25.0
#define TEXTURE_UNIT    GL_TEXTURE0
#define SHADOW_TEXTURE_UNIT GL_TEXTURE1

QOpenGLShaderProgram* Plane::s_program = Q_NULLPTR;
int Plane::s_positionLoc,
Plane::s_normalLoc,
Plane::s_texCoordLoc,
Plane::s_modelMatrixLoc,
Plane::s_viewMatrixLoc,
Plane::s_projectionMatrixLoc,
Plane::s_lightViewMatrixLoc,
Plane::s_lightProjectionMatrixLoc,
Plane::s_lightPositionLoc,
Plane::s_modelViewNormalMatrixLoc,
Plane::s_shadowTypeLoc,
Plane::s_count = 0;

Plane::Plane( QMatrix4x4& modelMatrix,
                  const QString& fileName,
                  ShadowType shadowType ):
    m_modelMatrix( modelMatrix ),
    m_shadowType( shadowType ),
    m_vertexBuffer( QOpenGLBuffer::VertexBuffer ),
    m_texture( QOpenGLTexture::Target2D )
{
    initializeOpenGLFunctions( );

    // 根据创建的次数来创建着色器
    if ( s_count++ == 0 )
    {
        s_program = new QOpenGLShaderProgram;
        s_program->addShaderFromSourceFile( QOpenGLShader::Vertex,
                                            ":/Common.vert" );
        s_program->addShaderFromSourceFile( QOpenGLShader::Fragment,
                                            ":/Common.frag" );
        s_program->link( );
        s_program->bind( );
        s_positionLoc = s_program->attributeLocation( "position" );
        s_normalLoc = s_program->attributeLocation( "normal" );
        s_texCoordLoc = s_program->attributeLocation( "texCoord" );
        s_modelMatrixLoc = s_program->uniformLocation( "modelMatrix" );
        s_viewMatrixLoc = s_program->uniformLocation( "viewMatrix" );
        s_projectionMatrixLoc = s_program->uniformLocation( "projectionMatrix" );
        s_lightPositionLoc = s_program->uniformLocation( "lightPosition" );
        s_lightViewMatrixLoc = s_program->uniformLocation( "lightViewMatrix" );
        s_lightProjectionMatrixLoc =
                s_program->uniformLocation( "lightProjectionMatrix" );
        s_modelViewNormalMatrixLoc =
                s_program->uniformLocation( "modelViewNormalMatrix" );
        s_shadowTypeLoc = s_program->uniformLocation( "shadowType" );
        int textureLoc = s_program->uniformLocation( "texture" );
        int shadowLoc = s_program->uniformLocation( "shadowTexture" );
        s_program->setUniformValue( textureLoc,
                                    TEXTURE_UNIT - GL_TEXTURE0 );
        s_program->setUniformValue( shadowLoc,
                                    SHADOW_TEXTURE_UNIT - GL_TEXTURE0 );

        s_program->release( );
    }

    // 设置顶点坐标
    qreal semi = CUBE_LENGTH / 2.0;
    const QVector3D vertices[] =
    {
        QVector3D( semi, 0.0f, -semi ),
        QVector3D( semi, 0.0f, semi ),
        QVector3D( -semi, 0.0f, -semi ),
        QVector3D( -semi, 0.0f, semi )
    };

    const QVector2D texCoords[] =
    {
        QVector2D( 0.0, 0.0 ),
        QVector2D( 0.0, 1.0 ),
        QVector2D( 1.0, 0.0 ),
        QVector2D( 1.0, 1.0 )
    };

    m_vertices = new Vertex[VERTEX_COUNT];
    Vertex* v = m_vertices;

    v[0].set( vertices[2], QVector3D( 0.0f, 1.0f, 0.0f ), texCoords[1] );
    v[1].set( vertices[1], QVector3D( 0.0f, 1.0f, 0.0f ), texCoords[2] );
    v[2].set( vertices[0], QVector3D( 0.0f, 1.0f, 0.0f ), texCoords[0] );
    v[3].set( vertices[2], QVector3D( 0.0f, 1.0f, 0.0f ), texCoords[1] );
    v[4].set( vertices[3], QVector3D( 0.0f, 1.0f, 0.0f ), texCoords[3] );
    v[5].set( vertices[1], QVector3D( 0.0f, 1.0f, 0.0f ), texCoords[2] );

    m_vertexBuffer.setUsagePattern( QOpenGLBuffer::DynamicDraw );
    m_vertexBuffer.create( );
    m_vertexBuffer.bind( );
    m_vertexBuffer.allocate( v, VERTEX_COUNT * sizeof( Vertex ) );
    m_vertexBuffer.release( );

    // 设置纹理滤波
    m_texture.setMinificationFilter( QOpenGLTexture::LinearMipMapLinear );
    m_texture.setMagnificationFilter( QOpenGLTexture::Linear );
    QImage image( fileName );
    Q_ASSERT( !fileName.isNull( ) );
    m_texture.setData( image );
}

Plane::~Plane( void )
{
    m_vertexBuffer.destroy( );
    m_texture.destroy( );
    delete []m_vertices;
    if ( --s_count == 0 )
    {
        delete s_program;
    }
}

void Plane::render( OpenGLWidget* widget )
{
    s_program->bind( );
    m_vertexBuffer.bind( );

    // 绘制box
    int offset = 0;
    setVertexAttribute( s_positionLoc, GL_FLOAT, 3, offset );
    offset += 3 * sizeof( GLfloat );
    setVertexAttribute( s_normalLoc, GL_FLOAT, 3, offset );
    offset += 3 * sizeof( GLfloat );
    setVertexAttribute( s_texCoordLoc, GL_FLOAT, 2, offset );

    // 摄像机的MVP矩阵
    QMatrix4x4& viewMatrix = widget->viewMatrix( );
    s_program->setUniformValue( s_modelMatrixLoc, m_modelMatrix );
    s_program->setUniformValue( s_viewMatrixLoc, viewMatrix );
    s_program->setUniformValue( s_projectionMatrixLoc, widget->projectionMatrix( ) );
    s_program->setUniformValue( s_modelViewNormalMatrixLoc,
                                ( viewMatrix * m_modelMatrix ).normalMatrix( ) );

    // 是否启用实时阴影
    //s_program->setUniformValue( s_shadowTypeLoc, m_shadowType );

    m_texture.bind( );
    if ( m_shadowType != NoShadow )
    {
        s_program->setUniformValue( s_lightPositionLoc, widget->lightPosition( ) );
        s_program->setUniformValue( s_lightViewMatrixLoc, widget->lightViewMatrix( ) );
        s_program->setUniformValue( s_lightProjectionMatrixLoc, widget->lightProjectionMatrix( ) );

        glActiveTexture( SHADOW_TEXTURE_UNIT );
        glBindTexture( GL_TEXTURE_2D, widget->shadowTexture( ) );
        glDrawArrays( GL_TRIANGLES, 0, VERTEX_COUNT );
        glBindTexture( GL_TEXTURE_2D, 0 );
        glActiveTexture( TEXTURE_UNIT );
    }
    else
    {
        glDrawArrays( GL_TRIANGLES, 0, VERTEX_COUNT );
    }
    m_texture.release( );
    m_vertexBuffer.release( );

    s_program->release( );
}

void Plane::renderShadow( OpenGLWidget* widget )
{
    m_vertexBuffer.bind( );
    QOpenGLShaderProgram& depthProgram = widget->depthProgram( );
    depthProgram.enableAttributeArray( "position" );
    depthProgram.setAttributeBuffer(
                "position",             // 位置
                GL_FLOAT,               // 类型
                0,                      // 偏移
                3,                      // 元大小
                sizeof( Vertex ) );     // 迈

    depthProgram.setUniformValue( "modelMatrix", m_modelMatrix );
    glDrawArrays( GL_TRIANGLES, 0, VERTEX_COUNT );
    m_vertexBuffer.release( );
}

void Plane::setVertexAttribute( int attributeLocation,
                                  GLenum elementType,
                                  quint32 elementSize,
                                  quint32 offset )
{
    s_program->enableAttributeArray( attributeLocation );
    s_program->setAttributeBuffer( attributeLocation,      // 位置
                                   elementType,            // 类型
                                   offset,                 // 偏移
                                   elementSize,            // 元大小
                                   sizeof( Vertex ) );     // 迈
}

static void canonicalPosition( QVector3D& position )
{
    if ( !qFuzzyIsNull( position.x( ) ) )
        position.setX( position.x( ) / fabsf( position.x( ) ) );
    if ( !qFuzzyIsNull( position.y( ) ) )
        position.setY( position.y( ) / fabsf( position.y( ) ) );
    if ( !qFuzzyIsNull( position.z( ) ) )
        position.setZ( position.z( ) / fabsf( position.z( ) ) );
}

void Plane::resize( qreal length )
{
    qreal semi = length / 2.0;
    m_vertexBuffer.bind( );
    Vertex* v = (Vertex*)m_vertexBuffer.map( QOpenGLBuffer::WriteOnly );
    for ( int i = 0; i < VERTEX_COUNT; ++i )
    {
        canonicalPosition( v[i].position );
        v[i].position *= semi;
    }
    m_vertexBuffer.unmap( );
    m_vertexBuffer.release( );
}
