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
            res.push(getAudioFeatures(token, beegArray[k].id));
        }
    }


    var slopes = [];
    for (let l = 0; l < featuresRes.length - 1; l++)
    {
        // accousticness / dX
        slopes.push(
                    {
                        accousticness: (featuresRes[l + 1].acousticness - featuresRes[l].acousticness) / 100,
                        danceability: (featuresRes[l + 1].danceability - featuresRes[l].danceability) / 100,
                        energy: (featuresRes[l + 1].energy - featuresRes[l].energy) / 100,
                        instrumentalness: (featuresRes[l + 1].instrumentalness - featuresRes[l].instrumentalness) / 100,
                        liveness: (featuresRes[l + 1].liveness - featuresRes[l].liveness) / 100,
                        speechiness: (featuresRes[l + 1].speechiness - featuresRes[l].speechiness) / 100,
                        valence: (featuresRes[l + 1].valence - featuresRes[l].valence) / 100,
                    });
    }

    var currentHundred = -100;
    for (let m = 0; m < beegArray.length; m++)
    {
        if (m % 100 == 0)
            currentSlope += 100;

        let obj = {
            // Lerp the values in the following format:
            // beginning + ((i / 99) * (end - beginning))
            //
            // This is very messy I know I am sorry
            accousticness: beegArray[0 + currentHundred].accousticness + (((m % 100) / 99) * (beegArray[99 + currentHundred].accousticness - beegArray[0 + currentHundred].accousticness)),
            danceability: beegArray[0 + currentHundred].danceability + (((m % 100) / 99) * (beegArray[99 + currentHundred].danceability - beegArray[0 + currentHundred].danceability)),
            energy: beegArray[0 + currentHundred].energy + (((m % 100) / 99) * (beegArray[99 + currentHundred].energy - beegArray[0 + currentHundred].energy)),
            instrumentalness: beegArray[0 + currentHundred].instrumentalness + (((m % 100) / 99) * (beegArray[99 + currentHundred].instrumentalness - beegArray[0 + currentHundred].instrumentalness)),
            liveness: beegArray[0 + currentHundred].liveness + (((m % 100) / 99) * (beegArray[99 + currentHundred].liveness - beegArray[0 + currentHundred].liveness)),
            speechiness: beegArray[0 + currentHundred].speechiness + (((m % 100) / 99) * (beegArray[99 + currentHundred].speechiness - beegArray[0 + currentHundred].speechiness)),
            valence: beegArray[0 + currentHundred].valence + (((m % 100) / 99) * (beegArray[99 + currentHundred].valence - beegArray[0 + currentHundred].valence)),
        };

        beegArray[m] = Object.assign(beegArray[m], obj);
    }


    // Necessary to carry the values over to C++
    // VERY LARGE STRING
    let js = JSON.stringify(beegArray)

    return js;
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
