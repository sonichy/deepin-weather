QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = deepin-weather
TEMPLATE = app

PKGCONFIG += dtkwidget
CONFIG += c++11 link_pkgconfig

SOURCES += \
        main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

RESOURCES += \
    res.qrc
