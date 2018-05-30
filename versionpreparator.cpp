#include "versionpreparator.h"

VersionPreparator::VersionPreparator(QObject *parent) : QObject(parent)
{
    connect(&dlMgr, &Downloader::currentProgress, this, &VersionPreparator::progressHandler);
}

void VersionPreparator::prepare(QString jsonFile, QString gamePath, QString javaPath, QString nickname)
{
    this->nativesList.clear();
    this->librariesString = " -cp ";
    this->javaPath = javaPath;
    this->nickname = nickname;
    this->gamePath = gamePath;

    emit statusChanged("Parsing version JSON");

    QFile json(jsonFile);
    json.open(QIODevice::ReadOnly);
    QJsonObject root = QJsonDocument::fromJson(json.readAll()).object();
    json.close();

    this->vId = root.value("id").toString();

    QJsonObject client = root.value("downloads").toObject().value("client").toObject();
    dlMgr.addDownload({this->vId + ".jar", QUrl(client.value("url").toString()), this->gamePath + "versions/" + vId + "/" + vId + ".jar", client.value("sha1").toString()});
    librariesString += this->gamePath + "versions/" + vId + "/" + vId + ".jar;";

    parseLibraries(root.value("libraries").toArray());

    this->mainClass = root.value("mainClass").toString();
    this->minecraftArguments = root.value("minecraftArguments").toString();

    librariesString = librariesString.left(librariesString.length() - 1);

    QJsonObject assets = root.value("assetIndex").toObject();
    this->assetsDir = this->gamePath + "assets/";
    this->assetsIndex = assets.value("id").toString();
    connect(&dlMgr, &Downloader::simpleDownloadFinished, this, parseResources);
    dlMgr.simpleDownload({this->assetsIndex + ".json", QUrl(assets.value("url").toString()), this->assetsDir + "indexes/" + this->assetsIndex + ".json", assets.value("sha1").toString()});
}

void VersionPreparator::progressHandler(QString file, int value)
{
    emit progressChanged(value);
    emit statusChanged("Downloading: " + file);
}

void VersionPreparator::parseResources()
{
    disconnect(&dlMgr, &Downloader::simpleDownloadFinished, this, parseResources);
    emit statusChanged("Downloading resources");
    QFile json(this->assetsDir + "indexes/" + this->assetsIndex + ".json");
    json.open(QIODevice::ReadOnly);
    QJsonObject root = QJsonDocument::fromJson(json.readAll()).object();
    QJsonObject objects = root.value("objects").toObject();
    json.close();

    bool legacy = false;
    if (root.value("virtual").toBool() == true)
    {
        legacy = true;
    }

    for (auto i: objects.keys())
    {
        QJsonObject item = objects.value(i).toObject();
        QString hash = item.value("hash").toString();
        QUrl url("https://resources.download.minecraft.net/" + hash.left(2) + "/" + hash);
        QString path;
        if (legacy)
            path = this->gamePath + "assets/virtual/legacy/" + i;
        else
            path = this->gamePath + "assets/objects/" + hash.left(2) + "/" + hash;
        dlMgr.addDownload({i, url, path, item.value("hash").toString()});
    }
    connect(&dlMgr, &Downloader::allDownloadsFinished, this, launch);
    dlMgr.download();
}

void VersionPreparator::launch()
{
    disconnect(&dlMgr, &Downloader::allDownloadsFinished, this, launch);
    emit statusChanged("Unpacking natives");
    nativesPath = this->gamePath + "versions/" + this->vId + "/natives";
    for (auto i:nativesList)
    {
        QString zipPath = this->gamePath + "libraries/" + i;
        QStringList files = JlCompress::getFileList(zipPath);
        for (auto j:files)
        {
            if(j.left(8) != "META-INF")
                JlCompress::extractFile(zipPath, j, nativesPath + "/" + j);
        }
    }

    this->minecraftArguments.replace("${auth_player_name}", this->nickname);
    this->minecraftArguments.replace("${version_name}", this->vId);
    this->minecraftArguments.replace("${game_directory}", this->gamePath);
    this->minecraftArguments.replace("${game_assets}", this->assetsDir + "virtual/legacy/");
    this->minecraftArguments.replace("${assets_root}", this->assetsDir);
    this->minecraftArguments.replace("${assets_index_name}", this->assetsIndex);
    this->minecraftArguments.replace("${auth_uuid}", this->uuid);
    this->minecraftArguments.replace("${auth_access_token}", this->accessToken);
    this->minecraftArguments.replace("${auth_session}", this->accessToken);
    this->minecraftArguments.replace("${user_properties}", this->userProperties);
    this->minecraftArguments.replace("${user_type}", this->userType);
    this->minecraftArguments.replace("${version_type}", this->versionType);

    QString launchString;
    launchString += this->javaPath;
    launchString += " -Djava.library.path=" + this->nativesPath;
    launchString += this->librariesString;
    launchString += " " + this->mainClass;
    launchString += " " + this->minecraftArguments;
    qDebug() << launchString;
    emit statusChanged("Done!");
    emit finished();
    bool res = QProcess::startDetached(launchString);  
}

void VersionPreparator::parseLibraries(QJsonArray libraries)
{
    for (int i = 0; i < libraries.count(); i++)
    {
        QJsonObject lib = libraries.at(i).toObject();
        bool dl = true;
        if (lib.contains("rules"))
        {
            //govnocode warning
            QJsonArray rules = lib.value("rules").toArray();
            if (rules.at(0).toObject().contains("os"))
            {
                if (rules.at(0).toObject().value("os").toObject().value("name").toString() != "windows")
                    dl = false;
                else if (rules.at(1).toObject().value("os").toObject().value("name").toString() == "windows")
                    dl = false;
            }
            else if (rules.at(1).toObject().value("os").toObject().value("name").toString() == "windows")
                dl = false;
        }
        if (dl == true)
        {
            QJsonObject downloads = lib.value("downloads").toObject();
            if (downloads.contains("artifact"))
            {

                QJsonObject artifact = downloads.value("artifact").toObject();
                dlMgr.addDownload({artifact.value("path").toString(), QUrl(artifact.value("url").toString()), this->gamePath + "libraries/" + artifact.value("path").toString(), artifact.value("sha1").toString()});
                librariesString += this->gamePath + "libraries/" + artifact.value("path").toString() + ";";
            }
            if (downloads.contains("classifiers"))
            {

                QJsonObject classifiers = downloads.value("classifiers").toObject();
                if (classifiers.contains("natives-windows"))
                {
                    QJsonObject natives_windows = classifiers.value("natives-windows").toObject();
                    dlMgr.addDownload({natives_windows.value("path").toString(), QUrl(natives_windows.value("url").toString()), this->gamePath + "libraries/" + natives_windows.value("path").toString(), natives_windows.value("sha1").toString()});
                    nativesList.push_back(natives_windows.value("path").toString());
                }
                if (classifiers.contains("natives-windows-64"))
                {
                    QJsonObject natives_windows_64 = classifiers.value("natives-windows-64").toObject();
                    dlMgr.addDownload({natives_windows_64.value("path").toString(), QUrl(natives_windows_64.value("url").toString()), this->gamePath + "libraries/" + natives_windows_64.value("path").toString(), natives_windows_64.value("sha1").toString()});
                    nativesList.push_back(natives_windows_64.value("path").toString());
                }
            }
        }
        else
            qDebug() << "skipped " + lib.value("name").toString() + " by rule";
    }
}
