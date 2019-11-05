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

    DEFINES += NEZHA_SFP=1

#include("../jklib/jklib.pri")
include("vopui.pri")
SOURCES += \
    uiconfig.cpp

HEADERS += \
    uiconfig.h

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    jsonparser/json_parser.cc \
    jsonparser/json_scanner.cc \
    jsonparser/json_scanner.cpp \
    jsonparser/parser.cpp \
    jsonparser/parserrunnable.cpp \
    jsonparser/qobjecthelper.cpp \
    jsonparser/serializer.cpp \
    jsonparser/serializerrunnable.cpp \
    membercenter/about.cpp \
    membercenter/experiencepro.cpp \
    tabstackedwidget.cpp \
    settingsstackedwidget.cpp \
    thumbnailimage.cpp \
    thumbnailview.cpp \
    membercenterwidget.cpp \
    membercenter/userlogin.cpp \
    wlantitlecell.cpp \
    busyrefreshlabel.cpp \
    imagepreviewdialog.cpp \
    moresettingsforscan.cpp \
    moresettingsforcopy.cpp \
    settingforipv6.cpp \
    authenticationdlg.cpp \
    animationdlg.cpp \
    ipv6status.cpp \
    promptdialog.cpp \
    settingwarming.cpp \
    wifisettingcell.cpp \
    wifisettingwepcell.cpp \
    requestcrm.cpp

HEADERS += \
        mainwindow.h \
    jsonparser/FlexLexer.h \
    jsonparser/json_parser.hh \
    jsonparser/json_scanner.h \
    jsonparser/location.hh \
    jsonparser/parser.h \
    jsonparser/parser_p.h \
    jsonparser/parserrunnable.h \
    jsonparser/position.hh \
    jsonparser/qjson_debug.h \
    jsonparser/qjson_export.h \
    jsonparser/qobjecthelper.h \
    jsonparser/serializer.h \
    jsonparser/serializerrunnable.h \
    jsonparser/stack.hh \
    membercenter/about.h \
    membercenter/experiencepro.h \
    tabstackedwidget.h \
    settingsstackedwidget.h \
    thumbnailimage.h \
    thumbnailview.h \
    membercenterwidget.h \
    membercenter/userlogin.h \
    wlantitlecell.h \
    busyrefreshlabel.h \
    typedefine.h \
    imagepreviewdialog.h \
    moresettingsforscan.h \
    moresettingsforcopy.h \
    settingforipv6.h \
    authenticationdlg.h \
    animationdlg.h \
    ipv6status.h \
    promptdialog.h \
    settingwarming.h \
    wifisettingcell.h \
    wifisettingwepcell.h \
    requestcrm.h

FORMS += \
        mainwindow.ui \
    membercenter/about.ui \
    membercenter/experiencepro.ui \
    tabstackedwidget.ui \
    settingsstackedwidget.ui \
    thumbnailimage.ui \
    membercenterwidget.ui \
    membercenter/userlogin.ui \
    wlantitlecell.ui \
    imagepreviewdialog.ui \
    moresettingsforscan.ui \
    moresettingsforcopy.ui \
    settingforipv6.ui \
    authenticationdlg.ui \
    animationdlg.ui \
    ipv6status.ui \
    promptdialog.ui \
    settingwarming.ui \
    wifisettingcell.ui \
    wifisettingwepcell.ui


RESOURCES += \
    resource.qrc \
    translation.qrc

DISTFILES += \
    MemberCenter/merchantJson \
    MemberCenter/StationJson \
    jsonparser/CMakeLists.txt \
    jsonparser/json_parser.yy \
    jsonparser/json_scanner.yy \
    translations/vop.en.qm \
    translations/vop.zh_CN.qm \


TRANSLATIONS = translations/vop.en.ts  \
                translations/vop.zh_CN.ts

equals(QT_MAJOR_VERSION,4){
contains(CONFIG ,static){
    QTPLUGIN += qjpeg qtiff qmng qgif qico
    DEFINES += STATIC_BUILD
    LIBS += -Wl,--wrap=memcpy
}
}

INCLUDEPATH += \
            $${PWD}/../libs/libtiff

!contains(CONFIG ,static){
LIBS += \
    -ltiff \
    -ljpeg \

    mac{
        LIBS += \
#                $${PWD}/../libs/mac/libtiff.a \
                -lz \

    }else{
        unix{
            contains(QT_ARCH, i386) {
#                LIBS += \
#                    $${PWD}/../libs/linux32/libtiff.a

            }else{
#                LIBS += \
#                    $${PWD}/../libs/linux64/libtiff.a \
#                    -lz \
            }
        }
    }
}

CONFIG(debug ,debug|release){
    DEFINES += LOG_TO_STDERR
    DEFINES += DEBUG_DEBUG
}else{
}
