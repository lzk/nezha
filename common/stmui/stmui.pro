#-------------------------------------------------
#
# Project created by QtCreator 2019-06-25T09:21:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include("../pro.pri")
TARGET = $${TARGET_stmui}
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

include("../jkinterface/jkinterface.pri")
include("../statusmonitor/statusmonitor.pri")
include(../common/common.pri)
include(../lshell/lshell.pri)
include(jklib/jklib.pri)
SOURCES += \
    uiconfig.cpp

HEADERS += \
    uiconfig.h

SOURCES += \
        main.cpp \
        mainwindow.cpp \

HEADERS += \
        mainwindow.h \

FORMS += \
        mainwindow.ui

RESOURCES += \
    images.qrc

equals(QT_MAJOR_VERSION,4){
contains(CONFIG ,static){
    QTPLUGIN += qjpeg qtiff qmng qgif qico
    DEFINES += STATIC_BUILD
#    LIBS += -Wl,--wrap=memcpy
}
}

TRANSLATIONS = translations/vop.en.ts  \
                translations/vop.zh_CN.ts

CONFIG(debug ,debug|release){
    DEFINES += LOG_TO_STDERR
    DEFINES += DEBUG_DEBUG
}else{
}
