#-------------------------------------------------
#
# Project created by QtCreator 2018-07-19T13:44:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include("../pro.pri")
TARGET = $${TARGET_stmui}
INCLUDEPATH += $${PATH_common}/stmui/


TEMPLATE = app

DEFINES += "HORNET=1"

include("../../common/jkinterface/jkinterface.pri")
include("../../common/statusmonitor/statusmonitor.pri")
include(../../common/common/common.pri)
include(../../common/stmui/jklib/jklib.pri)
include(../../common/lshell/lshell.pri)

SOURCES += \
        ../../common/stmui/main.cpp \
        ../../common/stmui/mainwindow.cpp \
    ../../common/stmui/uiconfig.cpp \

HEADERS  += ../../common/stmui/mainwindow.h \
    ../../common/stmui/uiconfig.h \

FORMS    += ../../common/stmui/mainwindow.ui

RESOURCES += \
    ../../common/stmui/images.qrc


CONFIG(debug ,debug|release){
    DEFINES += LOG_TO_STDERR
    DEFINES += DEBUG_DEBUG
}else{
}
