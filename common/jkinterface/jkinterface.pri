
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
    $$PWD/filelocker.h \
    $$PWD/trans_msg.h \
    $$PWD/trans_socket.h \
    $$PWD/trans_virtual.h \
    $$PWD/trans_file.h \
    $$PWD/trans_mem.h

SOURCES += \
    $${PWD}/usbapi_libusb.cpp \
    $${PWD}/testlibusb.c \
    $${PWD}/api_libcups.cpp \
    $${PWD}/log.cpp \
    $$PWD/jkconfig.cpp \
    $$PWD/deviceio.cpp \
    $$PWD/filelocker.cpp \
    $$PWD/trans_msg.cpp \
    $$PWD/trans_socket.cpp \
    $$PWD/trans_virtual.cpp \
    $$PWD/trans_file.cpp \
    $$PWD/trans_mem.cpp

macx:{
    DEFINES += JK_OS_MAC

LIBS += \
    -L/Volumes/work/software/libusb \
    -L$${PWD}/../libs/mac \
}else{

#contains(CONFIG ,static){

        contains(QT_ARCH, i386){
                LIBS += \
                    -L$${PWD}/../libs/linux32 \

        }else{
                LIBS += \
                    -L$${PWD}/../libs/linux64 \

        }
#}
}

LIBS += \
    -L$${PWD}/../libs \
    -lusb-1.0 \
    -lcups \
