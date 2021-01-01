/*
   levels acquarium V2
   by Fabrizio Allevi
   fabrizio . allevi @ tiscali . it
*/

#include "main.h"

#define LEVEL_PIN0 18
#define LEVEL_PIN1 19
#define PUMP_OUT 15

void initLevel() {
  pinMode(LEVEL_PIN0, INPUT); // level sensor 0
  pinMode(LEVEL_PIN1, INPUT); // level sensor 1
  pinMode(PUMP_OUT, OUTPUT); // pump
  digitalWrite(PUMP_OUT, HIGH); // 5V=spento 0V=acceso
}

long timeoutMax=10000; // 10sec massimi di riempimento
long timeoutLast=60000; // 60sec per riprovare a riempire
long tOn=-1;
long tLastStop=-1;
long tLastRead=-1;
bool bPump=false;
void handleWaterLevel() {
  long tNow=millis();
  if(tNow<tLastRead+5000 && tOn==-1)
    return;
  tLastRead=tNow;
  // read levels
  int l0=digitalRead(LEVEL_PIN0);
  int l1=digitalRead(LEVEL_PIN1);
  if(DEBUG) {
    // locate devices on the bus
    Serial.print("levels l0=");
    Serial.print(l0);
    Serial.print(" levels l1=");
    Serial.println(l1);
  }
  // handle pump
  if(!bPump && tOn==-1 && (tNow>tLastStop+timeoutLast || tLastStop==-1) && l0 && l1) {
    tOn=tNow;
    if(DEBUG)
      Serial.println("Pump is ON!");
    bPump=true;
  }
  else {
    if(bPump && ((!l0 && !l1) || (tOn!=-1 && tNow>tOn+timeoutMax))) {
      tOn=-1;
      tLastStop=tNow;
      if(DEBUG)
        Serial.println("Pump is OFF!");
      bPump=false;
    }
  }
  if(bPump)
    digitalWrite(PUMP_OUT, LOW);
  else
    digitalWrite(PUMP_OUT, HIGH);
}
