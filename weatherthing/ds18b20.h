#define H_DS18B20

float soiltempc = 0;

OneWire oneWire(ONEWIRE_PIN);
DallasTemperature ds(&oneWire);
DeviceAddress insideThermometer;

void init_ds18b20() {
  lcd.setCursor(6, 3);
  Serial.println(F("Init: DS18B20"));
  ds.begin();
  // locate devices on the bus
  ds.begin();
  bool dsdev = ds.getAddress(insideThermometer, 0);
  if (!dsdev) {
    noerrors = false;
    lcd.print('*');
    Serial.println(F("ERROR"));
    Serial.println();
    hasDS = false;
    return;
  }
  // report parasite power requirements
  Serial.print(F("Parasite power is "));
  if (ds.isParasitePowerMode()) Serial.println(F("ON."));
  else Serial.println(F("OFF."));

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  ds.setResolution(insideThermometer, 9);

  Serial.print(F("Resolution: "));
  Serial.print(ds.getResolution(insideThermometer), DEC);
  Serial.println();

  if (dsdev) {
    lcd.print('.');
    Serial.println(F("OK"));
    Serial.println();
    hasDS = true;
  }
}

void get_ds18b20() {
  Serial.println(F("Getting DS18B20."));
  ds.requestTemperatures();
  delay(1000);
  soiltempc = ds.getTempC(insideThermometer);
}
