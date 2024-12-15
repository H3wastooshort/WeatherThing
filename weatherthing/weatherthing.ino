#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#include <time.h>
#include <Wire.h>
#include <OneWire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SI1145.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

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
#include "general.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(ERROR_LED_PIN, OUTPUT);
  //analogReference(INTERNAL1V1);
  Wire.pins(WIRE_PINS);
  Wire.setClock(10000);
  Wire.begin(WIRE_PINS);
  lcd.begin(20,4);
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
    lcd_clear_line();
    lcd.setCursor(0, 3);
    lcd.print(F("Connecting..."));
    if (!connect_gprs()) {  //Give up and wait for Watchdog to reset the Arduino
      wdt_enable(WDTO_15MS);
      while (1) {}
    }
#ifdef UPLOAD_WU
    lcd.setCursor(0, 3);
    lcd_clear_line();
    lcd.setCursor(0, 3);
    lcd.print(F("Uploading to WU..."));
    uploadWU();
#endif
#ifdef UPLOAD_OWM
    lcd.setCursor(0, 3);
    lcd_clear_line();
    lcd.setCursor(0, 3);
    lcd.print(F("Uploading to OWM... "));
    uploadOWM();
#endif
#ifdef UPLOAD_STATS
    lcd.setCursor(0, 3);
    lcd_clear_line();
    lcd.setCursor(0, 3);
    lcd.print(F("Uploading Stats..."));
    uploadStats();
#endif

    lcd.setCursor(0, 3);
    lcd_clear_line();
    lcd.setCursor(0, 3);
    lcd.print(F("Disconnecting..."));
    disconnect_gprs();
    digitalWrite(13, LOW);
    delay(1000);
    lcd.setCursor(0, 3);
    lcd_clear_line();
  }
  delay(5000);
}