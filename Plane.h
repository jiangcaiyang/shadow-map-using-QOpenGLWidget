#ifndef MYPLANE_H
#define MYPLANE_H

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class OpenGLWidget;

class Plane: protected QOpenGLFunctions
{
    struct Vertex
    {
        void set( const QVector3D& _position, const QVector3D& _normal,
                  const QVector2D& _texCoord )
        {
            position = _position;
            normal = _normal;
            texCoord = _texCoord;
        }

        QVector3D               position;
        QVector3D               normal;
        QVector2D               texCoord;
    };
public:
    enum ShadowType
    {
        NoShadow = 0,// 以后依次递增
        SimpleShadow,
        PCFShadow
    };

    explicit Plane( QMatrix4x4& modelMatrix,
                     const QString& fileName,
                     ShadowType shadowType );
    ~Plane( void );
    void render( OpenGLWidget* widget );
    void renderShadow( OpenGLWidget* widget );
    void resize( qreal length );
    void setVertexAttribute( int attributeLocation,
                             GLenum elementType,
                             quint32 elementSize,
                             quint32 offset );
protected:
    QMatrix4x4&             m_modelMatrix;
    ShadowType              m_shadowType;
    QOpenGLBuffer           m_vertexBuffer;
    QOpenGLTexture          m_texture;
    Vertex*                 m_vertices;

    static QOpenGLShaderProgram* s_program;
    static int s_positionLoc, s_normalLoc,
    s_texCoordLoc, s_modelMatrixLoc,
    s_viewMatrixLoc, s_projectionMatrixLoc,
    s_lightViewMatrixLoc, s_lightProjectionMatrixLoc,
    s_lightPositionLoc, s_modelViewNormalMatrixLoc, s_shadowTypeLoc;
    static int              s_count;        // 计数
};


#endif // MYPLANE_H
