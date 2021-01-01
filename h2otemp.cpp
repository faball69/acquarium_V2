/*
   temps acquarium V2
   by Fabrizio Allevi
   fabrizio . allevi @ tiscali . it
*/

#include <OneWire.h>
#include <DallasTemperature.h>
#include "main.h"

// Data wire is plugged into port 13 on the ESP32
#define ONE_WIRE_BUS 13
#define FAN_OUT 12
// precision
#define TEMPERATURE_PRECISION 12

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress h2oThermometer;

void initTemp(void)
{
  delay(1000);
  pinMode(FAN_OUT, OUTPUT); // pump
  digitalWrite(FAN_OUT, HIGH); // 5V=spento 0V=acceso
  // Start up the library
  sensors.begin();
  if(DEBUG) {
    // locate devices on the bus
    Serial.print("Locating devices...");
    Serial.print("Found ");
    Serial.print(sensors.getDeviceCount(), DEC);
    Serial.println(" devices.");
    // report parasite power requirements
    Serial.print("Parasite power is: ");
    if (sensors.isParasitePowerMode())
      Serial.println("ON");
    else
      Serial.println("OFF");
  }
  // search sensor by index 0
  bool b=sensors.getAddress(h2oThermometer, 0);
  if(DEBUG) {
    if (!b)
      Serial.println("Unable to find address for Device 0");
    // show the addresses we found on the bus
    Serial.print("Device 0 Address: ");
    for (uint8_t i = 0; i < 8; i++) {
      // zero pad the address if necessary
      if (h2oThermometer[i] < 16)
        Serial.print("0");
      Serial.print(h2oThermometer[i], HEX);
    }
    Serial.println();
  }
  // set the resolution to 9 bit per device
  sensors.setResolution(h2oThermometer, TEMPERATURE_PRECISION);
  if(DEBUG) {
    Serial.print("Device 0 Resolution: ");
    Serial.print(sensors.getResolution(h2oThermometer), DEC);
    Serial.println();
  }
}

float tempHigh=25.0f;
float tempHyst=0.5f;
long tLastTemp=-1;
bool bFan=false;
void handleWaterTemperature() {
  long tNow=millis();
  if(tNow<tLastTemp+2000)
    return;
  tLastTemp=tNow;
  if(DEBUG)
    Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  float tempC=0.0f;
  tempC=sensors.getTempC(h2oThermometer );
  if(DEBUG) {
    if(tempC==DEVICE_DISCONNECTED_C) {
      Serial.println("Error: Could not read temperature data");
    }
    else {
      Serial.print("Temp C: ");
      Serial.println(tempC);
    }
  }
  // gestione ventola di raffreddamento
  if(!bFan && tempC>tempHigh) {
    bFan=true;
    if(DEBUG)
      Serial.println("Fan is ON!");
  }
  else if(bFan && tempC<tempHigh-tempHyst) {
    bFan=false;
    if(DEBUG)
      Serial.println("Fan is OFF!");
  }
  if(bFan)
    digitalWrite(FAN_OUT, LOW); // 0V=acceso
  else
    digitalWrite(FAN_OUT, HIGH); // 5V=spento
}
