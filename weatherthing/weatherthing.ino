#include <GSMSim.h>

#include <GSMSimGPRS.h>

#include <GSMSimHTTP.h>

#include <GSMSimSMS.h>

#include <GSMSimTime.h>

#include <time.h>

#include <Wire.h>

#include <OneWire.h>

#include <Adafruit_Sensor.h>

#include <Adafruit_SI1145.h>

#include <Adafruit_BME280.h>

#include <DallasTemperature.h>

#include <DHT.h>

#include <LiquidCrystal.h>

#include <avr/wdt.h>

// APN data for Netzclub
#define GPRS_APN       "pinternet.interkom.de" // replace your GPRS APN
#define GPRS_LOGIN     ""    // replace with your GPRS login
#define GPRS_PASSWORD  "" // replace with your GPRS password
#define RESET_PIN 9

//WU Credentials
const char serverWU[] = "weatherstation.wunderground.com";
const char pathWU[] = "/weatherstation/updateweatherstation.php";
const char WU_ID[] = "better";
const char WU_PASS [] = "error";

//OWM credentials
const String idOWM = "output";
const String keyOWM = "on lcd";

//SMS Stuff
char* sos_number = "1234567890";

//NTP Things
#define TIMEZONE 1
#define NTPSERVER "162.159.200.123"
//Sensor config
//#define WNW_SENS //Wind and Weather/Rain sensors upload enable
#define MPH_PER_RPM 1.492 //RPM per MPH
#define IN_PER_BCKT 0.011 //Inches per rain sensor flip
uint32_t uploadinterval = 300000;

//Library defines

GSMSim gsm(Serial1, RESET_PIN);
GSMSimGPRS gprs(Serial1, RESET_PIN);
GSMSimHTTP http(Serial1, RESET_PIN);
GSMSimSMS sms(Serial1, RESET_PIN);
GSMSimTime ntp(Serial1, RESET_PIN);

Adafruit_BME280 bme;
Adafruit_SI1145 uv = Adafruit_SI1145();
OneWire oneWire(12);
DallasTemperature ds(&oneWire);
DeviceAddress insideThermometer;
DHT dht(8, DHT11);
LiquidCrystal lcd(39, 43, 45, 47, 49, 51);

//OWM Credentials



//Units measured

float tempf     = 0;
float tempc     = 0;
float dewptc    = 0;
float dewptf    = 0;
float humidity  = 0;
float press_hpa = 0;
float press_in  = 0;
float soiltempf = 0;
float soiltempc = 0;
float idtempc   = 0;
float idtempf   = 0;
float idhumid   = 0;

float uv_index = 0;
float vis = 0;
float ir = 0;

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

int8_t gsmsigp = 100;

uint8_t second = 0;
uint8_t minute = 0;
uint8_t hour   = 0;
uint8_t day    = 0;
uint8_t month  = 0;
uint16_t year   = 0;
struct tm t;
time_t t_of_day;


bool noerrors = true;
bool nogsmerr = true;

unsigned long timenow_upload = 0;

//trash
uint16_t x = 0;

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

//init sensors

void init_bme280() {
  Serial.println(F("Init: BME280"));
  if (bme.begin(0x76)) {
    lcd.print(F("."));
    Serial.println(F("OK"));
    Serial.println(F(""));
  }
  else {
    noerrors = false;
    lcd.print(F("*"));
    Serial.println(F("ERROR"));
    Serial.println(F(""));
  }
}

void init_si1145 () {
  Serial.println(F("Init: SI1145"));
  if (uv.begin()) {
    lcd.print(F("."));
    Serial.println(F("OK"));
    Serial.println(F(""));
  }
  else {
    noerrors = false;
    lcd.print(F("*"));
    Serial.println(F("ERROR"));
    Serial.println(F(""));
  }
}

void init_ds18b20() {
  Serial.println(F("Init: DS18B20"));
  ds.begin();
  // locate devices on the bus
  ds.begin();
  bool dsdev = ds.getAddress(insideThermometer, 0);
  if (!dsdev) {
    noerrors = false;
    lcd.print(F("*"));
    Serial.println(F("ERROR"));
    Serial.println(F(""));
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
    lcd.print(F("."));
    Serial.println(F("OK"));
    Serial.println(F(""));
  }
}

void init_dht11() {
  Serial.println(F("Init: DHT11"));
  dht.begin();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (!isnan(h) || !isnan(t)) {
    lcd.print(F("."));
    Serial.println(F("OK"));
    Serial.println(F(""));
  }
  else {
    noerrors = false;
    lcd.print(F("*"));
    Serial.println(F("ERROR"));
    Serial.println(F(""));
  }
}

void init_gsm() {
  Serial.println(F("Init: GSM"));
  gsm.init();
  sms.initSMS();
  http.init();
  ntp.init();
  delay(500);
  if (!gsm.setRingerVolume(100)) {
    Serial.println(F("No GSM/GPRS Module detected!"));
    noerrors = false;
    nogsmerr = false;
    lcd.print(F("*"));
    Serial.println(F("ERROR"));
    Serial.println(F(""));
    return;
  }
  delay(50);
  if (!gsm.isSimInserted()) {
    Serial.println(F("No SIM Card detected!"));
    noerrors = false;
    nogsmerr = false;
    lcd.print(F("*"));
    Serial.println(F("ERROR"));
    Serial.println(F(""));
    return;
  }
  delay(50);
  Serial.print(F("IMEI: "));
  Serial.println(gsm.moduleIMEI());
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
  connect_gprs();
  disconnect_gprs();
  Serial.println(F("GSM OK"));
  Serial.println(F(""));
  digitalWrite(13, LOW);
}

void init_all() {
  lcd.setCursor(0, 0);
  lcd.print(F("    B  DG"));
  lcd.setCursor(0, 1);
  lcd.print(F("    MUDHS"));
  lcd.setCursor(0, 2);
  lcd.print(F("    EVSTM"));
  lcd.setCursor(0, 3);
  lcd.print(F("INIT"));
  init_bme280();
  init_si1145();
  init_ds18b20();
  init_dht11();
  init_gsm();
  lcd.print(F(" "));

  //noerrors=true; //Ignore all errors and carry on

  if (!noerrors) {
    Serial.println(F("!! OH SHIT! Something went wrong! !!"));

    lcd.print(F("ERROR"));
    if (nogsmerr) {
      Serial.println(F("Reporting via SMS."));
      sms.send(sos_number, "Some sensors reported missing while initializing!");
    }
    uint32_t reset_timenow = millis();
    while (true) {
      if (reset_timenow - millis() > 900000){ //Wait 15 Mins
        wdt_enable(WDTO_15MS); //reset via watchdog
      }
      digitalWrite(53, HIGH);
      delay(100);
      digitalWrite(53, LOW);
      delay(100);
      
    }
  }
  sms.send(sos_number, "Initialized without errors.");
  Serial.println(F("INIT OK"));
  lcd.print(F("OK"));
  delay(500);
  lcd.clear();
}

//reconnecting and disconnecting gprs
void connect_gprs() {
  wdt_enable(WDTO_8S); //Enable Watchdog
  uint8_t dontCrashOnMe = 0;
  Serial.print(F("Connecting Network"));
  while (!gsm.isRegistered()) {
    wdt_reset(); //Reset Watchdog
    Serial.print(F("."));
    delay(420);
    dontCrashOnMe++;
    if (dontCrashOnMe >= 100) {
      return;
    }
  }
  wdt_reset(); //Reset Watchdog
  delay(50);
  Serial.println(F(""));
  Serial.print(F("Connected to "));
  Serial.println(gsm.operatorName());
  Serial.println(F(""));
  wdt_reset(); //Reset Watchdog
  delay(50);
  dontCrashOnMe = 0;
  Serial.println(F("Connecting GPRS"));
  gprs.connect(); //Connect Command
  wdt_reset(); //Reset Watchdog
  while (!gprs.isConnected()) { //Wait for connection.
    Serial.print(F("."));
    wdt_reset(); //Reset Watchdog
    delay(420);
    dontCrashOnMe++; //Increment Trial counter.
    if (dontCrashOnMe >= 100) {
      wdt_reset(); //Reset Watchdog
      gprs.connect(); //Try connecting again
    }
    if (dontCrashOnMe >= 200) {
      while (true) {} //Give up and wait for Watchdog to reset the Arduino
    }
  }
  wdt_disable();
  delay(50);
  Serial.println(F(""));
  Serial.print(F("Connected!\nIP:"));
  Serial.println(gprs.getIP());
  delay(50);
  Serial.println(F("Getting Time"));
  ntp.syncFromServer();
  delay(5000);
  Serial.print(F("DateTime: "));
  Serial.println(ntp.getRaw());
  Serial.println(F(""));
  delay(50);

}

void disconnect_gprs() {
  gprs.closeConn();
  Serial.println(F("GPRS disconnected."));
}

//Write sensors to var

void get_bme280() {
  Serial.println(F("Getting BME280."));
  tempc = bme.readTemperature();
  tempf = (int)round(1.8 * tempc + 32);
  humidity = bme.readHumidity();
  dewptc = (tempc - (100 - humidity) / 5);
  dewptf = (dewptc * 9 / 5) + 32;
  press_hpa = bme.readPressure() / 100.0F;
  press_in = press_hpa * 0.02953F;
}

void get_si1145 () {
  Serial.println(F("Getting SI1145."));
  uv_index = uv.readUV();
  uv_index /= 100.0F;
  ir = uv.readIR();
  vis = uv.readVisible();
}

void get_ds18b20() {
  Serial.println(F("Getting DS18B20."));
  ds.requestTemperatures();
  delay(1000);
  soiltempc = ds.getTempC(insideThermometer);
  soiltempf = DallasTemperature::toFahrenheit(soiltempc);
}

void get_dht11() {
  Serial.println(F("Getting DHT11."));
  idtempc = dht.readTemperature();
  idtempf = dht.readTemperature(true);
  idhumid = dht.readHumidity();
}

void get_windvane() {
  Serial.println(F("Getting Wind Vane."));
  wind_dir = calc_windvane(A0);
}

int calc_windvane(int io) {
  unsigned int vane;
  vane = analogRead(io);
  if (vane < 380) return (113);
  if (vane < 393) return (69);
  if (vane < 414) return (90);
  if (vane < 456) return (158);
  if (vane < 508) return (135);
  if (vane < 551) return (203);
  if (vane < 615) return (180);
  if (vane < 680) return (23);
  if (vane < 746) return (45);
  if (vane < 801) return (248);
  if (vane < 833) return (225);
  if (vane < 878) return (338);
  if (vane < 913) return (0);
  if (vane < 940) return (293);
  if (vane < 967) return (315);
  if (vane < 990) return (270);
  return (360);
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

void get_gsm() {
  Serial.println(F("Getting GSM."));
  gsmsigp = map(gsm.signalQuality(), 0, 31, 0, 100);
  //ntp.get(x, x, x, hour, minute, second);
  String tRaw = ntp.getRaw();
  hour = tRaw.substring(9, 11).toInt(); // 0-23
  minute = tRaw.substring(12, 14).toInt(); // 0-59
  second = tRaw.substring(15, 17).toInt(); // 0-59
  day = tRaw.substring(6, 8).toInt(); // 1-31
  month = tRaw.substring(3, 5).toInt(); // 1-24
  year = (tRaw.substring(0, 2).toInt()) + 2000; // 2000-2099
  t.tm_year = year - 1900 + 30; // Year - 1900
  t.tm_mon = month - 1;         // Month, where 0 = jan
  t.tm_mday = day;          // Day of the month
  t.tm_hour = hour;
  t.tm_min = minute;
  t.tm_sec = second;
  t.tm_isdst = 0;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
  t_of_day = mktime(&t);
}

void get_sensors() {
  Serial.println(F("Getting Sensors."));
  Serial.println(F(""));
  lcd.setCursor(0, 3);
  lcd.print(F("Getting BME280...   "));
  get_bme280();
  lcd.setCursor(0, 3);
  lcd.print(F("Getting SI1145...   "));
  get_si1145();
  lcd.setCursor(0, 3);
  lcd.print(F("Getting DS18B20...  "));
  get_ds18b20();
  lcd.setCursor(0, 3);
  lcd.print(F("Getting DHT11...    "));
  get_dht11();
  lcd.setCursor(0, 3);
  lcd.print(F("Getting Windvane... "));
  get_windvane();
  lcd.setCursor(0, 3);
  lcd.print(F("Getting Windspeed..."));
  get_windspeed();
  lcd.setCursor(0, 3);
  lcd.print(F("Getting GSM&Time... "));
  get_gsm();
  lcd.setCursor(0, 3);
  lcd.print(F("        DONE        "));
  Serial.println(F(""));
  Serial.println(F("Done."));
  Serial.println(F(""));
}

//print values

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
  Serial.println(F(""));
}

void print_lcd() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("T: "));
  lcd.print(tempc);
  lcd.print(F("C H: "));
  lcd.print(humidity);
  lcd.print(F("%"));
  lcd.setCursor(0, 1);
  lcd.print(F("S: "));
  lcd.print((int)soiltempc);
  lcd.print(F("C P: "));
  lcd.print(press_hpa);
  lcd.print(F("hPa"));
  lcd.setCursor(0, 2);
  lcd.print(F("I: "));
  lcd.print((int)idtempc);
  lcd.print(F("C "));
  lcd.print((int)idhumid);
  lcd.print(F("% UV: "));
  lcd.print(uv_index);
  lcd.setCursor(0, 3);
  lcd.print(F("GSM: "));
  lcd.print(gsmsigp);
  lcd.print(F("% "));
  lcd.print(F("ULT: "));
  lcd.print((uint32_t)(millis() - timenow_upload));
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

//does what its name says
String uint64ToString(uint64_t input) {
  String result = "";
  uint8_t base = 10;

  do {
    char c = input % base;
    input /= base;

    if (c < 10)
      c += '0';
    else
      c += 'A' - 10;
    result = c + result;
  } while (input);
  return result;
}


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
  req += "&tempf=";
  req += tempf;
  req += "&dewptf=";
  req += dewptf;
  req += "&humidity=";
  req += humidity;
  req += "&UV=";
  req += uv_index;
  req += "&baromin=";
  req += press_in;
  req += "&soiltempf=";
  req += soiltempf;
  req += "&indoortempf=";
  req += idtempf;
  req += "&indoorhumidity=";
  req += idhumid;
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
  req += ", \"temperature\": ";
  req += tempc;
  req += ", \"humidity\": ";
  req += humidity;
  req += ", \"pressure\": ";
  req += press_hpa;
  req += ", \"dew_point\": ";
  req += dewptc;
#ifdef WNW_SENS
  req += ", \"rain_1h\": ";
  req += rain_1h;
  req += ", \"rain_24h\": ";
  req += rain24h;
  req += ", \"wind_speed\": ";
  req += (wind_mph * 0.44704 );
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  pinMode(53, OUTPUT);
  pinMode(52, OUTPUT);
  pinMode(41, OUTPUT);
  digitalWrite(41, LOW);
  attachInterrupt(0 , wind_cnt, FALLING);
  attachInterrupt(1 , rain_cnt, FALLING);
  Wire.setClock(10000);
  lcd.begin(20, 4);
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(100);
  digitalWrite(52, HIGH);
  delay(100);
  digitalWrite(52, LOW);
  delay(100);
  digitalWrite(53, HIGH);
  delay(100);
  digitalWrite(53, LOW);
  delay(100);
  digitalWrite(13, HIGH);
  init_all();
  digitalWrite(13, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  get_sensors();
  calc_avgs();
  print_sensors();
  print_lcd();

  if ((unsigned long)(millis() - timenow_upload) > uploadinterval) {
    timenow_upload = millis();
    digitalWrite(13, HIGH);
    lcd.setCursor(0, 3);
    lcd.print(F("Connecting...       "));
    connect_gprs();
    lcd.setCursor(0, 3);
    lcd.print(F("Uploading to WU...  "));
    uploadWU();
    lcd.setCursor(0, 3);
    lcd.print(F("Uploading to OWM... "));
    uploadOWM();
    lcd.setCursor(0, 3);
    lcd.print(F("Disconnecting...    "));
    disconnect_gprs();
    digitalWrite(13, LOW);
    delay(1000);
    lcd.setCursor(0, 3);
    lcd.print(F("                    "));
  }
  delay(5000);
}