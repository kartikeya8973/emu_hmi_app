#include "httpdownloader.h"
#include "common.h"

HttpDownloader::HttpDownloader(QObject *parent) : QObject(parent)
{

}

void HttpDownloader::doDownload()
{
    manager = new QNetworkAccessManager(this);

//    connect(manager, SIGNAL(finished()),this, SLOT(replyFinished()));

    connect(manager,
            &QNetworkAccessManager::finished,
            this,
            &HttpDownloader::replyFinished);

    manager->get(QNetworkRequest(QUrl("http://admin:admin@192.168.1.2/recordlist.cgi?starttime=2010-01-01T00:00:00Z&endtime=2010-12-31T23:59:59Z&maxcount=999")));
//    manager->get(QNetworkRequest(QUrl("http://admin:admin@192.168.1.2/playback.mp4?id=147&starttime=2010-02-13T23:09:06Z&endtime=2010-02-13T23:10:22Z")));
}

void HttpDownloader::replyFinished (QNetworkReply *reply)
{
    if(reply->error())
    {
        qDebug() << "ERROR!";
        qDebug() << reply->errorString();
    }
    else
    {
        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();
        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

        QFile *file = new QFile(streamListJsonPath);
        if(file->open(QFile::Append))
        {
            file->write(reply->readAll());
            file->flush();
            file->close();
        }
        delete file;
    }

    reply->deleteLater();
}
