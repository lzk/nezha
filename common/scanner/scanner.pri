INCLUDEPATH += \
            $${PWD} \
            $${PWD}/../libs/libjpeg

HEADERS += \
    $$PWD/scanner.h \
    $$PWD/scannerapi.h \
    $$PWD/scannerapp.h \
    $$PWD/rawtobmp.h \
    $$PWD/imagetrans.h \
    $$PWD/imageinfo.h \
    $$PWD/ntdcmsapi.h \
    $$PWD/trans_jpg.h

SOURCES += \
    $$PWD/scanner.cpp \
    $$PWD/scannerapi.cpp \
    $$PWD/scannerapp.cpp \
    $$PWD/rawtobmp.cpp \
    $$PWD/imagetrans.cpp \
    $$PWD/ntdcmsapi.cpp \
    $$PWD/trans_jpg.cpp



