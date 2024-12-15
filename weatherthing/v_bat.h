#define H_VBAT

float voltage = 12;

void get_stats() {
  Serial.println(F("Getting Stats."));
  uint16_t rawVolt = analogRead(VDIV_PIN);
  float v1 = map(rawVolt, 0, 1024, 0, (VOLTAGE_AT_1V1) * 100);
  v1 = v1 / 100;
  voltage = v1;
  delay(10);
}