#include "downloader.h"

#include <QDir>
#include <QFileInfo>

Downloader::Downloader(QObject *parent) : QObject(parent)
{
    mgr = new QNetworkAccessManager(this);
    connect(mgr, &QNetworkAccessManager::finished, this, &Downloader::onDownloadFinished);
}

void Downloader::download(const QString &url, const QString &path, const QString &hash, const QString &visibleName)
{
    counterTarget++;
    QFile file(path);
    if (file.exists())
    {
        file.open(QIODevice::ReadOnly);
        QByteArray blob = file.readAll();
        QString fileHash = QCryptographicHash::hash(blob, QCryptographicHash::Sha1).toHex();
        if (fileHash == hash)
        {
            qDebug() << "File " + visibleName + " already downloaded";
            counter++;
            return;
        }
        qDebug() << "File " + visibleName + " corrupted, redownloading";
    }
    else
        qDebug() << "Downloading " + visibleName;
    auto reply = mgr->get(QNetworkRequest(QUrl(url)));
    reply->setProperty("visibleName", visibleName);
    reply->setProperty("path", path);
    reply->setProperty("simple", false);
}

void Downloader::fixCounterTarget()
{
    targetSet = true;
    if (counter == counterTarget)
    {
        counterTarget = 0;
        counter = 0;
        targetSet = false;
        emit progressChanged("Done", 1, 1);
        emit allDownloadsFinished();
    }
}

void Downloader::simpleDownload(const QString &url, const QString &path, const QString &visibleName)
{
    auto reply = mgr->get(QNetworkRequest(QUrl(url)));
    reply->setProperty("path", path);
    reply->setProperty("simple", true);
    reply->setProperty("visibleName", visibleName);
}

void Downloader::onDownloadFinished(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << "Download failed: " + reply->property("visibleName").toString();
        return;
    }
    QString path = reply->property("path").toString();
    //create folders recursively
    QDir dir;
    QFile file(path);
    QFileInfo finfo(path);
    dir.mkpath(finfo.absolutePath());

    file.open(QIODevice::WriteOnly);
    file.write(reply->readAll());
    file.close();
    reply->deleteLater();
    qDebug() << "Download finished: " + path;
    if (reply->property("simple").toBool())
    {
        emit simpleDownloadFinished();
        return;
    }
    counter++;
    if (targetSet)
        emit progressChanged(reply->property("visibleName").toString(), counter, counterTarget);
    if (counter == counterTarget && targetSet)
    {
        counterTarget = 0;
        counter = 0;
        targetSet = false;
        emit progressChanged("Done", 1, 1);
        emit allDownloadsFinished();
    }
}
