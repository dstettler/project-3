function recommendationLoop(track)
{
    console.log("Started loop function");
    var beegArray = [];
    var token = "";
    for (let i = 0; i < 10; i++)
    {
        // I have no idea when the tokens expire so this is to avoid token timeouts
        if (i % 3 == 0)
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

    for (let k = 0; k < beegArray.length; k += 100)
    {
        if (k % 300 == 0)
        {
            token = getToken();
        }

        if (k % 100 == 0)
        {
            featuresRes.push(getAudioFeatures(token, beegArray[k].id));
        }
    }

    var currentHundred = -1;
    for (let m = 0; m < beegArray.length; m++)
    {
        if (m % 100 == 0)
            currentHundred++;

        let obj = {
            // Lerp the values in the following format:
            // beginning + ((i / 99) * (end - beginning))
            //
            // This is very messy I know I am sorry
            accousticness: lerp(beegArray[0+currentHundred].accousticness, beegArray[99+currentHundred].accousticness, m%100),
            danceability: lerp(beegArray[0+currentHundred].danceability, beegArray[99+currentHundred].danceability, m%100),
            energy: lerp(beegArray[0+currentHundred].energy, beegArray[99+currentHundred].energy, m%100),
            instrumentalness: lerp(beegArray[0+currentHundred].instrumentalness, beegArray[99+currentHundred].instrumentalness, m%100),
            liveness: lerp(beegArray[0+currentHundred].liveness, beegArray[99+currentHundred].liveness, m%100),
            speechiness: lerp(beegArray[0+currentHundred].speechiness, beegArray[99+currentHundred].speechiness, m%100),
            valence: lerp(beegArray[0+currentHundred].valence, beegArray[99+currentHundred].valence, m%100),
        };

        beegArray.push(obj);
    }


    // Necessary to carry the values over to C++
    // VERY LARGE STRING
    let js = JSON.stringify(beegArray)

    return js;
}

function lerp(begin, end, index)
{
    return begin + (((index % 100) / 99) * (end - begin));
}


function getAudioFeatures(token, track)
{
    var http = new XMLHttpRequest();
    var url = `https://api.spotify.com/v1/audio-features/${track}`;
    http.open("GET", url, false);

    http.setRequestHeader("Content-Type", "application/json");
    http.setRequestHeader("Authorization", `Bearer ${token}`);

    http.send(null);

    if (http.status === 200)
    {
        return http.responseText;
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
