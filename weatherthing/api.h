//Upload to PWS Networks

WiFiClient client;
HTTPClient http;

void uploadWU() {
  Serial.println(F("Uploading to Wunderground"));
  String req = "";
  req += serverWU;
  req += pathWU;
  req += "?ID=";
  req += WU_ID;
  req += "&PASSWORD=";
  req += WU_PASS;
  req += "&dateutc=now";
  if (hasBME) {
    req += "&tempf=";
    req += celsius_to_farenheit(tempc);
    req += "&dewptf=";
    req += celsius_to_farenheit(dewpointC(tempc, humidity));
    req += "&humidity=";
    req += humidity;
    req += "&baromin=";
    req += pascal_to_inHg(press_pa);
  }
  if (hasSI) {
    req += "&UV=";
    req += uv_index;
  }
  if (hasDS) {
    req += "&soiltempf=";
    req += celsius_to_farenheit(soiltempc);
  }
  if (hasDHT) {
    req += "&indoortempf=";
    req += celsius_to_farenheit(idtempc);
    req += "&indoorhumidity=";
    req += idhumid;
  }
#ifdef WNW_SENS
  req += "&windspeedmph=";
  req += wind_mph;
  req += "&windspdmph_avg2m=";
  req += wind_mph_2m;
  req += "&winddir=";
  req += wind_dir;
  req += "&winddir_avg2m=";
  req += wind_dir_2m;
#endif
  req += "&softwaretype=wetterdings_esp8266";
  Serial.print(F("Request: "));
  Serial.println(req);

  http.begin(client, req);
  int16_t code = http.GET();
  Serial.print(F("Response: "));
  Serial.println(code);
  if (code == HTTP_CODE_OK) {
    const String& payload = http.getString();
    Serial.println(payload);
  }
  http.end();
}

void uploadOWM() {
  String url = "http://api.openweathermap.org/data/3.0/measurements?appid=";
  url += keyOWM;

  String req = "[{";
  req += "\"station_id\": \"";
  req += idOWM;
  req += "\", \"dt\": ";
  req += uint64ToString((uint64_t)t_of_day);
  if (hasBME) {
    req += ", \"temperature\": ";
    req += tempc;
    req += ", \"humidity\": ";
    req += humidity;
    req += ", \"pressure\": ";
    req += press_pa / 100.0;
    req += ", \"dew_point\": ";
    req += dewpointC(tempc, humidity);
  }
#ifdef WNW_SENS
  req += ", \"rain_1h\": ";
  req += rain_1h;
  req += ", \"rain_24h\": ";
  req += rain24h;
  req += ", \"wind_speed\": ";
  req += (wind_mph * 0.44704);
  req += ", \"wind_deg\": ";
  req += wind_dir;
#endif
  req += "}]";

  Serial.print(F("Request URL: "));
  Serial.println(url);
  Serial.print(F("Request Data: "));
  Serial.println(req);

  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  int16_t code = http.POST(req);
  Serial.print(F("Response: "));
  Serial.println(code);
  if (code == HTTP_CODE_OK) {
    const String& payload = http.getString();
    Serial.println(payload);
  }
  http.end();
}

void uploadStats() {
  String url = statURL;
  url += "?v=";
  url += voltage;

  Serial.print(F("Request: "));
  Serial.println(url);
  http.begin(client, url);
  int16_t code = http.GET();
  Serial.print(F("Response: "));
  Serial.println(code);
  if (code == HTTP_CODE_OK) {
    const String& payload = http.getString();
    Serial.println(payload);
  }
  http.end();
}
