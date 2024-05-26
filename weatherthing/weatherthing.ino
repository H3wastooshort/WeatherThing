#include <GSMSim.h>
#include <GSMSimGPRS.h>
#include <GSMSimHTTP.h>
#include <GSMSimSMS.h>
#include <GSMSimTime.h>
#include <time.h>
#include <Wire.h>
#include <OneWire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SI1145.h>
#include <DallasTemperature.h>
#include <DHT.h>

#include <LiquidCrystal.h>
#include <avr/wdt.h>


#include "secrets.h"
#include "config.h"

#include "globals.h"
#include "util.h"
#include "lcd.h"
#include "gsm.h"
#include "v_bat.h"
#include "bme280.h"
#include "si1145.h"
#include "ds18b20.h"
#include "dht11.h"
#include "wnw.h"
#include "api.h"
#include "debug.h"


//Library defines



//trash
uint16_t x = 0;


void init_all() {
  lcd.setCursor(0, 0);
  lcd.print(F("    B  DG   . = OK "));
  lcd.setCursor(0, 1);
  lcd.print(F("    MUDHS   , = ErrU"));
  lcd.setCursor(0, 2);
  lcd.print(F("    EVSTM   * = ErrP"));
  lcd.setCursor(0, 3);
  lcd.print(F("INIT"));
  init_bme280();
  init_si1145();
  init_ds18b20();
  init_dht11();
  init_gsm();
  lcd.print(F(" "));

  //noerrors=true; //Ignore all errors and carry on


  if (!noerrors) {
    Serial.println(F("Missing Sensors!"));
    lcd.print(F("ERROR"));
    if (nogsmerr) {
      Serial.println(F("Sending SMS"));
      sms.send(sos_number, smsreport.c_str());
    }
    else {
      uint32_t reset_timenow = millis();
      while (true) {
        if (reset_timenow - millis() > 900000) { //Wait 15 Mins
          wdt_enable(WDTO_15MS); //reset via watchdog
        }
        digitalWrite(53, HIGH);
        delay(100);
        digitalWrite(53, LOW);
        delay(100);

      }
    }
  }
  Serial.println(F("INIT OK"));
  lcd.print(F("OK"));
  delay(500);
  lcd.clear();
}

void get_sensors() {
  Serial.println(F("Getting Sensors."));
  Serial.println(F(""));
  if (hasBME) {
    lcd.setCursor(0, 3);
    lcd.print(F("Getting BME280...   "));
    get_bme280();
  }
  if (hasSI) {
    lcd.setCursor(0, 3);
    lcd.print(F("Getting SI1145...   "));
    get_si1145();
  }
  if (hasDS) {
    lcd.setCursor(0, 3);
    lcd.print(F("Getting DS18B20...  "));
    get_ds18b20();
  }
  if (hasDHT) {
    lcd.setCursor(0, 3);
    lcd.print(F("Getting DHT11...    "));
    get_dht11();
  }
  lcd.setCursor(0, 3);
  lcd.print(F("Getting Windvane... "));
  get_windvane();
  lcd.setCursor(0, 3);
  lcd.print(F("Getting Windspeed..."));
  get_windspeed();
  lcd.setCursor(0, 3);
  lcd.print(F("Getting Stats..."));
  get_stats();
  lcd.setCursor(0, 3);
  if (hasGSM) {
    lcd.print(F("Getting GSM&Time... "));
    get_gsm();
  }
  lcd.setCursor(0, 3);
  lcd.print(F("        DONE        "));
  Serial.println(F(""));
  Serial.println(F("Done."));
  Serial.println(F(""));
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(ERROR_LED_PIN, OUTPUT);
  pinMode(GSM_LED_PIN, OUTPUT);
  pinMode(41, OUTPUT);
  analogReference(INTERNAL1V1);
  digitalWrite(41, LOW);
  Wire.setClock(10000);
  lcd.begin(20, 4);
  digitalWrite(STATUS_LED_PIN, HIGH);
  delay(100);
  digitalWrite(STATUS_LED_PIN, LOW);
  delay(100);
  digitalWrite(STATUS_LED_PIN, HIGH);
  delay(100);
  digitalWrite(STATUS_LED_PIN, LOW);
  delay(100);
  digitalWrite(ERROR_LED_PIN, HIGH);
  delay(100);
  digitalWrite(ERROR_LED_PIN, LOW);
  delay(100);
  digitalWrite(STATUS_LED_PIN, HIGH);
  init_all();
  digitalWrite(STATUS_LED_PIN, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  get_sensors();
  calc_avgs();
  print_sensors();
  print_lcd();

  if ((unsigned long)(millis() - timenow_upload) > uploadinterval) {
    timenow_upload = millis();
    digitalWrite(13, HIGH);
    lcd.setCursor(0, 3);
    lcd.print(F("Connecting...       "));
    connect_gprs();
    lcd.setCursor(0, 3);
    lcd.print(F("Uploading to WU...  "));
    uploadWU();
    lcd.setCursor(0, 3);
    lcd.print(F("Uploading to OWM... "));
    uploadOWM();
    lcd.setCursor(0, 3);
    lcd.print(F("Uploading Stats... "));
    uploadStats();
    lcd.setCursor(0, 3);
    lcd.print(F("Disconnecting...    "));
    disconnect_gprs();
    digitalWrite(13, LOW);
    delay(1000);
    lcd.setCursor(0, 3);
    lcd.print(F("                    "));
  }
  delay(5000);
}