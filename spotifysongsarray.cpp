#include "spotifysongsarray.h"

#include <QFile>
#include <QDebug>
#include <QProgressDialog>
#include <QApplication>
#include <QJsonObject>
#include <QJsonArray>


// Loads the .csv into this array object
/*SpotifySongsArray::SpotifySongsArray(QString filepath, QProgressDialog* dialog)
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

        SpotifySong* song = new SpotifySong();
        song->id = QString::fromUtf8(list.at(0));

        song->name = QString::fromUtf8(list.at(1));

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
*/

SpotifySong::SpotifySong(QJsonObject obj)
{
    id = obj.find("id").value().toString();
    name = obj.find("name").value().toString();
    album = obj.find("album").value().toObject().find("name").value().toString();
    albumId = obj.find("album").value().toObject().find("id").value().toString();
    artists = obj.find("artists").value().toArray().at(0).toObject().find("name").value().toString();
    artistsId = obj.find("artists").value().toArray().at(0).toObject().find("id").value().toString();

    nameby = name + " by " + artists;

    trackNumber = obj.find("track_number").value().toString();
    discNumber = obj.find("disc_number").value().toString();
    explicitness = obj.find("explicit").value().toBool();
    danceability = obj.find("danceability").value().toDouble();
    energy = obj.find("energy").value().toDouble();
    speechiness = obj.find("speechiness").value().toDouble();
    accousticness = obj.find("acousticness").value().toDouble();
    instrumentalness = obj.find("instrumentalness").value().toDouble();
    liveness = obj.find("liveness").value().toDouble();
    valence = obj.find("valence").value().toDouble();
    durationMs = obj.find("duration_ms").value().toInt();
    releaseDate = obj.find("album").value().toObject().find("release_date").value().toString();
    previewUrl = obj.find("preview_url").value().toString();
    albumArtUrl = obj.find("album").value().toObject().find("images").value().toArray().at(0).toObject().find("url").value().toString();
}

SpotifySong::SpotifySong()
{
    // Yes this does literally nothing
}

//Functions for Adj List

void AdjList::insert(SpotifySong* newSong) {
    nodeList.push_back(newSong);
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
    for (auto x : nodeList) {
            //Add every node to the adj list of the
            for (unsigned int y = 1; y < nodeList.size(); y++) {
                //Add every node to the adj list of the source node
                x->adjNodes.push_back(nodeList[y]);
            }
        }
}

//BFS and DFS used from Prof. Aman Kapoor's slides regarding the use of Graphs and Graph Algorithms
//Page 15, 8B - Graph Traversals and Algorithm's
//University of Florida
//CISE Department
//12/9/21
QVector<SpotifySong>& AdjList::BFS() {
    toReturnBFS.clear();
    QQueue<SpotifySong> queue;
    QSet<QString> visitedBFS;

    //Start out BFS by including source node as visited and putting it into queue
    visitedBFS.insert(source->id);
    queue.push_front(*source);

    while (!queue.empty()) {
        SpotifySong temp = queue.front();
        queue.pop_front();
        toReturnBFS.push_back(temp);
        //Neighbors
        QVector<SpotifySong*> n = temp.adjNodes;
        //Loop to traverse neighbors of the source
        for (auto x : n) {
            if (visitedBFS.find(x->id) == visitedBFS.end()) {
                if (simScore(source, x) > 0.0 && simScore(source, x) < 0.05) {
                    visitedBFS.insert(x->id);
                    queue.push_back(*x);
                }
            }
        }
    }
    //Replace with local variable just in case
    return toReturnBFS;






        /*QVector<bool> visited(nodeList.size());
        QQueue<int> queue;

        visited[source] = true;
        queue.push_back(source);

        while(!queue.empty()) {

            int front = queue.front();
            vector<int> vector;

            cout << front << " ";

            queue.pop();

            for (int i : graph.adjList[front]) {
                if (!visited[i]) {
                    visited[i] = true;
                    vector.push_back(i);
                    //queue.push(i);
                }

            }
            sort(vector.begin(), vector.end());
                for (int j = 0; j < vector.size(); j++) {
                    queue.push(vector[j]);
                }


        }
        */
}

QVector<SpotifySong>& AdjList::DFS() {
    toReturnDFS.clear();
    QQueue<SpotifySong> queue;
    QSet<QString> visitedDFS;

    //Start out BFS by including source node as visited and putting it into queue
    visitedDFS.insert(source->id);
    queue.push_front(*source);

    while (!queue.empty()) {
        SpotifySong temp = queue.front();
        queue.pop_front();
        toReturnDFS.push_back(temp);
        //Neighbors
        QVector<SpotifySong*> n = temp.adjNodes;
        //Loop to traverse neighbors of the source
        for (auto x : n) {
            if (visitedDFS.find(x->id) == visitedDFS.end()) {
                if (simScore(source, x) > 0.0 && simScore(source, x) < 0.05) {
                    visitedDFS.insert(x->id);
                    queue.push_back(*x);
                }
            }
        }
    }
    //Replace with local variable just in case
    return toReturnDFS;

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
    graphSSA.insert(s);
}
