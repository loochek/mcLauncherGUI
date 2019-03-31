#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>

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
    void download(const QString &url, const QString &path, const QString &hash, const QString &visibleName);
    void fixCounterTarget();
    void simpleDownload(const QString &url, const QString &path, const QString &visibleName);

signals:
    void allDownloadsFinished();
    void progressChanged(QString status, int current, int all);
    void simpleDownloadFinished();

private:
    friend class Singleton<Downloader>;
    QNetworkAccessManager *mgr;
    int counter = 0;
    int counterTarget = 0;
    bool targetSet = false;

    void onDownloadFinished(QNetworkReply *reply);
};

#endif // DOWNLOADER_H
