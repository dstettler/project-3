#ifndef NETWORKTOOLS_H
#define NETWORKTOOLS_H

#include <QObject>

#include <QtNetwork/QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonArray>
#include <QtConcurrent>
#include <QVariantList>

class MainWindow;

class NetworkTools : public QObject
{
    Q_OBJECT

signals:
    void postCompleted(QJsonObject obj);
    void getCompleted(QJsonObject obj);
    void recRes(QJsonArray obj);
    void doRecLoop(QString track);
    void recSearch(QJsonObject obj);
    void tokenFound(QString token);

private:
    QNetworkAccessManager manager;

public:
    NetworkTools();

    // Emits QString to parent
    void getToken(MainWindow* parent);
    void getToken();
    // Emits QJsonObject to parent
    void searchSong(QString token, QString query, MainWindow* parent);
    // Emits QJsonObject to parent
    void getSongInfo(QString token, QString songName, MainWindow* parent);

    void recommendationsLoop(QString songId, MainWindow* parent);

    void getRequest(QString url, QVector<QString> headers);
    void postRequest(QString url, QVector<QString> headers, QString body);
};

#endif // NETWORKTOOLS_H
