//Upload to PWS Networks

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
    req += tempf;
    req += "&dewptf=";
    req += dewptf;
    req += "&humidity=";
    req += humidity;
    req += "&baromin=";
    req += press_in;
  }
  if (hasSI) {
    req += "&UV=";
    req += uv_index;
  }
  if (hasDS) {
    req += "&soiltempf=";
    req += soiltempf;
  }
  if (hasDHT) {
    req += "&indoortempf=";
    req += idtempf;
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
  req += "&softwaretype=wetterdings_mega_2560";
  Serial.print(F("Request: "));
  Serial.println(req);
  String resp = http.get(req, true);
  Serial.print(F("Response: "));
  Serial.println(resp);
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
    req += press_hpa;
    req += ", \"dew_point\": ";
    req += dewptc;
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
  String resp = http.post(url, req, "application/json", true);
  Serial.print(F("Response: "));
  Serial.println(resp);
}

void uploadStats() {
  String url = statURL;
  url += "?v=";
  url += voltage;

  Serial.print(F("Request: "));
  Serial.println(url);
  String resp = http.get(url, true);
  Serial.print(F("Response: "));
  Serial.println(resp);
}
