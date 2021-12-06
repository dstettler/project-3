function recommendationLoop(track)
{
    var beegArray = [];
    var token = "";
    for (let i = 0; i < 10; i++)
    {
        if (i % 3 == 0)
            token = getToken();
        let rec = getRecommendation(token, track);
        track = rec.tracks[0].id;
        beegArray.push(rec);
    }

    return beegArray;
}

function getRecommendation(token, track)
{
    var http = new XMLHttpRequest();
    var url = `https://api.spotify.com/v1/recommendations?seed_tracks=${track}&limit=100`;
    http.open("GET", url, false);

    http.setRequestHeader("Content-Type", "application/json");
    http.setRequestHeader("Authorization", `Bearer ${token}`);

    http.send(null);

    if (request.status === 200)
    {
        return http.response;
    }
}

function getToken()
{
    var client_id = '5667d65d47f143d8922b8cfeea1f00e4';
    var client_secret = '0a1292ce57ce4c76933a223cb3ce1c88';

    var http = new XMLHttpRequest();
    var url = "https://accounts.spotify.com/api/token";
    http.open("POST", url, false);

    http.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    http.setRequestHeader("Authorization", `Basic ${(btoa(client_id + ":" + client_secret))}`);

    http.send("grant_type=client_credentials");

    if (request.status === 200)
    {
        return http.response.access_token;
    }

}
