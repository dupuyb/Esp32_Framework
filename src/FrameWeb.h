#ifndef FRAMEWEB_H   
#define FRAMEWEB_H

/**
 * @file FrameWeb.h
 * @brief Web Framework for ESP32 - WiFi management, web server, OTA, WebSocket and mDNS
 * @author Bruno Dupuy
 * @version 1.3.0
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
// Expressift system and reset reasons
#include "esp_system.h"

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

#define FrameVersion "1.3.0"   ///< Framework version
#define HOSTDEFAULT "esp32dudu" 

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

/**
 * @class FrameWeb
 * @brief Main framework class managing all ESP32 web services
 *
 * Provides a complete lifecycle for WiFi, HTTP server, OTA, WebSocket and mDNS.
 * Typical usage: instantiate once globally, call setup() in Arduino setup(),
 * and call loop() in Arduino loop().
 */
class FrameWeb {
public:
  /** @brief Constructor – stores a global pointer to this instance for callbacks. */
  FrameWeb();

  /** @brief Destructor – closes WebSocket, HTTP server and WiFiManager portal. */
  ~FrameWeb();

  /**
   * @brief Serialize the current configuration to a JSON string.
   * @return String JSON representation of the Config struct.
   */
  String JsonConfig();

  /**
   * @brief Format a byte count into a human-readable string.
   * @param bytes Raw size in bytes.
   * @return String Formatted string (e.g. "1.5MB", "256KB", "512B").
   */
  String formatBytes(size_t bytes);

  /**
   * @brief Recursively list files and directories to a string.
   * @param ret   Output string that receives the formatted listing.
   * @param fs    File system instance (SPIFFS).
   * @param dirname Root directory path to list.
   * @param levels  Maximum recursion depth (0 = current directory only).
   */
  void listDir(String& ret, fs::FS &fs, const char * dirname, uint8_t levels);

  /**
   * @brief Determine the MIME content-type for a given filename.
   * @param filename File name (including extension).
   * @return String MIME type string (e.g. "text/html", "image/png").
   *
   * Returns "application/octet-stream" when a "download" argument is present
   * in the HTTP request, forcing a browser download.
   */
  String getContentType(String filename);

  /**
   * @brief Save the current configuration to a JSON file on SPIFFS.
   * @param filename Target file path on SPIFFS (e.g. "/config.json").
   * @param config   Configuration structure to persist.
   * @return String Confirmation or error message.
   */
  String saveConfiguration(const char *filename, const Config &config);

  /**
   * @brief Initialize the SPIFFS file system.
   *
   * Automatically formats SPIFFS if it is not already formatted.
   * Sets bit 0 of initSetupState on success.
   */
  void startSPIFFS();

  /**
   * @brief Load configuration from a JSON file on SPIFFS.
   * @param filename File path to read (e.g. "/config.json").
   * @param config   Reference to the Config structure to populate.
   * @param hname    Optional hostname override; used when the file is absent
   *                 or does not contain a HostName entry.
   *
   * Falls back to built-in defaults when the file is missing or invalid:
   *  - HostName: "esp32dudu" (or the value of @p hname)
   *  - MAC: hardware WiFi MAC address
   *  - LoginName / LoginPassword: "admin" / "admin"
   *  - UseToolsLocal: true
   *
   * On JSON parse error the file is recreated with default values.
   * Sets bit 1 of initSetupState on success.
   */
  void loadConfiguration(const char *filename, Config &config, const char* hname=NULL);

  /**
   * @brief Start WiFiManager and establish a WiFi connection.
   *
   * Attempts to connect to the previously saved access point.
   * If the connection fails, an AP is opened so the user can choose a
   * new network via the captive portal web interface.
   * Applies the custom MAC address from Config, and resets saved WiFi
   * credentials when Config::ResetWifi is true.
   * Restarts the device if autoConnect times out.
   * Sets bit 2 of initSetupState on success.
   */
  void startWifiManager();

  /**
   * @brief Start the Arduino OTA (Over-The-Air) update service.
   *
   * Uses Config::HostName as the OTA hostname (visible in the Arduino IDE)
   * and Config::LoginPassword as the OTA password.
   * Sets bit 3 of initSetupState on success.
   */
  void startOTA();

  /**
   * @brief Print all HTTP arguments and headers to Serial (debug helper).
   *
   * Useful during development to inspect incoming POST/GET requests.
   * Output is prefixed with "[F]".
   */
  void showAH();

  /**
   * @brief Convert a WiFi status code to a human-readable string.
   * @param err WiFi status code (WL_* constants).
   * @return const char* Descriptive status string (e.g. "WL_CONNECTED").
   */
  const char* wifiStatus(int err);

  /**
   * @brief Convert an HTTP client error code to a human-readable string.
   * @param err HTTP status or client error code.
   * @return const char* Descriptive string (e.g. "OK", "CONNECTION_REFUSED").
   */
  const char* httpStatus(int err);

  /**
   * @brief Convert an ESP32 reset reason code to a human-readable string.
   * @param reason Reset reason value (RESET_REASON enum).
   * @return const char* Reset cause description (e.g. "POWERON_RESET", "SW_RESET").
   */
  const char* resetReason(int reason);

  /**
   * @brief Start the WebSocket server on port 81.
   *
   * Real-time bidirectional communication is handled by the user-supplied
   * webSocketEvent() callback.
   * Sets bit 4 of initSetupState on success.
   */
  void startWebSocket();

  /**
   * @brief Generate the built-in HTML page for the file uploader.
   * @return String Complete HTML document for the /upload page.
   */
  String simpleUpload();

  /**
   * @brief Generate the built-in HTML index/tools page.
   * @return String Complete HTML document shown when index.html is absent.
   *
   * The page content can be extended via the externalHtmlTools member.
   */
  String simpleIndex();

  /**
   * @brief Generate the built-in HTML page for firmware OTA update.
   * @return String Complete HTML document for the /update page.
   */
  String simpleFirmware();

  /**
   * @brief Serve a file from SPIFFS or fall back to a built-in page.
   * @param path Requested URL path.
   * @return bool true if the request was handled, false otherwise.
   *
   * Prefers the gzip-compressed version of a file when available.
   * Enforces HTTP Basic Authentication for /index.html and /update.
   */
  bool handleFileRead(String path);

  /**
   * @brief Handle a multipart file upload and write it to SPIFFS.
   *
   * Called automatically by the HTTP server for POST requests to /upload.
   * Removes any pre-existing .gz version of the uploaded file.
   * Redirects the browser to /success.html on completion.
   */
  void handleFileUpload();

  /**
   * @brief Build a plain-text 404 error message for the current request.
   * @return String Formatted 404 message including URI, method and arguments.
   */
  String textNotFound();

  /**
   * @brief Handle HTTP POST commands sent to the root route.
   *
   * Recognized commands (via the "cmd" argument):
   *  - "save-config" – persist current configuration to SPIFFS
   *  - "reset-wifi"  – set ResetWifi flag (reprocessed in loop())
   *  - "restart"     – set RebootAsap flag (reprocessed in loop())
   *  - "restore"     – set RestoreAsap flag (reprocessed in loop())
   *
   * All commands require HTTP Basic Authentication.
   * Redirects to "/" after execution.
   */
  inline void handlePost();

  /**
   * @brief Handle requests for paths not explicitly registered.
   *
   * Attempts to serve the path from SPIFFS; sends a 404 response if not found.
   */
  void handleNotFound();

  /**
   * @brief Build HTML for the file explorer listing.
   * @param ret     Output string receiving the generated HTML fragments.
   * @param fs      File system instance (SPIFFS).
   * @param dirname Root directory path to explore.
   * @param levels  Maximum recursion depth.
   *
   * Each file entry includes Download and Remove action buttons.
   */
  void explorer(String& ret, fs::FS &fs, const char * dirname, uint8_t levels);

  /**
   * @brief Send a SPIFFS file to the client as an attachment (download).
   * @param filename Absolute path of the file on SPIFFS (e.g. "/data.json").
   *
   * Sends HTTP 500 if the file cannot be opened.
   */
  void download(String filename);

  /**
   * @brief Send a plain-text directory listing of SPIFFS to the client.
   *
   * Handles the GET /ls route; no authentication required.
   */
  void sendLs();

  /**
   * @brief Handle GET /upload – authenticate and serve the upload page.
   *
   * Serves /upload.html from SPIFFS if present; otherwise serves the
   * built-in upload page when Config::UseToolsLocal is true.
   */
  void upload_get();

  /**
   * @brief Handle the POST /upload response after file transfer completes.
   *
   * Sends a 200 OK acknowledgement to the browser.
   */
  void upload_post();

  /**
   * @brief Handle GET /explorer – authenticate and serve the file explorer.
   *
   * Supports "cmd=remove" and "cmd=download" query parameters to act on files.
   * Requires HTTP Basic Authentication.
   */
  void exploreWeb();

  /**
   * @brief Handle the OTA firmware update POST response.
   *
   * Sends "OK" or "FAIL" and immediately restarts the device.
   */
  void update();

  /**
   * @brief Handle streaming OTA firmware upload data.
   *
   * Called by the HTTP server during multipart upload to flash the firmware.
   */
  void update2();

  /**
   * @brief Register all HTTP routes and start the web server on port 80.
   *
   * Registered routes: /post, /ls, /update, /upload, /explorer, and a
   * catch-all not-found handler.
   * Sets bit 5 of initSetupState on success.
   */
  void startWebServer();

  /**
   * @brief Start the mDNS responder.
   *
   * Announces the device as <HostName>.local and advertises http (80),
   * ws (81) and esp32 (8888) TCP services.
   * Sets bit 6 of initSetupState on success.
   */
  void startMDNS();

  /**
   * @brief Initialize all framework services (call once from Arduino setup()).
   * @param hostname Optional hostname override; passed to loadConfiguration().
   *
   * Calls in order: startSPIFFS → loadConfiguration → startWifiManager →
   * startOTA → startWebSocket → startWebServer → startMDNS.
   */
  void setup(const char* hostname=NULL);

  /**
   * @brief Service all framework handlers (call every iteration from Arduino loop()).
   *
   * Dispatches: HTTP server, WebSocket, ArduinoOTA.
   * Also processes deferred flags: ResetWifi, RebootAsap, RestoreAsap.
   */
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
