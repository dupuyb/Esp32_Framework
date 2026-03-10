#ifndef FRAMEWEB_H   
#define FRAMEWEB_H

/**
 * @file FrameWeb.h
 * @brief Web Framework for ESP32 - WiFi management, web server, OTA, WebSocket and mDNS
 * @author Bruno Dupuy
 * @version 1.2.6
 * 
 * This framework provides a complete solution for:
 *  - WiFi configuration with WiFiManager
 *  - HTTP web server for files and tools
 *  - OTA update (Over-The-Air)
 *  - WebSocket for bidirectional communication
 *  - mDNS for device discovery
 *  - SPIFFS file system management
 *  - HTTP basic authentication
 */

// ============ Dependency includes ============
// SPIFFS file system for ESP32
#include <FS.h>
#include <SPIFFS.h>

// JSON - ArduinoJson library (by Benoit Blanchon)
#include <ArduinoJson.h>

// OTA update (Over-The-Air)
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Update.h>

// WiFi and web server
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>

// WebSocket for real-time communication
#include <WebSocketsServer.h>

// mDNS for network discovery
#include "ESPmDNS.h"

// ============ Debug macros ============
/** 
 * Macros for debug output with "[F]" prefix (FrameWeb)
 * These macros are disabled if DEBUG_FRAMEWEB is not defined
 */
#ifdef DEBUG_FRAMEWEB
  #define FDBX(...) {Serial.print("[F]");Serial.print(__VA_ARGS__);}   ///< Simple output
  #define FDBXLN(...) {Serial.print("[F]");Serial.println(__VA_ARGS__);}   ///< Output with newline
  #define FDBXMF(...) {Serial.print("[F]");Serial.printf(__VA_ARGS__);}   ///< Formatted output
#else
  #define FDBX(...)
  #define FDBXLN(...)
  #define FDBXMF(...)
#endif

#define FrameVersion "1.2.6"   ///< Framework version

/**
 * @struct Config
 * @brief Configuration structure stored in config.json file
 * 
 * This structure contains all configuration parameters for the device.
 * Default values are:
 *  - HostName: "esp32dudu" (accessible on the local network)
 *  - MAC: hardware WiFi MAC address
 *  - LoginName: "admin"
 *  - LoginPassword: "admin"
 *  - ResetWifi: false (automatic reconnection to previous AP)
 *  - UseToolsLocal: true (use lightweight web interface if upload.html missing)
 */
struct Config {
  char HostName[20];              ///< Device hostname (ex: esp32dudu)
  byte MacAddress[6];             ///< WiFi MAC address
  bool ResetWifi;                 ///< Force reconnection in AP mode to change WiFi
  char LoginName[20];             ///< Username for HTTP authentication
  char LoginPassword[20];         ///< Password for HTTP authentication
  bool UseToolsLocal;             ///< Use lightweight web interface if external file missing
};

// ============ External callback functions ============
/**
 * @brief Callback called when AP (Access Point) mode is activated
 * 
 * This function must be implemented by the user to handle
 * AP mode activation (for example, blink an LED)
 * 
 * @param myWiFiManager Pointer to the WiFiManager instance
 */
void configModeCallback (WiFiManager *myWiFiManager);

/**
 * @brief Callback called when configuration must be saved
 * 
 * Implemented by the user to signal a configuration save.
 */
void saveConfigCallback();

/**
 * @brief Callback for WebSocket events
 * 
 * Implemented by the user to handle received WebSocket messages.
 * 
 * @param num WebSocket client number
 * @param type Event type: WStype_DISCONNECTED, WStype_CONNECTED, WStype_TEXT, etc.
 * @param payload Received data
 * @param length Data length
 */
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
  void startWifiManager();
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
  
  void setup(const char* hostname=NULL);
  void loop();

  // ============ Global variables ============
  const char *version = FrameVersion;           ///< Framework version
  const char *filename = "/config.json";       ///< Configuration file path in SPIFFS
  
  // Control flags
  bool RebootAsap = false;                      ///< Flag: immediate restart required (ex: after OTA)
  bool RestoreAsap = false;                     ///< Flag: reset to factory settings
  bool ResetWifi = false;                       ///< Flag: WiFi reset to choose new AP
  
  Config config;                                ///< Structure containing current configuration
  File fsUploadFile;                            ///< Temporary file for upload
  String externalHtmlTools = "";                ///< Custom HTML to add to tools page
  
  /**
   * @brief Initialization state (bitmask)
   * 
   * Each bit indicates if a service was successfully initialized:
   *  - Bit 0 (1<<0): startSPIFFS
   *  - Bit 1 (1<<1): loadConfiguration
   *  - Bit 2 (1<<2): startWifiManager
   *  - Bit 3 (1<<3): startOTA
   *  - Bit 4 (1<<4): startWebSocket
   *  - Bit 5 (1<<5): startWebServer
   *  - Bit 6 (1<<6): startMDNS
   */
  uint8_t initSetupState = 0;
  
  // Objects for JSON management (ArduinoJson v7)
  JsonDocument jsonBuffer = JsonDocument();
  
  // Network service objects
  WiFiManager wifiManager;                      ///< WiFi manager with AP mode
  WebServer server;                             ///< HTTP server on port 80
  WebSocketsServer webSocket = WebSocketsServer(81);  ///< WebSocket server on port 81
};
#endif 
