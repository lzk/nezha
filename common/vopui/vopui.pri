include("jklib/jklib.pri")
include(../includes/liblld.pri)

equals(QT_MAJOR_VERSION,4){
contains(CONFIG ,static){
    QTPLUGIN += qjpeg qtiff qmng qgif qico
    DEFINES += STATIC_BUILD
    LIBS += -Wl,--wrap=memcpy
}
}

INCLUDEPATH += \
            $${PWD} \
            $${PWD}/../includes/libtiff

LIBS += \
    -L$${PWD}/../libs \

mac{
LIBS += \
    -L$${PWD}/../libs/mac \
    -lz \

}
!contains(CONFIG ,static){
LIBS += \
    -ltiff \
    -ljpeg \
}

SOURCES += \
        $${PWD}/main.cpp \
        $${PWD}/mainwindow.cpp \
    $${PWD}/jsonparser/json_parser.cc \
    $${PWD}/jsonparser/json_scanner.cc \
    $${PWD}/jsonparser/json_scanner.cpp \
    $${PWD}/jsonparser/parser.cpp \
    $${PWD}/jsonparser/parserrunnable.cpp \
    $${PWD}/jsonparser/qobjecthelper.cpp \
    $${PWD}/jsonparser/serializer.cpp \
    $${PWD}/jsonparser/serializerrunnable.cpp \
    $${PWD}/membercenter/about.cpp \
    $${PWD}/membercenter/experiencepro.cpp \
    $${PWD}/tabstackedwidget.cpp \
    $${PWD}/settingsstackedwidget.cpp \
    $${PWD}/thumbnailimage.cpp \
    $${PWD}/thumbnailview.cpp \
    $${PWD}/membercenterwidget.cpp \
    $${PWD}/membercenter/userlogin.cpp \
    $${PWD}/wlantitlecell.cpp \
    $${PWD}/busyrefreshlabel.cpp \
    $${PWD}/imagepreviewdialog.cpp \
    $${PWD}/moresettingsforscan.cpp \
    $${PWD}/moresettingsforcopy.cpp \
    $${PWD}/settingforipv6.cpp \
    $${PWD}/authenticationdlg.cpp \
    $${PWD}/animationdlg.cpp \
    $${PWD}/ipv6status.cpp \
    $${PWD}/promptdialog.cpp \
    $${PWD}/settingwarming.cpp \
    $${PWD}/wifisettingcell.cpp \
    $${PWD}/wifisettingwepcell.cpp \
    $${PWD}/requestcrm.cpp \
    $${PWD}/appconfig.cpp \

HEADERS += \
        $${PWD}/mainwindow.h \
    $${PWD}/jsonparser/FlexLexer.h \
    $${PWD}/jsonparser/json_parser.hh \
    $${PWD}/jsonparser/json_scanner.h \
    $${PWD}/jsonparser/location.hh \
    $${PWD}/jsonparser/parser.h \
    $${PWD}/jsonparser/parser_p.h \
    $${PWD}/jsonparser/parserrunnable.h \
    $${PWD}/jsonparser/position.hh \
    $${PWD}/jsonparser/qjson_debug.h \
    $${PWD}/jsonparser/qjson_export.h \
    $${PWD}/jsonparser/qobjecthelper.h \
    $${PWD}/jsonparser/serializer.h \
    $${PWD}/jsonparser/serializerrunnable.h \
    $${PWD}/jsonparser/stack.hh \
    $${PWD}/membercenter/about.h \
    $${PWD}/membercenter/experiencepro.h \
    $${PWD}/tabstackedwidget.h \
    $${PWD}/settingsstackedwidget.h \
    $${PWD}/thumbnailimage.h \
    $${PWD}/thumbnailview.h \
    $${PWD}/membercenterwidget.h \
    $${PWD}/membercenter/userlogin.h \
    $${PWD}/wlantitlecell.h \
    $${PWD}/busyrefreshlabel.h \
    $${PWD}/typedefine.h \
    $${PWD}/imagepreviewdialog.h \
    $${PWD}/moresettingsforscan.h \
    $${PWD}/moresettingsforcopy.h \
    $${PWD}/settingforipv6.h \
    $${PWD}/authenticationdlg.h \
    $${PWD}/animationdlg.h \
    $${PWD}/ipv6status.h \
    $${PWD}/promptdialog.h \
    $${PWD}/settingwarming.h \
    $${PWD}/wifisettingcell.h \
    $${PWD}/wifisettingwepcell.h \
    $${PWD}/requestcrm.h \
    $${PWD}/appconfig.h \

FORMS += \
        $${PWD}/mainwindow.ui \
    $${PWD}/membercenter/about.ui \
    $${PWD}/membercenter/experiencepro.ui \
    $${PWD}/tabstackedwidget.ui \
    $${PWD}/settingsstackedwidget.ui \
    $${PWD}/thumbnailimage.ui \
    $${PWD}/membercenterwidget.ui \
    $${PWD}/membercenter/userlogin.ui \
    $${PWD}/wlantitlecell.ui \
    $${PWD}/imagepreviewdialog.ui \
    $${PWD}/moresettingsforscan.ui \
    $${PWD}/moresettingsforcopy.ui \
    $${PWD}/settingforipv6.ui \
    $${PWD}/authenticationdlg.ui \
    $${PWD}/animationdlg.ui \
    $${PWD}/ipv6status.ui \
    $${PWD}/promptdialog.ui \
    $${PWD}/settingwarming.ui \
    $${PWD}/wifisettingcell.ui \
    $${PWD}/wifisettingwepcell.ui


RESOURCES += \
    $${PWD}/resource.qrc \
    $${PWD}/translation.qrc

TRANSLATIONS = $${PWD}/translations/vop.en.ts  \
                $${PWD}/translations/vop.zh_CN.ts
