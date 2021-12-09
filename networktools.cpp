#include "networktools.h"
#include "mainwindow.h"

#include <QJsonObject>
#include <QJsonDocument>

#include <QWidget>
#include <QFunctionPointer>

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include <QList>
#include <QVariantList>

#include <QQmlEngine>
#include <QQmlComponent>
#include <QtQuick/QQuickItem>
#include <QObject>

#include <QJSValue>

#include <QtConcurrent/QtConcurrent>

// Lots of includes dang

NetworkTools::NetworkTools()
{
    qDebug() << "NetworkTools created";
}

void NetworkTools::getRequest(QString url, QVector<QString> headers)
{
    QNetworkRequest req;
    req.setUrl(url);

    for (int i = 0; i < headers.size() - 1; i += 2)
    {
        QByteArray h1 = headers.at(i).toUtf8();
        QByteArray h2 = headers.at(i + 1).toUtf8();
        req.setRawHeader(h1, h2);
    }

    qDebug() << "";
    qDebug() << "GET request req created with the following url: " << url;

    for (int i = 0; i < headers.size() - 1; i += 2)
    {
        qDebug() << "The following header: " << headers.at(i) << ": " << req.rawHeader(headers.at(i).toUtf8());
    }

    qDebug() << "";

    QNetworkReply* reply = manager.get(req);

    auto connEr = std::make_shared<QMetaObject::Connection>();
    *connEr = QObject::connect(reply, &QNetworkReply::errorOccurred, [=]() {
        qDebug() << "An error ocurred in the network request:";
        qDebug() << reply->errorString();
        qDebug() << "";
        QJsonObject jobj;
        emit getCompleted(jobj);
        QObject::disconnect(*connEr);
    });

    auto conn = std::make_shared<QMetaObject::Connection>();
    *conn = QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError)
        {
            QJsonObject json;
            QJsonDocument doc;
            QByteArray all = reply->readAll();
            qDebug() << "Recieved response with size: " << all.size();
            qDebug().noquote() << all;

            doc = doc.fromJson(all);

            json = doc.object();

            qDebug() << "Emitting JSON:";
            qDebug() << "";
            emit getCompleted(json);
        } else
        {
            qDebug() << "An error ocurred in parsing the reply, passing an empty JSON...";
            qDebug() << reply->error();
            qDebug() << "";
            QJsonObject jobj;
            jobj.insert("Error", reply->error());
            emit getCompleted(jobj);
        }

        QObject::disconnect(*conn);
    });
}

void NetworkTools::postRequest(QString url, QVector<QString> headers, QString body)
{
    QNetworkRequest req;
    req.setUrl(url);

    for (int i = 0; i < headers.size() - 1; i += 2)
    {
        QByteArray h1 = headers.at(i).toUtf8();
        QByteArray h2 = headers.at(i + 1).toUtf8();
        req.setRawHeader(h1, h2);
    }

    qDebug() << "";
    qDebug() << "POST request req created with the following url: " << url;

    for (int i = 0; i < headers.size() - 1; i += 2)
    {
        qDebug() << "The following header: " << headers.at(i) << ": " << req.rawHeader(headers.at(i).toUtf8());
    }

    qDebug() << "And the following body: " << body.toUtf8();
    qDebug() << "";
    QNetworkReply* reply = manager.post(req, body.toUtf8());

    auto connEr = std::make_shared<QMetaObject::Connection>();
    *connEr = QObject::connect(reply, &QNetworkReply::errorOccurred, [=]() {
        qDebug() << "An error ocurred in the network request:";
        qDebug() << reply->errorString();
        qDebug() << "";
        QObject::disconnect(*connEr);
    });

    auto conn = std::make_shared<QMetaObject::Connection>();
    *conn = QObject::connect(reply, &QNetworkReply::finished, [=]() {
        qDebug() << "Server responded without error";
        if (reply->error() == QNetworkReply::NoError)
        {
            QJsonObject json;
            QJsonDocument doc;
            QByteArray all = reply->readAll();
            qDebug() << "Recieved response with size: " << all.size();
            qDebug().noquote() << all;

            doc = doc.fromJson(all);

            json = doc.object();

            qDebug() << "Emitting JSON:";
            qDebug() << "";
            emit postCompleted(json);
        } else
        {
            qDebug() << "An error ocurred in parsing the reply, passing an empty JSON...";
            qDebug() << reply->error();
            qDebug() << "";
            QJsonObject jobj;
            emit postCompleted(jobj);
        }

        QObject::disconnect(*conn);
    });
}

void NetworkTools::getToken()
{
    // Generate the authentication information for the header
    // NOTE: these probably shouldn't be constants in the program, but it works for now
    QString client_id = "5667d65d47f143d8922b8cfeea1f00e4";
    QString client_secret = "0a1292ce57ce4c76933a223cb3ce1c88";
    QString auth = client_id + ":" + client_secret;
    auth = "Basic " + auth.toUtf8().toBase64();

    QVector<QString> headers;
    headers.push_back("Content-Type");
    headers.push_back("application/x-www-form-urlencoded");
    headers.push_back("Authorization");
    headers.push_back(auth);

    QJsonObject result;

    auto conn = std::make_shared<QMetaObject::Connection>();
    *conn = QObject::connect(this, &NetworkTools::postCompleted, [=](QJsonObject obj) {
        QString token = obj.find("access_token").value().toString();

        emit tokenFound(token);
        QObject::disconnect(*conn);
    });

    postRequest(
                "https://accounts.spotify.com/api/token",
                headers,
                "grant_type=client_credentials");

}

void NetworkTools::getToken(MainWindow* parent)
{
    // Generate the authentication information for the header
    // NOTE: these probably shouldn't be constants in the program, but it works for now
    QString client_id = "5667d65d47f143d8922b8cfeea1f00e4";
    QString client_secret = "0a1292ce57ce4c76933a223cb3ce1c88";
    QString auth = client_id + ":" + client_secret;
    auth = "Basic " + auth.toUtf8().toBase64();

    QVector<QString> headers;
    headers.push_back("Content-Type");
    headers.push_back("application/x-www-form-urlencoded");
    headers.push_back("Authorization");
    headers.push_back(auth);

    QJsonObject result;

    auto conn = std::make_shared<QMetaObject::Connection>();
    *conn = QObject::connect(this, &NetworkTools::postCompleted, [=](QJsonObject obj) {
        QString token = obj.find("access_token").value().toString();

        emit parent->tokenRequestCompleted(token);
        QObject::disconnect(*conn);
    });

    postRequest(
                "https://accounts.spotify.com/api/token",
                headers,
                "grant_type=client_credentials");

}

void NetworkTools::searchSong(QString token, QString query, MainWindow* parent)
{
    QVector<QString> vec;
    vec.push_back("Content-Type");
    vec.push_back("application/json");

    QString auth = "Bearer " + token;
    vec.push_back("Authorization");
    vec.push_back(auth);

    QString url = "https://api.spotify.com/v1/search?q=" + query + "&type=track";

    auto conn = std::make_shared<QMetaObject::Connection>();
    *conn = QObject::connect(this, &NetworkTools::getCompleted, [=](QJsonObject obj) {
        emit parent->searchResultCompleted(obj);
        QObject::disconnect(*conn);
    });

    getRequest(url, vec);
}

void NetworkTools::getSongInfo(QString token, QString songName, MainWindow *parent)
{
    QVector<QString> vec;
    vec.push_back("Content-Type");
    vec.push_back("application/json");

    QString auth = "Bearer " + token;
    vec.push_back("Authorization");
    vec.push_back(auth);

    QString url = "https://api.spotify.com/v1/audio-features/" + songName;

    auto conn = std::make_shared<QMetaObject::Connection>();
    *conn = QObject::connect(this, &NetworkTools::getCompleted, [=](QJsonObject obj) {
        emit parent->songResultCompleted(obj);
        // Disconnect lambda
        QObject::disconnect(*conn);
    });

    getRequest(url, vec);
}



void NetworkTools::recommendationsLoop(QString songId, MainWindow *parent)
{
    QQmlEngine engine;

    QQmlComponent component(&engine, QUrl("qrc:main/dummy.qml"), QQmlComponent::PreferSynchronous);

    qDebug() << component.errorString();
    if (component.status() == QQmlComponent::Ready)
    {
        QObject *object = component.create();

        QString list;
        QVariant returned;

        // Do not run a synchronous network request on the UI thread, you say?
        // Nay I say, nay
        // This *will* freeze the process
        QMetaObject::invokeMethod(object, "doRecLoop", Q_RETURN_ARG(QVariant, returned), Q_ARG(QString, songId));

        list = returned.toString();

        QJsonDocument doc = QJsonDocument::fromJson(list.toUtf8());
        QJsonArray arr = doc.array();
        emit (parent->recommendedCompleted(arr));
    }
}

QString NetworkTools::getTempo(QString songId)
{
    QQmlEngine engine;

    QQmlComponent component(&engine, QUrl("qrc:main/dummy.qml"), QQmlComponent::PreferSynchronous);

    if (component.status() == QQmlComponent::Ready)
    {
        QObject *object = component.create();

        QString tempo;
        QVariant returned;
        QVariant id = QVariant::fromValue(songId);

        // Do not run a synchronous network request on the UI thread, you say?
        // Nay I say, nay (again)
        // This *will* freeze the process (much less time than the other one tho)
        QMetaObject::invokeMethod(object, "getTempo", Q_RETURN_ARG(QVariant, returned), Q_ARG(QVariant, id));

        tempo = returned.toString();

        return tempo;
    }
    else
    {
        return "";
    }
}
