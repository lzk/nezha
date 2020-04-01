INCLUDEPATH += $${PWD}

HEADERS  += \
    $$PWD/pro_config.h \

LIB_CLDIR=lntgr1zcl
LIB_LLDDIR=$${OUT_PWD}/../liblntgr1z
TARGET_filterlib=lntgr1z
TARGET_filterservice=lntgr1zservice
TARGET_filtertrans=lntgr1zsm
TARGET_stmui=lntgr1zsmui
TARGET_vop=lntgr1zvop

PATH_common = $$PWD/../common
DEFINES += NEZHA=1
DEFINES += NEZHA_SFP=1

CONFIG(debug ,debug|release){
    DEFINES += LOG_TO_STDERR
    DEFINES += DEBUG_DEBUG
}else{
    DEFINES += LENOVO_GUOCHANHUA=1
}

COMMON_LIBS=-L$${LIB_LLDDIR} -l$${TARGET_filterlib}
#COMMON_LIBS += -L$$PWD/../common/libs/linux64 -lcrypto

contains(CONFIG ,static){
equals(QT_MAJOR_VERSION,4){
}else{
    COMMON_LIBS += -L$$PWD/../common/libs/linux64
}
}

#rm nezha/liblntgr1z/lib*.dylib
#make
#install_name_tool -add_rpath %{sourceDir}/common/libs/mac -change ntdcmsmac %{sourceDir}/common/libs/mac/liblntgr1zcl.dylib nezha/liblntgr1z/liblntgr1z.dylib

#install_name_tool -add_rpath `pwd`/../newcode/common/libs/mac -change ntdcmsmac `pwd`/../newcode/common/libs/mac/liblntgr1zcl.dylib nezha/liblntgr1z/liblntgr1z.dylib
