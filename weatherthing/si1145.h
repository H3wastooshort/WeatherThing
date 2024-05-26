#define H_SI1145

float uv_index = 0;
float vis = 0;
float ir = 0;

Adafruit_SI1145 uv = Adafruit_SI1145();

void init_si1145 () {
  Serial.println(F("Init: SI1145"));
  if (uv.begin()) {
    lcd.print(F("."));
    Serial.println(F("OK"));
    Serial.println(F(""));
    hasSI = true;
  }
  else {
    noerrors = false;
    lcd.print(F("*"));
    Serial.println(F("ERROR"));
    Serial.println(F(""));
    hasSI = false;
  }
}

void get_si1145 () {
  Serial.println(F("Getting SI1145."));
  uv_index = uv.readUV();
  uv_index /= 100.0F;
  ir = uv.readIR();
  vis = uv.readVisible();
}
