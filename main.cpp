#include <QApplication>
#include "OpenGLWidget.h"

int main( int argc, char** argv )
{
    QApplication app( argc, argv );

    QSurfaceFormat format;
    //format.setRenderableType( QSurfaceFormat::OpenGLES );
    format.setVersion(2, 0);
    format.setProfile( QSurfaceFormat::NoProfile );
    //QSurfaceFormat::setDefaultFormat( format );

    OpenGLWidget w;
    w.resize( 320, 480 );
    w.setFormat( format );
    w.show( );

    return app.exec( );
}


