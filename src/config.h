#ifndef CONFIG_H
#define CONFIG_H

//Check for new weather every X minutes
#define FREQUENCY_MINUTES 5

//frames
#define TIME_FRAME          (GRect(0, 78, 144, 54))

#define TIME_HOUR_FRAME     (GRect(0, 78, 65, 54))
#define TIME_SEP_FRAME      (GRect(66, 72, 13, 54))
#define TIME_MIN_FRAME      (GRect(79, 78, 66, 54))

#define DATE_FRAME          (GRect(0, 132, 144, 30))
#define LOCATION_FRAME      (GRect(50, 0, 94, 15))
#define WEATHER_ICON_FRAME  (GRect(0, 0, 50, 50))
#define WEATHER_TEMP_FRAME  (GRect(52, 8, 75, 50))
#define WEATHER_HL_FRAME    (GRect(0, 46, 144, 20))
#define PEDOMETER_FRAME     (GRect(0, 60, 144, 20))
#define WEATHER_HI_FRAME    (GRect(114, 8, 30, 20))
#define WEATHER_LO_FRAME    (GRect(114, 23, 30, 20))
#define INVERT_TOP_FRAME    (GRect(0, 0, 144, 83))
#define INVERT_BOTTOM_FRAME (GRect(0, 83, 144, 87))

#endif
