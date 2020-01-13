include("../lld/lld.pri")

contains(CONFIG ,static){
    QTPLUGIN += qjpeg qtiff qmng qgif qico
    DEFINES += STATIC_BUILD
    LIBS += -Wl,--wrap=memcpy
}else{
LIBS += \
    -ljpeg \
}

CONFIG(debug ,debug|release){
#    DEFINES -= LOG_TO_STDERR
}else{
}
