QT += core
QT -= gui

CONFIG += c++11

TARGET = RND
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    RNDDataSource.cpp \
    RNDModel.cpp \
    RNDTheme.cpp \
    LinearRegression.cpp \
    LrVector.cpp

HEADERS += \
    RNDDataSource.h \
    RNDTheme.h \
    RNDModel.h \
    LinearRegression.h \
    LrVector.h

win32: LIBS += -L$$PWD/../gdal_2.1.1/lib/ -lgdal_i

INCLUDEPATH += $$PWD/../gdal_2.1.1/include
DEPENDPATH += $$PWD/../gdal_2.1.1/include
