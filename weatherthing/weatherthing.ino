#include <Adafruit_Sensor.h>

#include <Adafruit_SI1145.h>

#include <Adafruit_BME280.h>

#include <Wire.h>

#include <OneWire.h>

#include <DallasTemperature.h>

#include <GSMSim.h>

// PIN Number
#define PINNUMBER "6423" //6=N 4=I 2=C 3=E

// APN data for Netzclub
#define GPRS_APN       "pinternet.interkom.de" // replace your GPRS APN
#define GPRS_LOGIN     ""    // replace with your GPRS login
#define GPRS_PASSWORD  "" // replace with your GPRS password


//Library defines

Adafruit_BME280 bme;
Adafruit_SI1145 uv = Adafruit_SI1145();
GSMSim gsm(9, 10, 11);
OneWire oneWire(12);
DallasTemperature ds(&oneWire);
DeviceAddress insideThermometer;

//WU Credentials
char serverWU[] = "weatherstation.wunderground.com";
char pathWU[] = "/weatherstation/updateweatherstation.php";
char WU_ID[] = "LOL";
char WU_PASS [] = "NO";

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

int uv_index = 0;
float vis = 0;
float ir = 0;

float wind_mph    = 0;
float wind_mph_2m = 0;
float gust_mph    = 0;
int wind_dir      = 0;
float wind_dir_2m = 0;
float rain_1h     = 0;
float rain_24h    = 0;

bool noerrors = true;

//interrupts

void wind_cnt() {

}

void rain_cnt() {

}

//init sensors

void init_bme280() {
  Serial.println(F("Init: BME280"));
  if (bme.begin()) {
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
  Serial.print(F("Locating devices..."));
  ds.begin();
  bool dsdev = ds.getAddress(insideThermometer, 0);

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
  else {
    noerrors = false;
    Serial.println(F("ERROR"));
    Serial.println(F(""));
  }
}

void init_gsm() {
  Serial.println(F("Init: GSM"));
  gsm.start(4800);
  if (!gsm.isSimInserted()) {
    Serial.println(F("No SIM Card detected!"));
    noerrors = false;
    Serial.println(F("ERROR"));
    Serial.println(F(""));
    return;
  }
  Serial.print(F("IMEI: "));
  Serial.println(gsm.moduleIMEI());
  gsm.setRingerVolume(100);
  gsm.setSpeakerVolume(100);
  delay(100);
  Serial.println(F("Connecting Network"));
  while (!gsm.isRegistered()) {
    Serial.print(F("."));
  }
  Serial.println(F(""));
  Serial.print(F("Connected to "));
  Serial.println(gsm.operatorName());
  Serial.println(F(""));

  Serial.println(F("Connecting GPRS"));
  gsm.gprsConnectBearer(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD);
  while (!gsm.gprsIsConnected()) {
    Serial.print(F("."));
  }
  Serial.println(F(""));
  Serial.print(F("Connected!\nIP:"));
  Serial.println(gsm.gprsGetIP());
  Serial.println(F(""));

  Serial.println(F("GSM OK"));
  Serial.println(F(""));
  digitalWrite(13, LOW);
}

void init_all() {
  init_bme280();
  init_si1145();
  init_ds18b20();
  init_gsm();
  if (!noerrors) {
    Serial.println(F("!! OH SHIT! Something went wrong! !!"));
    while (true) {
      digitalWrite(13, HIGH);
      delay(500);
      digitalWrite(13, LOW);
      delay(500);
    }
  }
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

void get_sensors() {
  Serial.println(F("Getting Sensors."));
  Serial.println(F(""));
  get_bme280();
  get_si1145();
  get_ds18b20();
  get_windvane();
  Serial.println(F(""));
  Serial.println(F("Done."));
  Serial.println(F(""));
}

//Calculate all of the averages

void calc_avgs() {

}

//Upload to PWS Networks

void uploadWU() {
  Serial.println(F("Uploading to Wunderground"));
  String req = "http://";
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
  req += "&windspeedmph=";
  req += wind_mph;
  req += "&windspdmph_avg2m=";
  req += wind_mph_2m;
  req += "&windgustmph=";
  req += gust_mph;
  req += "&winddir=";
  req += wind_dir;
  req += "&winddir_avg2m=";
  req += wind_dir_2m;

  String http = gsm.gprsHTTPGet(req, true);
  Serial.print(F("Response: "));
  Serial.println(http);
}

void uploadOWM() {

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  digitalWrite(13, HIGH);
  init_all();
  digitalWrite(13, LOW);
  attachInterrupt(0 , wind_cnt, FALLING);
  attachInterrupt(1 , rain_cnt, FALLING);
}

//Update interval
int uploadinterval = 60000;
unsigned long timenow_upload = 0;

void loop() {
  // put your main code here, to run repeatedly:
  get_sensors();
  calc_avgs();
  if (millis() >= timenow_upload + uploadinterval) {
    timenow_upload += uploadinterval;
    uploadWU();
    uploadOWM();
  }
  delay(500);
}
