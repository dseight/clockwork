TEMPLATE = lib
TARGET = clockwork

QT -= gui
QT += qml quick svg

CONFIG += c++14
CONFIG += hide_symbols
CONFIG += link_pkgconfig
PKGCONFIG += mlite5
PKGCONFIG += sailfishsilica
PKGCONFIG += glib-2.0

DEFINES += CLOCKWORK_BUILD_LIBRARY

SOURCES += \
    desktopentry.cpp \
    dynamicicon.cpp \
    iconpack.cpp \
    iconpackfactory.cpp \
    iconpackupdater.cpp \
    iconprovider.cpp \
    iconupdater.cpp \
    svgiconrender.cpp

PUBLICHEADERS += \
    clockworkglobal.h \
    dynamicicon.h \
    iconprovider.h \
    iconupdater.h \
    svgiconrender.h

HEADERS += \
    $$PUBLICHEADERS \
    clockworkglobal_p.h \
    desktopentry.h \
    dynamicicon_p.h \
    iconpack.h \
    iconpack_p.h \
    iconpackfactory.h \
    iconpackupdater.h \
    iconupdater_p.h

target.path = $$[QT_INSTALL_LIBS]

publicheaderfiles.files = $$PUBLICHEADERS
publicheaderfiles.path = /usr/include/clockwork

CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_NAME = lib$$TARGET
QMAKE_PKGCONFIG_VERSION = $$VERSION
QMAKE_PKGCONFIG_DESCRIPTION = Library for creating Clockwork modules
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$publicheaderfiles.path
QMAKE_PKGCONFIG_DESTDIR = pkgconfig

pkgconfig.files = $$TARGET.pc
pkgconfig.path = $$target.path/pkgconfig

INSTALLS += target publicheaderfiles pkgconfig
