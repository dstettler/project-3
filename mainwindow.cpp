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


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    net = new NetworkTools();
    qDebug() << QSslSocket::supportsSsl();

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(printNetworkResults()));
    //connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(itemReleasedSlot()));
    connect(this, SIGNAL(songListCompleted()), this, SLOT(itemReleasedSlot()));
    connect(this, SIGNAL(songListCompleted()), this, SLOT(icons()));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClickedSlot(QListWidgetItem*)));
    //connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemReleasedSlot()));
    connect(ui->toolButton, SIGNAL(clicked()), this, SLOT(playSongOne()));
    connect(ui->toolButton_2, SIGNAL(clicked()), this, SLOT(playSongTwo()));
    connect(ui->loadMore, SIGNAL(clicked()), this, SLOT(printNetworkResults()));
    //connect(this, SIGNAL(emittingCompleted(QNetworkReply*, QString)), this, SLOT(downloadImageDoneIcon(QNetworkReply*, QString)));

    QNetworkAccessManager *manager2 = new QNetworkAccessManager(this);
    connect(manager2, &QNetworkAccessManager::finished, this, &MainWindow::downloadPlayDoneOg);

    QUrl play = QUrl("https://cdn2.iconfinder.com/data/icons/media-player-ui/512/Media-Icon-13-512.png");
    QNetworkRequest playRequestOg(play);
    manager2->get(playRequestOg);





    QNetworkAccessManager *manager3 = new QNetworkAccessManager(this);
    connect(manager3, &QNetworkAccessManager::finished, this, &MainWindow::downloadPauseDoneOg);

    QUrl pause = QUrl("https://cdn4.iconfinder.com/data/icons/media-buttons-1/200/757-512.png");
    QNetworkRequest pauseRequest(pause);
    manager3->get(pauseRequest);


    songId = nullptr;
    songs = nullptr;
    songOneToggle = false;
    songTwoToggle = false;
    loaded = false;

}




MainWindow::~MainWindow()
{
    delete ui;
    delete net;
    delete songs;
    /*for (auto i = managerBoss.begin(); i != managerBoss.end(); i++) {
        delete i.value();
    }
    */
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

    QMessageBox msgBox;
    msgBox.setText("Finding Recommendations...");
    msgBox.setInformativeText("Might take 30 seconds");
    QFont fontT("Times", 10);
    msgBox.setFont(fontT);
    msgBox.setWindowTitle("Loading...");
    msgBox.exec();


    QString query = ui->lineEdit->text();



    if (loaded) {

        for (int j = 0; j < 100; j++) {


        connect(this, &MainWindow::recommendedCompleted, [=] (QJsonArray obj) {

            for (int i = 0; i < obj.size(); i++) {

                QJsonObject songInfo = obj.at(i).toObject();
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
                art[songToAdd] = artString;

                songToPlay[originalString] = originalMap["preview_url"].toString();

                songToPlay[songToAdd] = songInfoMap["preview_url"].toString();

                ui->listWidget->addItem(songToAdd);
                //managerBoss[songToAdd] = new QNetworkAccessManager(this);

            }
            lastToken = obj.at(obj.size() - 1).toObject().toVariantMap()["id"].toString();
            emit songListCompleted();
        });

        net->recommendationsLoop(lastToken, this);
        }
    }
    else {
        connect(this, &MainWindow::tokenRequestCompleted, [=] (QString token) {
            connect(this, &MainWindow::searchResultCompleted, [=] (QJsonObject obj) {
               QJsonObject tracks = obj.find("tracks")->toObject();
               QJsonArray items = tracks.find("items")->toArray();
               QJsonObject song = items[0].toObject();
               original = song;
               this->songId = song.find("id")->toString();


               connect(this, &MainWindow::recommendedCompleted, [=] (QJsonArray obj) {
                   for (int i = 0; i < obj.size(); i++) {

                       QJsonObject songInfo = obj.at(i).toObject();
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

                       //managerBoss[songToAdd] = new QNetworkAccessManager(this);
                   }
                   lastToken = obj.at(obj.size() - 1).toObject().toVariantMap()["id"].toString();
                   loaded = true;
                   qDebug() << songToPlay.size();
                   emit songListCompleted();

               });

               net->recommendationsLoop(songId, this);

            });
            net->searchSong(token, query, this);
        });

        net->getToken(this);
    }


}


void MainWindow::itemClickedSlot (QListWidgetItem * itemClicked)
{
    songClicked = itemClicked->text();
    songTwoToggle = false;
    player2->stop();
    ui->toolButton_2->setEnabled(true);

    qDebug() << songToPlay[songClicked].toString();
    if (songToPlay[songClicked].toString() == "") {
        ui->toolButton_2->setEnabled(false);
        ui->toolButton_2->setText("Song Preview Not Available");
        ui->toolButton_2->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);

    }
    else {
        ui->toolButton_2->setEnabled(true);
        ui->toolButton_2->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::downloadImageDone);

    QUrl image = QUrl(art[itemClicked->text()].toString());

    QNetworkRequest request(image);
    manager->get(request);



}

void MainWindow::itemReleasedSlot ()
{


    qDebug() << songToPlay[originalString].toString();

    if (songToPlay[originalString].toString() == "") {
        ui->toolButton->setEnabled(false);
        ui->toolButton->setText("Song Preview Not Available");
        ui->toolButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
    }
    else {
        ui->toolButton->setEnabled(true);
        ui->toolButton_2->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::downloadImageDone2);
    ui->stackedWidget->setCurrentIndex(1);
    ui->toolButton_2->setEnabled(false);
    ui->toolButton_2->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
    ui->toolButton_2->setText("Click on a recommended song first!");

    QUrl image2 = QUrl(art[originalString].toString());
    QNetworkRequest request2(image2);
    manager->get(request2);



}


void MainWindow::downloadImageDone(QNetworkReply* result) {
    QPixmap pictureToDisplay;
    pictureToDisplay.loadFromData(result->readAll());

    ui->label_2->setPixmap(pictureToDisplay);
}

void MainWindow::downloadImageDone2(QNetworkReply* result) {
    QPixmap pictureToDisplay;
    pictureToDisplay.loadFromData(result->readAll());

    ui->label->setPixmap(pictureToDisplay);

}


void MainWindow::on_actionHome_triggered()
{
    ui->stackedWidget->setCurrentIndex(0);
    loaded = false;
    ui->listWidget->clear();
    art.clear();
}

void MainWindow::playSongOne() {
    player2->stop();
    songTwoToggle = false;
    if (!songOneToggle) {
        player->setMedia(QUrl(songToPlay[originalString].toString()));
        player->setVolume(30);
        player->play();
        songOneToggle = true;
        ui->toolButton->setIcon(QIcon(pauseButton));
    }
    else {
        player->stop();
        songOneToggle = false;
        ui->toolButton->setIcon(QIcon(playButton));
    }


}
void MainWindow::playSongTwo() {
    player->stop();
    songOneToggle = false;
    if (!songTwoToggle) {

        player2->setMedia(QUrl(songToPlay[songClicked].toString()));
        player2->setVolume(30);
        player2->play();
        songTwoToggle = true;
        ui->toolButton_2->setIcon(QIcon(pauseButton));

    }
    else {
        player2->stop();
        songTwoToggle = false;
        ui->toolButton_2->setIcon(QIcon(playButton));
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



/*void MainWindow::icons() {
    int j = 0;

    for (auto i = managerBoss.begin(); i != managerBoss.end(); i++) {
        if (j == 10) {
            break;
        }
        j++;
        connect(i.value(), &QNetworkAccessManager::finished, this, &MainWindow::emitOneParam);
        temp.push_front(i.key());

        QUrl image = QUrl(art[i.key()].toString());
        QNetworkRequest request(image);
        i.value()->get(request);

    }


}


void MainWindow::downloadImageDoneIcon(QNetworkReply* result, QString test) {
    QPixmap pictureToDisplay;
    pictureToDisplay.loadFromData(result->readAll());
    QListWidgetItem* item = new QListWidgetItem(QIcon(pictureToDisplay), test);

    ui->listWidget->addItem(item);

}

void MainWindow::emitOneParam(QNetworkReply* result) {
    QString temp2 = temp.front();
    temp.pop_front();
    emit emittingCompleted(result, temp2);

}
*/
