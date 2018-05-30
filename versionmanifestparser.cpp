#include "versionmanifestparser.h"

VersionManifestParser::VersionManifestParser(QObject *parent) : QObject(parent)
{
    connect(&dlMgr, &Downloader::getFinished, this, &VersionManifestParser::onDownloadFinished);
}

void VersionManifestParser::update()
{
    dlMgr.get(QUrl("https://launchermeta.mojang.com/mc/game/version_manifest.json"));
}

QUrl VersionManifestParser::getUrlById(QString id)
{
    return QUrl(versionsList[id].second);
}

QStringList VersionManifestParser::getIdsList()
{
    return versionsList.keys();
}

void VersionManifestParser::onDownloadFinished(QByteArray data)
{
    QJsonDocument document = QJsonDocument::fromJson(data);
    QJsonObject root = document.object();
    QJsonArray versions = root.value("versions").toArray();
    for(int i = 0; i < versions.count(); i++)
    {
        QJsonObject version = versions.at(i).toObject();
        int type;
        QString typeS = version.value("type").toString();
        if (typeS == "release")
            type = 0;
        else if (typeS == "snapshot")
            //type = 1;
            continue;
        else if (typeS == "old_beta")
            //type = 2;
            continue;
        else if (typeS == "old_alpha")
            //type = 3;
            continue;
        versionsList[version.value("id").toString()] = qMakePair(type, version.value("url").toString());
    }
    qDebug() << versionsList;
    emit parseCompleted(versionsList);
}
