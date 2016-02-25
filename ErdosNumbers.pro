#-------------------------------------------------
#
# Project created by QtCreator 2015-12-27T14:14:59
#
#-------------------------------------------------

QT       += core gui
QT       += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ErdosNumbers
TEMPLATE = app
CONFIG += c++11
QMAKE_CXXFLAGS_RELEASE += -O2


SOURCES += main.cpp\
        erdoswindow.cpp \
    authorsgraph.cpp

HEADERS  += erdoswindow.h \
    authorsgraph.h

FORMS    += erdoswindow.ui
