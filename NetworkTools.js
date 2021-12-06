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

    // Necessary to carry the values over to C++
    // VERY LARGE STRING
    let js = JSON.stringify(beegArray)

    return js;
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
