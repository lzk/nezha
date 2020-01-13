include(../includes/liblld.pri)
include(jklib/jklib.pri)

INCLUDEPATH += $${PWD}

SOURCES += \
        $${PWD}/main.cpp \
        $${PWD}/mainwindow.cpp \
    $$PWD/appconfig.cpp \

HEADERS  += $${PWD}/mainwindow.h \
    $$PWD/appconfig.h \

FORMS    += $${PWD}/mainwindow.ui

RESOURCES += \
    $${PWD}/images.qrc

TRANSLATIONS = $${PWD}/translations/vop.en.ts  \
                $${PWD}/translations/vop.zh_CN.ts
