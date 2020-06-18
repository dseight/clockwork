#ifndef DESKTOPFILE_H
#define DESKTOPFILE_H

#include <glib.h>
#include <QObject>
#include <QScopedPointer>

class MDesktopEntry;

class DesktopEntry
{
public:
    explicit DesktopEntry(const QString &path);
    ~DesktopEntry();

    /* Application name, as displayed in launcher (e.g. "Clock"). */
    QString name();

    /* Icon used for application. */
    QString icon();

    /* Set icon to use for application. */
    void setIcon(const QString &icon);

    /* Write back all changes made in desktop file. Returns true on success. */
    bool save();

private:
    QString m_path;
    bool m_hasChanges;
    GKeyFile *m_keyFile;
    QScopedPointer<MDesktopEntry> m_desktopEntry;
};

#endif // DESKTOPFILE_H
