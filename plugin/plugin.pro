TEMPLATE = lib
TARGET = clockworksettingsplugin

MODULENAME = com/dseight/clockwork
TARGETPATH = $$[QT_INSTALL_QML]/$$MODULENAME

QT -= gui
QT += qml quick

CONFIG += c++14
CONFIG += plugin

INCLUDEPATH += ../lib
LIBS += -L../lib -lclockwork

DEFINES += VERSION_STRING=\\\"$${VERSION}\\\"

import.files = qmldir *.qml
import.path = $$TARGETPATH
target.path = $$TARGETPATH

SOURCES += \
    plugin.cpp \
    appgridiconprovider.cpp \
    clockworkimageprovider.cpp \
    dynamiciconsmodel.cpp \
    iconpacksmodel.cpp

HEADERS += \
    appgridiconprovider.h \
    clockworkimageprovider.h \
    dynamiciconsmodel.h \
    iconpacksmodel.h

OTHER_FILES += qmldir *.qml

INSTALLS += target import
