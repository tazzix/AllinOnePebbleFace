var mConfig = {};
var locationOptions = { "timeout": 15000, "maximumAge": 60000 }; 

/* Convenient function to automatically retry messages. */
Pebble.sendAppMessageWithRetry = function(message, retryCount, successCb, failedCb) {
  var retry = 0;
  var success = function(e) {
    if (typeof successCb == "function") {
      successCb(e);
    }
  };
  var failed = function(e) {
    /* console.log("Failed sending message: " + JSON.stringify(message) +
      " - Error: " + JSON.stringify(e) + " - Retrying..."); */
    retry++;
    if (retry < retryCount) {
      Pebble.sendAppMessage(message, success, failed);
    }
    else {
      if (typeof failedCb == "function") {
        failedCb(e);
      }
    }
  };
  Pebble.sendAppMessage(message, success, failed);
};

function fetchWeather(latitude, longitude) {
  //console.log("Fetch Weather");
  var response, tzone;
  tzone = 0-((new Date().getTimezoneOffset())/60);
  console.log("Tzone:"+tzone);
  var req = new XMLHttpRequest();
  req.open('GET', /*"http://www.mirz.com/Chunk2/Yahoo.php?"*/ "http://linuxvps.tazzix.com/pebapp01.php?" +
    "lat=" + latitude + "&long=" + longitude + "&units=" + UnitsToString(mConfig.units) + "&tzone=" + tzone, true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        //console.log(req.responseText);
        response = JSON.parse(req.responseText);
        var temperature, high, low, code, humidity, city, sunrise, sunset, fajr, dhuhur, asr, isha;
        var fc1h,fc1l,fc1d,fc1c,fc2h,fc2l,fc2d,fc2c,fc3h,fc3l,fc3d,fc3c;
        if (response) {
          var weatherResult = response;
          temperature = weatherResult.temp;
          code = weatherResult.code;
          high = weatherResult.high;
          low = weatherResult.low;
          humidity = weatherResult.humidity;
          city = weatherResult.city;
          sunrise = weatherResult.rise;
          sunset = weatherResult.set;
          fajr = weatherResult.fajr;
          dhuhur = weatherResult.dhuhur;
          asr = weatherResult.asr;
          isha = weatherResult.isha;
          
          fc1h = weatherResult.fc1h;
          fc1l = weatherResult.fc1l;
          fc1d = weatherResult.fc1d;
          fc1c = weatherResult.fc1c;
          fc2h = weatherResult.fc2h;
          fc2l = weatherResult.fc2l;
          fc2d = weatherResult.fc2d;
          fc2c = weatherResult.fc2c;
          fc3h = weatherResult.fc3h;
          fc3l = weatherResult.fc3l;
          fc3d = weatherResult.fc3d;
          fc3c = weatherResult.fc3c;

          Pebble.sendAppMessageWithRetry({
            "temperature": temperature,
            "icon": code,
            "high": high,
            "low": low,
            "humidity": humidity,
            "city": city,
            "sunrise": sunrise,
            "sunset": sunset,
            "fajr": fajr,
            "dhuhur": dhuhur,
            "asr": asr,
            "isha": isha,
            "fc1h": fc1h,
            "fc1l": fc1l,
            "fc1d": fc1d,
            "fc1c": fc1c,
            "fc2h": fc2h,
            "fc2l": fc2l,
            "fc2d": fc2d,
            "fc2c": fc2c,
            "fc3h": fc3h,
            "fc3l": fc3l,
            "fc3d": fc3d,
            "fc3c": fc3c
            }, 10);
        }

      } else {
        //console.log("Weather Error");
      }
    }
  };
  req.send(null);
}

function locationSuccess(pos) {
    //console.log("JS locationSuccess()");
    var coordinates = pos.coords;
    fetchWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
    //console.warn('JS locationError(' + err.code + '): ' + err.message);
    Pebble.sendAppMessageWithRetry({
    "temperature": 999,
    "icon": 48,
    "high": 0,
    "low": 0
    }, 10);
}

function saveLocalData(config) {
    localStorage.setItem("style", parseInt(config.style));  
    localStorage.setItem("bluetoothvibe", parseInt(config.bluetoothvibe)); 
    localStorage.setItem("hourlyvibe", parseInt(config.hourlyvibe)); 
    localStorage.setItem("units", parseInt(config.units));  
    localStorage.setItem("blink", parseInt(config.blink));
    localStorage.setItem("dateformat", parseInt(config.dateformat));	
	localStorage.setItem("language", parseInt(config.language));	

	localStorage.setItem("gpslat", config.gpslat===null?'':config.gpslat);
	localStorage.setItem("gpslon", config.gpslon===null?'':config.gpslon);

    loadLocalData();
}
function loadLocalData() {
    mConfig.style = parseInt(localStorage.getItem("style"));
    mConfig.bluetoothvibe = parseInt(localStorage.getItem("bluetoothvibe"));
    mConfig.hourlyvibe = parseInt(localStorage.getItem("hourlyvibe"));
    mConfig.units = parseInt(localStorage.getItem("units"));
    mConfig.blink = parseInt(localStorage.getItem("blink"));
    mConfig.dateformat = parseInt(localStorage.getItem("dateformat"));
	mConfig.language = parseInt(localStorage.getItem("language"));
    mConfig.configureUrl = "http://www.mirz.com/Chunk2/index2.1.html";
	mConfig.gpslat = localStorage.getItem("gpslat");
	mConfig.gpslon = localStorage.getItem("gpslon");

    if(isNaN(mConfig.style)) {
        mConfig.style = 1;
    }
    if(isNaN(mConfig.bluetoothvibe)) {
        mConfig.bluetoothvibe = 1;
    }
    if(isNaN(mConfig.hourlyvibe)) {
        mConfig.hourlyvibe = 0;
    }   
    if(isNaN(mConfig.units)) {
        mConfig.units = 1;
    } 
    if(isNaN(mConfig.blink)) {
        mConfig.blink = 1;
    } 
    if(isNaN(mConfig.dateformat)) {
        mConfig.dateformat = 0;
    } 
    if(isNaN(mConfig.language)) {
        mConfig.language = 0;
    } 
  
}
function returnConfigToPebble() {
    Pebble.sendAppMessageWithRetry({
        "style":parseInt(mConfig.style), 
        "bluetoothvibe":parseInt(mConfig.bluetoothvibe), 
        "hourlyvibe":parseInt(mConfig.hourlyvibe),
        "units":parseInt(mConfig.units),
        "blink":parseInt(mConfig.blink),
        "dateformat":parseInt(mConfig.dateformat),
		"language":parseInt(mConfig.language)
    }, 10);
    getWeather();
}
function UnitsToString(unit) {
  if(unit===0) {
    return "f";
  }
  return "c";
}

function getWeather() {
	if(mConfig.gpslat!=='' && mConfig.gpslon!=='') {
		//console.log("used fixed gps");
		fetchWeather(mConfig.gpslat, mConfig.gpslon);
	}
	else {
		//console.log("used auto gps");
		navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);		
	}
  
}

Pebble.addEventListener("ready", function(e) {
  loadLocalData();
  returnConfigToPebble();
});


Pebble.addEventListener("appmessage", function(e) {
  getWeather();
});

Pebble.addEventListener("showConfiguration", function(e) {
	Pebble.openURL(mConfig.configureUrl);
});

Pebble.addEventListener("webviewclosed", function(e) {
  if (e.response) {
    var config = JSON.parse(e.response);
    saveLocalData(config);
    returnConfigToPebble();
  }
});


