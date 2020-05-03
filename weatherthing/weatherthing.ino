#include <Wire.h>

#include <OneWire.h>

#include <Adafruit_Sensor.h>

#include <Adafruit_SI1145.h>

#include <Adafruit_BME280.h>

#include <DallasTemperature.h>

#include <DHT.h>

#include <LiquidCrystal.h>

#include <GSMSim.h>

// APN data for Netzclub
#define GPRS_APN       "pinternet.interkom.de" // replace your GPRS APN
#define GPRS_LOGIN     ""    // replace with your GPRS login
#define GPRS_PASSWORD  "" // replace with your GPRS password

//WU Credentials
const char serverWU[] = "weatherstation.wunderground.com";
const char pathWU[] = "/weatherstation/updateweatherstation.php";
const char WU_ID[] = "nah";
const char WU_PASS [] = "NO!";

//SMS Stuff
char* sos_number = "+6556987483654";

//NTP Things
#define TIMEZONE 1
#define NTPSERVER "162.159.200.123"
//Sensor config
//#define WNW_SENS //Wind and Weather/Rain sensors upload enable
#define MPH_PER_RPM 1.492 //RPM per MPH
#define IN_PER_BCKT 0.011 //Inches per rain sensor flip

//Library defines

Adafruit_BME280 bme;
Adafruit_SI1145 uv = Adafruit_SI1145();
GSMSim gsm(10, 11, 9, 52, true);
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

int8_t uv_index = 0;
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
int8_t second   = 0;
int8_t minute   = 0;
int8_t hour     = 0;
int raintime    = 0;
int rainlast    = 0;
uint16_t lastWindCheck = 0;
uint8_t windClicks = 0;
uint8_t avg_index = 0;

int8_t gsmsigp = 100;

bool noerrors = true;
bool nogsmerr = true;

//trash
uint8_t x = 0;

//interrupts

void wind_cnt() {
  windClicks += 1;
}

void rain_cnt() {
  raintime = millis();
  int raininterval = raintime - rainlast;
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
    Serial.println(F("OK"));
    Serial.println(F(""));
  }
  else {
    noerrors = false;
    Serial.println(F("ERROR"));
    Serial.println(F(""));
  }
}

void init_si1145 () {
  Serial.println(F("Init: SI1145"));
  if (uv.begin()) {
    Serial.println(F("OK"));
    Serial.println(F(""));
  }
  else {
    noerrors = false;
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
    Serial.println(F("OK"));
    Serial.println(F(""));
  }
  else {
    noerrors = false;
    Serial.println(F("ERROR"));
    Serial.println(F(""));
  }
}

void init_gsm() {
  Serial.println(F("Init: GSM"));
  gsm.start();
  delay(500);
  if (!gsm.setRingerVolume(100)) {
    Serial.println(F("No GSM/GPRS Module detected!"));
    noerrors = false;
    nogsmerr = false;
    Serial.println(F("ERROR"));
    Serial.println(F(""));
    return;
  }
  delay(50);
  if (!gsm.isSimInserted()) {
    Serial.println(F("No SIM Card detected!"));
    noerrors = false;
    nogsmerr = false;
    Serial.println(F("ERROR"));
    Serial.println(F(""));
    return;
  }
  delay(50);
  Serial.print(F("IMEI: "));
  Serial.println(gsm.moduleIMEI());
  delay(50);
  Serial.println("Setting NTP server");
  Serial.println(gsm.timeSetServer(TIMEZONE, NTPSERVER));
  delay(50);
  gsm.smsTextMode(true);
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
  lcd.print("INIT");
  init_bme280();
  lcd.print(".");
  init_si1145();
  lcd.print(".");
  init_ds18b20();
  lcd.print(".");
  init_dht11();
  lcd.print(".");
  init_gsm();
  lcd.print(". ");

  if (!noerrors) {
    Serial.println(F("!! OH SHIT! Something went wrong! !!"));
    lcd.print("ERROR");
    if (nogsmerr) {
      Serial.println(F("Reporting via SMS."));
      gsm.smsSend(sos_number, "Some sensors reported missing while initializing!");
    }
    while (true) {
      digitalWrite(53, HIGH);
      delay(100);
      digitalWrite(53, LOW);
      delay(100);
    }
  }
  gsm.smsSend(sos_number, "Initialized without errors.");
  Serial.println(F("INIT OK"));
  lcd.print("OK");
}

//reconnecting and disconnecting gprs
void connect_gprs() {
  Serial.print(F("Connecting Network"));
  while (!gsm.isRegistered()) {
    Serial.print(F("."));
    delay(500);
  }
  delay(50);
  Serial.println(F(""));
  Serial.print(F("Connected to "));
  Serial.println(gsm.operatorName());
  Serial.println(F(""));
  delay(50);
  Serial.println(F("Connecting GPRS"));
  gsm.gprsConnectBearer(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD);
  while (!gsm.gprsIsConnected()) {
    Serial.print(F("."));
  }
  delay(50);
  Serial.println(F(""));
  Serial.print(F("Connected!\nIP:"));
  Serial.println(gsm.gprsGetIP());
  delay(50);
  Serial.println(F("Getting Time"));
  gsm.timeSyncFromServer();
  delay(5000);
  Serial.print(F("DateTime: "));
  Serial.println(gsm.timeGetRaw());
  Serial.println(F(""));
  delay(50);

}

void disconnect_gprs() {
  gsm.gprsCloseConn();
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
  press_in = press_hpa * 0.03;
}

void get_si1145 () {
  Serial.println(F("Getting SI1145."));
  uv_index = uv.readUV();
  ir = uv.readIR();
  vis = uv.readVisible();
}

void get_ds18b20() {
  Serial.println(F("Getting DS18B20."));
  ds.requestTemperatures();
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
  gsm.timeGet(x, x, x, hour, minute, second);
}

void get_sensors() {
  Serial.println(F("Getting Sensors."));
  Serial.println(F(""));
  get_bme280();
  get_si1145();
  get_ds18b20();
  get_dht11();
  get_windvane();
  get_windspeed();
  get_gsm();
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
  Serial.println(F(""));
}

void print_lcd() {
  lcd.clear();
  lcd.print("T: ");
  lcd.print(tempc);
  lcd.print("C H: ");
  lcd.print((int)humidity);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("S: ");
  lcd.print((int)soiltempc);
  lcd.print("C P: ");
  lcd.print(press_hpa);
  lcd.print("hPa");
  lcd.setCursor(0, 2);
  lcd.print("IT: ");
  lcd.print(idtempc);
  lcd.print("C IH: ");
  lcd.print((int)idhumid);
  lcd.print("%");
}

//Calculate all of the averages
int avginterval = 15000;
unsigned long timenow_avg = 0;

void calc_avgs() {
  if (millis() >= timenow_avg + avginterval) {
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
  String http = gsm.gprsHTTPGet(req, true);
  Serial.print(F("Response: "));
  Serial.println(http);
}

void uploadOWM() {

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
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
  delay(1000);
  lcd.clear();
}

//Update interval
int uploadinterval = 300000;
unsigned long timenow_upload = 0;

void loop() {
  // put your main code here, to run repeatedly:
  get_sensors();
  calc_avgs();
  print_sensors();
  print_lcd();

  if (millis() >= timenow_upload + uploadinterval) {
    timenow_upload = millis();
    lcd.setCursor(0, 3);
    lcd.print("Upload... ");
    digitalWrite(13, HIGH);
    connect_gprs();
    uploadWU();
    uploadOWM();
    disconnect_gprs();
    digitalWrite(13, LOW);
    lcd.print("DONE");
    delay(1000);
    lcd.setCursor(0, 3);
    lcd.print("                    ");
  }
  delay(5000);
}