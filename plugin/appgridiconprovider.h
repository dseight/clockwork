#ifndef APPGRIDICONPROVIDER_H
#define APPGRIDICONPROVIDER_H

#include <QQuickImageProvider>

class IconPack;

class AppGridIconProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString iconPackName READ iconPackName WRITE setIconPackName NOTIFY iconPackNameChanged)

public:
    AppGridIconProvider(QObject *parent = nullptr);

    QString iconPackName();
    void setIconPackName(const QString &name);

    Q_INVOKABLE bool iconAvailable(const QString &desktopPath);
    Q_INVOKABLE QString iconSource(const QString &desktopPath);

signals:
    void iconPackNameChanged();

private:
    IconPack *m_iconPack;
};

#endif // APPGRIDICONPROVIDER_H
