#include "desktopentry.h"
#include <MDesktopEntry>
#include <QDebug>
#include <QFile>

DesktopEntry::DesktopEntry(const QString &path)
    : m_path(path)
    , m_hasChanges(false)
    , m_keyFile(g_key_file_new())
    , m_desktopEntry(new MDesktopEntry(path))
{
    QFile file(path);
    file.open(QIODevice::ReadOnly);

    g_autoptr(GError) error = nullptr;
    const auto content = file.readAll();

    const auto loaded = g_key_file_load_from_data(m_keyFile,
                                                  content.data(),
                                                  content.length(),
                                                  G_KEY_FILE_KEEP_TRANSLATIONS,
                                                  &error);

    if (!loaded) {
        qWarning() << "Could not load desktop file" << path << ":" << error->message;
    }
}

DesktopEntry::~DesktopEntry()
{
    g_key_file_free(m_keyFile);
}

QString DesktopEntry::name()
{
    return m_desktopEntry->name();
}

QString DesktopEntry::icon()
{
    return m_desktopEntry->icon();
}

void DesktopEntry::setIcon(const QString &icon)
{
    const auto iconBa = icon.toUtf8();
    g_key_file_set_string(m_keyFile, "Desktop Entry", "Icon", iconBa.data());
    m_hasChanges = true;
}

// QSetting cannot handle groups with spaces, e.g. "[Desktop Entry]"
// is written back as "[Desktop%20Entry]". That's the reason to have
// GLib-based serialization.
bool DesktopEntry::save()
{
    if (!m_hasChanges)
        return true;

    gsize length;
    g_autoptr(GError) error = nullptr;
    g_autofree char *content = g_key_file_to_data(m_keyFile, &length, &error);

    if (content == nullptr) {
        qWarning() << "Could serialize desktop file" << m_path << ":" << error->message;
        return false;
    }

    QFile file(m_path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file for write" << m_path << ":" << file.errorString();
        return false;
    }

    QByteArray contentBa(content, length);
    if (!file.write(contentBa)) {
        qWarning() << "Could not write new content to" << m_path << ":" << file.errorString();
        return false;
    }

    file.close();

    m_hasChanges = false;
    return true;
}
