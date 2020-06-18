TEMPLATE = aux
PLUGIN_NAME = clockwork

qml.path = /usr/share/jolla-settings/pages/$$PLUGIN_NAME
qml.files = *.qml *.svg

json.path = /usr/share/jolla-settings/entries
json.files = $${PLUGIN_NAME}.json

INSTALLS += json qml
OTHER_FILES += *.qml *.json *.svg
