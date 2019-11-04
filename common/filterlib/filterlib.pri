INCLUDEPATH += $${PWD}/../filterlib \
               $${PWD}/../jkinterface

HEADERS += \
    $${PWD}/filterlib.h \
    $${PWD}/filterlibconfig.h \
    $$PWD/filtercommon.h \
    $$PWD/filterjobhistory.h


SOURCES += \
    $$PWD/filtercommon.cpp \
    $$PWD/filterjobhistory.cpp
