#-------------------------------------------------
#
# Project created by QtCreator 2020-03-04T18:45:06
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TerminalClient
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        src/main.cpp \
    src/leddev.cpp \
    src/mainform.cpp \
    src/mainwindow.cpp \
    src/devcontrol.cpp \
    src/test.pb.cc \
    src/database.cpp \
    src/setform.cpp \
    src/digitalclock.cpp \
    src/led.cpp \
    src/animationbutton.cpp

HEADERS += \
        include/mainwindow.h \
    include/leddev.h \
    include/mainform.h \
    include/devcontrol.h \
    include/test.pb.h \
    include/database.h \
    include/setform.h \
    include/digitalclock.h \
    include/led.h \
    include/animationbutton.h

FORMS += \
        ui/mainwindow.ui \
    ui/mainform.ui \
    ui/devcontrol.ui \
    ui/setform.ui \
    ui/led.ui
UI_DIR=./
INCLUDEPATH += /usr/local/include

#LIBS += -L /home/gec/protobuftoarm/libproto*.a
LIBS += -L /usr/local/lib/libproto*.a
#LIBS +=-L /usr/local/lib/libboost*.a -lboost_system #-lphtread

RESOURCES += \
    resource/file.qrc

DISTFILES +=




