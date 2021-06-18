#-------------------------------------------------
#
# Project created by QtCreator 2020-02-29T09:42:56
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myClient
TEMPLATE = app


SOURCES += src/main.cpp\
        src/eventcontrol.cpp \
    src/loginwin.cpp \
    src/mainwin.cpp \
    src/ledwin.cpp \
    src/dataanalyst.cpp \
    src/animationbutton.cpp \
    src/led.cpp \
    src/test.pb.cc

HEADERS  += include/eventcontrol.h \
    include/loginwin.h \
    include/mainwin.h \
    include/ledwin.h \
    include/dataanalyst.h \
    include/common.h \
    include/common.h \
    include/animationbutton.h \
    include/led.h \
    include/test.pb.h

FORMS    += ui/eventcontrol.ui \
    ui/loginwin.ui \
    ui/mainwin.ui \
    ui/ledwin.ui \
    ui/led.ui

#INCLUDEPATH += /usr/local/include
#INCLUDEPATH += C:/Users/lizhuocong/Desktop/share/backup/myClient
#LIBS += -L /home/gec/protobuftoarm/libproto*.a
#LIBS += -L /usr/local/lib/libproto*.a
#LIBS += -L C:/Qt/Qt5.7.0/5.7/mingw53_32/lib/libproto*.dll.a -lprotobuf
RESOURCES += \
    resource/file.qrc



win32: LIBS += -L$$PWD/lib/ -lprotobuf

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/protobuf.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/lib/libprotobuf.a
