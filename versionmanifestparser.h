#ifndef VERSIONMANIFESTPARSER_H
#define VERSIONMANIFESTPARSER_H

#define dlMgr Singleton<Downloader>::instance()

#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QUrl>
#include <QMap>
#include "downloader.h"

class VersionManifestParser : public QObject
{
    Q_OBJECT
public:
    explicit VersionManifestParser(QObject *parent = nullptr);
    void update();
    QUrl getUrlById(QString id);
    QStringList getIdsList();

signals:
    void parseCompleted(QMap<QString, QPair<int, QString>> versions);

public slots:
    void onDownloadFinished(QByteArray data);

private:
    QMap<QString, QPair<int, QString>> versionsList;

};

#endif // VERSIONMANIFESTPARSER_H
