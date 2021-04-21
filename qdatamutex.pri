HEADERS += \
        $$PWD/src/qdatamutex.h

INCLUDEPATH += $$PWD/src/

CONFIG(QOPTION_INCLUDED) {
        message("QOption already included")
} else {
        INCLUDEPATH += $$PWD/QOption
        DISTFILES += $$PWD/QOption/QOption
}

