QT = core network websockets

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
        main.cpp \
        r3client.cpp

HEADERS += \
    r3client.h
