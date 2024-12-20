void print_lcd() {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (hasBME) {
    lcd.print(F("T: "));
    lcd.print(tempc);
    lcd.print(F("C H: "));
    lcd.print(humidity);
    lcd.print(F("%"));
  }
  else {
    lcd.print(F("No BME280!"));
  }
  lcd.setCursor(0, 1);
  if (hasDS) {
    lcd.print(F("S: "));
    lcd.print((int)soiltempc);
    lcd.print(F("C"));
  }
  else {
    lcd.print(F("No DS."));
  }
  if (hasBME) {
    lcd.print(F(" P: "));
    lcd.print(press_pa / 100.0);
    lcd.print(F("hPa"));
  }

  if (hasDHT) {
    lcd.setCursor(0, 2);
    lcd.print(F("I: "));
    lcd.print((int)idtempc);
    lcd.print(F("C "));
    lcd.print((int)idhumid);
    lcd.print(F("%"));
  }
  else {
    lcd.print(F("No DHT."));
  }
  if (hasSI) {
  lcd.print(F(" UV: "));
  lcd.print(uv_index);
  }
  else {
    lcd.print(F("No SI."));
  }
  
  lcd.setCursor(0, 3);
  lcd.print(F("GSM: "));
  lcd.print(gsmsigp);
  lcd.print(F("% "));
  lcd.print(F("ULT: "));
  lcd.print((uint32_t)(millis() - timenow_upload));
}

void print_sensors() {
  Serial.println(F("Sensor Values:"));
  Serial.println(F(""));
  Serial.println(F("Temp [°C]:"));
  Serial.println(tempc);
  Serial.println(F("Dew [°C]:"));
  Serial.println(dewpointC(tempc, humidity));
  Serial.println(F("Hum [%]:"));
  Serial.println(humidity);
  Serial.println(F("Press [Pa]:"));
  Serial.println(press_pa);
  Serial.println(F("Soil [°C]:"));
  Serial.println(soiltempc);
  Serial.println(F("UV:"));
  Serial.println(uv_index);
  Serial.println(F("IR:"));
  Serial.println(ir);
  Serial.println(F("Vis:"));
  Serial.println(vis);
  Serial.println(F("Indoor Temp [°C]:"));
  Serial.println(idtempc);
  Serial.println(F("Indoor Hum [%]:"));
  Serial.println(idhumid);
  Serial.println(F("Speed [mph]:"));
  Serial.println(wind_mph);
  Serial.println(wind_mph_2m);
  Serial.println(F("Dir [°]:"));
  Serial.println(wind_dir);
  Serial.println(wind_dir_2m);
  Serial.println(F("Rain:"));
  Serial.println(rain_1h);
  Serial.println(rain_day);
  Serial.println(F("GSM Signal [%]:"));
  Serial.println(gsmsigp);
  Serial.println(F("Time:"));
  Serial.println(F("UNIX Timestamp:"));
  Serial.println(uint64ToString((uint64_t)t_of_day));
  Serial.println(F("Voltage:"));
  Serial.println(voltage);
  Serial.println(F(""));
}
