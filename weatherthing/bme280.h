#define H_BME280

float tempf     = 0;
float tempc     = 0;
float dewptc    = 0;
float dewptf    = 0;
float humidity  = 0;
double press_hpa = 0;
double press_in  = 0;

Adafruit_BME280 bme;

void init_bme280() {
  Serial.println(F("Init: BME280"));
  if (bme.begin(0x76)) {
    lcd.print(F("."));
    smsreport += "BME: OK\n";
    Serial.println(F("OK"));
    Serial.println(F(""));
    hasBME = true;
  }
  else {
    noerrors = false;
    lcd.print(F("*"));
    smsreport += "BME: ERR\n";
    Serial.println(F("ERROR"));
    Serial.println(F(""));
    hasBME = false;
  }
}

void get_bme280() {
  Serial.println(F("Getting BME280."));
  tempc = bme.readTemperature() - 1.5F; //Compensating for heat produced by sensor
  tempf = (tempc * 9 / 5) + 32;
  humidity = bme.readHumidity();
  dewptc = (tempc - (100 - humidity) / 5);
  dewptf = (dewptc * 9 / 5) + 32;
  press_hpa = bme.readPressure() / 100.0F;
  press_in = press_hpa * 0.02953F;
}
