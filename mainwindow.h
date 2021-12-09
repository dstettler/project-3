#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "networktools.h"
#include "spotifysongsarray.h"
#include <QtMultimedia/QMediaPlayer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void tokenRequestCompleted(QString token);
    void searchResultCompleted(QJsonObject obj);
    void songResultCompleted(QJsonObject obj);
    void recommendedCompleted(QJsonArray list);
    void songListCompleted();
    void timeForPlayButton();
    //void emittingCompleted(QNetworkReply* reply, QString test);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void printNetworkResults();
    void itemClickedSlot (QListWidgetItem* itemClicked);
    void downloadImageDone(QNetworkReply* result);
    void downloadImageDone2(QNetworkReply* result);
    void itemReleasedSlot();
    void playSongOne();
    void playSongTwo();
    void downloadPlayDoneOg(QNetworkReply* result);
    void downloadPauseDoneOg(QNetworkReply* result);
    void printNetworkResultsLoadMore();



    void on_actionHome_triggered();

private:
    Ui::MainWindow *ui;
    NetworkTools* net;
    QString songId;
    SpotifySongsArray* songs;
    QVariantMap art;
    QVariantMap songToPlay;
    QString songToAdd;
    QJsonObject original;
    QString originalString;
    QString songClicked;
    bool songOneToggle;
    bool songTwoToggle;
    QMediaPlayer *player = new QMediaPlayer;
    QMediaPlayer *player2 = new QMediaPlayer;
    QString lastToken;
    bool searchAgain;
    bool loaded;
    QPixmap playButton;
    QPixmap pauseButton;


};
#endif // MAINWINDOW_H
