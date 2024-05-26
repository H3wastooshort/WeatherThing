#define H_DHT11

float idtempc   = 0;
float idhumid   = 0;

DHT dht(DHT_PIN, DHT11);

void init_dht11() {
  Serial.println(F("Init: DHT11"));
  dht.begin();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (!isnan(h) || !isnan(t)) {
    lcd.print(F("."));
    Serial.println(F("OK"));
    Serial.println(F(""));
    hasDHT = true;
  }
  else {
    noerrors = false;
    lcd.print(F("*"));
    Serial.println(F("ERROR"));
    Serial.println(F(""));
    hasDHT = false;
  }
}

void get_dht11() {
  Serial.println(F("Getting DHT11."));
  idtempc = dht.readTemperature();
  idhumid = dht.readHumidity();
}
