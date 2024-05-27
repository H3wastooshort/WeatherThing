//Sensor config
//#define WNW_SENS //Wind and Weather/Rain sensors upload enable
const float MPH_PER_RPM = 1.492;  //RPM per MPH
const float IN_PER_BCKT = 0.011;  //Inches per rain sensor flip
const uint32_t uploadinterval = 300000;

//Pins
const uint8_t ONEWIRE_PIN = 12;
const uint8_t DHT_PIN = 8;
#define LCD_PINS 39, 43, 45, 47, 49, 51
const uint8_t STATUS_LED_PIN = 13;
const uint8_t GSM_LED_PIN = 52;
const uint8_t ERROR_LED_PIN = 53;
const uint8_t WIND_PIN = 2;
const uint8_t RAIN_PIN = 3;
const uint8_t WINDVANE_PIN = A0;

//GSM Module
#define GSM_SERIAL Serial1
const uint8_t RESET_PIN = 9;
constexpr uint32_t NTP_SYNC_INTERVAL = 1 * 24 * 60 * 60 * 1000;

//Voltage Divider
const float VOLTAGE_AT_1V1 = 14.6;  //Input voltage that results in 1.1 Volts at A3
const uint8_t VDIV_PIN = A3;

//NTP Things
const uint8_t TIMEZONE = 1;
const char* NTPSERVER = "162.159.200.123";
