#define H_GSM

int8_t gsmsigp = 100;

time_t t_of_day = 0;

WiFiUDP ntpUDP;
NTPClient ntp(ntpUDP, NTPSERVER);

void reset_gsm() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);
}


uint32_t last_ntp_sync = 0;
void sync_time() {
  Serial.println(F("Getting Time"));
  ntp.forceUpdate();
  last_ntp_sync = millis();
}

//reconnecting and disconnecting gprs
bool connect_gprs() {
  return true;
}

void disconnect_gprs() {
}

void get_gsm() {
  Serial.println(F("Getting GSM."));
  gsmsigp = WiFi.RSSI();
  t_of_day=ntp.getEpochTime();
}

void init_gsm() {
  Serial.println(F("Init: WiFi"));

  reset_gsm();

  uint8_t idx = 0;
  char sym[] = { '/', '-', '\\', '|' };
  while (!WiFi.isConnected()) {
    lcd.setCursor(8, 3);
    lcd.write(sym[idx]);
    idx++;
    idx %= sizeof(sym) / sizeof(sym[0]);
  }

  ntp.begin();

  lcd.setCursor(8, 3);
  lcd.print('.');
  Serial.println(F("WiFi OK"));
  Serial.println();
  digitalWrite(13, LOW);
  hasGSM = true;
}
