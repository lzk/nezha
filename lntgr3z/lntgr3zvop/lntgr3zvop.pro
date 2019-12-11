#-------------------------------------------------
#
# Project created by QtCreator 2018-11-05T13:35:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

include("../pro.pri")
TARGET = $${TARGET_vop}
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

#    DEFINES += NEZHA_SFP=1

include("../../common/vopui/vopui.pri")
LIBS += \
    -llntgr3zcl \

SOURCES += \
    ../../common/vopui/uiconfig.cpp

HEADERS += \
    ../../common/vopui/uiconfig.h

LIBS += \
#    $${PWD}/../libs \
    -llntgr3zcl \

INCLUDEPATH += \
            $${PWD} \
