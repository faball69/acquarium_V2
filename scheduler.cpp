/*
   scheduler per programma acq_v2
   by Fabrizio Allevi
   fabrizio . allevi @ tiscali . it
*/
#include "main.h"
#define LAMP1_PIN 27
#define LAMP2_PIN 14
#define LAMP3_PIN 26

void initScheduler() {
  pinMode(LAMP1_PIN, OUTPUT);
  pinMode(LAMP2_PIN, OUTPUT);
  pinMode(LAMP3_PIN, OUTPUT);
  digitalWrite(LAMP1_PIN, SPENTO);
  digitalWrite(LAMP2_PIN, SPENTO);
  digitalWrite(LAMP3_PIN, SPENTO);
}

long tTriggerL1=0;
long tTriggerL2=0;
long tTriggerL3=0;
bool bLamp1, bLamp2, bLamp3;
bool bLastLamp1, bLastLamp2, bLastLamp3;
long tLastSched=-60000;
void handleScheduler() {
  if(okTime) {
    long tNow=millis();
    if(bForce || tNow>tLastSched+60000) {
      tLastSched=tNow;
      int minuteNow = timeNow.tm_hour*60+timeNow.tm_min;
      if (DEBUG) {
        Serial.print("minuteNow=");
        Serial.println(minuteNow);
      }
      // lamp1
      if(sto.lamp[0].bEnable && minuteNow>sto.lamp[0].timeStart && minuteNow<sto.lamp[0].timeEnd)
        bLamp1=true;
      else
        bLamp1=false;
      // lamp2
      if(sto.lamp[1].bEnable && minuteNow>sto.lamp[1].timeStart && minuteNow<sto.lamp[1].timeEnd)
        bLamp2=true;
      else
        bLamp2=false;
      // lamp3
      if(sto.lamp[2].bEnable && minuteNow>sto.lamp[2].timeStart && minuteNow<sto.lamp[2].timeEnd)
        bLamp3=true;
      else
        bLamp3=false;
    }
    //Lamp1
    if(bLamp1!=bLastLamp1) {
      tTriggerL1=tNow;
      bLastLamp1=bLamp1;
    }
    if(tTriggerL1 && tNow>tTriggerL1+400)
      tTriggerL1=0;
    // actuator
    if(tTriggerL1)
      digitalWrite(LAMP1_PIN, ACCESO);
    else
      digitalWrite(LAMP1_PIN, SPENTO);
    //Lamp2
    if(bLamp2!=bLastLamp2) {
      tTriggerL2=tNow;
      bLastLamp2=bLamp2;
    }
    if(tTriggerL2 && tNow>tTriggerL2+400)
      tTriggerL2=0;
    // actuator
    if(tTriggerL2)
      digitalWrite(LAMP2_PIN, ACCESO);
    else
      digitalWrite(LAMP2_PIN, SPENTO);
    //Lamp3
    if(bLamp3!=bLastLamp3) {
      tTriggerL3=tNow;
      bLastLamp3=bLamp3;
    }
    if(tTriggerL3 && tNow>tTriggerL3+400)
      tTriggerL3=0;
    // actuator
    if(tTriggerL3)
      digitalWrite(LAMP3_PIN, ACCESO);
    else
      digitalWrite(LAMP3_PIN, SPENTO);
  }
}
