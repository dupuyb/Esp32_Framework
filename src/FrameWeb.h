#ifndef FRAMEWEB_H
#define FRAMEWEB_H
// File FS SPI Flash File System
// #include "eth_phy/phy.h"
#include <FS.h>
#include <SPIFFS.h>
// JSon install ArduinoJson by Benoit Blanchon
#include <ArduinoJson.h>
// OTA need WiFiUdp
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Update.h>
// WiFimanager need WiFi WebServer WiFimanager
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
 //https://github.com/tzapu/WiFiManager  NOT ESP32 full compatible.
#include <WiFiManager.h>
// WebSocket
#include <WebSocketsServer.h>
// mDNS
#include "ESPmDNS.h"

// Debug macro
#ifdef DEBUG_FRAMEWEB
  #define FDBX(...) {Serial.print("[F]");Serial.print(__VA_ARGS__);}
  #define FDBXLN(...) {Serial.print("[F]");Serial.println(__VA_ARGS__);}
  #define FDBXMF(...) {Serial.print("[F]");Serial.printf(__VA_ARGS__);}
#else
  #define FDBX(...)
  #define FDBXLN(...)
  #define FDBXMF(...)
#endif

#define FrameVersion "1.2.4"

// Default value in loadConfiguration function
struct Config {            // First connexion LAN:esp32dudu IPAddress(192,168,0,1)
  char HostName[20];       // Default hostname "esp32dudu"
  byte MacAddress[6];      // Default MAC from HW es:0x30,0xAE,0xA4,0x90,0xFD,0xC8
  bool ResetWifi;          // Default false WiFimanager reconnect with last data
  char LoginName[20];      // Default login admin For OTA and Web tools
  char LoginPassword[20];  // Default password admin
  bool UseToolsLocal;      // True if simpleUpload must be called in case of not Upload.html
};

//---- These callback functions must be defined extrenally ------
// configModeCallback callback when entering into AP mode
void configModeCallback (WiFiManager *myWiFiManager) ;

// callback notifying us of the need to save config
void saveConfigCallback () ;

// callback for web socket event
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

// Frame Web master class
class FrameWeb {
public:
  FrameWeb();
  ~FrameWeb();

  String JsonConfig();
  String formatBytes(size_t bytes);
  void listDir(String& ret, fs::FS &fs, const char * dirname, uint8_t levels);
  String getContentType(String filename);
  String saveConfiguration(const char *filename, const Config &config);
  void startSPIFFS();
  void loadConfiguration(const char *filename, Config &config, const char* hname=NULL);
  void startWifiManager( /* void (*func)(WiFiManager* myWiFiManager ) = NULL*/ );
  void startOTA();
  void showAH();
  const char* wifiStatus(int err);
  const char* httpStatus(int err);
  const char* resetReason(int reason);
  void startWebSocket();
  String simpleUpload();
  String simpleIndex();
  String simpleFirmware();
  bool handleFileRead(String path);
  void handleFileUpload();
  String textNotFound();
  inline void handlePost();
  void handleNotFound();
  void explorer(String& ret, fs::FS &fs, const char * dirname, uint8_t levels);
  void download(String filename);
  void sendLs();
  void upload_get();
  void upload_post();

  void exploreWeb();
  void update();
  void update2();
  void startWebServer();
  void startMDNS();
  
  void setup( /*void (*func)(WiFiManager* myWiFiManager)=NULL,*/ const char* hostname=NULL);
  void loop();

  // variables Global
  const char *version = FrameVersion;
  const char *filename = "/config.json"; // file configuration
  bool RebootAsap      = false;   // Error OTA
  bool RestoreAsap     = false;   // Reset to factory settings all was cleaned
  bool ResetWifi       = false;   // only Wifi
  Config config;                  // Struct Config
  File fsUploadFile;              // File variable to temporarily store the received file
  String externalHtmlTools = "";  // Html paragraph append on tools

  //Init JSON ArduinoJson 6
  DynamicJsonDocument jsonBuffer = DynamicJsonDocument(500);
  // Wifi
  WiFiManager wifiManager;
  // services WEB
  WebServer server; //(80);
  WebSocketsServer webSocket = WebSocketsServer(81);

};
#endif 