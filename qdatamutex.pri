HEADERS += \
        $$PWD/src/qdatamutex.h

INCLUDEPATH += $$PWD/src/
INCLUDEPATH += $$PWD/

CONFIG(QOPTION_INCLUDED) {
        message("QOption already included")
} else {
        INCLUDEPATH += $$PWD/QOption
        DISTFILES += $$PWD/QOption/QOption
}

DISTFILES += \
        $$PWD/QDataMutex

