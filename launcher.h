#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include "downloader.h"

class Launcher : public QObject
{
    Q_OBJECT
public:
    explicit Launcher(QObject *parent = nullptr);
    void setJvmPath(const QString &jvmPath);
    void setGamePath(const QString &gamePath);
    void setNickname(const QString &nick);
    void launch(const QString &verId);
signals:

public slots:

private:
    QString jvmPath;
    QString gamePath;

    QString nick;
    QString classPath;
    QJsonObject jsonRoot;
    QString assetIndexPath;
    QString vId;

    QStringList nativeLibraries;

    Downloader *dl;

    int downloadCounter = 0;

    void launch2();
    void launch3();
    bool downloadDecision(QJsonArray rules);
    void downloadLibraries();
    void downloadAssets();
};

#endif // LAUNCHER_H
