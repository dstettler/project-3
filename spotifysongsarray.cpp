#include "spotifysongsarray.h"

#include <QFile>
#include <QDebug>
#include <QProgressDialog>
#include <QApplication>


// Loads the .csv into this array object
SpotifySongsArray::SpotifySongsArray(QString filepath, QProgressDialog* dialog)
{
    const int MAXVALS = 200000;

    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug () << "what";
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

        song.key = (list.at(11)).toInt();

        song.loudness = (list.at(12)).toDouble();

        song.mode = ((list.at(13)).toInt() == 0) ? false : true;

        song.speechiness = (list.at(14)).toDouble();

        song.accousticness = (list.at(15)).toDouble();

        song.instrumentalness = (list.at(16)).toDouble();

        song.liveness = (list.at(17)).toDouble();

        song.valence = (list.at(18)).toDouble();

        song.tempo = (list.at(19)).toFloat();

        song.durationMs = (list.at(20)).toUInt();

        song.timeSignature = (list.at(21)).toFloat();

        song.year = (list.at(22)).toUInt();

        song.releaseDate = QString::fromUtf8(list.at(23));


        songs.push_back(song);

        dialog->setValue(i);
        QApplication::processEvents();
    }

    file.close();
}

SpotifySong SpotifySongsArray::at(unsigned int i)
{
    return songs.at(i);
}

