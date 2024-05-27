void init_all() {
  lcd.createChar(1, gfx_airTemp);
  lcd.createChar(2, gfx_sun);
  lcd.createChar(3, gfx_soilTemp);
  lcd.createChar(4, gfx_indoorTemp);
  lcd.createChar(5, gfx_antenna);
  lcd.home();
  lcd.clear();
  lcd.setCursor(0, 2);
  lcd.print(F("    \x01\x02\x03\x04\x05           "));
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
      String smsreport = F("Report:");
      smsreport += F("\nBME ");
      smsreport += hasBME;
      smsreport += F("\nSI ");
      smsreport += hasSI;
      smsreport += F("\nDHT ");
      smsreport += hasDHT;
      smsreport += F("\nDS ");
      smsreport += hasDS;
      sms.send(sos_number, smsreport.c_str());
    } else {
      uint32_t reset_timenow = millis();
      while (true) {
        if (reset_timenow - millis() > 900000) {  //Wait 15 Mins
          wdt_enable(WDTO_15MS);                  //reset via watchdog
        }
        digitalWrite(ERROR_LED_PIN, HIGH);
        delay(100);
        digitalWrite(ERROR_LED_PIN, LOW);
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