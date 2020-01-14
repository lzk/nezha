
INCLUDEPATH += \
            $${PWD} \
            $$PWD/../../includes \
            $$PWD/../../includes/cups-2.2.8

HEADERS += \
    $${PWD}/usbapi_libusb.h \
    $${PWD}/api_libcups.h \
    $${PWD}/log.h \
    $${PWD}/trans.h \
    $$PWD/jkinterface.h \
    $$PWD/deviceio.h \
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
    $$PWD/deviceio.cpp \
    $$PWD/filelocker.cpp \
    $$PWD/trans_msg.cpp \
    $$PWD/trans_socket.cpp \
    $$PWD/trans_virtual.cpp \
    $$PWD/trans_file.cpp \
    $$PWD/trans_mem.cpp

LIBS += \
    -L$${PWD}/../../libs \
    -lusb-1.0 \
    -lcups \
