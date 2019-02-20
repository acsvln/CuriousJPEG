#-------------------------------------------------
#
# Project created by QtCreator 2014-08-20T19:18:21
#
#-------------------------------------------------

QT       -= gui
QT       -= core

TARGET = CuriousJPEG

CONFIG   -= app_bundle
CONFIG   += console

# C++17
CONFIG   += c++1z

TEMPLATE = app

SOURCES += main.cpp \
    processor.cpp \
    sof0_decoder.cpp \
    dqt_decoder.cpp

HEADERS += \
    context.hpp \
    decoder.hpp \
    processor.hpp \
    sof0_decoder.hpp \
    dqt_decoder.hpp \
    definition.hpp

RESOURCES += \
    Resources.qrc
