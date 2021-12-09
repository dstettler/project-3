import QtQuick 2.0
import "./NetworkTools.js" as NetworkFunctions


Item
{
    id: dummyItem

    function doRecLoop (track)
    {
        var ret = NetworkFunctions.recommendationLoop(track);
        return ret;
    }

    function getTempo(track)
    {
        let tempo = NetworkFunctions.getTempo(track);
        return tempo;
    }
}
