#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "networktools.h"
#include "spotifysongsarray.h"

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
    void recommendedCompleted(QJsonObject obj);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void printNetworkResults();
    void getRecommendations();

private:
    Ui::MainWindow *ui;
    NetworkTools* net;
    QString songId;
    SpotifySongsArray* songs;

};
#endif // MAINWINDOW_H
