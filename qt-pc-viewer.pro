QT       += core widgets gui

CONFIG += c++latest
CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -O0
    TARGET = qt-pc-viewer_debug
} else {
    QMAKE_CXXFLAGS += -Ofast
    TARGET = qt-pc-viewer
}

DESTDIR = $$PWD/bin

TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/viewerwindow.cpp \
    src/common/openglwindow.cpp \
    src/common/renderingdialog.cpp \
    src/common/tinyply.cpp \
    src/gl/glbasisobject.cpp \
    src/gl/glpointcloudobject.cpp \
    src/gl/glpointobject.cpp \
    src/gl/glcameraobject.cpp \
    src/gl/glgroundgridobject.cpp \

HEADERS += \
    include/common/tinycolormap.hpp \
    include/common/tinyply.h \
    include/mainwindow.h \
    include/viewerwindow.h \
    include/common/pointcloudcontroldialog.h \
    include/common/plyloader.h \
    include/common/renderingdialog.h \
    include/common/openglwindow.h \
    include/common/graphics_math.hpp \
    include/common/opengl_helper.hpp \
    include/common/camera.h \
#    include/common/viewcontroller.h \
#    include/common/arcballviewcontroller.h \
#    include/common/fpsviewcontroller.h \
#    include/common/targetballviewcontroller.h \
    include/gl/glbasisobject.h \
    include/gl/glpointcloudobject.h \
    include/gl/glpointobject.h \
    include/gl/glcameraobject.h \
    include/gl/glgroundgridobject.h \

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/include/gl
INCLUDEPATH += $$PWD/include/common
