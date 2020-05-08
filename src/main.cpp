#include <Arduino.h>
#include <U8g2lib.h>
#define DEBUG_FRAME   // Debug frame before header
#include "FrameWeb.h"
FrameWeb frame;

// Oled 128x32 on my i2c
int cntOled = 0;
#define pinSDA  23
#define pinSCL  22
#define i2cADD  0x3c
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ pinSCL, /* data=*/ pinSDA);   // pin remapping with ESP8266 HW I2C
#define OLEDC() u8g2.clearBuffer(); 
#define OLEDS() u8g2.sendBuffer();
#define OLEDF(x,y,format, ...) { \
  char temp[80];\
  snprintf(temp, 80, format, __VA_ARGS__); \
  u8g2.setFontMode(1);	\
  u8g2.setFontDirection(0); \
  u8g2.drawUTF8(x, y, temp); \
}
const uint8_t wifi_1[] = {
   0xfe, 0x1f, 0x01, 0x20, 0xf9, 0x27, 0x0d, 0x2c, 0xe3, 0x31, 0x19, 0x26,
   0xc5, 0x28, 0x31, 0x23, 0x09, 0x24, 0xc1, 0x20, 0xe1, 0x21, 0xe1, 0x21,
   0x01, 0x20, 0xfe, 0x1f};
const uint8_t wifi_0[] = {
   0xfe, 0x1f, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20,
   0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0xc1, 0x20, 0xe1, 0x21, 0xe1, 0x21,
   0x01, 0x20, 0xfe, 0x1f };
const uint8_t jeedom_0[] = {
     0xfe, 0x1f, 0x01, 0x20, 0xc5, 0x2c, 0xed, 0x2d, 0x19, 0x2b, 0xb1, 0x26,
   0x6d, 0x2d, 0xd5, 0x2a, 0xa9, 0x25, 0x79, 0x23, 0xe9, 0x26, 0xe9, 0x2d,
   0x01, 0x20, 0xfe, 0x1f };
const uint8_t jeedom_1[] = {
0xfe, 0x1f, 0x01, 0x20, 0xc1, 0x2c, 0xe1, 0x2d, 0x31, 0x2b, 0xd9, 0x26,
   0xed, 0x2d, 0xf5, 0x2b, 0xe9, 0x27, 0xf9, 0x27, 0xe9, 0x27, 0xe9, 0x23,
   0x01, 0x20, 0xfe, 0x1f };
const uint8_t flame_1[] = {  
   0xfe, 0x1f, 0x01, 0x20, 0x81, 0x21, 0xc1, 0x20, 0xc1, 0x2d, 0x99, 0x2d,
   0xb9, 0x27, 0xf1, 0x23, 0xf9, 0x26, 0x4d, 0x2c, 0x9d, 0x26, 0x39, 0x23,
   0x01, 0x20, 0xfe, 0x1f };
const uint8_t flame_0[] = {  
   0xfe, 0x1f, 0x01, 0x20, 0x01, 0x20, 0x99, 0x2d, 0x55, 0x22, 0xd5, 0x26,
   0x55, 0x22, 0x55, 0x22, 0x4d, 0x22, 0x01, 0x20, 0x01, 0x20, 0xa9, 0x2a,
   0x01, 0x20, 0xfe, 0x1f };
const uint8_t valve_1[] = {  
   0xfe, 0x1f, 0x01, 0x20, 0x81, 0x2e, 0xc1, 0x23, 0x41, 0x21, 0xe1, 0x23,
   0x1d, 0x2c, 0x03, 0x30, 0x01, 0x20, 0x03, 0x30, 0x3d, 0x2e, 0xc1, 0x21,
   0x01, 0x20, 0xfe, 0x1f };
const uint8_t valve_0[] = {  
      0xfe, 0x1f, 0x01, 0x20, 0x81, 0x2e, 0xc1, 0x23, 0xc1, 0x21, 0xe1, 0x23,
   0xfd, 0x2f, 0xff, 0x3f, 0xff, 0x3f, 0xff, 0x3f, 0xfd, 0x2f, 0xc1, 0x21,
   0x01, 0x20, 0xfe, 0x1f };
// Time facilities
const long gmtOffset_sec     = 3600;
const int daylightOffset_sec = 3600;
struct tm timeinfo;            // time struct
const char* ntpServer        = "pool.ntp.org";

// Time HH:MM.ss
String getTime() {
  char temp[10];
  snprintf(temp, 20, "%02d:%02d:%02d", timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec );
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
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  Serial.printf("[%u] get Message: %s\r\n", num, payload);
  switch(type) {
    case WStype_DISCONNECTED:
    break;
    case WStype_CONNECTED:
    {
      IPAddress ip = frame.webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
    }
    break;
    case WStype_TEXT:
    {
      String _payload = String((char *) &payload[0]);
      Serial.println(_payload);
    }
    break;
    case WStype_BIN:
    break;
    default:
    break;
  }
}
//  configModeCallback callback when entering into AP mode
void configModeCallback (WiFiManager *myWiFiManager) {

}
//callback notifying us of the need to save config
void saveConfigCallback () {

}

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
void myConfigModeCallback (WiFiManager *myWiFiManager) {
  // Clear OLED 
  OLEDC();
  u8g2.setFont(u8g2_font_7x14_tf);
  OLEDF( 0, 10, "AP: %s", myWiFiManager->getConfigPortalSSID().c_str()); 
  OLEDF( 0, 31, "IP: %s", WiFi.softAPIP().toString().c_str()); 
  OLEDS();
}

// https://github.com/zhouhan0126/WIFIMANAGER-ESP32
void setup() { 
  Serial.begin(115200);
  Serial.println("Start Frame.Setup()");
// Start Oled 128x32
  u8g2.begin();
  OLEDC();
  u8g2.setFont(u8g2_font_10x20_tf);
  OLEDF( 0, 14, "Survey:%s", "000");
  OLEDS();
  // Set pin mode
  pinMode(EspLedBlue, OUTPUT);     // Led is BLUE at statup
  digitalWrite(EspLedBlue, HIGH);  // After 5 seconds blinking indicate WiFI is OK
  Serial.println("Start Frame.frame_setup()");
  // Start framework
  frame.setup ( myConfigModeCallback );
  Serial.println("Server WEB started. V00.1");
    // Init time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //init and get the time
  // Start
  getLocalTime(&timeinfo);
}
 
// Main loop -----------------------------------------------------------------
bool wifiLost,retJeedom,flame,isValveClosed;
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
  wifiLost=!wifiLost;
  retJeedom=!retJeedom;
  flame=!flame;
  isValveClosed=!isValveClosed;

    // Set Oled dsp
    OLEDC();
    // Icons
    if (wifiLost==0) u8g2.drawXBMP(0,0,14,14,wifi_1);
    else u8g2.drawXBMP(0,0,14,14,wifi_0);
    if (retJeedom) u8g2.drawXBMP(16,0,14,14,jeedom_1);
    else u8g2.drawXBMP(16,0,14,14,jeedom_0);
    if (flame) u8g2.drawXBMP(32,0,14,14,flame_1);
    else u8g2.drawXBMP(32,0,14,14,flame_0);
    if (isValveClosed) u8g2.drawXBMP(48,0,14,14,valve_0);
    else u8g2.drawXBMP(48,0,14,14,valve_1);
    // Text
    u8g2.setFont(u8g2_font_7x14_tf);
    OLEDF( 73,14, "%s", getTime().c_str());
    switch (cntOled) {
      case 0: OLEDF( 0, 31, "Eau: %.3f m3", 12.333); break;
      case 1: OLEDF( 0, 31, "Vanne: %s", (isValveClosed)?("Fermé"):("Ouverte") ); break;
      case 2: OLEDF( 0, 31, "Flamme: %s", (flame)?("Allumé"):("Eteinte") ); break;
      case 3: OLEDF( 0, 31, "Wifi:%s", WiFi.localIP().toString().c_str() ); break;
      case 4: OLEDF( 0, 31, "Mac:%s",  WiFi.macAddress().c_str() ); break;
      case 5: OLEDF( 0, 31, "Jeedom: %s",  ((retJeedom)?("OK"):("Erreur")) ); break;
      case 6: OLEDF( 0, 31, "Date: %s",  getDate(true).c_str() ); break;
      default: cntOled=0; break;
    }
    OLEDS();
    if ( timeinfo.tm_sec % 3==0 ) {
       cntOled++;
    }
  }
}
