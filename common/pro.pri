INCLUDEPATH += $${PWD}

HEADERS  += \
    $$PWD/pro_config.h \

LIB_CLDIR=lntgr1zcl
LIB_LLDDIR=$${OUT_PWD}/../liblld
TARGET_filterlib=lntgr1z
TARGET_filterservice=lntgr1zservice
TARGET_filtertrans=lntgr1zsm
TARGET_stmui=lntgr1zsmui
TARGET_vop=lntgr1zvop

PATH_common = $$PWD/../common
DEFINES += NEZHA=1
DEFINES += NEZHA_SFP=1

CONFIG(debug ,debug|release){
#    DEFINES += LOG_TO_STDERR
    DEFINES += DEBUG_DEBUG
}else{
    DEFINES += LENOVO_GUOCHANHUA=1
}

COMMON_LIBS=-L$${LIB_LLDDIR} -l$${TARGET_filterlib}
