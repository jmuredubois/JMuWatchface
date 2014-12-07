// fonction intermediaire acces OpenWeatherMap
var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};


// fonctions pour localisation
function locationSuccess(pos) {
  // url pour acces OpenWeatherMap
  var url = "http://api.openweathermap.org/data/2.5/weather?lat=" + 
    pos.coords.latitude + "&lon=" + pos.coords.longitude;
  
  // envoi requete OpenWeatherMap
  xhrRequest(url, 'GET',
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
      
      // la temperature en Kelvin doit etre ramenee en Celsius
      var temperature = Math.round(json.main.temp - 273.15);
      console.log("Température : " + temperature);
      
      // ensoleillement
      var conditions = json.weather[0].description;
      console.log("Ensoleil.: " + conditions);
      
      // conditions id
      var weathid = json.weather[0].id;
      console.log("ID meteo : " + weathid);

      // conditions id
      var city = json.name;
      var date = new Date(json.dt*1000);
      console.log("Openweather dt : " + json.dt);
      console.log("Heure meteo : " + date.toString());
      city = city + " - " + date.getHours() + ":" + date.getMinutes();
      console.log("Ville meteo : " + city);

      
        // construction d'un dictionnaire utilisant les cles
  var dictionary = {
    "KEY_TEMPERATURE" : temperature,
    "KEY_CONDITIONS" : conditions,
    "KEY_WEATHID" : weathid,
    "KEY_CITY" : city
  };

  // envoi vers Pebble
  Pebble.sendAppMessage(dictionary,
    function(e) {
      console.log("Infos meteo envoyees !");
    },
    function(e) {
      console.log("Erreur envoi infos meteo vers Pebble !");
    }
  );
      
    }
  );
}

function locationError(err) {
  console.log("Erreur recherche position !");
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Ecoute pour ouverture de la watchface
Pebble.addEventListener('ready',
    function(e) {
      console.log("PebbleKit JS est pret !");
      
      // recupere la meteo initiale
      getWeather();
    }
);

//Ecoute pour reception d'un AppMessage
Pebble.addEventListener('appmessage',
    function(e) {
      console.log("AppMessage recu !");
      getWeather();
    }
);


