QT -= gui

CONFIG += c++1z console conan_basic_setup
CONFIG -= app_bundle

QMAKE_CXXFLAGS=-D_GLIBCXX_USE_CXX11_ABI=0

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dht_decoder_t.cpp \
        main_t.cpp \
    dht_decoder.cpp \
utility.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include(conanbuildinfo.pri)

HEADERS += \
    dht_decoder.hpp \
    decoder.hpp \
    context.hpp \
utility.hpp

