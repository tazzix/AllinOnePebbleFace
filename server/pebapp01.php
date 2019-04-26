<?php

// http://linvps.tazz.net/pebapp01.php?lat=33.68&long=73.04&units=c&tzone=5

define("FLICKR_KEY", "GETYOUROWNKEY"); // GET YOUR OWN KEY

include('PrayTime.php');
error_reporting(0);

$lat = $_GET['lat'];
$long = $_GET['long'];
$units = $_GET['units'];
$tzone = $_GET['tzone'];

if($lat=='' || $long=='' || $units=='') {
  $lat = "50.72471";
  $long = "-3.50782";
  $units = "c";
  $tzone = 5;
}

$prayTime = new PrayTime();
$prayTime->setCalcMethod($prayTime->Karachi);
$prayTime->setAsrMethod($prayTime->Hanafi);
$times = $prayTime->getPrayerTimes( time(), $lat, $long, $tzone);
$fajr = (int)str_replace(':','',$times[0]);
$sunrise = (int)str_replace(':','',$times[1]);
$dhuhur = (int)str_replace(':','',$times[2]);
$asr = (int)str_replace(':','',$times[3]);
$sunset = (int)str_replace(':','',$times[4]);
$maghrib = (int)str_replace(':','',$times[5]);
$isha = (int)str_replace(':','',$times[6]);
$prayers = $times;

$flickrResponse = get_data('https://api.flickr.com/services/rest/?method=flickr.places.findByLatLon&format=json&api_key=' . FLICKR_KEY . '&lat=' . $lat . '&lon=' . $long);
$flickrResponse = json_decode(substr($flickrResponse, 14, strlen($flickrResponse) - 15), true);
$woeid = $flickrResponse['places']['place'][0]['woeid'];
$woename = $flickrResponse['places']['place'][0]['woe_name'];
if($woeid) {
        $xml = simplexml_load_file('http://weather.yahooapis.com/forecastrss?w=' . $woeid . '&u=' . $units);
        $xml->registerXPathNamespace('yweather', 'http://xml.weather.yahoo.com/ns/rss/1.0');

        $condition = $xml->channel->item->xpath('yweather:condition');
        $code = (int)$condition[0]['code'];
        $temperature = (int)round($condition[0]['temp']);

        $atmosphere = $xml->channel->xpath('yweather:atmosphere');
        $humidity = (int)$atmosphere[0]['humidity'];
/*
        $astronomy = $xml->channel->xpath('yweather:astronomy');
        $sunrise_s = str_replace(' ', '', $astronomy[0]['sunrise']);
        $sunrise = str_replace(':', '', $sunrise_s);
        $sunrise = str_replace('am', '', $sunrise);
        $sunrise = (int)str_replace('pm', '', $sunrise);
        if(strpos($sunrise_s,'pm')>0) $sunrise += 1200;
        $sunset_s = str_replace(' ', '', $astronomy[0]['sunset']);
        $sunset = str_replace(':', '', $sunset_s);
        $sunset = str_replace('am', '', $sunset);
        $sunset = (int)str_replace('pm', '', $sunset);
        if(strpos($sunset_s,'pm')>0) $sunset += 1200;
*/
        $forecast = $xml->channel->item->xpath('yweather:forecast');
        $forecasthigh = (int)$forecast[0]['high'];
        $forecastlow = (int)$forecast[0]['low'];

        $fc1h = (int)$forecast[1]['high']; $fc1l = (int)$forecast[1]['low']; $fc1d = (string)$forecast[1]['day']; $fc1c = (int)$forecast[1]['code'];
        $fc2h = (int)$forecast[2]['high']; $fc2l = (int)$forecast[2]['low']; $fc2d = (string)$forecast[2]['day']; $fc2c = (int)$forecast[2]['code'];
        $fc3h = (int)$forecast[3]['high']; $fc3l = (int)$forecast[3]['low']; $fc3d = (string)$forecast[3]['day']; $fc3c = (int)$forecast[3]['code'];

        $location = $xml->channel->xpath('yweather:location');
        $city = $location[0]['city'];
}
else {

        $code = 3200;
        $temperature = 999;
        $forecasthigh = 99;
        $forecastlow = 0;
        $humidity = 99;

}
// yahoo code => watch face icon id // yahoo condition => watch face condition
/*
$icons = array(
        0 => 5, //tornado => wind
        1 => 5, //tropical storm => wind
        2 => 5, //hurricane => wind
        3 => 10, //severe thunderstorms => thunder
        4 => 10, //thunderstorms => thunder
        5 => 11, //mixed rain and snow => rain-snow
        6 => 12, //mixed rain and sleet => rain-sleet
        7 => 13, //mixed snow and sleet => snow-sleet
        8 => 2, //freezing drizzle => rain
        9 => 2, //drizzle => rain
        10 => 2, //freezing rain => rain
        11 => 2, //showers => rain
        12 => 2, //showers => rain
        13 => 3, //snow flurries => snow
        14 => 3, //light snow showers => snow
        15 => 3, //blowing snow => snow
        16 => 3, //snow => snow
        17 => 4, //hail => sleet
        18 => 4, //sleet => sleet
        19 => 6, //dust => fog
        20 => 6, //foggy => fog
        21 => 6, //haze => fog
        22 => 6, //smoky => fog
        23 => 5, //blustery => wind
        24 => 5, //windy => wind
        25 => 13, //cold => cold
        26 => 7, //cloudy => cloudy
        27 => 9, //mostly cloudy (night) => partly-cloudy-night
        28 => 8, //mostly cloudy (day) => partly-cloudy-day
        29 => 9, //partly cloudy (night) => partly-cloudy-night
        30 => 8, //partly cloudy (day) => partly-cloudy-day
        31 => 1, //clear (night) => clear-night
        32 => 0, //sunny => clear-day
        33 => 9, //fair (night) => partly-cloudy-night
        34 => 8, //fair (day) => partly-cloudy-day
        35 => 12, //mixed rain and hail => rain-sleet
        36 => 14, //hot => hot
        37 => 10, //isolated thunderstorms => thunder
        38 => 10, //scattered thunderstorms => thunder
        39 => 10, //scattered thunderstorms => thunder
        40 => 2, //scattered showers => rain
        41 => 3, //heavy snow => snow
        42 => 3, //scattered snow showers => snow
        43 => 3, //heavy snow => snow
        44 => 8, //partly cloudy => partly-cloudy-day
        45 => 10, //thundershowers => thunder
        46 => 3, //snow showers => snow
        47 => 10, //isolated thundershowers => thunder
        3200 => 15 //not available
);
*/
$data = array();

if($code == 3200) {
        $code=48;
}

$data["temp"] = $temperature;
$data["high"] = $forecasthigh; 
$data["low"] = $forecastlow;
$data["code"] = $code;
$data["rise"] = $sunrise;
$data["set"] = $sunset;
$data["city"] = $woename;
$data["humidity"] = $humidity;
$data["fajr"] = $fajr;
$data["dhuhur"] = $dhuhur;
$data["asr"] = $asr;
$data["isha"] = $isha;
$data["fc1h"] = $fc1h;
$data["fc1l"] = $fc1l;
$data["fc1d"] = $fc1d;
$data["fc1c"] = $fc1c;
$data["fc2h"] = $fc2h;
$data["fc2l"] = $fc2l;
$data["fc2d"] = $fc2d;
$data["fc2c"] = $fc2c;
$data["fc3h"] = $fc3h;
$data["fc3l"] = $fc3l;
$data["fc3d"] = $fc3d;
$data["fc3c"] = $fc3c;

//$data["prayers"] = $times;
/*
$logentry = date('Y-m-d H:i:s') . PHP_EOL . $pebbleid . PHP_EOL . json_encode($payload) . PHP_EOL . json_encode($data) . PHP_EOL; 
$fp = @fopen('log.txt', 'a');  
fputs($fp, $logentry . PHP_EOL);  
@fclose($fp);  
*/

header('Content-Type: application/json');
header('Cache-Control: max-age=840');
print json_encode($data);

function get_data($url) {
    $ch = curl_init();
    $timeout = 5;
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, $timeout);
    $data = curl_exec($ch);
    curl_close($ch);
    return $data;
}

?>
