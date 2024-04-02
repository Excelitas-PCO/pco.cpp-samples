QT += core gui widgets

CONFIG += c++17

INSTALLS += target

RESOURCES += \
    ImageViewer.qrc

HEADERS += \
ImageViewer.h \
    ../../pco.camera/cameraexception.h \
    ../../pco.camera/camera.h \
    ../../pco.camera/stdafx.h \
    ../../pco.camera/image.h \
    ../../pco.camera/defs.h

SOURCES += \
    ../../pco.camera/cameraexception.cpp \
    ../../pco.camera/camera.cpp \
    ../../pco.camera/image.cpp \
    ImageViewer.cpp \
    main.cpp

LIBS += -L$$PWD/../../lib/ -lpco_recorder
LIBS += -L$$PWD/../../lib/ -lpco_sc2cam
LIBS += -L$$PWD/../../lib/ -lpco_convert

unix: QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/../../lib\'"
unix: QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
unix: QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/../lib\'"
unix: QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/../lib/qt/lib\'"
unix: QMAKE_LFLAGS += "-Wl,-z,origin"

unix: DEFINES += PCO_LINUX
INCLUDEPATH += $$PWD/../../include

win32: DEPENDPATH += $$PWD/../../bin
unix: DEPENDPATH += $$PWD/../../lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/lib
else: unix:!android: target.path = /opt/$${TARGET}/lib
!isEmpty(target.path): INSTALLS += target
