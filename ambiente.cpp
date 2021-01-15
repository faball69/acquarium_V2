/*
  temp e umidità ambiente
  by Fabrizio Allevi
  fabrizio . allevi @ tiscali . it
*/

#include "DHTesp.h"
#include "main.h"

DHTesp dht;
#define DHT11_PIN 15
float ambT=0.0f, ambH=0.0f;

void initAmb() {
  // Initialize temperature sensor
	dht.setup(DHT11_PIN, DHTesp::DHT11);
  if(DEBUG)
	  Serial.println("DHT initiated");
}

long tLastAmb=-60000;
void handleAmb(float &temperature, float &humidity) {
  long tNew=millis();
  if(tNew>tLastAmb+60000) { // one per minute
    //delay(dht.getMinimumSamplingPeriod());
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    if(DEBUG) {
      Serial.print(dht.getStatusString());
      Serial.print(" humidity=");
      Serial.print(humidity, 1);
      Serial.print(" temperature=");
      Serial.print(temperature, 1);
      Serial.println("°C");
    }
    tLastAmb=tNew;
  }
}
