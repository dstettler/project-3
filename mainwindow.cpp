#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "networktools.h"
#include "spotifysongsarray.h"

#include <QMessageBox>
#include <QtConcurrent>
#include <QSslSocket>
#include <QObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    net = new NetworkTools();
    qDebug() << QSslSocket::supportsSsl();
    connect( ui->pushButton, SIGNAL(clicked()), this, SLOT(printNetworkResults()) );
    songId = nullptr;
    songs = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete net;
    delete songs;
}

void MainWindow::getRecommendations()
{
    if (songId != nullptr)
    {
        connect (this, &MainWindow::tokenRequestCompleted, [=] (QString token) {
            connect(this, &MainWindow::recommendedCompleted, [=] (QJsonObject obj) {
                // TODO: Implement loop
            });

            net->getRecommendations(token, songId, this);
        });

        net->getToken(this);
    }
}

void MainWindow::printNetworkResults()
{
    qDebug() << "clicked";


    QProgressDialog diag("Loading song database...", "Cancel", 0, 1, this);
    diag.show();
    songs = new SpotifySongsArray("C:/songs.csv", &diag);
    qDebug() << songs->at(0).album;
    diag.close();


    /*QString query = ui->lineEdit->text();

    connect(this, &MainWindow::tokenRequestCompleted, [=] (QString token) {
        connect(this, &MainWindow::searchResultCompleted, [=] (QJsonObject obj) {
            // sdata.tracks.items[0].id
            QJsonObject tracks = obj.find("tracks")->toObject();
            QJsonArray items = tracks.find("items")->toArray();
            QJsonObject song = items[0].toObject();
            this->songId = song.find("id")->toString();

            connect(this, &MainWindow::songResultCompleted, [=](QJsonObject obj) {
                QMessageBox b;
                QString str = obj.find("tempo").value().toString();
                b.setText(str);
                b.exec();
            });

            net->getSongInfo(token, this->songId, this);
        });

        net->searchSong(token, query, this);
    });

    net->getToken(this); */
}
