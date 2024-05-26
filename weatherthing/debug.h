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
    lcd.print(press_hpa);
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
  Serial.println(F("Temp:"));
  Serial.println(tempc);
  Serial.println(tempf);
  Serial.println(F("Dew:"));
  Serial.println(dewptc);
  Serial.println(dewptf);
  Serial.println(F("Hum:"));
  Serial.println(humidity);
  Serial.println(F("Press:"));
  Serial.println(press_hpa);
  Serial.println(press_in);
  Serial.println(F("Soil:"));
  Serial.println(soiltempc);
  Serial.println(soiltempf);
  Serial.println(F("UV:"));
  Serial.println(uv_index);
  Serial.println(F("IR:"));
  Serial.println(ir);
  Serial.println(F("Vis:"));
  Serial.println(vis);
  Serial.println(F("Indoor Temp:"));
  Serial.println(idtempc);
  Serial.println(idtempf);
  Serial.println(F("Indoor Hum:"));
  Serial.println(idhumid);
  Serial.println(F("Speed:"));
  Serial.println(wind_mph);
  Serial.println(wind_mph_2m);
  Serial.println(F("Dir:"));
  Serial.println(wind_dir);
  Serial.println(wind_dir_2m);
  Serial.println(F("Rain:"));
  Serial.println(rain_1h);
  Serial.println(rain_day);
  Serial.println(F("GSM Signal %:"));
  Serial.println(gsmsigp);
  Serial.println(F("Time:"));
  Serial.print(F("Hour: "));
  Serial.println(hour);
  Serial.print(F("Minute: "));
  Serial.println(minute);
  Serial.print(F("Second: "));
  Serial.println(second);
  Serial.print(F("Day: "));
  Serial.println(day);
  Serial.print(F("Month: "));
  Serial.println(month);
  Serial.print(F("Year: "));
  Serial.println(year);
  Serial.println(F("UNIX Timestamp:"));
  Serial.println(uint64ToString((uint64_t)t_of_day));
  Serial.println(F("Voltage:"));
  Serial.println(voltage);
  Serial.println(F(""));
}
