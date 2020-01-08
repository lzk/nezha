
QT += core network

INCLUDEPATH += \
            $${PWD} \
            $$PWD/../libs \
            $$PWD/../libs/cups-2.2.8

HEADERS += \
    $$PWD/serverthread.h \
    $$PWD/commonapi.h \
    $$PWD/usbio.h \
    $$PWD/netio.h \
    $$PWD/devicemanager.h \
    $$PWD/snmpapi.h

SOURCES += \
    $$PWD/serverthread.cpp \
    $$PWD/commonapi.cpp \
    $$PWD/usbio.cpp \
    $$PWD/netio.cpp \
    $$PWD/devicemanager.cpp \
    $$PWD/snmpapi.cpp

mac{
    SOURCES += \
    $$PWD/macapi.cpp

}else{
    SOURCES += \
    $$PWD/linuxapi.cpp

}

#contains(CONFIG ,static){
#        contains(QT_ARCH, i386){
#                LIBS += \
#                    $${PWD}/../libs/linux32/libnetsnmp.a \

#        }else{
#            CONFIG(debug ,debug|release){
#                LIBS += \
#                    -lnetsnmp

#            }else{
#                LIBS += \
#                    $${PWD}/../libs/linux64/libnetsnmp.a \
#            }

#        }
#}else{
LIBS += \
    -lnetsnmp \
#}
