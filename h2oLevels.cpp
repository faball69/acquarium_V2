/*
   levels acquarium V2
   by Fabrizio Allevi
   fabrizio . allevi @ tiscali . it
*/

#include "main.h"

#define LEVEL_PIN0 23
#define LEVEL_PIN1 19
#define PUMP_OUT 26

void initLevel() {
  pinMode(LEVEL_PIN0, INPUT_PULLUP); // level sensor 0
  pinMode(LEVEL_PIN1, INPUT_PULLUP); // level sensor 1
  pinMode(PUMP_OUT, OUTPUT); // pump
  digitalWrite(PUMP_OUT, SPENTO); // 5V=spento 0V=acceso
  sto.pump.maskErr=0; // code error on pump 0x0=OK 0x1=timeoutRefill 0x2=warningSensorLevel
}

long tTriggerP=0;
long tOn=-1;
long tLastStop=-1;
long tLastRead=-1000;
long tIniSens=-1;
long tIniHyst=-1;
bool bPump=false, bLastPump=false;
bool bEmpty=false;
int nMaxRetry=3;
int l0=0, l1=0;
void handleWaterLevel() {
  long tRefill=sto.pump.timeMaxRefill*1000;
  long tHyst=sto.pump.timeHyst*1000;
  long tRetry=tRefill*6;
  long tNow=millis();
  if(bForce || tNow>tLastRead+1000) {
    tLastRead=tNow;
    // read levels
    l0=digitalRead(LEVEL_PIN0);
    l1=digitalRead(LEVEL_PIN1);
    if(DEBUG) {
      // locate devices on the bus
      Serial.print("levels l0=");
      Serial.print(l0);
      Serial.print(" levels l1=");
      Serial.println(l1);
    }
    if(sto.pump.bEnable) {
      if(l0^l1) { // warn in sensors levels
        if(tIniSens==-1)
          tIniSens=tNow;
        if(tIniSens!=-1 && tNow>tIniSens+tHyst)
          sto.pump.maskErr|=0x00000002; // code error on pump 0x0=OK 0x1=timeoutRefill 0x2=warningSensorLevel
        bEmpty=false; // comunque lo ritengo pieno
      }
      else {
        tIniSens=-1;
        sto.pump.maskErr&=~0x00000002;  // code error on pump 0x0=OK 0x1=timeoutRefill 0x2=warningSensorLevel
        if(l0&&l1)
          bEmpty=false;
        else
          bEmpty=true;
      }
      // hysteresis on level + time
      if(!bEmpty && bPump) {
        if(tIniHyst==-1)
          tIniHyst=tNow;
        if(tIniHyst!=-1 && tNow<tIniHyst+tHyst)
          bEmpty=true;
      }
      else
        tIniHyst=-1;
      // handle pump
      if(!bPump && tOn==-1 && (tNow>tLastStop+tRetry || tLastStop==-1) && bEmpty && !(sto.pump.maskErr&0x00000004)) {
        tOn=tNow;
        if(DEBUG)
          Serial.println("Pump is ON!");
        bPump=true;
      }
      else if(bPump && !bEmpty || (tOn!=-1 && tNow>tOn+tRefill)) {
        if(tNow>tOn+tRefill) {
          sto.pump.maskErr|=0x00000001; // code error on pump 0x0=OK 0x1=timeoutRefill 0x2=warningSensorLevel
          nMaxRetry--;
        }
        else {
          sto.pump.maskErr&=~0x00000001;  // code error on pump 0x0=OK 0x1=timeoutRefill 0x2=warningSensorLevel
          nMaxRetry=3;
        }
        if(nMaxRetry<1)
          sto.pump.maskErr|=0x00000004; // stop pump nMaxRetry exided
        else
          sto.pump.maskErr&=~0x00000004;
        tOn=-1;
        tLastStop=tNow;
        if(DEBUG)
          Serial.println("Pump is OFF!");
        bPump=false;
      }
    }
    else
      bPump=false;
  }
  if(bPump!=bLastPump) {
    tTriggerP=tNow;
    bLastPump=bPump;
  }
  if(tTriggerP && tNow>tTriggerP+400)
    tTriggerP=0;
  // actuator
  if(tTriggerP)
    digitalWrite(PUMP_OUT, ACCESO);
  else
    digitalWrite(PUMP_OUT, SPENTO);
}
