QT -= gui

CONFIG += c++17 console conan_basic_setup
CONFIG -= app_bundle


QMAKE_CXXFLAGS=-D_GLIBCXX_USE_CXX11_ABI=0 -static-libstdc++

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(conanbuildinfo.pri)

SOURCES += \
    dqt_decoder_t.cpp \
    main_t.cpp \
    dht_decoder_t.cpp \
    sof0_decoder_t.cpp \
    dht_decoder.cpp \
    dqt_decoder.cpp \
    sof0_decoder.cpp \
    sos_decoder.cpp \
    sos_decoder_t.cpp \
    utility.cpp \
    utility_t.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    decoder.hpp \
    dht_decoder.hpp \
    sof0_decoder.hpp \
    context.hpp \
    sos_decoder.hpp \
    utility.hpp
