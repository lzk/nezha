INCLUDEPATH += $${PWD}

HEADERS  += \
    $$PWD/pro_config.h \

LIB_CLDIR=lntgr2zcl
LIB_LLDDIR=$${OUT_PWD}/../liblntgr2z -Wl,-rpath=$$PWD/../common/libs
TARGET_filterlib=lntgr2z
TARGET_filterservice=lntgr2zservice
TARGET_filtertrans=lntgr2zsm
TARGET_stmui=lntgr2zsmui
TARGET_vop=lntgr2zvop

PATH_common = $$PWD/../common
DEFINES += SUNWUKONG_SFP=1
DEFINES += NEZHA_SFP=1

CONFIG(debug ,debug|release){
    DEFINES += LOG_TO_STDERR
    DEFINES += DEBUG_DEBUG
}else{
    DEFINES += LENOVO_GUOCHANHUA=1
}

COMMON_LIBS=-L$${LIB_LLDDIR} -l$${TARGET_filterlib}
#rm suwukong/liblntgr2z/lib*.dylib
#make
#install_name_tool -add_rpath %{sourceDir}/common/libs/mac -change ntdcmsmac %{sourceDir}/common/libs/mac/liblntgr2zcl.dylib nezha/liblntgr2z/liblntgr2z.dylib

#install_name_tool -add_rpath `pwd`/../newcode/common/libs/mac -change ntdcmsmac `pwd`/../newcode/common/libs/mac/liblntgr2zcl.dylib nezha/liblntgr2z/liblntgr2z.dylib
