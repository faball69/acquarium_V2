/*
 * wifi service
 * by Fabrizio Allevi
 * fabrizio . allevi @ tiscali . it
 */

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "main.h"
#include "secret.h"

const char* ssid=SECRET_SSID_0;
//const char* ssid=SECRET_SSID_1; per OTA collegare il laptop alla rete1.
const char* password=SECRET_PASS;

//const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
IPAddress ipAddr;
bool okTime=false;

#define LED_BUILTIN 1
void initWifi() {
  //connect to WiFi
  if(DEBUG)
    Serial.printf("Connecting to %s ", ssid);
  else
    pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while(WiFi.status()!=WL_CONNECTED) {
      if(DEBUG) {
        delay(1000);
        Serial.print(".");
      }
      else {
        // led builtin
        for(int v=0;v<2;v++) {
          delay(250);
          digitalWrite(LED_BUILTIN, LOW);
          delay(250);
          digitalWrite(LED_BUILTIN, HIGH);
        }
      }
  }
  if(DEBUG)
    Serial.println(" CONNECTED");

  //init and get the time
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.inrim.it", "time.nist.gov");
  struct tm timeinfo;
  bool b=getLocalTime(&timeinfo);
  if(DEBUG) {
    if(!b)
      Serial.println("Failed to obtain time");
    else {
      Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      okTime=true;
    }
  }
  // OTA service
  ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else // U_SPIFFS
          type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        if(DEBUG)
          Serial.println("Start updating " + type);
      })
      .onEnd([]() {
        if(DEBUG)
          Serial.println("\nEnd");
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        if(DEBUG)
          Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([](ota_error_t error) {
        if(DEBUG) {
          Serial.printf("Error[%u]: ", error);
          if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
          else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
          else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
          else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
          else if (error == OTA_END_ERROR) Serial.println("End Failed");
        }
      });
  ArduinoOTA.begin();
  ipAddr=WiFi.localIP();
  if(DEBUG)
    Serial.println(ipAddr);

  //disconnect WiFi as it's no longer needed
  //WiFi.disconnect(true);
  //WiFi.mode(WIFI_OFF);
}

void handleWifi() {
  ArduinoOTA.handle();
}
