#define H_DHT11

float idtempc   = 0;
float idhumid   = 0;

DHT dht(DHT_PIN, DHT11);

void init_dht11() {
  Serial.println(F("Init: DHT11"));
  dht.begin();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  lcd.setCursor(7, 3);
  if (!isnan(h) || !isnan(t)) {
    lcd.print('.');
    Serial.println(F("OK"));
    Serial.println();
    hasDHT = true;
  }
  else {
    noerrors = false;
    lcd.print('*');
    Serial.println(F("ERROR"));
    Serial.println();
    hasDHT = false;
  }
}

void get_dht11() {
  Serial.println(F("Getting DHT11."));
  idtempc = dht.readTemperature();
  idhumid = dht.readHumidity();
}
