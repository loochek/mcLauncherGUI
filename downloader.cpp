#include "downloader.h"

Downloader::Downloader(QObject *parent) : QObject(parent)
{
    connect(&mgr, &QNetworkAccessManager::finished, this, &Downloader::onDownloadFinished);
    connect(&simpleMgr, &QNetworkAccessManager::finished, this, &Downloader::onSimpleDownloadFinished);
    connect(&getMgr, &QNetworkAccessManager::finished, this, &Downloader::onGetFinished);
    connect(this, &Downloader::downloadFinished, download);
}

bool Downloader::addDownload(Download dl)
{
    if (!busy)
    {
        initValue++;
        dlQueue.push_back(dl);
        return true;
    }
    else
        return false;
}

void Downloader::download()
{
    if (dlQueue.empty())
    {
        busy = false;
        qDebug() << "all done!";
        initValue = 0;
        emit allDownloadsFinished();
        return;
    }
    thisDownload = dlQueue.front();
    dlQueue.pop_front();
    qDebug() << "downloading: " + thisDownload.path;
    emit currentProgress(thisDownload.visibleName, round((float) (initValue - dlQueue.length()) / initValue * 100));
    QFileInfo finfo(thisDownload.path);

    if (finfo.exists() )
    {
 //hash checking is shit
        QCryptographicHash checksum(QCryptographicHash::Algorithm::Sha1);
        QFile fin(thisDownload.path);
        fin.open(QIODevice::ReadOnly);
        checksum.addData(&fin);
        if (QString(checksum.result().toHex()) == thisDownload.hash)
        {
            qDebug() << "ne nado";
            emit downloadFinished();
        }
        else
            mgr.get(QNetworkRequest(thisDownload.url));
        //qDebug() << "ne nado";
        //emit downloadFinished();
    }
    else
        mgr.get(QNetworkRequest(thisDownload.url));
}

void Downloader::get(QUrl url)
{
    getMgr.get(QNetworkRequest(url));
}

void Downloader::simpleDownload(Download dl)
{
    thisSimpleDownload = dl;
    qDebug() << "downloading: " + thisSimpleDownload.path;
    //emit currentFile(thisSimpleDownload.visibleName);
    simpleMgr.get(QNetworkRequest(thisSimpleDownload.url));
}

void Downloader::onDownloadFinished(QNetworkReply *reply)
{
    if (!reply->error())
    {
        QDir dir;
        QFileInfo finfo(thisDownload.path);
        dir.mkpath(finfo.absolutePath());

        QFile file(thisDownload.path);
        file.open(QIODevice::WriteOnly);
        QByteArray data = reply->readAll();
        file.write(data);
        file.close();
        qDebug() << "done1!";
    }
    emit downloadFinished();
    reply->deleteLater();
}

void Downloader::onSimpleDownloadFinished(QNetworkReply *reply)
{
    if (!reply->error())
    {
        QDir dir;
        QFileInfo finfo(thisSimpleDownload.path);
        dir.mkpath(finfo.absolutePath());

        QFile file(thisSimpleDownload.path);
        file.open(QIODevice::WriteOnly);
        QByteArray data = reply->readAll();
        file.write(data);
        file.close();
        qDebug() << "done2!";
    }
    emit simpleDownloadFinished();
    reply->deleteLater();
}

void Downloader::onGetFinished(QNetworkReply *reply)
{
    QByteArray data;
    if (!reply->error())
    {
        data = reply->readAll();
        qDebug() << "get done!";
    }
    emit getFinished(data);
    reply->deleteLater();
}
