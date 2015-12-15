#include <pebble.h>
#include <time.h>
#include <ctype.h>

#include "config.h"
#include "locale_helpers.h"

///////
language_t mLanguage = ENGLISH;
	
static Window *mWindow;
static Window *popup_window;

static Layer *mWindowLayer;
static Layer *mBackgroundLayer;

static BitmapLayer *mWeatherIconLayer;

static GBitmap *battery_image;
static BitmapLayer *battery_image_layer;

static InverterLayer *mInvertBottomLayer;
static InverterLayer *mInvertTopLayer;

static TextLayer *mLocationLayer;
static TextLayer *mDateLayer;
static Layer *mTimeLayer;
static TextLayer *mTimeHourLayer;
static TextLayer *mTimeSeparatorLayer;
static TextLayer *mTimeMinutesLayer;
static TextLayer *mTemperatureLayer;
static TextLayer *mHighLowLayer;
static TextLayer *mPedometerLayer;
static TextLayer *mHiLayer;
static TextLayer *mLoLayer;

static char mTemperatureText[8];
static char mHighLowText[50];
static char mPedometerText[50];
static char mHiText[5];
static char mLoText[5];

static GBitmap *mWeatherIcon;

static GFont *mDateFont;
static GFont *mTimeFont;
static GFont *mTemperatureFont;   
static GFont *mHighLowFont;

static int mTimerMinute = FREQUENCY_MINUTES;
static int mInitialMinute;

static int mConfigStyle;               //1=BlackOnWhite, 2=Split1(WhiteTop), 3=WhiteOnBlack, 4=Split2(BlackTop)
static int mConfigBluetoothVibe;       //0=off 1=on
static int mConfigHourlyVibe;          //0=off 1=on
static int mConfigWeatherUnit;         //1=Celsius 0=Fahrenheit
static int mConfigBlink;               //0=Static 1=Blink
static int mConfigDateFormat;          //0=Default 1=NoSuffix
static int mConfigLanguage;            //0=ENGLISH 1=ITALIAN 2=GERMAN 3=FRENCH 4=RUSSIAN

static int mTemperatureDegrees=999;    //-999 to 999
static int mTemperatureIcon=48;        //0 to 48
static int mTemperatureHigh=999;       //-999 to 999
static int mTemperatureLow=999;        //-999 to 999

static int mHumidity=99;
static int mRise=0;
static int mSet=0;
static int mNextPrayer=0;
static char mCity[50];
static int batteryPercent;

static int fc1h=0;
static int fc1l=0;
static int fc1c=0;
static int fc2h=0;
static int fc2l=0;
static int fc2c=0;
static int fc3h=0;
static int fc3l=0;
static int fc3c=0;
static char fc1d[4];
static char fc2d[4];
static char fc3d[4];

static bool popup;

//static int mPedometer=1;
//static int mLastPedo=0;
//static int mSleep=0;
//static int mIdle=0;
//static int mActivity=0;

static int mFajr=0;
static int mDhuhur=0;
static int mAsr=0;
static int mIsha=0;

static int currTime=0;

enum {
  STYLE_KEY = 0x0,                     // TUPLE_INT
  BLUETOOTHVIBE_KEY = 0x1,             // TUPLE_INT
  HOURLYVIBE_KEY = 0x2,                // TUPLE_INT
  WEATHER_UNITS = 0x3,                 // TUPLE_INT
  WEATHER_TEMPERATURE_KEY = 0x4,       // TUPLE_INT
  WEATHER_ICON_KEY = 0x5,              // TUPLE_INT
  WEATHER_mTemperatureHigh_KEY = 0x6,  // TUPLE_INT
  WEATHER_mTemperatureLow_KEY = 0x7,   // TUPLE_INT
  BLINK_KEY = 0x8,                     // TUPLE_INT
  DATEFORMAT_KEY = 0x9,                // TUPLE_INT
  LANGUAGE_KEY = 0xA,                  // TUPLE_INT
  WEATHER_HUMID_KEY = 0xB,
  WEATHER_RISE_KEY = 0xC,
  WEATHER_SET_KEY = 0xD,
  WEATHER_CITY_KEY = 0xE,
  PRAYER_FAJR_KEY = 0xF,
  PRAYER_DHUHUR_KEY = 0x10,
  PRAYER_ASR_KEY = 0x11,
  PRAYER_ISHA_KEY = 0x12
};

static uint8_t BATTERY_ICONS[] = {
	RESOURCE_ID_BATTERY_100,
	RESOURCE_ID_BATTERY_080,
	RESOURCE_ID_BATTERY_060,
	RESOURCE_ID_BATTERY_040,
	RESOURCE_ID_BATTERY_020,
	RESOURCE_ID_BATTERY_000
};

static uint8_t WEATHER_ICONS[] = {
	RESOURCE_ID_IMAGE_TORNADO,
	RESOURCE_ID_IMAGE_TROPICAL_STORM,
	RESOURCE_ID_IMAGE_HURRICANE,
	RESOURCE_ID_IMAGE_SEVERE_THUNDERSTORMS,
	RESOURCE_ID_IMAGE_THUNDERSTORMS,
	RESOURCE_ID_IMAGE_MIXED_RAIN_AND_SNOW,
	RESOURCE_ID_IMAGE_MIXED_RAIN_AND_SLEET,
	RESOURCE_ID_IMAGE_MIXED_SNOW_AND_SLEET,
	RESOURCE_ID_IMAGE_FREEZING_DRIZZLE,
	RESOURCE_ID_IMAGE_DRIZZLE,
	RESOURCE_ID_IMAGE_FREEZING_RAIN,
	RESOURCE_ID_IMAGE_SHOWERS,
	RESOURCE_ID_IMAGE_SHOWERS2,
	RESOURCE_ID_IMAGE_SNOW_FLURRIES,
	RESOURCE_ID_IMAGE_LIGHT_SNOW_SHOWERS,
	RESOURCE_ID_IMAGE_BLOWING_SNOW,
	RESOURCE_ID_IMAGE_SNOW,
	RESOURCE_ID_IMAGE_HAIL,
	RESOURCE_ID_IMAGE_SLEET,  
	RESOURCE_ID_IMAGE_DUST,
	RESOURCE_ID_IMAGE_FOGGY,
	RESOURCE_ID_IMAGE_HAZE,
	RESOURCE_ID_IMAGE_SMOKY,
	RESOURCE_ID_IMAGE_BLUSTERY,
	RESOURCE_ID_IMAGE_WINDY,
	RESOURCE_ID_IMAGE_COLD,
	RESOURCE_ID_IMAGE_CLOUDY,
	RESOURCE_ID_IMAGE_MOSTLY_CLOUDY_NIGHT,
	RESOURCE_ID_IMAGE_MOSTLY_CLOUDY_DAY,
	RESOURCE_ID_IMAGE_PARTLY_CLOUDY_NIGHT,
	RESOURCE_ID_IMAGE_PARTLY_CLOUDY_DAY,
	RESOURCE_ID_IMAGE_CLEAR_NIGHT,
	RESOURCE_ID_IMAGE_SUNNY,
	RESOURCE_ID_IMAGE_FAIR_NIGHT,
	RESOURCE_ID_IMAGE_FAIR_DAY,
	RESOURCE_ID_IMAGE_MIXED_RAIN_AND_HAIL,
	RESOURCE_ID_IMAGE_HOT,
	RESOURCE_ID_IMAGE_ISOLATED_THUNDERSTORMS,
	RESOURCE_ID_IMAGE_SCATTERED_THUNDERSTORMS,
	RESOURCE_ID_IMAGE_SCATTERED_THUNDERSTORMS2,
	RESOURCE_ID_IMAGE_SCATTERED_SHOWERS,
	RESOURCE_ID_IMAGE_HEAVY_SNOW,
	RESOURCE_ID_IMAGE_SCATTERED_SNOW_SHOWERS,
	RESOURCE_ID_IMAGE_HEAVY_SNOW2,
	RESOURCE_ID_IMAGE_PARTLY_CLOUDY,
	RESOURCE_ID_IMAGE_THUNDERSHOWERS,
	RESOURCE_ID_IMAGE_SNOW_SHOWERS,
	RESOURCE_ID_IMAGE_ISOLATED_THUNDERSHOWERS,
	RESOURCE_ID_IMAGE_NOT_AVAILABLE
};

typedef enum {
	WEATHER_ICON_TORNADO=0,
	WEATHER_ICON_TROPICAL_STORM=1,
	WEATHER_ICON_HURRICANE=2,
	WEATHER_ICON_SEVERE_THUNDERSTORMS=3,
	WEATHER_ICON_THUNDERSTORMS=4,
	WEATHER_ICON_MIXED_RAIN_AND_SNOW=5,
	WEATHER_ICON_MIXED_RAIN_AND_SLEET=6,
	WEATHER_ICON_MIXED_SNOW_AND_SLEET=7,
	WEATHER_ICON_FREEZING_DRIZZLE=8,
	WEATHER_ICON_DRIZZLE=9,
	WEATHER_ICON_FREEZING_RAIN=10,
	WEATHER_ICON_SHOWERS=11,
	WEATHER_ICON_SHOWERS2=12,
	WEATHER_ICON_SNOW_FLURRIES=13,
	WEATHER_ICON_LIGHT_SNOW_SHOWERS=14,
	WEATHER_ICON_BLOWING_SNOW=15,
	WEATHER_ICON_SNOW=16,
	WEATHER_ICON_HAIL=17,
	WEATHER_ICON_SLEET=18,
	WEATHER_ICON_DUST=19,
	WEATHER_ICON_FOGGY=20,
	WEATHER_ICON_HAZE=21,
	WEATHER_ICON_SMOKY=22,
	WEATHER_ICON_BLUSTERY=23,
	WEATHER_ICON_WINDY=24,
	WEATHER_ICON_COLD=25,
	WEATHER_ICON_CLOUDY=26,
	WEATHER_ICON_MOSTLY_CLOUDY_NIGHT=27,
	WEATHER_ICON_MOSTLY_CLOUDY_DAY=28,
	WEATHER_ICON_PARTLY_CLOUDY_NIGHT=29,
	WEATHER_ICON_PARTLY_CLOUDY_DAY=30,
	WEATHER_ICON_CLEAR_NIGHT=31,
	WEATHER_ICON_SUNNY=32,
	WEATHER_ICON_FAIR_NIGHT=33,
	WEATHER_ICON_FAIR_DAY=34,
	WEATHER_ICON_MIXED_RAIN_AND_HAIL=35,
	WEATHER_ICON_HOT=36,
	WEATHER_ICON_ISOLATED_THUNDERSTORMS=37,
	WEATHER_ICON_SCATTERED_THUNDERSTORMS=38,
	WEATHER_ICON_SCATTERED_THUNDERSTORMS2=39,
	WEATHER_ICON_SCATTERED_SHOWERS=40,
	WEATHER_ICON_HEAVY_SNOW=41,
	WEATHER_ICON_SCATTERED_SNOW_SHOWERS=42,
	WEATHER_ICON_HEAVY_SNOW2=43,
	WEATHER_ICON_PARTLY_CLOUDY=44,
	WEATHER_ICON_THUNDERSHOWERS=45,
	WEATHER_ICON_SNOW_SHOWERS=46,
	WEATHER_ICON_ISOLATED_THUNDERSHOWERS=47,
	WEATHER_ICON_NOT_AVAILABLE=48
} WeatherIcon;

#define SAMPLE_INTERVAL_S	8
#define BATCH_SIZE			10
#define SAMPLE_SIZE			(SAMPLE_INTERVAL_S * BATCH_SIZE)

// Total Steps (TS)
#define TS 1
// Total Steps Default (TSD)
#define TSD 1

// interval to check for next step (in ms)
const int ACCEL_STEP_MS = 475;
// value to auto adjust step acceptance 
const int PED_ADJUST = 2;
// steps required per calorie
const int STEPS_PER_CALORIE = 22;
// value by which step goal is incremented
const int STEP_INCREMENT = 50;
// values for max/min number of calibration options 
const int MAX_CALIBRATION_SETTINGS = 3;
const int MIN_CALIBRATION_SETTINGS = 1;

int X_DELTA = 43;//35;
int Y_DELTA, Z_DELTA = 200;//185;
int YZ_DELTA_MIN = 190;//175;
int YZ_DELTA_MAX = 210;//195; 
int X_DELTA_TEMP, Y_DELTA_TEMP, Z_DELTA_TEMP = 0;
int lastX, lastY, lastZ, currX, currY, currZ = 0;

long stepGoal = 5000;
//long pedometerCount = 0;
//long caloriesBurned = 0;
//long tempTotal = 0;

bool did_pebble_vibrate = false;
bool validX, validY, validZ = false;
bool startedSession = false;
//static long totalSteps = TSD;

// Timer used to determine next step check
static AppTimer *timer;
static AppTimer *popup_timer;
static int POPUP_TIMEOUT=10000;

char *upcase(char *str){
  char *s = str;
  while (*s) {
    *s++ = toupper((int)*s);
  }
  return str;
}

const char *getDaySuffix(int day) {
    if (day%100 > 10 && day%100 < 14)
        return "th";
    switch (day%10) {
        case 1:  return "st";
        case 2:  return "nd";
        case 3:  return "rd";
        default: return "th";
    };
}

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
  GBitmap *old_image = *bmp_image;
  *bmp_image = gbitmap_create_with_resource(resource_id);
  GRect frame = (GRect) {
    .origin = origin,
    .size = (*bmp_image)->bounds.size
  };
  bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
  layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);
  gbitmap_destroy(old_image);
}

static void remove_invert_top() {
  if(mInvertTopLayer!=NULL) {
    layer_remove_from_parent(inverter_layer_get_layer(mInvertTopLayer));
    inverter_layer_destroy(mInvertTopLayer);
    mInvertTopLayer = NULL;
  }
}

static void remove_invert_bottom() {
  if(mInvertBottomLayer!=NULL) {
    layer_remove_from_parent(inverter_layer_get_layer(mInvertBottomLayer));
    inverter_layer_destroy(mInvertBottomLayer);
    mInvertBottomLayer = NULL;
  }
}

static void set_invert_top() {
  if(!mInvertTopLayer) {
    mInvertTopLayer = inverter_layer_create(INVERT_TOP_FRAME);  
    layer_add_child(mWindowLayer, inverter_layer_get_layer(mInvertTopLayer));
  }
}

static void set_invert_bottom() {
  if(!mInvertBottomLayer) {
    mInvertBottomLayer = inverter_layer_create(INVERT_BOTTOM_FRAME);  
    layer_add_child(mWindowLayer, inverter_layer_get_layer(mInvertBottomLayer));
  }
}

static void setStyle() {
  remove_invert_top();
  remove_invert_bottom();
  layer_set_hidden(mBackgroundLayer, true);
  switch(mConfigStyle) {
    case 1:
	  layer_set_hidden(mBackgroundLayer, false);
      break;
    case 2:
      set_invert_bottom();
      break;
    case 3:
      set_invert_top();
      set_invert_bottom();
	  layer_set_hidden(mBackgroundLayer, false);
      break;
    case 4:
      set_invert_top();
      break;      
  }
}

void resetUpdate() {
	lastX = currX;
	lastY = currY;
	lastZ = currZ;
	validX = false;
	validY = false;
	validZ = false;
}

void update_ui_callback() {
  /*
	if ((validX && validY && !did_pebble_vibrate) || (validX && validZ && !did_pebble_vibrate)) {
		mPedometer++;
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "PEDO3 update ui: %d", mPedometer);
		//caloriesBurned = (int) (pedometerCount / STEPS_PER_CALORIE);
    snprintf(mPedometerText, sizeof(mPedometerText), "%05d  %02d:%02d  %02d:%02d", mPedometer, (int)(mSleep/60), mSleep%60, (int)(mNextPrayer/100), mNextPrayer%100);
    text_layer_set_text(mPedometerLayer, mPedometerText);  

		if (stepGoal > 0 && mPedometer == stepGoal) {
			vibes_long_pulse();
			vibes_long_pulse();
			//window_set_window_handlers(window, (WindowHandlers ) { .load = window_load, .unload = window_unload, });
			//window_stack_push(window, true);
		}
	}
  */
	resetUpdate();
}
/*
void autoCorrectZ(){
	if (Z_DELTA > YZ_DELTA_MAX){
		Z_DELTA = YZ_DELTA_MAX; 
	} else if (Z_DELTA < YZ_DELTA_MIN){
		Z_DELTA = YZ_DELTA_MIN;
	}
}

void autoCorrectY(){
	if (Y_DELTA > YZ_DELTA_MAX){
		Y_DELTA = YZ_DELTA_MAX; 
	} else if (Y_DELTA < YZ_DELTA_MIN){
		Y_DELTA = YZ_DELTA_MIN;
	}
}

void pedometer_update() {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "PEDO2 pedometer_update()");
	if (startedSession) {
		X_DELTA_TEMP = abs(abs(currX) - abs(lastX));
		if (X_DELTA_TEMP >= X_DELTA) {
			validX = true;
		}
		Y_DELTA_TEMP = abs(abs(currY) - abs(lastY));
		if (Y_DELTA_TEMP >= Y_DELTA) {
			validY = true;
			if (Y_DELTA_TEMP - Y_DELTA > 200){
				autoCorrectY();
				Y_DELTA = (Y_DELTA < YZ_DELTA_MAX) ? Y_DELTA + PED_ADJUST : Y_DELTA;
			} else if (Y_DELTA - Y_DELTA_TEMP > 175){
				autoCorrectY();
				Y_DELTA = (Y_DELTA > YZ_DELTA_MIN) ? Y_DELTA - PED_ADJUST : Y_DELTA;
			}
		}
		Z_DELTA_TEMP = abs(abs(currZ) - abs(lastZ));
		if (abs(abs(currZ) - abs(lastZ)) >= Z_DELTA) {
			validZ = true;
			if (Z_DELTA_TEMP - Z_DELTA > 200){
				autoCorrectZ();
				Z_DELTA = (Z_DELTA < YZ_DELTA_MAX) ? Z_DELTA + PED_ADJUST : Z_DELTA;
			} else if (Z_DELTA - Z_DELTA_TEMP > 175){
				autoCorrectZ();
				Z_DELTA = (Z_DELTA < YZ_DELTA_MAX) ? Z_DELTA + PED_ADJUST : Z_DELTA;
			}
		}
	} else {
		startedSession = true;
	}
}
*/
static void timer_callback(void *data) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "PEDO1 timer_callback()");
  /*
	AccelData accel = (AccelData ) { .x = 0, .y = 0, .z = 0 };
	accel_service_peek(&accel);

	if (!startedSession) {
		lastX = accel.x;
		lastY = accel.y;
		lastZ = accel.z;
	} else {
		currX = accel.x;
		currY = accel.y;
		currZ = accel.z;
	}
	
	did_pebble_vibrate = accel.did_vibrate;
  */
	//pedometer_update();
	update_ui_callback();

	//layer_mark_dirty(window_get_root_layer(pedometer));
	timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}


static void updateNextPrayer(void) {
  if(currTime < mFajr) mNextPrayer = mFajr;
  else if(currTime < mRise)   mNextPrayer = mRise;
  else if(currTime < mDhuhur) mNextPrayer = mDhuhur;
  else if(currTime < mAsr)    mNextPrayer = mAsr;
  else if(currTime < mSet)    mNextPrayer = mSet;
  else if(currTime < mIsha)   mNextPrayer = mIsha;
  else mNextPrayer = mFajr;
  
  snprintf(mPedometerText, sizeof(mPedometerText), "%02d:%02d %s", (int)(mNextPrayer/100), mNextPrayer%100, mCity);
  text_layer_set_text(mPedometerLayer, mPedometerText);  
}

void weather_set_icon(WeatherIcon icon) {	
  layer_remove_from_parent(bitmap_layer_get_layer(mWeatherIconLayer));
  mWeatherIconLayer = bitmap_layer_create(WEATHER_ICON_FRAME);  
  set_container_image(&mWeatherIcon, mWeatherIconLayer, WEATHER_ICONS[icon], GPoint(9, 2));  
	layer_add_child(mWindowLayer, bitmap_layer_get_layer(mWeatherIconLayer));
  
  //Reapply inverter
  setStyle();
}

void weather_set_temperature(int16_t t) {
	if(t==999) {
		//snprintf(mTemperatureText, sizeof(mTemperatureText), "%s\u00B0", "???");
	} else {
		snprintf(mTemperatureText, sizeof(mTemperatureText), "%d\u00B0", t);
	}
	
	text_layer_set_text(mTemperatureLayer, mTemperatureText);  
}

void weather_set_highlow(int16_t high, int16_t low, int16_t humidity, int16_t rise, int16_t set) {
	
  snprintf(mHighLowText, sizeof(mHighLowText), "%02d:%02d/%02d:%02d   P:%d%%", (int)(rise/100), rise-(((int)(rise/100))*100), (int)(set/100), set-(((int)(set/100))*100), humidity);
	text_layer_set_text(mHighLowLayer, mHighLowText);

  if((high==99 && low==0)||(high==0 && low==0)) {
    //snprintf(mHiText, sizeof(mHiText), "%s\u00B0", "?");
    //snprintf(mLoText, sizeof(mLoText), "%s\u00B0", "?");
	}
	else {
    snprintf(mHiText, sizeof(mHiText), "%d\u00B0", high);
    snprintf(mLoText, sizeof(mLoText), "%d\u00B0", low);
	}
	
	text_layer_set_text(mHighLowLayer, mHighLowText);
  text_layer_set_text(mHiLayer, mHiText);
  text_layer_set_text(mLoLayer, mLoText);
  
  updateNextPrayer();
}

void weather_set_loading() {
  //if(mTemperatureHigh!=99 && mTemperatureLow!=0) {
    weather_set_highlow(mTemperatureHigh,mTemperatureLow,mHumidity,mRise,mSet);
    weather_set_temperature(mTemperatureDegrees);
    weather_set_icon(mTemperatureIcon);
    //text_layer_set_text(mLocationLayer, mCity);
    return;
  /*}
  //snprintf(mCity, sizeof(mCity), "%s", "Location");
	snprintf(mHighLowText, sizeof(mHighLowText), "%s", "CHUNK v2.1"); //"LOW 999\u00B0 HIGH 999\u00B0"); //
  snprintf(mPedometerText, sizeof(mPedometerText), "%s", "22:45 50% Location");
  snprintf(mHiText, sizeof(mHiText), "%s", "Hi");
  snprintf(mLoText, sizeof(mLoText), "%s", "Lo");
  */
  //text_layer_set_text(mLocationLayer, mCity);
	//text_layer_set_text(mHighLowLayer, mHighLowText);
  //text_layer_set_text(mPedometerLayer, mPedometerText);
	//text_layer_set_text(mHiLayer, mHiText);
	//text_layer_set_text(mLoLayer, mLoText);
	//weather_set_icon(48);  
	//weather_set_temperature(999);
}

// HORIZONTAL LINE //
void update_background_callback(Layer *me, GContext* ctx) { 
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_draw_line(ctx, GPoint(0, 83), GPoint(144, 83));		
}

static void fetch_data(void);
static void popup_timeout(void *data);
static Layer *popupLayer;
static Layer *pBackLayer;
static BitmapLayer *pBatteryLayer;
static TextLayer *pFajrLayer;
static TextLayer *pRiseLayer;
static TextLayer *pDhuhurLayer;
static TextLayer *pAsrLayer;
static TextLayer *pSetLayer;
static TextLayer *pIshaLayer;
static char pFajr[8];
static char pRise[8];
static char pDhuhur[8];
static char pAsr[8];
static char pSet[12];
static char pIsha[8];
static TextLayer *pTimeLayer;
static char pTime[8];
static BitmapLayer *pFc1ImgLayer;
static GBitmap *pFc1Img;
static TextLayer *pFc1TxtLayer;
static char pFc1Txt[50];
static BitmapLayer *pFc2ImgLayer;
static GBitmap *pFc2Img;
static TextLayer *pFc2TxtLayer;
static char pFc2Txt[50];
static BitmapLayer *pFc3ImgLayer;
static GBitmap *pFc3Img;

static void popup_init(void) {
  popup_window = window_create();
  
  popupLayer = window_get_root_layer(popup_window);
  window_set_background_color(popup_window, GColorWhite);
  
  // BACKGROUND //
  pBackLayer = layer_create(layer_get_frame(popupLayer));
  layer_add_child(popupLayer, pBackLayer);
  layer_set_update_proc(pBackLayer, update_background_callback);
	
  //BATTERY_ICONS
  GRect frame4 = (GRect) {
    .origin = { .x = 50, .y = 82 },
    .size = battery_image->bounds.size
  };
  pBatteryLayer = bitmap_layer_create(frame4);
  bitmap_layer_set_bitmap(pBatteryLayer, battery_image);
  layer_add_child(popupLayer, bitmap_layer_get_layer(pBatteryLayer));
  
  pFajrLayer = text_layer_create(GRect( 0,  84, 60, 20));
	text_layer_set_font(pFajrLayer, mHighLowFont);
	text_layer_set_text_alignment(pFajrLayer, GTextAlignmentLeft);
  snprintf(pFajr, sizeof(pFajr), "%02d:%02d", (int)mFajr/100, mFajr%100);
  text_layer_set_text(pFajrLayer, pFajr);
	layer_add_child(popupLayer, text_layer_get_layer(pFajrLayer));

  pRiseLayer = text_layer_create(  GRect(60,  84, 60, 20));
	text_layer_set_font(pRiseLayer, mHighLowFont);
	text_layer_set_text_alignment(pRiseLayer, GTextAlignmentLeft);
  snprintf(pRise, sizeof(pRise), "%02d:%02d", (int)mRise/100, mRise%100);
  text_layer_set_text(pRiseLayer, pRise);
	layer_add_child(popupLayer, text_layer_get_layer(pRiseLayer));

  pDhuhurLayer = text_layer_create(GRect( 0, 104, 60, 20));
	text_layer_set_font(pDhuhurLayer, mHighLowFont);
	text_layer_set_text_alignment(pDhuhurLayer, GTextAlignmentLeft);
  snprintf(pDhuhur, sizeof(pDhuhur), "%02d:%02d", (int)mDhuhur/100, mDhuhur%100);
  text_layer_set_text(pDhuhurLayer, pDhuhur);
	layer_add_child(popupLayer, text_layer_get_layer(pDhuhurLayer));

  pAsrLayer = text_layer_create(   GRect( 0, 124, 60, 20));
	text_layer_set_font(pAsrLayer, mHighLowFont);
	text_layer_set_text_alignment(pAsrLayer, GTextAlignmentLeft);
  snprintf(pAsr, sizeof(pAsr), "%02d:%02d", (int)mAsr/100, mAsr%100);
  text_layer_set_text(pAsrLayer, pAsr);
	layer_add_child(popupLayer, text_layer_get_layer(pAsrLayer));

  pSetLayer = text_layer_create(   GRect(60, 104, 84, 20));
	text_layer_set_font(pSetLayer, mHighLowFont);
	text_layer_set_text_alignment(pSetLayer, GTextAlignmentLeft);
  snprintf(pSet, sizeof(pSet), "%02d:%02d %02d%%", (int)mSet/100, mSet%100, batteryPercent);
  text_layer_set_text(pSetLayer, pSet);
	layer_add_child(popupLayer, text_layer_get_layer(pSetLayer));

  pIshaLayer = text_layer_create(  GRect( 0, 144, 60, 20));
	text_layer_set_font(pIshaLayer, mHighLowFont);
	text_layer_set_text_alignment(pIshaLayer, GTextAlignmentLeft);
  snprintf(pIsha, sizeof(pIsha), "%02d:%02d", (int)mIsha/100, mIsha%100);
  text_layer_set_text(pIshaLayer, pIsha);
	layer_add_child(popupLayer, text_layer_get_layer(pIshaLayer));

  pTimeLayer = text_layer_create(  GRect( 60, 124, 80, 40));
	text_layer_set_font(pTimeLayer, mTemperatureFont);
	text_layer_set_text_alignment(pTimeLayer, GTextAlignmentLeft);
  snprintf(pTime, sizeof(pTime), "%02d:%02d", (int)currTime/100, currTime%100);
  text_layer_set_text(pTimeLayer, pTime);
	layer_add_child(popupLayer, text_layer_get_layer(pTimeLayer));
  
  pFc1ImgLayer = bitmap_layer_create(GRect( 0,0,50,50));  
  set_container_image(&pFc1Img, pFc1ImgLayer, WEATHER_ICONS[fc1c], GPoint(0, 0));  
	layer_add_child(popupLayer, bitmap_layer_get_layer(pFc1ImgLayer));
  pFc2ImgLayer = bitmap_layer_create(GRect(48,0,50,50));  
  set_container_image(&pFc2Img, pFc2ImgLayer, WEATHER_ICONS[fc2c], GPoint(48, 0));  
	layer_add_child(popupLayer, bitmap_layer_get_layer(pFc2ImgLayer));
  pFc3ImgLayer = bitmap_layer_create(GRect(96,0,50,50));  
  set_container_image(&pFc3Img, pFc3ImgLayer, WEATHER_ICONS[fc3c], GPoint(96, 0));  
	layer_add_child(popupLayer, bitmap_layer_get_layer(pFc3ImgLayer));

  pFc2TxtLayer = text_layer_create(GRect(0, 60, 144, 20));
	text_layer_set_font(pFc2TxtLayer, mHighLowFont);
	text_layer_set_text_alignment(pFc2TxtLayer, GTextAlignmentLeft);
  snprintf(pFc2Txt, sizeof(pFc2Txt), " %s      %s      %s", fc1d,fc2d,fc3d);
  text_layer_set_text(pFc2TxtLayer, pFc2Txt);
	layer_add_child(popupLayer, text_layer_get_layer(pFc2TxtLayer));
  pFc1TxtLayer = text_layer_create(GRect(0, 46, 144, 20));
	text_layer_set_font(pFc1TxtLayer, mHighLowFont);
	text_layer_set_text_alignment(pFc1TxtLayer, GTextAlignmentLeft);
  snprintf(pFc1Txt, sizeof(pFc1Txt), "%02d/%02d   %02d/%02d   %02d/%02d", fc1h,fc1l, fc2h,fc2l, fc3h,fc3l);
  text_layer_set_text(pFc1TxtLayer, pFc1Txt);
	layer_add_child(popupLayer, text_layer_get_layer(pFc1TxtLayer));
}

static void popup_deinit(void) {
  layer_remove_from_parent(text_layer_get_layer(pFc2TxtLayer));
  text_layer_destroy(pFc2TxtLayer);
  layer_remove_from_parent(text_layer_get_layer(pFc1TxtLayer));
  text_layer_destroy(pFc1TxtLayer);
  layer_remove_from_parent(bitmap_layer_get_layer(pFc3ImgLayer));
  bitmap_layer_destroy(pFc3ImgLayer);
  layer_remove_from_parent(bitmap_layer_get_layer(pFc2ImgLayer));
  bitmap_layer_destroy(pFc2ImgLayer);
  layer_remove_from_parent(bitmap_layer_get_layer(pFc1ImgLayer));
  bitmap_layer_destroy(pFc1ImgLayer);
  layer_remove_from_parent(text_layer_get_layer(pTimeLayer));
  text_layer_destroy(pTimeLayer);
  layer_remove_from_parent(text_layer_get_layer(pIshaLayer));
  text_layer_destroy(pIshaLayer);
  layer_remove_from_parent(text_layer_get_layer(pSetLayer));
  text_layer_destroy(pSetLayer);
  layer_remove_from_parent(text_layer_get_layer(pAsrLayer));
  text_layer_destroy(pAsrLayer);
  layer_remove_from_parent(text_layer_get_layer(pDhuhurLayer));
  text_layer_destroy(pDhuhurLayer);
  layer_remove_from_parent(text_layer_get_layer(pRiseLayer));
  text_layer_destroy(pRiseLayer);
  layer_remove_from_parent(text_layer_get_layer(pFajrLayer));
  text_layer_destroy(pFajrLayer);
  layer_remove_from_parent(bitmap_layer_get_layer(pBatteryLayer));
  bitmap_layer_destroy(pBatteryLayer);
  window_destroy(popup_window);
  app_timer_cancel(popup_timer);
}

static void showPopup() {  
	popup_init();
	window_stack_push(popup_window, true);
  
  popup_timer = app_timer_register(POPUP_TIMEOUT, popup_timeout, NULL);
}

static void hidePopup() {
	window_stack_pop(true);
	popup_deinit();
}

static void popup_timeout(void *data) {
  hidePopup();
  popup=false;
}

static void handle_shake(AccelAxisType axis, int32_t direction) {
  if(popup) hidePopup();
  else showPopup();
  
  popup = !popup;
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {

  if(mTemperatureDegrees == 999) fetch_data();

  if (units_changed & DAY_UNIT) {
  
    static char date_day[6];
    static char date_monthday[3];
    static char date_month[8];    
    static char full_date_text[26];
    
    //if(startedSession) mPedometer = TSD;
    
    /*strftime(date_day,
               sizeof(date_day),
               "%a",
               tick_time);*/

	snprintf(date_day, 
                sizeof(date_day), 
                "%s",
			 locale_day_name(tick_time->tm_wday, mLanguage));
	  
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "DAY NAME: %s", locale_day_name(tick_time->tm_wday, mLanguage));

    strftime(date_monthday,
             sizeof(date_monthday),
             "%d",
             tick_time);
             
    if (date_monthday[0] == '0') {
      memmove(&date_monthday[0], 
              &date_monthday[1], 
              sizeof(date_monthday) - 1); //remove leading zero
    }

    /* strftime(date_month,
             sizeof(date_month),
             "%b",
             tick_time); */
	  

	snprintf(date_month, 
                sizeof(date_month), 
                "%s",
			 locale_month_name(tick_time->tm_mon, mLanguage)); //tick_time->tm_mon
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "MONTH NAME: %s", locale_month_name(tick_time->tm_mon, mLanguage));
             
    if(mConfigDateFormat==0) {
      snprintf(full_date_text, 
                sizeof(full_date_text), 
                "%s %s%s %s", 
                date_day, //upcase(date_day), 
                date_monthday, 
                getDaySuffix(tick_time->tm_mday),
                date_month); //upcase(date_month)); 
    }
    else {
      snprintf(full_date_text, 
                sizeof(full_date_text), 
                "%s %s %s", 
                date_day, //upcase(date_day), 
                date_monthday, 
                date_month); //upcase(date_month)); 
    }
    text_layer_set_text(mDateLayer, full_date_text);
  }
	
    
  if (units_changed & HOUR_UNIT) { 
    static char hour_text[] = "00";
    /*
    if(tick_time->tm_hour==20) { 
      mSleep = 0;
      snprintf(mPedometerText, sizeof(mPedometerText), "%05d  %02d:%02d  %02d:%02d", mPedometer, (int)(mSleep/60), mSleep%60, (int)(mNextPrayer/100), mNextPrayer%100);
      text_layer_set_text(mPedometerLayer, mPedometerText);  
    }*/
    
    if(mConfigHourlyVibe) {
      //vibe!
      vibes_short_pulse();
    }
    if(mConfigStyle==5) {
      if(tick_time->tm_hour<6 || tick_time->tm_hour>17) {
        //nightMode();
      }
    }
    if (tick_time->tm_hour%2==0) fetch_data();
    if(clock_is_24h_style()) {
      strftime(hour_text, sizeof(hour_text), "%H", tick_time);
    }
    else {
      strftime(hour_text, sizeof(hour_text), "%I", tick_time);
      if (hour_text[0] == '0') {
        layer_set_frame(mTimeLayer, GRect(-13, 0, 144, 168)); //shift time left to centralise it
        memmove(&hour_text[0], &hour_text[1], sizeof(hour_text) - 1); //remove leading zero
      }
      else {
        layer_set_frame(mTimeLayer, GRect(0, 0, 144, 168));
      }
    }
    text_layer_set_text(mTimeHourLayer, hour_text);
  }
  if (units_changed & MINUTE_UNIT) {
    static char minute_text[] = "00";	
    strftime(minute_text, sizeof(minute_text), "%M", tick_time);	
    text_layer_set_text(mTimeMinutesLayer, minute_text);
    
    currTime = (tick_time->tm_hour*100)+tick_time->tm_min;
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "C/N: %d, %d", currTime, mNextPrayer);
    
    if(currTime==mNextPrayer) {
      // Prayer alarm;
      vibes_double_pulse();
      vibes_double_pulse();
      vibes_double_pulse();
    }
    if(currTime>(mNextPrayer+15)) {
      // update for next prayer
      fetch_data();
      updateNextPrayer();
    }

    //if(startedSession) {
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "IDLING.....:%d:%d", mIdle, mActivity);
      //mIdle++;
      //mActivity=mPedometer-mLastPedo;
      //if(mIdle>4) {
        //mSleep++; // mIdle;
        //mIdle=0;
        //TODO: fix
        //snprintf(mPedometerText, sizeof(mPedometerText), "%05d  %02d:%02d  %02d:%02d", mPedometer, (int)(mSleep/60), mSleep%60, (int)(mNextPrayer/100), mNextPrayer%100);
        //snprintf(mPedometerText, sizeof(mPedometerText), "%02d:%02d %d%% %s", (int)(mNextPrayer/100), mNextPrayer%100, batteryPercent, mCity);
        //text_layer_set_text(mPedometerLayer, mPedometerText);  
      //}
      /*
      if(mActivity>15) {
        mIdle=0;
        mLastPedo=mPedometer;
      }
      */
    //}
    /*
    if(FREQUENCY_MINUTES >= mTimerMinute || mTemperatureDegrees == 999) {
      fetch_data();
      mTimerMinute = 0;
    }
    else {
      mTimerMinute++;
    } 
    */
  }
  if (mConfigBlink && (units_changed & SECOND_UNIT)) {
    layer_set_hidden(text_layer_get_layer(mTimeSeparatorLayer), tick_time->tm_sec%2);
  }
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  bool setHighLow = false;
  Tuple *style_tuple = dict_find(iter, STYLE_KEY);
  if (style_tuple && style_tuple->value->uint8 != mConfigStyle) {
    mConfigStyle = style_tuple->value->uint8;
    setStyle();
  }
  Tuple *bluetoothvibe_tuple = dict_find(iter, BLUETOOTHVIBE_KEY);
  if (bluetoothvibe_tuple) {
    mConfigBluetoothVibe = bluetoothvibe_tuple->value->uint8;
  }
  Tuple *hourlyvibe_tuple = dict_find(iter, HOURLYVIBE_KEY);
  if (hourlyvibe_tuple) {
    mConfigHourlyVibe = hourlyvibe_tuple->value->uint8;
  }  
  Tuple *blink_tuple = dict_find(iter, BLINK_KEY);
  if (blink_tuple) {
    mConfigBlink = blink_tuple->value->uint8;
    tick_timer_service_unsubscribe();
    if(mConfigBlink) {
      tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
    }
    else {
	  layer_set_hidden(text_layer_get_layer(mTimeSeparatorLayer), false);
      tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
    }
  }
  Tuple *dateformat_tuple = dict_find(iter, DATEFORMAT_KEY);
  if (dateformat_tuple && dateformat_tuple->value->uint8 != mConfigDateFormat) {
    mConfigDateFormat = dateformat_tuple->value->uint8;
    time_t now = time(NULL);
    struct tm *tick_time = localtime(&now);  
    handle_tick(tick_time, DAY_UNIT);
  }  
  Tuple *units_tuple = dict_find(iter, WEATHER_UNITS);
  if (units_tuple) {
    if(units_tuple->value->uint8 != mConfigWeatherUnit) {
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "UNIT! %d, %d", mConfigWeatherUnit, units_tuple->value->uint8);
        mConfigWeatherUnit = units_tuple->value->uint8;
        fetch_data();
		return;
    }
  }
  Tuple *language_tuple = dict_find(iter, LANGUAGE_KEY);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "LANGUAGE_KEY! INIT");
	if (language_tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "LANGUAGE_KEY! %d", language_tuple->value->uint8);
	}
  if (language_tuple && language_tuple->value->uint8 != mConfigLanguage) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "mConfigLanguage! %d", mConfigLanguage);
	mConfigLanguage = language_tuple->value->uint8;
	switch(mConfigLanguage) {
        case 1: 
  			mLanguage = ITALIAN;
			break;
        ;
        case 2: 
			mLanguage = GERMAN;
			break;
        ;
        case 3: 
			mLanguage = FRENCH;
			break;
        ;      
        case 4: 
			mLanguage = RUSSIAN;
			break;
		;
        default: 
			mLanguage = ENGLISH;
        ;		
	}
    time_t now = time(NULL);
    struct tm *tick_time = localtime(&now);  
    handle_tick(tick_time, DAY_UNIT + HOUR_UNIT + MINUTE_UNIT + SECOND_UNIT);
	  setHighLow = true;
  }
  Tuple *weather_temperature_tuple = dict_find(iter, WEATHER_TEMPERATURE_KEY);
  if (weather_temperature_tuple && weather_temperature_tuple->value->int16 != mTemperatureDegrees) {
    mTemperatureDegrees = weather_temperature_tuple->value->int16;
    weather_set_temperature(mTemperatureDegrees);
  }
  Tuple *weather_icon_tuple = dict_find(iter, WEATHER_ICON_KEY);
  if (weather_icon_tuple && weather_icon_tuple->value->uint8 != mTemperatureIcon) {
    mTemperatureIcon = weather_icon_tuple->value->uint8;
    weather_set_icon(mTemperatureIcon);
  }
  Tuple *weather_high_tuple = dict_find(iter, WEATHER_mTemperatureHigh_KEY);
  if (weather_high_tuple && weather_high_tuple->value->int16 != mTemperatureHigh) {
    mTemperatureHigh = weather_high_tuple->value->int16;
    setHighLow = true;
  }
  Tuple *weather_low_tuple = dict_find(iter, WEATHER_mTemperatureLow_KEY);
  if (weather_low_tuple && weather_low_tuple->value->int16 != mTemperatureLow) {
    mTemperatureLow = weather_low_tuple->value->int16;
    setHighLow = true;
  }
  Tuple *weather_humidity = dict_find(iter, WEATHER_HUMID_KEY);
  if (weather_humidity && weather_humidity->value->int16 != mHumidity) {
    mHumidity = weather_humidity->value->int16;
    setHighLow = true;
  }
  Tuple *weather_sunrise = dict_find(iter, WEATHER_RISE_KEY);
  if (weather_sunrise && weather_sunrise->value->int16 != mRise) {
    mRise = weather_sunrise->value->int16;
    setHighLow = true;
  }
  Tuple *weather_sunset = dict_find(iter, WEATHER_SET_KEY);
  if (weather_sunset && weather_sunset->value->int16 != mSet) {
    mSet = weather_sunset->value->int16;
    setHighLow = true;
  }
  Tuple *weather_city = dict_find(iter, WEATHER_CITY_KEY);
  if(weather_city && weather_city->value) {
    memcpy(mCity, weather_city->value->cstring, weather_city->length);
    //text_layer_set_text(mLocationLayer, mCity);
  }
  
  Tuple *prayer_fajr = dict_find(iter, PRAYER_FAJR_KEY);
  if(prayer_fajr && prayer_fajr->value->int16 != mFajr) {
    mFajr = prayer_fajr->value->int16;
  }
  Tuple *prayer_dhuhur = dict_find(iter, PRAYER_DHUHUR_KEY);
  if(prayer_dhuhur && prayer_dhuhur->value->int16 != mDhuhur) {
    mDhuhur = prayer_dhuhur->value->int16;
  }
  Tuple *prayer_asr = dict_find(iter, PRAYER_ASR_KEY);
  if(prayer_asr && prayer_asr->value->int16 != mFajr) {
    mAsr = prayer_asr->value->int16;
  }
  Tuple *prayer_isha = dict_find(iter, PRAYER_ISHA_KEY);
  if(prayer_isha && prayer_isha->value->int16 != mFajr) {
    mIsha = prayer_isha->value->int16;
  }
  Tuple *t1;
  t1 = dict_find(iter,30); if(t1&&t1->value->int16!=fc1h) fc1h = t1->value->int16;
  t1 = dict_find(iter,31); if(t1&&t1->value->int16!=fc1l) fc1l = t1->value->int16;
  t1 = dict_find(iter,33); if(t1&&t1->value->int16!=fc1c) fc1c = t1->value->int16;
  t1 = dict_find(iter,40); if(t1&&t1->value->int16!=fc2h) fc2h = t1->value->int16;
  t1 = dict_find(iter,41); if(t1&&t1->value->int16!=fc2l) fc2l = t1->value->int16;
  t1 = dict_find(iter,43); if(t1&&t1->value->int16!=fc2c) fc2c = t1->value->int16;
  t1 = dict_find(iter,50); if(t1&&t1->value->int16!=fc3h) fc3h = t1->value->int16;
  t1 = dict_find(iter,51); if(t1&&t1->value->int16!=fc3l) fc3l = t1->value->int16;
  t1 = dict_find(iter,53); if(t1&&t1->value->int16!=fc3c) fc3c = t1->value->int16;
  t1 = dict_find(iter,32); if(t1&&t1->value) memcpy(fc1d,t1->value->cstring,t1->length);
  t1 = dict_find(iter,42); if(t1&&t1->value) memcpy(fc2d,t1->value->cstring,t1->length);
  t1 = dict_find(iter,52); if(t1&&t1->value) memcpy(fc3d,t1->value->cstring,t1->length);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "PT:%s:%d,%d,%d,%d,%d,%d", mCity, mFajr, mRise, mDhuhur, mAsr, mSet, mIsha);
  
  if(setHighLow) {
    weather_set_highlow(mTemperatureHigh, mTemperatureLow, mHumidity, mRise, mSet);
  }  
}

static void toggle_bluetooth(bool connected) {
  if(!connected && mConfigBluetoothVibe) {
    //vibe!
    vibes_long_pulse();
  }
}
void bluetooth_connection_callback(bool connected) {
  toggle_bluetooth(connected);
}

static void fetch_data(void) {

  Tuplet style_tuple = TupletInteger(STYLE_KEY, 0);
  Tuplet bluetoothvibe_tuple = TupletInteger(BLUETOOTHVIBE_KEY, 0);
  Tuplet hourlyvibe_tuple = TupletInteger(HOURLYVIBE_KEY, 0);
  Tuplet blink_tuple = TupletInteger(BLINK_KEY, 0);
  Tuplet dateformat_tuple = TupletInteger(DATEFORMAT_KEY, 0);
  Tuplet language_tuple = TupletInteger(LANGUAGE_KEY, 0);
  Tuplet units_tuple = TupletInteger(WEATHER_UNITS, 0);
  
  Tuplet weather_temperature_tuple = TupletInteger(WEATHER_TEMPERATURE_KEY, 0);
  Tuplet weather_icon_tuple = TupletInteger(WEATHER_ICON_KEY, 0);
  Tuplet weather_high_tuple = TupletInteger(WEATHER_mTemperatureHigh_KEY, 0);
  Tuplet weather_low_tuple = TupletInteger(WEATHER_mTemperatureLow_KEY, 0);
  Tuplet weather_sunrise = TupletInteger(WEATHER_RISE_KEY, 0);
  Tuplet weather_sunset = TupletInteger(WEATHER_RISE_KEY, 0);
  Tuplet weather_humidity = TupletInteger(WEATHER_HUMID_KEY, 0);
  Tuplet weather_city = TupletCString(WEATHER_CITY_KEY, "Location");

  Tuplet prayer_fajr = TupletInteger(PRAYER_FAJR_KEY, 0);
  Tuplet prayer_dhuhur = TupletInteger(PRAYER_DHUHUR_KEY, 0);
  Tuplet prayer_asr = TupletInteger(PRAYER_ASR_KEY, 0);
  Tuplet prayer_isha = TupletInteger(PRAYER_ISHA_KEY, 0);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &style_tuple);
  dict_write_tuplet(iter, &bluetoothvibe_tuple);
  dict_write_tuplet(iter, &hourlyvibe_tuple);
  dict_write_tuplet(iter, &units_tuple);
  dict_write_tuplet(iter, &weather_temperature_tuple);
  dict_write_tuplet(iter, &weather_icon_tuple);
  dict_write_tuplet(iter, &weather_high_tuple);
  dict_write_tuplet(iter, &weather_low_tuple);
  dict_write_tuplet(iter, &blink_tuple);
  dict_write_tuplet(iter, &dateformat_tuple);
  dict_write_tuplet(iter, &language_tuple);
  dict_write_tuplet(iter, &weather_sunrise);
  dict_write_tuplet(iter, &weather_sunset);
  dict_write_tuplet(iter, &weather_humidity);
  dict_write_tuplet(iter, &weather_city);
  dict_write_tuplet(iter, &prayer_fajr);
  dict_write_tuplet(iter, &prayer_dhuhur);
  dict_write_tuplet(iter, &prayer_asr);
  dict_write_tuplet(iter, &prayer_isha);
  dict_write_end(iter);

  app_message_outbox_send();
}

static void app_message_init(void) {
  app_message_register_inbox_received(in_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  //// pebble_pedometer
  //accel_data_service_subscribe(0, NULL);
  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
	
  //// On11
  //accel_data_service_subscribe(BATCH_SIZE, &processAccelerometerData);
	//accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);

  fetch_data();
}

static void update_battery(BatteryChargeState charge_state) {
  uint8_t img;

  batteryPercent = charge_state.charge_percent;
  
  if(batteryPercent>=90) {
     img = 0;
  }
  else if(batteryPercent>=80) {
     img = 1;
  }
  else if(batteryPercent>=60) {
     img = 2;
  }
  else if(batteryPercent>=40) {
     img = 3;
  }
  else if(batteryPercent>=20) {
     img = 4;
  }
  else {
     img = 5;
  }
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "BATTERY %d %d", batteryPercent, img);
  set_container_image(&battery_image, battery_image_layer, BATTERY_ICONS[img], GPoint(50, 82));
}

void handle_init(void) {

  //mPedometer = persist_exists(1) ? persist_read_int(1) : 1;

  mTemperatureDegrees = persist_exists(2) ? persist_read_int(2) : 0;
  mTemperatureIcon = persist_exists(3) ? persist_read_int(3) : 0;
  mTemperatureHigh = persist_exists(4) ? persist_read_int(4) : 0;
  mTemperatureLow = persist_exists(5) ? persist_read_int(5) : 0;
  mHumidity = persist_exists(6) ? persist_read_int(6) : 0;
  mRise = persist_exists(7) ? persist_read_int(7) : 0;
  mSet = persist_exists(8) ? persist_read_int(8) : 0;
  if(persist_exists(9)) persist_read_string(9, mCity, sizeof(mCity));
  //mSleep = persist_exists(10) ? persist_read_int(10) : 0;
  mFajr = persist_exists(11) ? persist_read_int(11) : 0;
  mDhuhur = persist_exists(12) ? persist_read_int(12) : 0;
  mAsr = persist_exists(13) ? persist_read_int(13) : 0;
  mIsha = persist_exists(14) ? persist_read_int(14) : 0;
  
  fc1h = persist_exists(30)?persist_read_int(30):0;
  fc1l = persist_exists(31)?persist_read_int(31):0;
  if(persist_exists(32)) persist_read_string(32, fc1d, sizeof(fc1d));
  fc1c = persist_exists(33)?persist_read_int(33):0;
  fc2h = persist_exists(40)?persist_read_int(40):0;
  fc2l = persist_exists(41)?persist_read_int(41):0;
  if(persist_exists(42)) persist_read_string(42, fc2d, sizeof(fc2d));
  fc2c = persist_exists(43)?persist_read_int(43):0;
  fc3h = persist_exists(50)?persist_read_int(50):0;
  fc3l = persist_exists(51)?persist_read_int(51):0;
  if(persist_exists(52)) persist_read_string(52, fc3d, sizeof(fc3d));
  fc3c = persist_exists(53)?persist_read_int(53):0;

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "persist_read %d:%d (%d)", TS, mPedometer, persist_exists(TS));
  
  // WINDOW //
  mWindow = window_create();
  if (mWindow == NULL) {
      return;
  }
  window_stack_push(mWindow, true /* Animated */);
  mWindowLayer = window_get_root_layer(mWindow);
  window_set_background_color(mWindow, GColorWhite);
  
  // BACKGROUND //
  mBackgroundLayer = layer_create(layer_get_frame(mWindowLayer));
  layer_add_child(mWindowLayer, mBackgroundLayer);
  layer_set_update_proc(mBackgroundLayer, update_background_callback);
	
  //BATTERY_ICONS
  battery_image = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_100);
  GRect frame4 = (GRect) {
    .origin = { .x = 50, .y = 90 },
    .size = battery_image->bounds.size
  };

  battery_image_layer = bitmap_layer_create(frame4);
  bitmap_layer_set_bitmap(battery_image_layer, battery_image);
  layer_add_child(mWindowLayer, bitmap_layer_get_layer(battery_image_layer));

  // FONTS //
	ResHandle res_d = resource_get_handle(RESOURCE_ID_SMALL_26);
	ResHandle res_t = resource_get_handle(RESOURCE_ID_BIG_52);
	ResHandle res_temp = resource_get_handle(RESOURCE_ID_MEDIUM_34);
	//ResHandle res_hl = resource_get_handle(RESOURCE_ID_SMALL_22);
  
	mDateFont = fonts_load_custom_font(res_d);
	mTimeFont = fonts_load_custom_font(res_t);
	mTemperatureFont = fonts_load_custom_font(res_temp);
	mHighLowFont = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);//fonts_load_custom_font(res_hl);
  
  // TIME LAYER //  
  mTimeLayer = layer_create(layer_get_frame(mWindowLayer));
  layer_add_child(mWindowLayer, mTimeLayer);
  
  // TIME HOUR LAYER //
  mTimeHourLayer = text_layer_create(TIME_HOUR_FRAME);  
	text_layer_set_background_color(mTimeHourLayer, GColorClear);
  text_layer_set_text_color(mTimeHourLayer, GColorBlack);
	text_layer_set_font(mTimeHourLayer, mTimeFont);
	text_layer_set_text_alignment(mTimeHourLayer, GTextAlignmentRight);
	layer_add_child(mTimeLayer, text_layer_get_layer(mTimeHourLayer));
  
  // TIME SEPARATOR LAYER //
  mTimeSeparatorLayer = text_layer_create(TIME_SEP_FRAME);  
	text_layer_set_background_color(mTimeSeparatorLayer, GColorClear);
  text_layer_set_text_color(mTimeSeparatorLayer, GColorBlack);
	text_layer_set_font(mTimeSeparatorLayer, mTimeFont);
	text_layer_set_text_alignment(mTimeSeparatorLayer, GTextAlignmentCenter);
  text_layer_set_text(mTimeSeparatorLayer, ":");
	layer_add_child(mTimeLayer, text_layer_get_layer(mTimeSeparatorLayer));
  
  // TIME MINUTES LAYER //
  mTimeMinutesLayer = text_layer_create(TIME_MIN_FRAME);  
	text_layer_set_background_color(mTimeMinutesLayer, GColorClear);
  text_layer_set_text_color(mTimeMinutesLayer, GColorBlack);
	text_layer_set_font(mTimeMinutesLayer, mTimeFont);
	text_layer_set_text_alignment(mTimeMinutesLayer, GTextAlignmentLeft);
	layer_add_child(mTimeLayer, text_layer_get_layer(mTimeMinutesLayer));  

	// DATE LAYER //
	mDateLayer = text_layer_create(DATE_FRAME);  
	text_layer_set_background_color(mDateLayer, GColorClear);
	text_layer_set_text_color(mDateLayer, GColorBlack);
	text_layer_set_font(mDateLayer, mDateFont); //fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD)
	text_layer_set_text_alignment(mDateLayer, GTextAlignmentCenter);
	layer_add_child(mWindowLayer, text_layer_get_layer(mDateLayer)); 
  	
  // WEATHER ICON //
  mWeatherIconLayer = bitmap_layer_create(WEATHER_ICON_FRAME);
  layer_add_child(mWindowLayer, bitmap_layer_get_layer(mWeatherIconLayer));
  
	// TEMPERATURE //
  mTemperatureLayer = text_layer_create(WEATHER_TEMP_FRAME);  
	text_layer_set_background_color(mTemperatureLayer, GColorClear);
  text_layer_set_text_color(mTemperatureLayer, GColorBlack);
	text_layer_set_font(mTemperatureLayer, mTemperatureFont); //fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD)); //
	text_layer_set_text_alignment(mTemperatureLayer, GTextAlignmentCenter);
	layer_add_child(mWindowLayer, text_layer_get_layer(mTemperatureLayer));

  // LOCATION //
  //mLocationLayer = text_layer_create(LOCATION_FRAME);  
	//text_layer_set_background_color(mLocationLayer, GColorClear);
  //text_layer_set_text_color(mLocationLayer, GColorBlack);
	//text_layer_set_font(mLocationLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	//text_layer_set_text_alignment(mLocationLayer, GTextAlignmentRight);
	//layer_add_child(mWindowLayer, text_layer_get_layer(mLocationLayer));
  
  // PEDOMETER //
  mPedometerLayer = text_layer_create(PEDOMETER_FRAME);  
	text_layer_set_background_color(mPedometerLayer, GColorClear);
  text_layer_set_text_color(mPedometerLayer, GColorBlack);
	text_layer_set_font(mPedometerLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(mPedometerLayer, GTextAlignmentLeft);
	layer_add_child(mWindowLayer, text_layer_get_layer(mPedometerLayer));

	// HIGHLOW //
  mHighLowLayer = text_layer_create(WEATHER_HL_FRAME);  
	text_layer_set_background_color(mHighLowLayer, GColorClear);
  text_layer_set_text_color(mHighLowLayer, GColorBlack);
	text_layer_set_font(mHighLowLayer, mHighLowFont); //fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD)
	text_layer_set_text_alignment(mHighLowLayer, GTextAlignmentLeft);
	layer_add_child(mWindowLayer, text_layer_get_layer(mHighLowLayer));

  mHiLayer = text_layer_create(WEATHER_HI_FRAME);  
	text_layer_set_background_color(mHiLayer, GColorClear);
  text_layer_set_text_color(mHiLayer, GColorBlack);
	text_layer_set_font(mHiLayer, mHighLowFont); //fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD)
	text_layer_set_text_alignment(mHiLayer, GTextAlignmentCenter);
	layer_add_child(mWindowLayer, text_layer_get_layer(mHiLayer));

  mLoLayer = text_layer_create(WEATHER_LO_FRAME);  
	text_layer_set_background_color(mLoLayer, GColorClear);
  text_layer_set_text_color(mLoLayer, GColorBlack);
	text_layer_set_font(mLoLayer, mHighLowFont); //fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD)
	text_layer_set_text_alignment(mLoLayer, GTextAlignmentCenter);
	layer_add_child(mWindowLayer, text_layer_get_layer(mLoLayer));

	weather_set_loading();

  app_message_init();

  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);  

  mInitialMinute = (tick_time->tm_min % FREQUENCY_MINUTES);

  handle_tick(tick_time, DAY_UNIT + HOUR_UNIT + MINUTE_UNIT + SECOND_UNIT);
  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
  
  bluetooth_connection_service_subscribe(bluetooth_connection_callback);
	
  update_battery(battery_state_service_peek());
  battery_state_service_subscribe(&update_battery);
  accel_tap_service_subscribe(handle_shake);
}

void handle_deinit(void) {

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "persist_write %d:%d", TS, mPedometer);
/*
static int mPedometer=1;
static int mTemperatureDegrees=999;    //-999 to 999
static int mTemperatureIcon=48;        //0 to 48
static int mTemperatureHigh=999;       //-999 to 999
static int mTemperatureLow=999;        //-999 to 999
atic int mHumidity=99;
static int mRise=0;
static int mSet=0;
static char mCity[50];
static int mSleep=653;
static int mFajr=0;
static int mDhuhur=0;
static int mAsr=0;
static int mIsha=0;
*/
  //persist_write_int(1, mPedometer);
  persist_write_int(2, mTemperatureDegrees);
  persist_write_int(3, mTemperatureIcon);
  persist_write_int(4, mTemperatureHigh);
  persist_write_int(5, mTemperatureLow);
  persist_write_int(6, mHumidity);
  persist_write_int(7, mRise);
  persist_write_int(8, mSet);
  persist_write_string(9, mCity);
  //persist_write_int(10, mSleep);
  persist_write_int(11, mFajr);
  persist_write_int(12, mDhuhur);
  persist_write_int(13, mAsr);
  persist_write_int(14, mIsha);

  persist_write_int(30, fc1h);
  persist_write_int(31, fc1l);
  persist_write_string(32, fc1d);
  persist_write_int(33, fc1c);
  persist_write_int(40, fc2h);
  persist_write_int(41, fc2l);
  persist_write_string(42, fc2d);
  persist_write_int(43, fc2c);
  persist_write_int(50, fc3h);
  persist_write_int(51, fc3l);
  persist_write_string(52, fc3d);
  persist_write_int(53, fc3c);

	//accel_data_service_unsubscribe();
  
  fonts_unload_custom_font(mTimeFont);
  fonts_unload_custom_font(mDateFont);
  fonts_unload_custom_font(mTemperatureFont);
  //fonts_unload_custom_font(mHighLowFont);

  layer_remove_from_parent(bitmap_layer_get_layer(mWeatherIconLayer));
  bitmap_layer_destroy(mWeatherIconLayer);
	
  layer_remove_from_parent(bitmap_layer_get_layer(battery_image_layer));
  bitmap_layer_destroy(battery_image_layer);
  gbitmap_destroy(battery_image);
  battery_image = NULL;
  
  tick_timer_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  battery_state_service_unsubscribe();
  accel_tap_service_unsubscribe();
  
  window_destroy(mWindow);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
