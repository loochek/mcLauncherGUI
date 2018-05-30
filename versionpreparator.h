#ifndef VERSIONPREPARATOR_H
#define VERSIONPREPARATOR_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <QStringList>
#include <JlCompress.h>
#include "downloader.h"
#include <QProcess>

#define dlMgr Singleton<Downloader>::instance()

class VersionPreparator : public QObject
{
    Q_OBJECT
public:
    explicit VersionPreparator(QObject *parent = nullptr);

signals:
    void statusChanged(QString status);
    void progressChanged(int value);

public slots:
    void progressHandler(QString file, int value);
    void prepare(QString jsonFile, QString gamePath, QString javaPath, QString nickname);
    void parseResources();
    void parseLibraries(QJsonArray libraries);

private:
    void launch();

    QVector<QString> nativesList;

    QString javaPath;
    QString nativesPath;
    QString librariesString = " -cp ";
    QString mainClass;
    QString nickname;
    QString vId;
    QString gamePath;
    QString assetsDir;
    QString assetsIndex;
    QString uuid = "1a2b3c4d5e6f7g8h9i0g";
    QString accessToken = "1a2b3c4d5e6f7g8h9i0g";
    QString userProperties = "{\"twitch_access_token\":[\"1a2b3c4d5e6f7g8h9i0g\"]}";
    QString userType = "mojang";
    QString versionType = "release";

    QString minecraftArguments;

};

#endif // VERSIONPREPARATOR_H
