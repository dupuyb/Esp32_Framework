#include <Arduino.h>
#define DEBUG_FRAME   // Debug frame before header
#include "FrameWeb.h"
FrameWeb frame;

/*
 Main sample for FrameWeb 
*/
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

#define EspLedBlue 2
long previousMillis  = 0;       // Use in loop

// Web socket Use in FrameWeb for external command
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {}

//callback notifying us of the need to save config
void saveConfigCallback () {}

// Force other host name and mac Addresses  // Set config or defaults
// Addr 192.168.1.23
byte new_mac[6] = {0x00,0xAA,0xAA,0x99,0xFF,0xCC}; // TEST 
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
void configModeCallback (WiFiManager *myWiFiManager) {

}

// https://github.com/zhouhan0126/WIFIMANAGER-ESP32
void setup() { 
  Serial.begin(115200);
  Serial.println("Start Frame.Setup()");
  // Set pin mode
  pinMode(EspLedBlue, OUTPUT);     // Led is BLUE at statup
  digitalWrite(EspLedBlue, HIGH);  // After 5 seconds blinking indicate WiFI is OK
  Serial.println("Start Frame.frame_setup()");
  // Start framework
  frame.setup ();
  Serial.println("Server WEB started. V00.1");
    // Init time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //init and get the time
  // Start
  getLocalTime(&timeinfo);
}
 
// Main loop -----------------------------------------------------------------
bool wifiLost;
int count = 0;
char cmd;
void loop() {
    // Get Serial commands
	while (Serial.available() > 0) {
	  uint8_t c = (uint8_t)Serial.read();
	  if (c != 13 && c != 10 ) {
      cmd = c;
    } else {
      if (c==13) {
        if (cmd=='h') { Serial.println(); Serial.println("- Help info:\n\r r=reboot i=myip d=debug m=MAC");}
			  else if (cmd=='r') { ESP.restart(); }
        else if (cmd=='i') { Serial.printf("Heap:%u IP:%s MAC:%s \n\r",ESP.getFreeHeap(), WiFi.localIP().toString().c_str() , WiFi.macAddress().c_str()); }
        else if (cmd=='m') { Serial.println("Mode config feilds (Mac, Host,...)."); forceNewMac(); cmd=' ';}
        else { Serial.printf("Stop serial: %s \n\r",FrameVersion); }
      }
		}
  }
  // Call frame loop
  frame.loop();
  // Is alive
  if ( millis() - previousMillis > 1000L) {
    previousMillis = millis();
    digitalWrite(EspLedBlue, !digitalRead(EspLedBlue));
    getLocalTime(&timeinfo);
  }
}
