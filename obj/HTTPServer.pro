QT += core network
QT -= gui

CONFIG += c++11

OBJECTS_DIR = ./
DESTDIR = ../bin
TARGET = httpserver.bin
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    ../src/main.cpp \
    ../src/httpserver.cpp \
    ../src/cronparser.cpp \
    ../src/datamanager.cpp \
    ../src/mytimer.cpp

HEADERS += \
    ../src/httpserver.h \
    ../src/cronparser.h \
    ../src/datamanager.h \
    ../src/mytimer.h

