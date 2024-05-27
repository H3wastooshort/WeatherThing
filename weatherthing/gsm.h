#define H_GSM

int8_t gsmsigp = 100;

uint8_t second = 0;
uint8_t minute = 0;
uint8_t hour = 0;
uint8_t day = 0;
uint8_t month = 0;
uint16_t year = 0;
struct tm t;
time_t t_of_day;

GSMSim gsm(GSM_SERIAL, RESET_PIN, GSM_LED_PIN, true);
GSMSimGPRS gprs(GSM_SERIAL, RESET_PIN, GSM_LED_PIN, true);
GSMSimHTTP http(GSM_SERIAL, RESET_PIN, GSM_LED_PIN, true);
GSMSimSMS sms(GSM_SERIAL, RESET_PIN, GSM_LED_PIN, true);
GSMSimTime ntp(GSM_SERIAL, RESET_PIN, GSM_LED_PIN, true);

void reset_gsm() {
  gsm.reset();
  gsm.init();
  sms.initSMS();
  http.init();
  ntp.init();
  delay(500);
  Serial.print(F("IMEI: "));
  Serial.println(gsm.moduleIMEI());

  gsm.setPhoneFunc(1);
  delay(50);
  Serial.println("Setting APN");
  gprs.gprsInit(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD);
  Serial.println("Setting NTP server");
  Serial.println(ntp.setServer(TIMEZONE, NTPSERVER));
  delay(50);
  sms.setTextMode(true);
  delay(50);
  gsm.setRingerVolume(100);
  delay(50);
  gsm.setSpeakerVolume(100);
  delay(100);
}


uint32_t last_ntp_sync = 0;
void sync_time() {
  Serial.println(F("Getting Time"));
  ntp.syncFromServer();
  last_ntp_sync = millis();
  Serial.print(F("DateTime: "));
  Serial.println(ntp.getRaw());
  Serial.println();
}

//reconnecting and disconnecting gprs
bool connect_gprs() {
  wdt_enable(WDTO_8S);  //Enable Watchdog
  uint8_t dontCrashOnMe = 0;
  Serial.print(F("Connecting Network"));
  while (!gsm.isRegistered()) {
    wdt_reset();  //Reset Watchdog
    Serial.print(F("."));
    delay(420);
    dontCrashOnMe++;
    if (dontCrashOnMe >= 100) {
      return false;
    }
  }
  wdt_reset();  //Reset Watchdog
  delay(50);
  Serial.println(F(""));
  Serial.print(F("Connected to "));
  Serial.println(gsm.operatorName());
  Serial.println(F(""));
  wdt_reset();  //Reset Watchdog
  delay(50);
  dontCrashOnMe = 0;
  Serial.println(F("Connecting GPRS"));
  gprs.connect();                //Connect Command
  wdt_reset();                   //Reset Watchdog
  while (!gprs.isConnected()) {  //Wait for connection.
    Serial.print(F("."));
    wdt_reset();  //Reset Watchdog
    delay(420);
    dontCrashOnMe++;  //Increment Trial counter.
    if (dontCrashOnMe >= 100) {
      Serial.println(F("Timeout. Resetting GSM Module..."));
      wdt_reset();  //Reset Watchdog
      gprs.closeConn();
      reset_gsm();
      gprs.connect();  //Try connecting again
    }
    if (dontCrashOnMe >= 200) {
      return false;
    }
  }
  wdt_disable();
  delay(50);
  Serial.println(F(""));
  Serial.print(F("Connected!\nIP:"));
  Serial.println(gprs.getIP());
  if (millis() - last_ntp_sync > NTP_SYNC_INTERVAL) sync_time();
  delay(50);
  return true;
}

void disconnect_gprs() {
  gprs.closeConn();
  Serial.println(F("GPRS disconnected."));
}

void get_gsm() {
  Serial.println(F("Getting GSM."));
  gsmsigp = map(gsm.signalQuality(), 0, 31, 0, 100);
  //ntp.get(x, x, x, hour, minute, second);
  String tRaw = ntp.getRaw();
  hour = tRaw.substring(9, 11).toInt();          // 0-23
  minute = tRaw.substring(12, 14).toInt();       // 0-59
  second = tRaw.substring(15, 17).toInt();       // 0-59
  day = tRaw.substring(6, 8).toInt();            // 1-31
  month = tRaw.substring(3, 5).toInt();          // 1-24
  year = (tRaw.substring(0, 2).toInt()) + 2000;  // 2000-2099
  t.tm_year = year - 1900 + 30;                  // Year - 1900
  t.tm_mon = month - 1;                          // Month, where 0 = jan
  t.tm_mday = day;                               // Day of the month
  t.tm_hour = hour;
  t.tm_min = minute;
  t.tm_sec = second;
  t.tm_isdst = 0;  // Is DST on? 1 = yes, 0 = no, -1 = unknown
  t_of_day = mktime(&t);
}

void init_gsm() {
  Serial.println(F("Init: GSM"));
  GSM_SERIAL.begin(9600);

  reset_gsm();


  if (!gsm.setRingerVolume(100)) {
    Serial.println(F("No GSM/GPRS Module detected!"));
    lcd.setCursor(0, 0);
    lcd.print(F("No GSM module!"));
    noerrors = false;
    nogsmerr = false;
    lcd.setCursor(8, 3);
    lcd.print('*');
    hasGSM = false;
    return;
  }
  delay(50);
  if (!gsm.isSimInserted()) {
    Serial.println(F("No SIM Card detected!"));
    lcd.setCursor(0, 0);
    lcd.print(F("No SIM!"));
    noerrors = false;
    nogsmerr = false;
    lcd.setCursor(8, 3);
    lcd.print(',');
    hasGSM = false;
    return;
  }

  lcd.setCursor(0, 0);
  lcd_clear_line();
  lcd.setCursor(0, 0);
  lcd.print(F("Testing GPRS..."));

  if (connect_gprs()) {
    Serial.println(F("GPRS ERROR! Check APN, and SIM's Data Plan."));
    lcd.setCursor(0, 0);
    lcd.print(F("GPRS ERROR!"));
    noerrors = false;
    nogsmerr = false;
    lcd.setCursor(8, 3);
    lcd.print('*');
    hasGSM = false;
    return;
  }

  Serial.print(F("Syncing Time..."));
  lcd.setCursor(0, 0);
  lcd_clear_line();
  lcd.setCursor(0, 0);
  lcd.print(F("Syncing Time..."));
  sync_time();

  disconnect_gprs();
  lcd.setCursor(0, 0);
  lcd_clear_line();

  lcd.setCursor(8, 3);
  lcd.print('.');
  Serial.println(F("GSM OK"));
  Serial.println();
  digitalWrite(13, LOW);
  hasGSM = true;
}
