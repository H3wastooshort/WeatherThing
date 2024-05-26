#define H_BME280

float tempc     = 0;
float humidity  = 0;
float press_pa = 0;

Adafruit_BME280 bme;

void init_bme280() {
  Serial.println(F("Init: BME280"));
  if (bme.begin(0x76)) {
    lcd.print(F("."));
    Serial.println(F("OK"));
    Serial.println(F(""));
    hasBME = true;
  }
  else {
    noerrors = false;
    lcd.print(F("*"));
    Serial.println(F("ERROR"));
    Serial.println(F(""));
    hasBME = false;
  }
}

void get_bme280() {
  Serial.println(F("Getting BME280."));
  tempc = bme.readTemperature() - 1.5F; //Compensating for heat produced by sensor
  humidity = bme.readHumidity();
  press_pa = bme.readPressure();
}
