INCLUDEPATH += $${PWD} \
               $${PWD}/../jkinterface

LIBS += -L$${OUT_PWD}/../toec -ltjgd1z

LIBS += -L$${PWD}/../libs -lusb-1.0  -lcups

macx: LIBS += -L/Volumes/work/software/libusb

HEADERS += \
    $${PWD}/filter_check_finger.h \

SOURCES += \
