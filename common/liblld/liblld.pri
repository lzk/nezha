include("../lld/lld.pri")

contains(CONFIG ,static){
equals(QT_MAJOR_VERSION,4){
    QTPLUGIN += qjpeg qtiff qmng qgif qico
}else{
#    QTPLUGIN += qtiff qico
}
    DEFINES += STATIC_BUILD
#    LIBS += -Wl,--wrap=memcpy
}else{
LIBS += \
    -ljpeg \
}

CONFIG(debug ,debug|release){
#    DEFINES -= LOG_TO_STDERR
}else{
}
