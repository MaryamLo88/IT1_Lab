QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bigtown.cpp \
    city.cpp \
    dialognewcity.cpp \
    dialognewstreet.cpp \
    dijkstra.cpp \
    main.cpp \
    mainwindow.cpp \
    map.cpp \
    mapiofileinput.cpp \
    mapionrw.cpp \
    motorway.cpp \
    smalltown.cpp \
    stateroad.cpp \
    street.cpp

HEADERS += \
    abstractmap.h \
    bigtown.h \
    city.h \
    dialognewcity.h \
    dialognewstreet.h \
    dijkstra.h \
    mainwindow.h \
    map.h \
    mapio.h \
    mapiofileinput.h \
    mapionrw.h \
    motorway.h \
    smalltown.h \
    stateroad.h \
    street.h

FORMS += \
    dialognewcity.ui \
    dialognewstreet.ui \
    mainwindow.ui

TRANSLATIONS += \
    StreetPlaner00_fa_IR.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
