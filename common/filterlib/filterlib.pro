#-------------------------------------------------
#
# Project created by QtCreator 2018-11-05T13:51:53
#
#-------------------------------------------------

QT       -= gui
#QT       -= core gui

include("../pro.pri")
TARGET = $${TARGET_filterlib}
TEMPLATE = lib

CONFIG += shared

DEFINES += TOEC_LIBRARY

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
#include("../common/common.pri")
#include("../statusmonitor/statusmonitor.pri")
#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}

SOURCES += \
    filterlib.cpp \
    filtermanager.cpp


HEADERS += \
    filterlib.h \
    filterlibconfig.h \
    filtermanager.h


CONFIG(debug ,debug|release){
#    DEFINES += LOG_TO_STDERR
    DEFINES += DEBUG_DEBUG
}else{
    DEFINES += LOG_TO_STDERR
}
