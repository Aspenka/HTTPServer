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
    ../src/timer.cpp \
    ../src/datamanager.cpp

HEADERS += \
    ../src/httpserver.h \
    ../src/cronparser.h \
    ../src/timer.h \
    ../src/datamanager.h

