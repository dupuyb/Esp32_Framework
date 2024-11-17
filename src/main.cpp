#include <Arduino.h>
//#define DEBUG_FRAMEWEB   // Debug frameweb
#include "FrameWeb.h"
FrameWeb frame;

// Main sample for FrameWeb 
#define EspLedBlue 2
long previousMillis  = 0;       // Use in loop
byte new_mac[6] = {0x00,0xAA,0xAA,0x99,0xFF,0xCC}; // TEST 
char cmd;

// Time facilities
const long gmtOffset_sec     = 3600;
const int daylightOffset_sec = 3600;
struct tm timeinfo;            // time struct
const char* ntpServer        = "pool.ntp.org";

// Time HH:MM.ss
String getTime() {
  char temp[10];
  snprintf(temp, 10, "%02d:%02d:%02d", timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec );
  return String(temp);
}

// Date as europeen format
String getDate(bool sh = false){
  char temp[20];
  if (sh)
    snprintf(temp, 20, "%02d/%02d/%04d",
           timeinfo.tm_mday, (timeinfo.tm_mon+1), (1900+timeinfo.tm_year) );
  else
    snprintf(temp, 20, "%02d/%02d/%04d %02d:%02d:%02d",
           timeinfo.tm_mday, (timeinfo.tm_mon+1), (1900+timeinfo.tm_year),  timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec );
  return String(temp);
}

// Web socket Use in FrameWeb for external command
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {}

//callback notifying us of the need to save config
void saveConfigCallback () {}

// Force other host name and mac Addresses  
// Set config or defaults
void forceNewMac() {
  strlcpy(frame.config.HostName, "esp32dudu",sizeof(frame.config.HostName));
  for (int i=0; i<6; i++)
    frame.config.MacAddress[i] = new_mac[i];
  frame.config.ResetWifi = false;
  strlcpy(frame.config.LoginName, "admin",sizeof(frame.config.LoginName));
  strlcpy(frame.config.LoginPassword, "admin",sizeof(frame.config.LoginPassword));
  frame.config.UseToolsLocal = true;
  String ret = frame.saveConfiguration(frame.filename, frame.config);
}

//  configModeCallback callback when entering into AP mode
void configModeCallback (WiFiManager *myWiFiManager) {}

// https://github.com/zhouhan0126/WIFIMANAGER-ESP32
void setup() { 
  Serial.begin(115200);
  #ifdef DEBUG_FRAMEWEB
  Serial.println("Start Frame.Setup()");
  #endif
  // Set pin mode
  pinMode(EspLedBlue, OUTPUT);     // Led is BLUE at statup
  digitalWrite(EspLedBlue, HIGH);  // After 5 seconds blinking indicate WiFI is OK
  // Start framework
  frame.setup ();
    // Init time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //init and get the time
  // Start
  getLocalTime(&timeinfo);
  #ifdef DEBUG_FRAMEWEB
  Serial.println("Server WEB started.");
  #endif
}
 
// Main loop -----------------------------------------------------------------
void loop() {
  #ifdef DEBUG_FRAMEWEB
    // Get Serial commands
	while (Serial.available() > 0) {
	  uint8_t c = (uint8_t)Serial.read();
	  if (c != 13 && c != 10 ) {
      cmd = c;
      if (cmd=='h') { Serial.println("\n\t- Help info:\n\r r=reboot i=show info. f=force MAC(00-AA-AA-99-FF-CC)");}
		  else if (cmd=='r') { ESP.restart(); }
      else if (cmd=='i') { Serial.printf("Heap:%u Mac:%s IP:%s Version:%s\n\r",ESP.getFreeHeap(), WiFi.macAddress().c_str(), WiFi.localIP().toString().c_str(),FrameVersion );}
      else if (cmd=='f') { Serial.println("Mode config feilds (Mac, Host,...)."); forceNewMac();}
      cmd = ' ';
		}
  }
  #endif
  // Call frame loop
  frame.loop();
  // Is alive
  if ( millis() - previousMillis > 1000L) {
    previousMillis = millis();
    digitalWrite(EspLedBlue, !digitalRead(EspLedBlue));
    getLocalTime(&timeinfo);
  }
}
