include("jkinterface/jkinterface.pri")
include("statusmonitor/statusmonitor.pri")
include(common/common.pri)
include(lshell/lshell.pri)
include(scanner/scanner.pri)
include(filterlib/filterlib.pri)

INCLUDEPATH += $${PWD} \

SOURCES += \
    $$PWD/lld.cpp

HEADERS += \
    $$PWD/lld.h
