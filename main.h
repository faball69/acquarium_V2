/*
 * main include file
 * by Fabrizio Allevi
 * fabrizio . allevi @ tiscali . it
 */

// defines
//#define DEBUG true
#define DEBUG false

#include <Arduino.h>
#include <time.h>

// inversion ON OFF for bad relay
#define ACCESO LOW
#define SPENTO HIGH

// temp
void initTemp(void);
void handleWaterTemperature();
// levels
void initLevel();
void handleWaterLevel();
// WiFi
void initWifi();
void handleWifi();
// Web
void initWeb();
void handleWeb();
// ambiente
void initAmb();
void handleAmb(float &temperature, float &humidity);
// scheduler
void initScheduler();
void handleScheduler();

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
struct stPump {
  bool bEnable;  // enable pump
  int timeMaxRefill; // max timeout tMaxRefill
  int timeHyst;  // hyst time
  int maskErr;  // code error on pump 0x0=OK 0x1=timeoutRefill 0x2=warningSensorLevel
};

struct stFan {
  bool bEnable;  // enable fan
  float tempMaxH2o; // max tempertature h2o
  float tempHyst;  // ofs hyst temperature
};

struct stLamp {
  bool bEnable;  // enable lamp
  int timeStart; // start time in min
  int timeEnd;  // end time in min
};

struct stDataStore {
  stPump pump;
  stFan fan;
  stLamp lamp[3];
};
extern stDataStore sto;
extern IPAddress ipAddr;
extern struct tm timeNow;
extern bool okTime;
extern float ambT, ambH;
extern bool bPump, bFan, bLamp1, bLamp2, bLamp3, bForce;
extern int l0, l1;
extern float H2oTemp;
