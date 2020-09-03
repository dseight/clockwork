TEMPLATE = aux

TRANSLATIONS = \
    settings-clockwork-ru.ts \
    settings-clockwork-zh_CN.ts \
    settings-clockwork.ts       # Default translation

for(t, TRANSLATIONS) {
    TRANSLATIONS_IN += $${_PRO_FILE_PWD_}/$$t
}

qm.files = $$replace(TRANSLATIONS_IN, \.ts, .qm)
qm.path = /usr/share/clockwork/translations
qm.CONFIG += no_check_exist
qm.commands += \
    lupdate -no-obsolete -locations none $$IN_PWD/.. -ts $$TRANSLATIONS_IN \
    && lrelease -idbased $$TRANSLATIONS_IN

QMAKE_EXTRA_TARGETS += qm
INSTALLS += qm
