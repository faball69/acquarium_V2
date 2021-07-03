/*
 * web service
 * by Fabrizio Allevi
 * fabrizio . allevi @ tiscali . it
 */

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "main.h"
#include <SPIFFS.h>

WebServer server(80);
stDataStore sto = {{true, 10, 5, 0},
                   {true, 28.0, 0.5},
                   {{true, 14*60, 20*60}, {true, 14*60, 20*60}}
};
bool bForce=false;

bool saveData() {
  byte *p=(byte*)&sto;
  File file = SPIFFS.open("/acq_v2", FILE_WRITE);
  if(!file) {
    if(DEBUG)
      Serial.println("An Error has occurred while opening /acq_v2 file");
    return false;
  }
  else {
    file.write(p, sizeof(sto));
  }
  file.close();
  return true;
}

bool loadData() {
  byte *p=(byte*)&sto;
  if(!SPIFFS.begin(true)){
    if(DEBUG)
      Serial.println("An Error has occurred while mounting SPIFFS");
    return false;
  }
  File file = SPIFFS.open("/acq_v2", FILE_READ);
  if(!file)
    saveData(); // no file we need to create it
  else {
    if(file.size()!=sizeof(sto)) {
      if(DEBUG)
        Serial.println("different size between file and struct....now update file!");
      saveData();
    }
    else
      file.read(p, sizeof(sto));
  }
  file.close();
  return true;
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void _handleRoot(const char *message) {
  char temp[5000];
  memset(temp, 0, sizeof(temp));
  snprintf(temp, 5000,
    "<html><body>\
    <script>if(typeof window.history.pushState == 'function') {window.history.pushState({}, \"Hide\", \"/\");}</script>\
    <h1>acq_v2</h1>\
    <h2>Parameters:</h2>\
    <strong>Environment Temperature: </strong> %.1f <strong> Humidity: </strong> %.1f <strong> Time: </strong> %.02d:%.02d <br><br>\
    <form action=\"/get\">\
    <table style=\"border-collapse: collapse; width: 100%%;\" border=\"1\">\
    <tbody>\<tr>\
    <td style=\"width: 12.5%%; text-align: center;\" rowspan=\"3\"><strong>Lamp3</strong><br>[%s]</td>\
    <td style=\"width: 12.5%%;\">timeStart[hh:mm]:<input type=\"time\" name=\"TS3\" value=\"%.02d:%.02d\" /></td>\
    <td style=\"width: 12.5%%; text-align: center;\" rowspan=\"3\"><strong>Fan</strong><br>[%s]<br>H2oTemp=%.1f</td>\
    <td style=\"width: 12.5%%;\">tempMaxH2o[deg]:<input min=\"0.0\" max=\"40.0\" name=\"PF1\" step=\"0.1\" type=\"number\" value=\"%.1f\" /></td>\
    <td style=\"width: 12.5%%; text-align: center;\" rowspan=\"3\"><strong>Lamp1</strong><br>[%s]</td>\
    <td style=\"width: 12.5%%;\">timeStart[hh:mm]:<input type=\"time\" name=\"TS1\" value=\"%.02d:%.02d\" /></td>\
    <td style=\"width: 12.5%%; text-align: center;\" rowspan=\"3\"><strong>Lamp2</strong><br>[%s]</td>\
    <td style=\"width: 12.5%%;\">timeStart[hh:mm]:<input type=\"time\" name=\"TS2\" value=\"%.02d:%.02d\" /></td>\
    </tr><tr>\
    <td style=\"width: 12.5%%;\">timeEnd[hh:mm]:<input type=\"time\" name=\"TE3\" value=\"%.02d:%.02d\" /></td>\
    <td style=\"width: 12.5%%;\">tempHyst[deg]:<input min=\"0.0\" max=\"1.0\" name=\"PF2\" step=\"0.1\" type=\"number\" value=\"%.1f\" /></td>\
    <td style=\"width: 12.5%%;\">timeEnd[hh:mm]:<input type=\"time\" name=\"TE1\" value=\"%.02d:%.02d\" /></td>\
    <td style=\"width: 12.5%%;\">timeEnd[hh:mm]:<input type=\"time\" name=\"TE2\" value=\"%.02d:%.02d\" /></td>\
    </tr><tr>\
    <td style=\"width: 12.5%%;\">bEnable: <input type=\"checkbox\" name=\"E5\" %s /></td>\
    <td style=\"width: 12.5%%;\">bEnable: <input type=\"checkbox\" name=\"E2\" %s /></td>\
    <td style=\"width: 12.5%%;\">bEnable: <input type=\"checkbox\" name=\"E3\" %s /></td>\
    <td style=\"width: 12.5%%;\">bEnable: <input type=\"checkbox\" name=\"E4\" %s /></td>\
    </tr></tbody>\
    </table><br>\
    <input type=submit value=Change+Store>\
    </form>\<br>\
    %s \
    </body></html>\n", ambT, ambH, timeNow.tm_hour, timeNow.tm_min,
                       /*(bPump?"ON":"OFF"), l0, l1, sto.pump.timeMaxRefill,*/ (bLamp3?"ON":"OFF"), sto.lamp[2].timeStart/60, sto.lamp[2].timeStart%60, (bFan?"ON":"OFF"), H2oTemp, sto.fan.tempMaxH2o, (bLamp1?"ON":"OFF"), sto.lamp[0].timeStart/60, sto.lamp[0].timeStart%60, (bLamp2?"ON":"OFF"), sto.lamp[1].timeStart/60,sto.lamp[1].timeStart%60,
                       /*sto.pump.timeHyst,*/ sto.lamp[2].timeEnd/60, sto.lamp[2].timeEnd%60, sto.fan.tempHyst, sto.lamp[0].timeEnd/60, sto.lamp[0].timeEnd%60, sto.lamp[1].timeEnd/60, sto.lamp[1].timeEnd%60,
                       /*sto.pump.bEnable?"checked":"",*/sto.lamp[2].bEnable?"checked":"",  sto.fan.bEnable?"checked":"", sto.lamp[0].bEnable?"checked":"", sto.lamp[1].bEnable?"checked":"",
                       /*sto.pump.maskErr,*/
                       message);
  server.send(200, "text/html", temp);
}

void handleRoot() {
  _handleRoot("");
}

void handleReset() {
  sto.pump.maskErr=0;
  _handleRoot("Pump Error resetted!");
}

void handleSetPar() {
  // for all pars
  // http://192.168.2.103/?PP1=10&PF1=28.0&TS1=14:00&TS2=14:00&PP2=5&PF2=0.5&TE1=20:00&TE2=20:00&E1=on&E2=on&E3=on&E4=on
  for(int i=0;i<server.args();i++) {
    // pump
    String txtPar/*=server.arg("PP1")*/;
    /*if(txtPar!="")
      sto.pump.timeMaxRefill=txtPar.toInt();
    txtPar=server.arg("PP2");
    if(txtPar!="")
      sto.pump.timeHyst=txtPar.toInt();
    txtPar=server.arg("E1");
    sto.pump.bEnable=(txtPar=="on")?true:false;*/
    // fan
    txtPar=server.arg("PF1");
    if(txtPar!="")
      sto.fan.tempMaxH2o=txtPar.toFloat();
    txtPar=server.arg("PF2");
    if(txtPar!="")
      sto.fan.tempHyst=txtPar.toFloat();
    txtPar=server.arg("E2");
    sto.fan.bEnable=(txtPar=="on")?true:false;
    // lamp1
    txtPar=server.arg("TS1");
    if(txtPar!="")
      sto.lamp[0].timeStart=txtPar.substring(0,2).toInt()*60+txtPar.substring(3).toInt();
    txtPar=server.arg("TE1");
    if(txtPar!="")
      sto.lamp[0].timeEnd=txtPar.substring(0,2).toInt()*60+txtPar.substring(3).toInt();
    txtPar=server.arg("E3");
    sto.lamp[0].bEnable=(txtPar=="on")?true:false;
    // lamp2
    txtPar=server.arg("TS2");
    if(txtPar!="")
      sto.lamp[1].timeStart=txtPar.substring(0,2).toInt()*60+txtPar.substring(3).toInt();
    txtPar=server.arg("TE2");
    if(txtPar!="")
      sto.lamp[1].timeEnd=txtPar.substring(0,2).toInt()*60+txtPar.substring(3).toInt();
    txtPar=server.arg("E4");
    sto.lamp[1].bEnable=(txtPar=="on")?true:false;
    // lamp2
    txtPar=server.arg("TS3");
    if(txtPar!="")
      sto.lamp[2].timeStart=txtPar.substring(0,2).toInt()*60+txtPar.substring(3).toInt();
    txtPar=server.arg("TE3");
    if(txtPar!="")
      sto.lamp[2].timeEnd=txtPar.substring(0,2).toInt()*60+txtPar.substring(3).toInt();
    txtPar=server.arg("E5");
    sto.lamp[2].bEnable=(txtPar=="on")?true:false;
  }
  saveData();
  bForce=true;
  _handleRoot("Parameters saved!");
}

void initWeb() {
  loadData();
  if(DEBUG) {
    Serial.println("");
    if (MDNS.begin("esp32")) {
      Serial.println("MDNS responder started");
    }
  }
  server.on("/", handleRoot);
  server.on("/get", handleSetPar);
  server.on("/reset", handleReset);
  server.onNotFound(handleNotFound);
  server.begin();
  if(DEBUG)
   Serial.println("HTTP server started");
}

 void handleWeb() {
   if(bForce)
    bForce=false;
   server.handleClient();
 }
