/*
   scheduler per programma acq_v2
   by Fabrizio Allevi
   fabrizio . allevi @ tiscali . it
*/
#include "main.h"

#define LAMP1_PIN 27
#define LAMP2_PIN 14

void initScheduler() {
  pinMode(LAMP1_PIN, OUTPUT);
  pinMode(LAMP2_PIN, OUTPUT);
  digitalWrite(LAMP1_PIN, HIGH);
  digitalWrite(LAMP2_PIN, HIGH);
}

long tLastSched=-60000;
void handleScheduler() {
  if(okTime) {
    long tNow=millis();
    if(tNow<tLastSched+60000)
      return;
    tLastSched=tNow;
    int minuteNow = timeNow.tm_hour*60+timeNow.tm_min;
    if (DEBUG) {
      Serial.print("minuteNow=");
      Serial.println(minuteNow);
    }
    // lamp1
    if(minuteNow>sto.lamp[0].timeStart && minuteNow<sto.lamp[0].timeEnd)
      digitalWrite(LAMP1_PIN, LOW);
    else
      digitalWrite(LAMP1_PIN, HIGH);
    // lamp2
    if(minuteNow>sto.lamp[1].timeStart && minuteNow<sto.lamp[1].timeEnd)
      digitalWrite(LAMP2_PIN, LOW);
    else
      digitalWrite(LAMP2_PIN, HIGH);
  }
  /*delay(1000);
  digitalWrite(LAMP1_PIN, LOW);
  delay(100);
  digitalWrite(LAMP1_PIN, HIGH);
  delay(1000);
  digitalWrite(LAMP2_PIN, LOW);
  delay(100);
  digitalWrite(LAMP2_PIN, HIGH);*/
}
