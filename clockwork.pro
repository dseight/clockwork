TEMPLATE = subdirs

SUBDIRS += \
    daemon \
    dynamic-icons \
    lib \
    plugin \
    settings \
    translations

daemon.depends = lib
dynamic-icons.depends = lib
plugin.depends = lib

OTHER_FILES += rpm/*.spec
