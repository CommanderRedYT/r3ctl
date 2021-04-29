QT = core gui widgets network websockets multimedia

CONFIG += c++latest

DEFINES += QT_DEPRECATED_WARNINGS QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
        bpmdetector.cpp \
        main.cpp \
        mainwindow.cpp \
        r3client.cpp

HEADERS += \
    audioformat.h \
    bpmdetector.h \
    mainwindow.h \
    r3client.h

FORMS += \
    mainwindow.ui
