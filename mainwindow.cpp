#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "networktools.h"
#include "spotifysongsarray.h"
#include <QMessageBox>
#include <QtConcurrent>
#include <QSslSocket>
#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMovie>
#include <chrono>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    net = new NetworkTools();
    qDebug() << QSslSocket::supportsSsl();

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(printNetworkResults()));
    connect(this, SIGNAL(songListCompleted()), this, SLOT(itemReleasedSlot()));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClickedSlot(QListWidgetItem*)));
    connect(ui->toolButton, SIGNAL(clicked()), this, SLOT(playSongOne()));
    connect(ui->toolButton_2, SIGNAL(clicked()), this, SLOT(playSongTwo()));
    connect(ui->loadMore, SIGNAL(clicked()), this, SLOT(printNetworkResultsLoadMore()));

    setWindowIcon(QIcon(":/images/soundwave.png"));
    ui->toolButton->setIcon(QIcon(":/images/play.png"));
    ui->toolButton_2->setIcon(QIcon(":/images/play.png"));

    QMovie* frontLogo = new QMovie(":/images/logo.gif");

    ui->label_3->setMovie(frontLogo);
    frontLogo->start();

    songArray = nullptr;
    ui->loadMore->hide();
    ui->actionResults->setEnabled(false);
    songId = nullptr;
    songOneToggle = false;
    songTwoToggle = false;
    loaded = false;

}




MainWindow::~MainWindow()
{
    delete ui;
    delete net;
    delete songArray;
}

void MainWindow::printNetworkResults()
{
    //QProgressDialog diag("Loading song database...", "Cancel", 0, 1, this);
    //diag.show();
    //songs = new SpotifySongsArray("C:/songs.csv", &diag);
    //qDebug() << songs->at(0).album;
    //diag.close();


    //for  (int i = 0; i < 100; i++) {
      //  ui->listWidget->addItem(QString::number((i)));
    //}

    //gets user input
    QString query = ui->lineEdit->text();
    ui->listWidget->clear();
    //art.clear();
    ui->label_2->clear();
    if (songArray) {
        songArray->graphSSA.clear();
        delete songArray;
    }
    //songArray = nullptr;



    if (ui->actionResults->isEnabled() == false) {
        ui->actionResults->setEnabled(true);
    }

    //moves page to the "now loading" page
    ui->stackedWidget->setCurrentIndex(2);

    //check if user didnt put anything in search box
    if (query == "") {
        QMessageBox msgBoxError;
        msgBoxError.setText("Please enter a song in the search box!");
        QFont fontT("Times", 10);
        msgBoxError.setFont(fontT);
        msgBoxError.setWindowTitle("Error");
        msgBoxError.setWindowIcon(QIcon(":/images/error.png"));
        msgBoxError.exec();
    }
    else {

        //displays pop up alerting that song recs will be found
        QMessageBox msgBox;
        msgBox.setText("Finding Recommendations...");
        msgBox.setInformativeText("Might take 30 seconds");
        QFont fontT("Times", 10);
        msgBox.setFont(fontT);
        msgBox.setWindowTitle("Loading...");
        msgBox.setWindowIcon(QIcon(":/images/loading.png"));
        msgBox.exec();



        //art.clear();
        //songToPlay.clear();
        //originalString.clear();
        ui->loadMore->show();


        auto tokenConn = std::make_shared<QMetaObject::Connection>();

           *tokenConn = connect(this, &MainWindow::tokenRequestCompleted, [=] (QString token) {
            auto searchConn = std::make_shared<QMetaObject::Connection>();
               *searchConn = connect(this, &MainWindow::searchResultCompleted, [=] (QJsonObject obj) {
                   QJsonObject tracks = obj.find("tracks")->toObject();
                   QJsonArray items = tracks.find("items")->toArray();
                   QJsonObject song = items[0].toObject();
                   original = song;
                   this->songId = song.find("id")->toString();

                   auto conn = std::make_shared<QMetaObject::Connection>();
                   *conn = connect(this, &MainWindow::recommendedCompleted, [=] (QJsonArray obj) {

                        //insert source here
                       songArray = new SpotifySongsArray(obj[0].toObject());


                       SpotifySong temp(obj[0].toObject());
                       original = temp;

                       for (int i = 1; i < obj.size(); i++) {

                           SpotifySong* songToAdd = new SpotifySong(obj[i].toObject());

                           songArray->graphSSA.insert(songToAdd);

                           /*QJsonObject songInfo = obj.at(i).toObject();
                           QVariantMap songInfoMap = songInfo.toVariantMap();
                           QVariantMap albumInfoMap = songInfoMap["album"].toJsonObject().toVariantMap();
                           QVariantList temp = albumInfoMap["images"].toList();
                           QVariantMap temp2 = temp[0].toMap();

                           QString artString = temp2["url"].toString();
                           QVariantMap originalMap = original.toVariantMap();

                           QVariantMap ogMapArtist = originalMap["artists"].toJsonArray().toVariantList().at(0).toJsonObject().toVariantMap();

                           originalString = originalMap["name"].toString() + " by " + ogMapArtist["name"].toString();

                           art[originalString] = originalMap["album"].toJsonObject().toVariantMap()["images"].toList()[0].toMap()["url"].toString();


                           QVariantMap songArtistMap = songInfoMap["artists"].toJsonArray().at(0).toObject().toVariantMap();


                           QString songToAdd = songInfoMap["name"].toString();
                           songToAdd += " by " + songArtistMap["name"].toString();
                           //add url for album art into a map, key being song name + artist
                           art[songToAdd] = artString;

                           //add searched song audio url into map, key being song name + artist
                           songToPlay[originalString] = originalMap["preview_url"].toString();
                           //add current song in loop audio url into map, key being song name + artist
                           songToPlay[songToAdd] = songInfoMap["preview_url"].toString();
                           ui->listWidget->addItem(songToAdd);
                           */

                       }
                       songArray->graphSSA.updateAdj();

                       lastToken = obj.at(obj.size() - 1).toObject().toVariantMap()["id"].toString();
                       emit songListCompleted();

                       QObject::disconnect(*conn);
                   });

                   net->recommendationsLoop(songId, this);
                   QObject::disconnect(*searchConn);
                });
                net->searchSong(token, query, this);
                QObject::disconnect(*tokenConn);
            });

            net->getToken(this);
    }


}


void MainWindow::itemClickedSlot (QListWidgetItem * itemClicked)
{

    QVector<SpotifySong> order = songArray->graphSSA.BFS();

    SpotifySong playing;

    for (int i = 0; i < order.size(); i++) {
        QString temp = order.at(i).name + " by " + order.at(i).artists;

        if (temp == itemClicked->text()) {
            playing = order.at(i);
            break;
        }
    }
    songClicked = itemClicked->text();

    songTwoToggle = false;
    player2->stop();
    ui->toolButton_2->setEnabled(true);
    ui->toolButton_2->setIcon(QIcon(":/images/play.png"));

    //qDebug() << songToPlay[songClicked].toString();

    if (playing.previewUrl == "") {
        ui->toolButton_2->setEnabled(false);
        ui->toolButton_2->setText("Song Preview Not Available");
        ui->toolButton_2->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);

    }
    else {
        ui->toolButton_2->setEnabled(true);
        ui->toolButton_2->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::downloadImageDone2);

    ui->name2->setText(playing.name);
    ui->album2->setText(playing.album);
    QString tempo = "Tempo: " + net->getTempo(playing.id);
    ui->tempo2->setText(tempo);
    ui->artist2->setText(playing.artists);

    QUrl image = QUrl(playing.albumArtUrl);

    QNetworkRequest request(image);
    manager->get(request);



}

void MainWindow::itemReleasedSlot ()
{

    //qDebug() << songToPlay[originalString].toString();
    ui->label_2->clear();
    if (original.previewUrl == "") {
        ui->toolButton->setEnabled(false);
        ui->toolButton->setText("Song Preview Not Available");
        ui->toolButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
    }
    else {
        ui->toolButton->setEnabled(true);
        ui->toolButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
        ui->toolButton_2->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::downloadImageDone);

    ui->stackedWidget->setCurrentIndex(1);
    ui->toolButton_2->setEnabled(false);
    ui->toolButton_2->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
    ui->toolButton_2->setText("Click on a recommended song first!");

    auto temp = songArray->graphSSA.BFS();

    for (auto i : temp) {

        ui->listWidget->addItem(i.nameby);
    }
    ui->name2->setText("Song");
    ui->album2->setText("Album");
    ui->tempo2->setText("Tempo");
    ui->artist2->setText("Artist");
    ui->name1->setText(original.name);
    ui->album1->setText(original.album);
    QString tempo = "Tempo: " + net->getTempo(original.id);

    ui->tempo1->setText(tempo);
    ui->artist1->setText(original.artists);

    QUrl image2 = QUrl(original.albumArtUrl);
    QNetworkRequest request2(image2);
    manager->get(request2);
}


void MainWindow::downloadImageDone(QNetworkReply* result) {
    QPixmap pictureToDisplay;
    pictureToDisplay.loadFromData(result->readAll());

    ui->label->setPixmap(pictureToDisplay);
}

void MainWindow::downloadImageDone2(QNetworkReply* result) {
    QPixmap pictureToDisplay;
    pictureToDisplay.loadFromData(result->readAll());

    ui->label_2->setPixmap(pictureToDisplay);
}


void MainWindow::on_actionHome_triggered()
{
    ui->stackedWidget->setCurrentIndex(0);
    //ui->listWidget->clear();
    //art.clear();
    //ui->label_2->clear();
    ui->loadMore->hide();
}

void MainWindow::playSongOne() {
    player2->stop();
    songTwoToggle = false;


    ui->toolButton_2->setIcon(QIcon(":/images/play.png"));
    if (!songOneToggle) {
        player->setMedia(QUrl(original.previewUrl));
        player->setVolume(30);
        player->play();
        songOneToggle = true;
        ui->toolButton->setIcon(QIcon(pauseButton));
        ui->toolButton->setIcon(QIcon(":/images/pause.png"));

    }
    else {
        player->stop();
        songOneToggle = false;
        ui->toolButton->setIcon(QIcon(playButton));
        ui->toolButton->setIcon(QIcon(":/images/play.png"));

    }


}
void MainWindow::playSongTwo() {
    player->stop();
    songOneToggle = false;
    ui->toolButton->setIcon(QIcon(":/images/play.png"));

    QVector<SpotifySong> order = songArray->graphSSA.BFS();

    SpotifySong playing;

    for (int i = 0; i < order.size(); i++) {
        QString temp = order.at(i).name + " by " + order.at(i).artists;

        if (temp == songClicked) {
            playing = order.at(i);
            break;
        }
    }


    if (!songTwoToggle) {

        player2->setMedia(QUrl(playing.previewUrl));
        player2->setVolume(30);
        player2->play();
        songTwoToggle = true;

        ui->toolButton_2->setIcon(QIcon(pauseButton));
        ui->toolButton_2->setIcon(QIcon(":/images/pause.png"));

    }
    else {
        player2->stop();
        songTwoToggle = false;
        ui->toolButton_2->setIcon(QIcon(playButton));
        ui->toolButton_2->setIcon(QIcon(":/images/play.png"));
    }

}



void MainWindow::downloadPlayDoneOg(QNetworkReply* result) {

    playButton.loadFromData(result->readAll());
    QIcon ButtonIcon(playButton);
    ui->toolButton->setIcon(ButtonIcon);
    ui->toolButton_2->setIcon(QIcon(playButton));

}


void MainWindow::downloadPauseDoneOg(QNetworkReply* result) {
    pauseButton.loadFromData(result->readAll());
}



void MainWindow::printNetworkResultsLoadMore() {

    QMessageBox msgBox;
    msgBox.setText("Finding Recommendations...");
    msgBox.setInformativeText("Might take 30 seconds");
    QFont fontT("Times", 10);
    msgBox.setFont(fontT);
    msgBox.setWindowTitle("Loading...");
    msgBox.exec();

    auto conn = std::make_shared<QMetaObject::Connection>();

    connect(this, &MainWindow::recommendedCompleted, [=] (QJsonArray obj) {

        for (int i = 0; i < obj.size(); i++) {

            SpotifySong* songToAdd = new SpotifySong(obj[i].toObject());

             songArray->graphSSA.insert(songToAdd);

            /*QJsonObject songInfo = obj.at(i).toObject();
            QVariantMap songInfoMap = songInfo.toVariantMap();
            QVariantMap albumInfoMap = songInfoMap["album"].toJsonObject().toVariantMap();
            QVariantList temp = albumInfoMap["images"].toList();
            QVariantMap temp2 = temp[0].toMap();

            QString artString = temp2["url"].toString();



            QVariantMap songArtistMap = songInfoMap["artists"].toJsonArray().at(0).toObject().toVariantMap();



            QString songToAdd = songInfoMap["name"].toString();
            songToAdd += " by " + songArtistMap["name"].toString();
            art[songToAdd] = artString;


            songToPlay[songToAdd] = songInfoMap["preview_url"].toString();

            ui->listWidget->addItem(songToAdd);
            */

        }
        songArray->graphSSA.updateAdj();
        lastToken = obj.at(obj.size() - 1).toObject().toVariantMap()["id"].toString();
        emit songListCompleted();
        QObject::disconnect(*conn);
    });

    net->recommendationsLoop(lastToken, this);
}


void MainWindow::on_actionResults_triggered()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_bfs_clicked()
{

    //clock_t start = clock();
    auto timerName = std::chrono::steady_clock::now();

    auto temp = songArray->graphSSA.BFS();
    std::chrono::duration<double> duration = std::chrono::steady_clock::now() - timerName;



    /*clock_t stop = clock();
    clock_t tick = stop - start;
    double time = tick / (double)CLOCKS_PER_SEC;
    time*= 100;
    */

    ui->bfsTime->setText(QString::number(duration.count()));



}
void MainWindow::on_dfs_clicked()
{


    auto timerName = std::chrono::steady_clock::now();


    auto temp = songArray->graphSSA.DFS();
    std::chrono::duration<double> duration = std::chrono::steady_clock::now() - timerName;


    /*clock_t stop = clock();
    clock_t tick = stop - start;
    double time = tick / (double)CLOCKS_PER_SEC;
    time *= 100;
    */



    ui->dfsTime->setText(QString::number(duration.count()));

}

