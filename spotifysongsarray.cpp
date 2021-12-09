#include "spotifysongsarray.h"

#include <QFile>
#include <QDebug>
#include <QProgressDialog>
#include <QApplication>
#include <QJsonObject>
#include <QJsonArray>


// Loads the .csv into this array object
SpotifySongsArray::SpotifySongsArray(QString filepath, QProgressDialog* dialog)
{

    const int MAXVALS = 200000;

    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "what";
    }

    // Read over the csv metadata, it isn't needed here
    file.readLine();
    dialog->setMinimum(0);
    dialog->setMaximum(MAXVALS);
    for (int i = 0; i < MAXVALS; i++)
    {
        QByteArray line = file.readLine();
        QList<QByteArray> list = line.split(',');

        SpotifySong song;
        song.id = QString::fromUtf8(list.at(0));

        song.name = QString::fromUtf8(list.at(1));

        song.album = QString::fromUtf8(list.at(2));

        song.albumId = QString::fromUtf8(list.at(3));

        song.artists = QString::fromUtf8(list.at(4));

        song.artistsId = QString::fromUtf8(list.at(5));

        song.trackNumber = QString::fromUtf8(list.at(6));

        song.discNumber = QString::fromUtf8(list.at(7));

        song.explicitness = ((list.at(8)).toInt() == 0) ? false : true;

        song.danceability = (list.at(9)).toDouble();

        song.energy = (list.at(10)).toDouble();

        song.loudness = (list.at(12)).toDouble();

        song.mode = ((list.at(13)).toInt() == 0) ? false : true;

        song.speechiness = (list.at(14)).toDouble();

        song.accousticness = (list.at(15)).toDouble();

        song.instrumentalness = (list.at(16)).toDouble();

        song.liveness = (list.at(17)).toDouble();

        song.valence = (list.at(18)).toDouble();

        song.tempo = (list.at(19)).toFloat();

        song.durationMs = (list.at(20)).toUInt();

        song.releaseDate = QString::fromUtf8(list.at(23));


        //songs.push_back(song);
        graphSSA.insert(song);

        dialog->setValue(i);
        QApplication::processEvents();
    }

    file.close();
}

SpotifySong::SpotifySong(QJsonObject obj)
{
    id = obj.find("id")->toString();
    name = obj.find("name")->toString();
    album = obj.find("album")->toObject().find("name")->toString();
    albumId = obj.find("album")->toObject().find("id")->toString();
    artists = obj.find("artists")->toArray().at(0).toObject().find("name")->toString();
    artistsId = obj.find("artists")->toArray().at(0).toObject().find("id")->toString();

    nameby = name + " by " + artists;

    trackNumber = obj.find("track_number")->toString();
    discNumber = obj.find("disc_number")->toString();
    explicitness = obj.find("explicit")->toBool();
    danceability = obj.find("danceability")->toDouble();
    energy = obj.find("energy")->toDouble();
    speechiness = obj.find("speechiness")->toDouble();
    accousticness = obj.find("acousticness")->toDouble();
    instrumentalness = obj.find("instrumentalness")->toDouble();
    liveness = obj.find("liveness")->toDouble();
    valence = obj.find("valence")->toDouble();
    durationMs = obj.find("duration_ms")->toInt();
    releaseDate = obj.find("album")->toObject().find("release_date")->toString();
    previewUrl = obj.find("preview_url")->toString();
    albumArtUrl = obj.find("album")->toObject().find("images")->toArray().at(0).toString();
}

SpotifySong::SpotifySong()
{
    // Yes this does literally nothing
}

//Functions for Adj List

void AdjList::insert(SpotifySong newSong) {
    SpotifySong* insertSong = &newSong;
    nodeList.push_back(insertSong);
}

SpotifySong* AdjList::search(QString req) {
    //req is song id
    SpotifySong* result = nullptr;
    for (auto i : nodeList) {
        if (i->id == req) {
            result = i;
            return result;
        }
    }
    //Returns nullptr if we dont find a match
    return result;
}

void AdjList::updateAdj() {
    for (unsigned int i = 1; i < nodeList.size(); i++) {
         //Add every node to the adj list of the source node
         source->adjNodes.push_back(nodeList[i]);
    }
}

QVector<SpotifySong>& AdjList::BFS() {


    for (auto i : source->adjNodes) {
        if (simScore(source, i) > 0.0 && simScore(source, i) < 0.05) {
            visited.push_back(*i);
        }
    }
    return visited;
}

double AdjList::simScore(SpotifySong* src, SpotifySong* adj) {
    //Score set to a default -1 to be updated
    double score = -1;
    //Composite score to be
    double avgSrc = 0;
    double avgAdj = 0;
    //Evaluate score
    avgSrc = (src->danceability + src->energy + src->speechiness + src->accousticness + src->instrumentalness + src->liveness + src->valence) / double(7);
    avgAdj = (adj->danceability + adj->energy + adj->speechiness + adj->accousticness + adj->instrumentalness + adj->liveness + adj->valence) / double(7);

    //Final Calculation
    score = abs(avgSrc - avgAdj);
    return score;
}

AdjList::AdjList()
{
    source = nullptr;
}

AdjList::AdjList (SpotifySong* src) {
    source = src;
}

SpotifySongsArray::SpotifySongsArray(QJsonObject sourceSong) {
    SpotifySong* s = new SpotifySong(sourceSong);
    AdjList temp(s);
    graphSSA = temp;
}
