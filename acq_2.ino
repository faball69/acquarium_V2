/*
   acq_2 acquarium V2
   by Fabrizio Allevi
   fabrizio . allevi @ tiscali . it
*/

#include "main.h"

void setup(void)
{
  // Open serial communications and wait for port to open:
  if(DEBUG) {
    Serial.begin(9600);
    while (!Serial) {
      delay(1000); // wait for serial port to connect. Needed for native USB port only
    }
  }
  //initTemp();
  //initLevel();
  initWifi();
  if(DEBUG)
    Serial.println("End of setup()");
}

void loop(void)
{
  //handleWaterTemperature();
  //handleWaterLevel();
  handleWifi();
}
