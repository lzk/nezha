#-------------------------------------------------
#
# Project created by QtCreator 2018-07-19T13:44:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

include("../pro.pri")
TARGET = $${TARGET_stmui}
LIBS += $${COMMON_LIBS}

include(../../common/stmui/stmui.pri)

INCLUDEPATH += \
            $${PWD} \
