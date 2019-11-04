
INCLUDEPATH += \
            $${PWD} \
            $$PWD/../libs \
            $$PWD/../libs/cups-2.2.8

HEADERS += \
    $${PWD}/usbapi_libusb.h \
    $${PWD}/api_libcups.h \
    $${PWD}/log.h \
    $${PWD}/trans.h \
    $$PWD/jkinterface.h \
    $$PWD/deviceio.h \
    $$PWD/jkconfig.h \
    $$PWD/filelocker.h

SOURCES += \
    $${PWD}/usbapi_libusb.cpp \
    $${PWD}/testlibusb.c \
    $${PWD}/api_libcups.cpp \
    $${PWD}/log.cpp \
    $${PWD}/trans.cpp \
    $$PWD/jkconfig.cpp \
    $$PWD/deviceio.cpp \
    $$PWD/filelocker.cpp

macx:{
    DEFINES += JK_OS_MAC

LIBS += \
    -L/Volumes/work/software/libusb \
    -L$${PWD}/../libs/mac \
}

LIBS += \
    -L$${PWD}/../libs \
    -lusb-1.0 \
    -lcups \

contains(CONFIG ,static){

        contains(QT_ARCH, i386){
                LIBS += \
                    -L$${PWD}/../libs/linux32 \

        }else{
                LIBS += \
                    -L$${PWD}/../libs/linux64 \

        }
}

