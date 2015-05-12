# 逐渐贴近Qt Quick的环境，首先解决的问题是模仿MMDScene里面的渲染步�?

QT += core gui widgets opengl

SOURCES += \
    main.cpp \
    OpenGLWidget.cpp \
    Cube.cpp \
    Plane.cpp

HEADERS += \
    OpenGLWidget.h \
    Cube.h \
    Plane.h

RESOURCES += \
    Shader.qrc \
    Image.qrc
