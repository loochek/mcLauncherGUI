#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQueue>
#include <QFile>
#include <QByteArray>
#include <QDir>

struct Download
{
    QString visibleName;
    QUrl url;
    QString path;
    QString hash;
};

template <class T>
class Singleton {
public:
    static T& instance()
    {
        static T instance;
        return instance;
    }
private:
    Singleton();
    ~Singleton();
    Singleton(const Singleton &);
    Singleton& operator=(const Singleton &);
};

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = nullptr);
    bool addDownload(Download dl);
    void download();
    void simpleDownload(Download dl);

signals:
    void downloadFinished();
    void allDownloadsFinished();
    void simpleDownloadFinished();
    void currentProgress(QString file, int value);

public slots:
    void onDownloadFinished(QNetworkReply *reply);
    void onSimpleDownloadFinished(QNetworkReply *reply);

private:
    friend class Singleton<Downloader>;

    QNetworkAccessManager mgr;
    QNetworkAccessManager simpleMgr;
    bool busy = false;
    QQueue<Download> dlQueue;
    Download thisDownload;
    Download thisSimpleDownload;
    int initValue = 0;
};

#endif // DOWNLOADER_H
