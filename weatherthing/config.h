//Sensor config
//#define WNW_SENS //Wind and Weather/Rain sensors upload enable
#define MPH_PER_RPM 1.492 //RPM per MPH
#define IN_PER_BCKT 0.011 //Inches per rain sensor flip
uint32_t uploadinterval = 300000;

//Pins
#define ONEWIRE_PIN 12
#define DHT_PIN 8
#define LCD_PINS 39, 43, 45, 47, 49, 51

//GSM Module
#define GSM_SERIAL Serial1
#define RESET_PIN 9

//Voltage Divider
#define VOLTAGE_AT_1V1 14.6 //Input voltage that results in 1.1 Volts at A3

//NTP Things
#define TIMEZONE 1
#define NTPSERVER "162.159.200.123"

