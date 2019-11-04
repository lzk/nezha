#-------------------------------------------------
#
# Project created by QtCreator 2018-11-05T13:35:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

include("../pro.pri")
TARGET = $${TARGET_vop}
INCLUDEPATH += $${PATH_common}/vopui/
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

include("../../common/vopui/vopui.pri")
SOURCES += \
    uiconfig.cpp

HEADERS += \
    ../../common/vopui/uiconfig.h


SOURCES += \
        ../../common/vopui/main.cpp \
        ../../common/vopui/mainwindow.cpp \
    ../../common/vopui/jsonparser/json_parser.cc \
    ../../common/vopui/jsonparser/json_scanner.cc \
    ../../common/vopui/jsonparser/json_scanner.cpp \
    ../../common/vopui/jsonparser/parser.cpp \
    ../../common/vopui/jsonparser/parserrunnable.cpp \
    ../../common/vopui/jsonparser/qobjecthelper.cpp \
    ../../common/vopui/jsonparser/serializer.cpp \
    ../../common/vopui/jsonparser/serializerrunnable.cpp \
    ../../common/vopui/membercenter/about.cpp \
    ../../common/vopui/membercenter/experiencepro.cpp \
    ../../common/vopui/tabstackedwidget.cpp \
    ../../common/vopui/settingsstackedwidget.cpp \
    ../../common/vopui/thumbnailimage.cpp \
    ../../common/vopui/thumbnailview.cpp \
    ../../common/vopui/membercenterwidget.cpp \
    ../../common/vopui/membercenter/userlogin.cpp \
    ../../common/vopui/wlantitlecell.cpp \
    ../../common/vopui/busyrefreshlabel.cpp \
    ../../common/vopui/imagepreviewdialog.cpp \
    ../../common/vopui/moresettingsforscan.cpp \
    ../../common/vopui/moresettingsforcopy.cpp \
    ../../common/vopui/settingforipv6.cpp \
    ../../common/vopui/authenticationdlg.cpp \
    ../../common/vopui/animationdlg.cpp \
    ../../common/vopui/ipv6status.cpp \
    ../../common/vopui/promptdialog.cpp \
    ../../common/vopui/settingwarming.cpp \
    ../../common/vopui/wifisettingcell.cpp \
    ../../common/vopui/wifisettingwepcell.cpp \
    ../../common/vopui/requestcrm.cpp \

HEADERS += \
        ../../common/vopui/mainwindow.h \
    ../../common/vopui/jsonparser/FlexLexer.h \
    ../../common/vopui/jsonparser/json_parser.hh \
    ../../common/vopui/jsonparser/json_scanner.h \
    ../../common/vopui/jsonparser/location.hh \
    ../../common/vopui/jsonparser/parser.h \
    ../../common/vopui/jsonparser/parser_p.h \
    ../../common/vopui/jsonparser/parserrunnable.h \
    ../../common/vopui/jsonparser/position.hh \
    ../../common/vopui/jsonparser/qjson_debug.h \
    ../../common/vopui/jsonparser/qjson_export.h \
    ../../common/vopui/jsonparser/qobjecthelper.h \
    ../../common/vopui/jsonparser/serializer.h \
    ../../common/vopui/jsonparser/serializerrunnable.h \
    ../../common/vopui/jsonparser/stack.hh \
    ../../common/vopui/membercenter/about.h \
    ../../common/vopui/membercenter/experiencepro.h \
    ../../common/vopui/tabstackedwidget.h \
    ../../common/vopui/settingsstackedwidget.h \
    ../../common/vopui/thumbnailimage.h \
    ../../common/vopui/thumbnailview.h \
    ../../common/vopui/membercenterwidget.h \
    ../../common/vopui/membercenter/userlogin.h \
    ../../common/vopui/wlantitlecell.h \
    ../../common/vopui/busyrefreshlabel.h \
    ../../common/vopui/typedefine.h \
    ../../common/vopui/imagepreviewdialog.h \
    ../../common/vopui/moresettingsforscan.h \
    ../../common/vopui/moresettingsforcopy.h \
    ../../common/vopui/settingforipv6.h \
    ../../common/vopui/authenticationdlg.h \
    ../../common/vopui/animationdlg.h \
    ../../common/vopui/ipv6status.h \
    ../../common/vopui/promptdialog.h \
    ../../common/vopui/settingwarming.h \
    ../../common/vopui/wifisettingcell.h \
    ../../common/vopui/wifisettingwepcell.h \
    ../../common/vopui/requestcrm.h \

FORMS += \
        ../../common/vopui/mainwindow.ui \
    ../../common/vopui/membercenter/about.ui \
    ../../common/vopui/membercenter/experiencepro.ui \
    ../../common/vopui/tabstackedwidget.ui \
    ../../common/vopui/settingsstackedwidget.ui \
    ../../common/vopui/thumbnailimage.ui \
    ../../common/vopui/membercenterwidget.ui \
    ../../common/vopui/membercenter/userlogin.ui \
    ../../common/vopui/wlantitlecell.ui \
    ../../common/vopui/imagepreviewdialog.ui \
    ../../common/vopui/moresettingsforscan.ui \
    ../../common/vopui/moresettingsforcopy.ui \
    ../../common/vopui/settingforipv6.ui \
    ../../common/vopui/authenticationdlg.ui \
    ../../common/vopui/animationdlg.ui \
    ../../common/vopui/ipv6status.ui \
    ../../common/vopui/promptdialog.ui \
    ../../common/vopui/settingwarming.ui \
    ../../common/vopui/wifisettingcell.ui \
    ../../common/vopui/wifisettingwepcell.ui


RESOURCES += \
    ../../common/vopui/resource.qrc \
    ../../common/vopui/translation.qrc

TRANSLATIONS = ../../common/vopui/translations/vop.en.ts  \
                ../../common/vopui/translations/vop.zh_CN.ts

equals(QT_MAJOR_VERSION,4){
contains(CONFIG ,static){
    QTPLUGIN += qjpeg qtiff qmng qgif qico
    DEFINES += STATIC_BUILD
    LIBS += -Wl,--wrap=memcpy
}
}

INCLUDEPATH += \
            ../../common/libs/libtiff

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
