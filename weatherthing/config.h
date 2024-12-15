//Sensor config
//#define WNW_SENS //Wind and Weather/Rain sensors upload enable
const float MPH_PER_RPM = 1.492;  //RPM per MPH
const float IN_PER_BCKT = 0.011;  //Inches per rain sensor flip
const uint32_t uploadinterval = 300000;

//Pins
const uint8_t ONEWIRE_PIN = 13;
const uint8_t DHT_PIN = 12;
#define LCD_ADDRESS 0x27
const uint8_t STATUS_LED_PIN = 2;
const uint8_t ERROR_LED_PIN = 0;
const uint8_t WIND_PIN = 14;
const uint8_t RAIN_PIN = 15;
const uint8_t WINDVANE_PIN = A0;

#define WIRE_PINS 4,5

//Voltage Divider
const float VOLTAGE_AT_1V1 = 14.6;  //Input voltage that results in 1.1 Volts at A3
const uint8_t VDIV_PIN = A0;

//NTP Things
const uint8_t TIMEZONE = 1;
const char* NTPSERVER = "pool.ntp.org";
constexpr uint32_t NTP_SYNC_INTERVAL = 1 * 24 * 60 * 60 * 1000;
