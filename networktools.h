#ifndef NETWORKTOOLS_H
#define NETWORKTOOLS_H

#include <QObject>

#include <QtNetwork/QNetworkAccessManager>
#include <QJsonObject>

class MainWindow;

class NetworkTools : public QObject
{
    Q_OBJECT

signals:
    void postCompleted(QJsonObject obj);
    void getCompleted(QJsonObject obj);
    void recRes(QJsonObject obj);
    void recSearch(QJsonObject obj);

private:
    QNetworkAccessManager manager;
    bool resultNotFound;
    QString token;

public:
    NetworkTools();

    // Emits QString to parent
    void getToken(MainWindow* parent);
    // Emits QJsonObject to parent
    void searchSong(QString token, QString query, MainWindow* parent);
    // Emits QJsonObject to parent
    void getSongInfo(QString token, QString songName, MainWindow* parent);
    // Emits QJsonArray to parent
    void getRecommendations(QString token, QString songId, MainWindow* parent);

    void recommendationsLoop(MainWindow* parent);

    void getRequest(QString url, QVector<QString> headers);
    void postRequest(QString url, QVector<QString> headers, QString body);
};

#endif // NETWORKTOOLS_H
