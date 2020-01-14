#include(../filterlib/filterlib.pri)
include(../includes/liblld.pri)

INCLUDEPATH += $${PWD}
INCLUDEPATH += $${PWD}/../filterlib
SOURCES += \
        $${PWD}/main.cpp \
    $${PWD}/appserver.cpp \
    $${PWD}/watcher.cpp \
    $${PWD}/appconfig.cpp \
    $${PWD}/../lld/statusmonitor/statussaver.cpp \

HEADERS += \
    $${PWD}/appserver.h \
    $${PWD}/appconfig.h \
    $${PWD}/watcher.h \
    $${PWD}/../lld/statusmonitor/statussaver.h \

