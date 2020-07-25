#include <ESP8266HTTPClient.h>

#include <ArduinoOTA.h>

#include <strings_en.h>
#include <WiFiManager.h>

// include the library code:
#include <LiquidCrystal.h>


#define STATS_SERVER "http://your.server/path/latest.php"

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(D2, D1, D5, D6, D7, D8);
HTTPClient http;

byte WiFiSymbol[8] = {
  0b00000,
  0b01110,
  0b10001,
  0b00100,
  0b01010,
  0b00000,
  0b00100,
  0b00000
};

void wm_ap_c(WiFiManager *myWiFiManager) {
  Serial.print(F("WiFi not found. Please configure via AP."));
  lcd.print("ERROR");
}

void setup() {
  // put your setup code here, to run once:
  pinMode(D4, OUTPUT);
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.print("WiFi... ");
  WiFiManager wm;
  wm.setAPCallback(wm_ap_c);
  if (!wm.autoConnect()) {
    ESP.restart();
  }
  WiFi.hostname("WiFi Display");
  lcd.print("OK");
  delay(500);

  lcd.clear();
  lcd.print("OTA... ");
  ArduinoOTA.setHostname("WiFi Display");
  ArduinoOTA.onStart([]() {
    lcd.clear();
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
    lcd.clear();
    lcd.print("OTA... " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    lcd.clear();
    lcd.print("OTA... End");
    digitalWrite(D4, HIGH);
    delay(250);
    digitalWrite(D4, LOW);
    delay(250);
    digitalWrite(D4, HIGH);
    delay(250);
    digitalWrite(D4, LOW);
    delay(250);
    digitalWrite(D4, HIGH);
    delay(250);
    digitalWrite(D4, LOW);
    lcd.clear();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    digitalWrite(D4, HIGH);
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    lcd.setCursor(0, 1);
    lcd.printf("Progress: %u%%\r", (progress / (total / 100)));
    delay(50);
    digitalWrite(D4, LOW);
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
      lcd.clear();
      lcd.print("Auth Failed");
      digitalWrite(D4, HIGH);
      delay(5000);
      digitalWrite(D4, LOW);
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
      lcd.clear();
      lcd.print("Begin Failed");
      digitalWrite(D4, HIGH);
      delay(5000);
      digitalWrite(D4, LOW);
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.print("Connect Failed");
      lcd.clear();
      lcd.println("Connect Failed");
      digitalWrite(D4, HIGH);
      delay(5000);
      digitalWrite(D4, LOW);
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.print("Receive Failed");
      lcd.clear();
      lcd.print("Receive Failed");
      digitalWrite(D4, HIGH);
      delay(5000);
      digitalWrite(D4, LOW);
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
      lcd.clear();
      lcd.print("End Failed");
      digitalWrite(D4, HIGH);
      delay(5000);
      digitalWrite(D4, LOW);
    }
    lcd.clear();
  });
  ArduinoOTA.begin();
  lcd.print("OK");
  delay(500);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(D4, HIGH);
  while (WiFi.status() != WL_CONNECTED) {
    lcd.createChar(0, WiFiSymbol);
    lcd.home();
    lcd.setCursor(14, 0);
    lcd.write((uint8_t)0);
    lcd.setCursor(15, 0);
    lcd.print("!");
    digitalWrite(D4, HIGH);
    delay(250);
    digitalWrite(D4, LOW);
    delay(250);
    Serial.print(".");
  }
  lcd.setCursor(14, 0);
  lcd.print("  ");
  http.begin(STATS_SERVER);
  int httpCode = http.GET();
  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println(payload);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WX Voltage:");
    lcd.setCursor(0, 1);
    lcd.print(payload);
  }
  else {
    lcd.clear();
    lcd.home();
    lcd.print("HTTP Error");
  }
  http.end();
  digitalWrite(D4, LOW);
  delay(5000);
}