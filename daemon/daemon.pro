TEMPLATE = app
TARGET = clockwork

CONFIG += c++14
CONFIG += link_pkgconfig
PKGCONFIG += mlite5

INCLUDEPATH += ../lib
LIBS += -L../lib -lclockwork

SOURCES += daemon.cpp

target.path = /usr/bin

systemd.files = clockwork.service
systemd.path = $$USERUNITDIR

INSTALLS += target systemd
