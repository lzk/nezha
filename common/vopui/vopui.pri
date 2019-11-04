include("../jkinterface/jkinterface.pri")
include("../statusmonitor/statusmonitor.pri")
include(../common/common.pri)
include(../lshell/lshell.pri)
include(../scanner/scanner.pri)
include("jklib/jklib.pri")

LIBS += \
    -L$${PWD}/../libs \
    -lusb-1.0 \
    -lcups \
#    -llntgr1zcl \

!contains(CONFIG ,static){
LIBS += \
    -lnetsnmp
}

mac{

    DEFINES += JK_OS_MAC

LIBS += \
    -L/Volumes/work/software/libusb \
    -lnetsnmp \
    -L$${PWD}/../libs/mac \
    -ljpeg \

}else{
    unix{
        contains(QT_ARCH, i386){
                LIBS += \
                    -L$${PWD}/../libs/linux32 \
                    $${PWD}/../libs/linux32/libnetsnmp.a \

        }else{
            CONFIG(debug ,debug|release){
                LIBS += \
                    -lnetsnmp

            }else{
                LIBS += \
                    -L$${PWD}/../libs/linux64 \
                    $${PWD}/../libs/linux64/libnetsnmp.a \
            }

        }
    }
}

