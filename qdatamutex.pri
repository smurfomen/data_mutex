HEADERS += \
        $$PWD/src/qdatamutex.h

INCLUDEPATH += $$PWD/src/

CONFIG(QOPTION_INCLUDED) {
        message("QOption already included")
        INCLUDEPATH += $$PWD/QOption
}
