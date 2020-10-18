#-------------------------------------------------
#
# Project created by QtCreator 2014-08-20T19:18:21
#
#-------------------------------------------------

QT       -= gui
QT       -= core

TARGET = CuriousJPEG

CONFIG   -= app_bundle
CONFIG   += console conan_basic_setup

# C++17
CONFIG   += c++1z

TEMPLATE = app

include(conanbuildinfo.pri)

SOURCES += main.cpp \
    bit_extractor.cpp \
    data_reader.cpp \
    decoder_base.cpp \
    decoder_processor.cpp \
    dht_decoder.cpp \
    dqt_decoder.cpp \
    exceptions.cpp \
    huffman_tree.cpp \
    idecoder.cpp \
    sof0_decoder.cpp \
    sos_decoder.cpp \
    utility.cpp

HEADERS += \
    bit_extractor.hpp \
    data_reader.hpp \
    decoder_base.hpp \
    decoder_context.hpp \
    decoder_processor.hpp \
    definition.hpp \
    dht_decoder.hpp \
    dqt_decoder.hpp \
    exceptions.hpp \
    huffman_tree.hpp \
    idecoder.hpp \
    sof0_decoder.hpp \
    sos_decoder.hpp \
    utility.hpp

DISTFILES += \
    conanfile.txt \
    .gitignore \
    README.md
