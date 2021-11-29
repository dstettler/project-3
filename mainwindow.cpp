#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "networktools.h"

#include <QMessageBox>
#include <QtConcurrent>

#include <QObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    net = new NetworkTools();

    connect( ui->pushButton, SIGNAL(clicked()), this, SLOT(printNetworkResults()) );

}

MainWindow::~MainWindow()
{
    delete ui;
    delete net;
}

void MainWindow::printNetworkResults()
{
    qDebug() << "clicked";

    QString query = ui->lineEdit->text();

    connect(this, &MainWindow::tokenRequestCompleted, [=](QString token) {
        connect(this, &MainWindow::searchResultCompleted, [=] (QJsonObject obj) {
            // sdata.tracks.items[0].id
            QJsonObject tracks = obj.find("tracks")->toObject();
            QJsonArray items = tracks.find("items")->toArray();
            QJsonObject song = items[0].toObject();
            QString songId = song.find("id")->toString();

            connect(this, &MainWindow::songResultCompleted, [=](QJsonObject obj) {
                QMessageBox b;
                QString str = obj.find("tempo").value().toString();
                b.setText(str);
                b.exec();
            });

            net->getSongInfo(token, songId, this);
        });

        net->searchSong(token, query, this);
    });

    net->getToken(this);
}
