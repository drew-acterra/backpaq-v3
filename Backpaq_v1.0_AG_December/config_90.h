//// BackpAQ    V0.98S                                /////
//// (c) 2021, 2024 BackpAQ Labs LLC and Sustainable Silicon Valley
//// Written by A. L. Clark 
/*
MIT License

Copyright (c) 2021 BackpAQ Labs LLC and Sustainable Silicon Valley

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
*/
const char* version = "V0.99I"; // for Blynk IOT Feather Huzzah32 ESP32 WiFi w/OLED and SDC41X CO2 and Plantower PMAS003i PM
// Blynk Parameters
char blynk_token[34] = "YOUR_BLYNK_TOKEN"; // will be filled in at WiFi config time and stored in eeprom
char Blynk_server[] = "blynk-cloud.com";  // URL for Blynk Cloud Server
/* SSID and PW for Router */
String Router_SSID;
String Router_Pass;
//char server[] = "api.thingspeak.com"; // ThingSpeak Server
//char Blynk_server[] = "45.55.96.146";  // URL for Blynk Cloud Server
//char auth[] = "xifSaUx1c679vHhDYYAB7KG8I0O8HiFc";  // This is Drew's private token. Insert your Blynk private token "auth" here or use config portal
#define BLYNK_PRINT Serial
#define BLYNK_MAX_READBYTES 2048
#define blynk_map_index 8
// Heartbeat period in seconds.
#define BLYNK_HEARTBEAT      10
//////////////////////////////////////////////////////
///////// Default Wifi connection information  /////// 
////// Not currently used as we're using WiFiManager Config /
//////////////////////////////////////////////////////
char ssid[] = "artemis"; // WiFi SSID
char pass[] = "opaopaopa";  // WiFi Password
//char ssid[] = "Drew's 5s iPhone";
//char pass[] = "esjt938qrh9f0";
char userID[20] = "xxxxxxxxxx";

/////////////////////// Blynk timer intervals :: avoid overlaps! ///////////////////////////////
#define PMSENSORINT 2116L // PM Sensor I2C stream updates every n sec
#define PMAVERAGEINT 10025L // average PM and update Blynk, local display
#define CO2SENSORINT 5030L // Sensirion CO2 sensor, 5 second interval per spec sheet
#define CO2AVERAGEINT 11050 // average CO2 and update Blynk, local display
#define TIMEDATEINT 1000L
#define GPSINT  1500L // update GPS for mobile use
#define GPSINTF  10000L // update GPS for fixed use
#define MAPGPSINT 3900L
#define MAPGPSINTF 11000L
#define OLEDUPDATEINT 1050L
#define GETBATTERYVINT 15000L
#define THINGSPEAKSENDINT 40000L // Interval to send to ThingSpeak cloud
#define SOUNDSENSORINT 7000L // sample sound sensor
#define HEARTBEATINT 30000L
#define BLINKLEDINT 800L
int thingspeakSendInterval = 40000L; // Interval to send to ThingSpeak cloud (defined and passed thru Blynk metadata)
//// ////////////////////////////////// TVOC, CO2 sensor parameters //////////////////////////////////////////
const int EEpromWrite = 2; // interval in which to write new TVOC/CO2 baselines into EEPROM in hours
unsigned long previousMillis = 0; // Millis at which the interval started
/////////////////////////////////////// Particle Sensor Parameters //////////////////////////////////////////
//#define MIN_WARM_TIME 35000 //warm-up period requred for Plantower sensor to enable fan and prepare air chamber
#define MIN_WARM_TIME 30000 //warm-up period requred for SEN55 sensor to enable fan and prepare air chamber
const int pmSampleTime = 2000; //delay until next PM sample
const int MAX_WIFI_TRY = 4;
#define TIMEOUT 10000 //Microseconds to wait for PMS do wakeup 
const int   SLEEP_TIME = 5 * 60 * 1000; // 5 minutes sleep time
/////////////////////////////////  Misc Sensor Parameters ///////////////////////////////////////////////////
#define BME280_ADDRESS 0x76 // I2C address for temp/humidity sensor
 const float dCorr = 13.0; // est. temp correction for scd41 sensor due to interior case heating
#define BATT_LEVEL A0 // analog pin to measure battery voltage
#define FBATTERY   4200 //The default battery is 4200mv (3700mv) when the battery is fully charged.
#define CRITICALBATTERY 3.2; // battery critical!
//float XS = 0.001700; // scaling factor
float XS = 0.001800; // scaling factor
//float XS = 0.00225;   // scaling factor
uint16_t MUL = 1000;
uint16_t MMUL = 100;
// How many minutes the ESP should sleep
#define DEEP_SLEEP_TIME 60 // one hour
////////////////////////////////////////// Thingspeak  //////////////////////////////////////////////////////
 // int thingspeak_Send_Interval = 40000L; // Send data to Thingspeak roughly every 40 seconds
//  int thingspeak_Send_Interval2 = 41000L; // Send data to Thingspeak roughly every 41 seconds
  const char* ThingSpeak_API_Server = "api.thingspeak.com";

  //  Create three Thingspeak channels with the following fields collection
  //  The names of the field are not important, but the order of the fields should be defined as below
  ////// Replace with your Thingspeak channels and API keys in config portal  ////////////////////////////////////////////////
  
  String thingspeak_A_key_s = "xxxxxxxxxxxxxxxx";
  String thingspeak_B_key_s = "xxxxxxxxxxxxxxxx";
  String thingspeak_C_key_s = "xxxxxxxxxxxxxxxx";
  
  // BackpAQ PM Data 1:
  
  const char * thingspeak_A_key_c = "A9L9601U1FROH6BE";
  char thingspeak_A_channel[16] = "xxxxxxx";
  unsigned long thingspeak_A_channel_i;
  char thingspeak_A_key[20] = "xxxxxxxxxxxxxxxx";

  // Field1= Atm PM 1.0 (μg/m3)
  // Field2= Atm PM 2.5 (μg/m3)
  // Field3= Atm PM 10 (μg/m3)
  // Field4= Temperature (deg F)
  // Field5= Humidity (%)
  // Field6= Latitude (deg)
  // Field7= Longitude (deg)
  // Field8= AQI (US Standard)

  // BackpAQ PM Data 2:
  
  const char * thingspeak_B_key_c = "6C1HZQR08LSTF5AG";
  unsigned long thingspeak_B_channel_i; 
  char thingspeak_B_channel[16] = "xxxxxxx";
  char thingspeak_B_key[20] = "xxxxxxxxxxxxxxxx";
 
  // Field1= Particulate count 0.1 µm per 0.1 L
  // Field2= Particulate count 0.3 µm per 0.1 L
  // Field3= Particulate count 0.5 µm per 0.1 L
  // Field4= Particulate count 1.0 µm per 0.1 L
  // Field5= Particulate count 2.5 µm per 0.1 L
  // Field6= Particulate count 10 µm per 0.1 L
  // Field7= TVOC
  // Field8= eCO2

  // BackpAQ PM Data 3: Tracks generated by BackpAQ, to be displayed with AQView
  
  const char * thingspeak_C_key_c = "LXGG1URMRCNL8MZX";
  unsigned long thingspeak_C_channel_i = 1198056;
  char thingspeak_C_channel[16] = "1198056";
  char thingspeak_C_key[20] = "LXGG1URMRCNL8MZX";
 
  // Field1= TimeDate
  // Field2= Latitude
  // Field3= Longitude
  // Field4= PM2.5
  // Field5= AQI
  // Field6= URL to fetch icon, color to match AQI
  // Field7= Comments
  // Field8  Userid
  
namespace color {
  const uint32_t red = 0xFF0000;
  const uint32_t green = 0x00FF00;
  const uint32_t blue = 0x0000FF;
  const uint32_t black = 0x000000;
  const uint32_t magenta = 0xFF00FF;
}

///////////////  Following are AQI Calculation Parameters //////
#define    PURPLEICONURL  "https://maps.google.com/mapfiles/kml/paddle/purple-circle_maps.png";
#define    REDICONURL     "https://maps.google.com/mapfiles/kml/paddle/red-circle_maps.png";
#define    ORANGEICONURL  "https://maps.google.com/mapfiles/kml/paddle/orange-circle_maps.png";
#define    YELLOWICONURL  "https://maps.google.com/mapfiles/kml/paddle/ylw-circle_maps.png";
#define    GREENICONURL   "https://maps.google.com/mapfiles/kml/paddle/grn-circle_maps.png";
////////////////////////////////////////////////////////////////
// Define AQI Color Scheme (according to US EPA standard)
//////////////////////////////////////////////////////////////
#define AQI_GREEN     "#009966" // Safe
#define AQI_YELLOW    "#ffde33" // Moderate
#define AQI_ORANGE    "#ff9933" // Unhealthy for some
#define AQI_RED       "#cc0033" // Unhealthy
#define AQI_PURPLE    "#660099" // Very Unhealthy
#define AQI_MAROON    "#7e0023" // Hazardous
////////////////////////////////////////////////////////////////
//var aqi_colors = [
//    '009966',
//    'ffde33',
//    'ff9933',
//    'cc0033',
//    '660099',
//    '7e0023',
///  
//];
//var aqi_breakpoints = [
//    [0, 50],
//    [51, 100],
//    [101, 150],
//    [151, 200],
//    [201, 300],
//    [301, 400],
//    [401, 500],
//];
//var pm25_breakpoints = [
//    [0.0, 12.0],
 //   [12.1, 35.4],
///   [35.5, 55.4],
//    [55.5, 150.4],
//    [150.5, 250.4],
 //   [250.5, 350.4],
 //   [350.5, 500.0],
///];

/* set up CO2 forecast "traffic light" parameters */
String trafficLight = "green";
int greenLevel = 0;     // threshold - enterng green level
int yellowLevel = 500;  // threshold - entering yellow level
int redLevel = 1000;    // threshold - entering red level

// definitions and variables for noise sampling
//
// define in seconds, how often a message will be sent to ThingSpeak
#define CYCLETIME   60  
// set microphone dependent correction in dB
// for SPH0645 define -1.8
// for ICS43434 define 1.5
// otherwise define 0.0
#define MIC_OFFSET -1.8

/*PM Sensor global variables*/
unsigned long plantowercount = 0;

//unsigned int concPM1_0_CF1 = 0;
//unsigned int concPM2_5_CF1 = 0;
//unsigned int concPM10_0_CF1 = 0;
double concPM1_0_amb;
double concPM2_5_amb;
double concPM10_0_amb;
//unsigned int rawGt0_3um = 0;
//unsigned int rawGt0_5um = 0;
//unsigned int rawGt1_0um = 0;
//unsigned int rawGt2_5um = 0;
//unsigned int rawGt5_0um = 0;
//unsigned int rawGt10_0um = 0;
unsigned long tempconcPM1_0_amb;
unsigned long tempconcPM2_5_amb;
unsigned long tempconcPM10_0_amb;

// Sen55 Vars
  float massConcentrationPm1p0;
  float massConcentrationPm2p5;
  float massConcentrationPm4p0;
  float massConcentrationPm10p0;
  float ambientHumidity;
  float ambientTemperature;
  float vocIndex;
  float noxIndex;

bool calibration = 1; // use the slope fit calibration for PM values

double PM1_0;
double PM2_5;
double PM10_0;
unsigned long sensor_timer = 100;       //Frequency of query for Plantower/HDC1080
