import QtQuick 2.0
import "./NetworkTools.js" as NetworkFunctions


Item {
    id: dummyItem

    function doRecLoop (track: string) : string
    {
        var ret = NetworkFunctions.recommendationLoop(track);
        return ret;
    }
}
