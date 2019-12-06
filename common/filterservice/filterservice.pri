include(../filterlib/filterlib.pri)
include("../jkinterface/jkinterface.pri")
include(../common/common.pri)
include(../statusmonitor/statusmonitor.pri)

INCLUDEPATH += $${PWD}
SOURCES += \
        $${PWD}/main.cpp \
    $${PWD}/appserver.cpp \
    $${PWD}/watcher.cpp \

HEADERS += \
    $${PWD}/appserver.h \
    $${PWD}/appconfig.h \
    $${PWD}/watcher.h


CONFIG(debug ,debug|release){
    DEFINES += LOG_TO_STDERR
    DEFINES += DEBUG_DEBUG
}else{
}
