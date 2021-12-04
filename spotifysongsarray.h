#ifndef SPOTIFYSONGSARRAY_H
#define SPOTIFYSONGSARRAY_H

#include <QList>
#include <QString>
#include <QProgressDialog>
// "id"	"name"	"album"	"album_id"	"artists"	"artist_ids"	"track_number"	"disc_number"
// "explicit"	"danceability"	"energy"	"key"	"loudness"	"mode"	"speechiness"	"acousticness"
// "instrumentalness"	"liveness"	"valence"	"tempo"	"duration_ms"	"time_signature"	"year"	"release_date"

struct SpotifySong
{
    QString id;
    QString name;
    QString album;
    QString albumId;
    QString artists;
    QString artistsId;
    QString trackNumber;
    QString discNumber;
    bool explicitness;
    double danceability;
    double energy;
    int key;
    double loudness;
    bool mode;
    double speechiness;
    double accousticness;
    double instrumentalness;
    double liveness;
    double valence;
    float tempo;
    unsigned int durationMs;
    float timeSignature;
    unsigned int year;
    QString releaseDate;
};

class SpotifySongsArray
{
    QList<SpotifySong> songs;

public:
    SpotifySongsArray(QString filepath, QProgressDialog* dialog);

    SpotifySong at(unsigned int i);
};

#endif // SPOTIFYSONGSARRAY_H
