INCLUDEPATH += $${PWD}

HEADERS  += \
    $$PWD/pro_config.h \

LIB_CLDIR=lntgr3zcl
LIB_LLDDIR=$${OUT_PWD}/../liblntgr3z
TARGET_filterlib=lntgr3z
TARGET_filterservice=lntgr3zservice
TARGET_filtertrans=lntgr3zsm
TARGET_stmui=lntgr3zsmui
TARGET_vop=lntgr3zvop

PATH_common = $$PWD/../common
DEFINES += SUNWUKONG=1

CONFIG(debug ,debug|release){
#    DEFINES += LOG_TO_STDERR
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

#rm -rf lntgr3z/liblntgr3z/lib*.dylib
#make
#install_name_tool -add_rpath %{sourceDir}/common/libs/mac -change ntdcmsmac %{sourceDir}/common/libs/mac/liblntgr3zcl.dylib lntgr3z/liblntgr3z/liblntgr3z.dylib

#install_name_tool -add_rpath `pwd`/../newcode/common/libs/mac -change ntdcmsmac `pwd`/../newcode/common/libs/mac/liblntgr3zcl.dylib lntgr3z/liblntgr3z/liblntgr3z.dylib
