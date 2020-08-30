TEMPLATE = lib
TARGET = default-dynamic-icons

QT += dbus

CONFIG += c++14
CONFIG += plugin
CONFIG += no_plugin_name_prefix
CONFIG += link_pkgconfig
PKGCONFIG += mlite5

INCLUDEPATH += ../lib
LIBS += -L../lib -lclockwork

ASSETS_PATH = /usr/share/clockwork/dynamic-icons/$$TARGET
DEFINES += ASSETS_PATH=\\\"$${ASSETS_PATH}\\\"

SOURCES += \
    calendar.cpp \
    clock.cpp \
    devicelockstatus.cpp

HEADERS += \
    devicelockstatus.h

target.path = /usr/share/clockwork/dynamic-icons

assets.path = $$ASSETS_PATH
assets.files = \
    assets/icon-launcher-calendar.svg \
    assets/icon-launcher-clock.svg

INSTALLS += target assets

OTHER_FILES += *.svg
