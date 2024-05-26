#define H_WNW

float wind_mph    = 0;
float wind_mph_2m = 0;
int wind_dir      = 0;
float wind_dir_2m = 0;
float rain_1h     = 0;
float rain_day    = 0;
uint16_t rainHour[60];
uint8_t wind2min[8];
int dir2min[8];
uint32_t raintime    = 0;
uint32_t rainlast    = 0;
uint32_t lastWindCheck = 0;
uint16_t windClicks = 0;
uint8_t avg_index = 0;

//interrupts
void wind_cnt() {
  windClicks += 1;
}
void rain_cnt() {
  raintime = millis();
  uint32_t raininterval = raintime - rainlast;
  if (raininterval > 10) {
    rain_day += IN_PER_BCKT * 100;
    rainHour[minute] += IN_PER_BCKT * 100;
    rainHour[minute + 1] = 0;
    rainlast = raintime;
  }
}

void init_wnw() {
  
  pinMode(WIND_PIN, INPUT_PULLUP);
  pinMode(RAIN_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(WIND_PIN) , wind_cnt, FALLING);
  attachInterrupt(digitalPinToInterrupt(RAIN_PIN) , rain_cnt, FALLING);
}




int16_t calc_windvane(int io) { //TODO: interpolate between values, use array
  unsigned int vane;
  vane = analogRead(io);
  if (vane < 380) return 113;
  else if (vane < 393) return 69;
  else if (vane < 414) return 90;
  else if (vane < 456) return 158;
  else if (vane < 508) return 135;
  else if (vane < 551) return 203;
  else if (vane < 615) return 180;
  else if (vane < 680) return 23;
  else if (vane < 746) return 45;
  else if (vane < 801) return 248;
  else if (vane < 833) return 225;
  else if (vane < 878) return 338;
  else if (vane < 913) return 0;
  else if (vane < 940) return 293;
  else if (vane < 967) return 315;
  else if (vane < 990) return 270;
  return 360;
}

void get_windvane() {
  Serial.println(F("Getting Wind Vane."));
  wind_dir = calc_windvane(WINDVANE_PIN);
}

void get_windspeed() {
  float deltaTime = millis() - lastWindCheck;
  deltaTime /= 1000;
  float windSpeed = (float)windClicks / deltaTime;
  windClicks = 0;
  lastWindCheck = millis();
  windSpeed *= MPH_PER_RPM;
  wind_mph = windSpeed;

}

//Calculate all of the averages
uint16_t avginterval = 15000;
unsigned long timenow_avg = 0;

void calc_avgs() {
  if ((unsigned long)(millis() - timenow_avg) > avginterval) {
    timenow_avg = millis();
    avg_index += 1;
    if (avg_index >= 8) {
      avg_index = 0;
    }
    wind2min[avg_index] = wind_mph;
    dir2min[avg_index] = wind_dir;
  }
  //Rain sum
  int sum0 = 0;
  for (int thisReading = 0; thisReading < 60; thisReading++) {
    sum0 += rainHour[thisReading];
  }
  // calculate the sum:
  rain_1h = sum0 / 100;


  //Wind Average
  uint8_t sum1 = 0;
  for (int thisReading = 0; thisReading < 60; thisReading++) {
    sum1 += wind2min[thisReading];
  }
  // calculate the average:
  wind_mph_2m = (sum1 / 4) / 100;

  //Wind Dir Average
  int sum2 = 0;
  for (int thisReading = 0; thisReading < 60; thisReading++) {
    sum2 += dir2min[thisReading];
  }
  // calculate the average:
  wind_dir_2m = (sum2 / 4) / 100;
  //reset day counter
  if (hour == 1 and minute == 1) {
    rain_day = 0;
  }

}
