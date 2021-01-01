/*
 * main include file
 * by Fabrizio Allevi
 * fabrizio . allevi @ tiscali . it
 */

// defines
#define DEBUG true
//#define DEBUG false

#include <Arduino.h>
#include <time.h>

// temp
void initTemp(void);
void handleWaterTemperature();
// levels
void initLevel();
void handleWaterLevel();
// WiFi
void initWifi();
void handleWifi();
// scheduler


/*
struct tm {
  int	tm_sec;
  int	tm_min;
  int	tm_hour;
  int	tm_mday;
  int	tm_mon;
  int	tm_year;
  int	tm_wday;
  int	tm_yday;
  int	tm_isdst;
#ifdef __TM_GMTOFF
  long	__TM_GMTOFF;
#endif
#ifdef __TM_ZONE
  const char *__TM_ZONE;
#endif
};
*/
