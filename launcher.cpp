#include "launcher.h"

#include <QJsonArray>

Launcher::Launcher(QObject *parent) : QObject(parent)
{
    dl = &Singleton<Downloader>::instance();
}

void Launcher::setJvmPath(const QString &jvmPath)
{
    this->jvmPath = jvmPath;
}

void Launcher::setGamePath(const QString &gamePath)
{
    this->gamePath = gamePath;
}

void Launcher::setNickname(const QString &nick)
{
    this->nick = nick;
}

void Launcher::launch(const QString &verId)
{
    QFile file(QString("%1versions/%2/%2.json").arg(gamePath, verId));
    file.open(QIODevice::ReadOnly);
    jsonRoot = QJsonDocument::fromJson(file.readAll()).object();
    file.close();
    vId = jsonRoot["id"].toString();
    //download asset index
    QJsonObject assetIndex = jsonRoot["assetIndex"].toObject();
    assetIndexPath = "assets/indexes/" + assetIndex["id"].toString() + ".json";
    connect(dl, &Downloader::simpleDownloadFinished, this, &Launcher::launch2);
    dl->simpleDownload(assetIndex["url"].toString(),
                       gamePath + assetIndexPath,
                       assetIndex["id"].toString() + ".json");
}

bool Launcher::downloadDecision(QJsonArray rules)
{
    for (auto i : rules)
    {
        QJsonObject rule = i.toObject();
        if (rule["action"].toString() == "allow" && rule.contains("os"))
#if defined(Q_OS_WIN)
        {
            if (rule["os"].toObject()["name"].toString() != "windows")
                return false;
        }
#elif defined(Q_OS_LINUX)
        {
            if (rule["os"].toObject()["name"].toString() != "linux")
                return false;
        }
#elif defined(Q_OS_MACOS)
        //Bpolaro4ka
        {
            if (rule["os"].toObject()["name"].toString() != "macos")
                return false;
            else if (rule["os"].toObject()["name"].toString() != "osx")
                return false;
        }
#endif
        else if (rule["action"].toString() == "disallow" && rule.contains("os"))
#if defined(Q_OS_WIN)
        {
            if (rule["os"].toObject()["name"] == "windows")
                return false;
        }
#elif defined(Q_OS_LINUX)
        {
            if (rule["os"].toObject()["name"] == "linux")
                return false;
        }
#elif defined(Q_OS_MACOS)
        //Bpolaro4ka
        {
            if (rule["os"].toObject()["name"] == "osx")
                return false;
            if (rule["os"].toObject()["name"] == "macos")
                return false;
        }
#endif
    }
    return true;
}

void Launcher::downloadLibraries()
{
    for (auto i : jsonRoot["libraries"].toArray())
    {
        QJsonObject library = i.toObject();
        if (library.contains("rules"))
        {
            //download or not
            QJsonArray rules = library["rules"].toArray();
            if (!downloadDecision(rules))
                continue;
        }
        QJsonObject downloads = library["downloads"].toObject();
        //download artifact library
        QJsonObject artifact = downloads["artifact"].toObject();
        dl->download(artifact["url"].toString(),
                     gamePath + "libraries/" + artifact["path"].toString(),
                     artifact["sha1"].toString(),
                     artifact["path"].toString());
        //download os- and arch-specific libraries
        //TODO - x32 support
        if (downloads.contains("classifiers"))
        {
            QJsonObject classifiers = downloads["classifiers"].toObject();
#if defined(Q_OS_WIN)
            if (classifiers.contains("natives-windows"))
            {
                QJsonObject native = classifiers["natives-windows"].toObject();
                dl->download(native["url"].toString(),
                             gamePath + "libraries/" + native["path"].toString(),
                             native["sha1"].toString(),
                             native["path"].toString());
            }
            if (classifiers.contains("natives-windows-64"))
            {
                QJsonObject native = classifiers["natives-windows-64"].toObject();
                dl->download(native["url"].toString(),
                             gamePath + "libraries/" + native["path"].toString(),
                             native["sha1"].toString(),
                             native["path"].toString());
            }
#elif defined(Q_OS_LINUX)
            if (classifiers.contains("natives-linux"))
            {
                QJsonObject native = classifiers["natives-linux"].toObject();
                dl->download(native["url"].toString(),
                             gamePath + "libraries/" + native["path"].toString(),
                             native["sha1"].toString(),
                             native["path"].toString());
            }
            if (classifiers.contains("natives-linux-64"))
            {
                QJsonObject native = classifiers["natives-linux-64"].toObject();
                dl->download(native["url"].toString(),
                             gamePath + "libraries/" + native["path"].toString(),
                             native["sha1"].toString(),
                             native["path"].toString());
            }
#elif defined(Q_OS_MACOS)
            //Bpolaro4ka
            if (classifiers.contains("natives-osx"))
            {
                QJsonObject native = classifiers["natives-osx"].toObject();
                dl->download(native["url"].toString(),
                             gamePath + "libraries/" + native["path"].toString(),
                             native["sha1"].toString(),
                             native["path"].toString());
            }
            if (classifiers.contains("natives-macos"))
            {
                QJsonObject native = classifiers["natives-macos"].toObject();
                dl->download(native["url"].toString(),
                             gamePath + "libraries/" + native["path"].toString(),
                             native["sha1"].toString(),
                             native["path"].toString());
            }
#endif
        }
    }
}

void Launcher::downloadAssets()
{
    //read asset index
    QFile assetIndexFile(gamePath + assetIndexPath);
    assetIndexFile.open(QIODevice::ReadOnly);
    QJsonObject assetIndex = QJsonDocument::fromJson(assetIndexFile.readAll()).object();
    assetIndexFile.close();
    QJsonObject assets = assetIndex["objects"].toObject();
    for (QString assetName : assets.keys())
    {
        QJsonObject asset = assets[assetName].toObject();
        QString hash = asset["hash"].toString();
        dl->download(QString("http://resources.download.minecraft.net/%1/%2").arg(hash.left(2), hash),
                     QString("%1assets/objects/%2/%3").arg(gamePath, hash.left(2), hash),
                     hash,
                     assetName);
    }
}

void Launcher::launch2()
{
    disconnect(dl, &Downloader::simpleDownloadFinished, this, &Launcher::launch2);
    QJsonObject client = jsonRoot["downloads"].toObject()["client"].toObject();
    dl->download(client["url"].toString(),
            QString("%1versions/%2/%2.jar").arg(gamePath, vId),
            client["sha1"].toString(),
            vId + ".jar");
    downloadLibraries();
    downloadAssets();
    connect(dl, &Downloader::allDownloadsFinished, this, &Launcher::launch3);
    dl->fixCounterTarget();
}

void Launcher::launch3()
{
    qDebug() << "downloads done";
}
