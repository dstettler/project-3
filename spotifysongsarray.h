#ifndef SPOTIFYSONGSARRAY_H
#define SPOTIFYSONGSARRAY_H

#include <QList>
#include <QString>
#include <QProgressDialog>
#include <vector>
// "id"	"name"	"album"	"album_id"	"artists"	"artist_ids"	"track_number"	"disc_number"
// "explicit"	"danceability"	"energy"	"key"	"loudness"	"mode"	"speechiness"	"acousticness"
// "instrumentalness"	"liveness"	"valence"	"tempo"	"duration_ms"	"time_signature"	"year"	"release_date"

//***NOTES***
/*We need to find a way to update the source that is store in the Adj list object*/


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
    double loudness;
    bool mode;
    double speechiness;
    double accousticness;
    double instrumentalness;
    double liveness;
    double valence;
    float tempo;
    int durationMs;
    QString releaseDate;
    QString previewUrl;
    QString albumArtUrl;
    QString nameby;


    SpotifySong(QJsonObject obj);
    SpotifySong();


    //Adjacent Songs vector
    QVector<SpotifySong*> adjNodes;
};


class AdjList {
    //Could use this to easily get the source
    //We would create this object in the mainwindow and set the source here
    SpotifySong* source;


    //Contains original song and all similar songs
    QVector<SpotifySong*> nodeList;
    QVector<SpotifySong> visited;




public:
    void insert(SpotifySong newSong);

    AdjList();
    AdjList(SpotifySong* src);

    //I dont see why we need removal capability

    //Search
    //How to return if we dont find anything?
    SpotifySong* search(QString id);

    //Update the Adj List for each song
    void updateAdj();

    //BFS returns a vector of most similar songs
    QVector<SpotifySong>& BFS();

    //Find similarity score between source node and adj node
    double simScore(SpotifySong* src, SpotifySong* adj);

    //Update Adjacent Nodes (we need to create a threshold of similarity)

    void clear() {
        nodeList.clear();
    }
};

struct SpotifySongsArray
{
    AdjList graphSSA;


    SpotifySongsArray(QString filepath, QProgressDialog* dialog);

    SpotifySongsArray(QJsonObject sourceSong);
    //SpotifySong at(unsigned int i);
};


#endif // SPOTIFYSONGSARRAY_H
