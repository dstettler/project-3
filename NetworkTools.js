function recommendationLoop(track)
{
    console.log("Started loop function");
    var beegArray = [];
    var token = getToken();

    let initTrack = JSON.parse(getTrack(token, track));
    beegArray.push(initTrack);

    for (let i = 0; i < 10; i++)
    {
        // I have no idea when the tokens expire so this is to avoid token timeouts
        if (i % 10 == 0)
            token = getToken();

        let rec = JSON.parse(getRecommendation(token, track));

        for (let j = 0; j < rec.tracks.length; j++)
        {
            beegArray.push(rec.tracks[j]);
        }

        track = rec.tracks[0].id;
    }

    let val = Math.floor(beegArray.length);

    var featuresRes = [];

    for (let k = 0; k < beegArray.length; k++)
    {
        if (k % 1000 == 0)
        {
            token = getToken();
        }

        if (k % 100 == 1 || k == 0)
        {
            let res = getAudioFeatures(token, beegArray[k].id);
            let resJson = JSON.parse(res)
            featuresRes.push(resJson);
        }
    }

    var currentHundred = -1;
    for (let m = 0; m < beegArray.length; m++)
    {
        if (m % 101 == 0 && currentHundred + 1 < featuresRes.length)
        {
            currentHundred++;
        }

        let obj = {
            // Lerp the values in the following format:
            // beginning + ((i / 99) * (end - beginning))
            //
            // This is very messy I know I am sorry
            acousticness: lerp(featuresRes[0+currentHundred].acousticness, featuresRes[1+currentHundred].acousticness, m%100),
            danceability: lerp(featuresRes[0+currentHundred].danceability, featuresRes[1+currentHundred].danceability, m%100),
            energy: lerp(featuresRes[0+currentHundred].energy, featuresRes[1+currentHundred].energy, m%100),
            instrumentalness: lerp(featuresRes[0+currentHundred].instrumentalness, featuresRes[1+currentHundred].instrumentalness, m%100),
            liveness: lerp(featuresRes[0+currentHundred].liveness, featuresRes[1+currentHundred].liveness, m%100),
            speechiness: lerp(featuresRes[0+currentHundred].speechiness, featuresRes[1+currentHundred].speechiness, m%100),
            valence: lerp(featuresRes[0+currentHundred].valence, featuresRes[1+currentHundred].valence, m%100),
        };

        // Append these values to the already created node
        beegArray[m] = Object.assign(beegArray[m], obj);
    }

    // Necessary to carry the values over to C++
    // VERY LARGE STRING
    let js = JSON.stringify(beegArray)

    return js;
}

function lerp(begin, end, index)
{
    // Magic linear interpolation function to make my life easier
    return begin + (((index % 100) / 99) * (end - begin));
}


function getTempo(track)
{
    let token = getToken();
    let features = JSON.parse(getAudioFeatures(token, track));
    return features.tempo.toString();
}

function getAudioFeatures(token, track)
{
    console.log("Getting features for track: " + track)
    var http = new XMLHttpRequest();
    var url = `https://api.spotify.com/v1/audio-features/${track}`;
    http.open("GET", url, false);

    http.setRequestHeader("Content-Type", "application/json");
    http.setRequestHeader("Authorization", `Bearer ${token}`);

    http.send(null);

    if (http.status === 200)
    {
        console.log("Response: " + http.responseText);
        return http.responseText;
    }
    else
    {
        return "{ msg: error }";
    }
}

function getTrack(token, track)
{
    var http = new XMLHttpRequest();
    var url = `https://api.spotify.com/v1/tracks/${track}`;
    http.open("GET", url, false);

    http.setRequestHeader("Content-Type", "application/json");
    http.setRequestHeader("Authorization", `Bearer ${token}`);

    http.send(null);

    if (http.status === 200)
    {
        console.log("Response: " + http.responseText);
        return http.responseText;
    }
    else
    {
        return "{ msg: error }";
    }
}

function getRecommendation(token, track)
{
    var http = new XMLHttpRequest();
    var url = `https://api.spotify.com/v1/recommendations?seed_tracks=${track}&limit=100`;
    http.open("GET", url, false);

    http.setRequestHeader("Content-Type", "application/json");
    http.setRequestHeader("Authorization", `Bearer ${token}`);

    http.send(null);

    if (http.status === 200)
    {
        return http.responseText;
    }
}

function getToken()
{
    var client_id = '5667d65d47f143d8922b8cfeea1f00e4';
    var client_secret = '0a1292ce57ce4c76933a223cb3ce1c88';
    var auth = Qt.btoa(client_id + ":" + client_secret);
    var fullAuth = "Basic " + auth;

    var http = new XMLHttpRequest();
    var url = "https://accounts.spotify.com/api/token";
    http.open("POST", url, false);
    http.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    http.setRequestHeader("Authorization", fullAuth);

    http.send("grant_type=client_credentials");

    if (http.status === 200)
    {
        var js = JSON.parse(http.responseText);
        return js.access_token;
    }

}
