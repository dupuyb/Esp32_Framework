#include "FrameWeb.h"

/**
 * @file FrameWeb.cpp
 * @brief Implementation of the ESP32 Web Framework
 * 
 * This file contains the complete implementation of the web system for ESP32,
 * including WiFi management, HTTP server, OTA and WebSocket handling.
 */

// ============ Pre-generated HTML (generated from FrameWeb.html) ============
// These character strings PROGMEM are stored in Flash to save RAM
 //! Warning  paragraph between 'Start Generated' and 'End Generated' comment is generated 
 //! from src/FrameWeb.html file, any modification will be lost after next generation. To modify HTML content, edit src/FrameWeb.html file 
 //! and re-generate FrameWeb.cpp file using extra_script.py python script.
 //! See platfoemio.ini for configuration of extra_script.py script execution during build process.
//---- Start Generated from src/FrameWeb.html file --- 2026-03-13 10:25:15
const char HTTP_HEADAL[] PROGMEM = "<!DOCTYPE html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>ESP32</title><style>:root {--primary: #4CAF50;--primary-dark: #45a049;--bg-light: #f5f5f5;--bg-item: #f9f9f9;--shadow: 0 2px 4px rgba(0,0,0,.1);--radius: 8px;}* { margin: 0; padding: 0; box-sizing: border-box; }body { font-family: Arial, sans-serif; background: var(--bg-light); padding: 10px; color: #333; }.container { max-width: 900px; margin: 0 auto; background: #fff; border-radius: var(--radius); box-shadow: var(--shadow); padding: 20px; }.section-title, header { border-bottom: 2px solid #ddd; margin-bottom: 15px; padding-bottom: 10px; }header { text-align: center; border-color: var(--primary); margin-bottom: 30px; }h1 { background: linear-gradient(135deg, #87CEEB, var(--primary)); color: #fff; padding: 15px; border-radius: 5px; }.action-item, .form-group {display: grid; grid-template-columns: 1fr auto; gap: 15px; align-items: center;padding: 10px; background: var(--bg-item); border-left: 4px solid var(--primary); border-radius: 4px; margin-bottom: 10px;}.button-group { display: flex; gap: 8px; flex-wrap: wrap; justify-content: flex-end; }button {padding: 8px 16px; background: var(--primary); color: #fff; border: none; border-radius: 4px;cursor: pointer; transition: 0.3s;}button:hover { background: var(--primary-dark); }.system-actions { background: #f0f8ff; padding: 15px; border-radius: 4px; margin-bottom: 20px; }.footnotes { font-size: 12px; color: #666; background: #fffacd; border-left: 4px solid #f0ad4e; padding: 15px; border-radius: 4px; }@media (max-width: 768px) {.action-item, .form-group { grid-template-columns: 1fr; }.button-group { justify-content: flex-start; }h1 { font-size: 1.2rem; }}</style></head>";
//---- len : 1800 bytes
const char HTTP_BODYUP[] PROGMEM = "<body><div class='container'><header><h1>ESP32 Uploader</h1></header><p class='message'>Transfer a file to SPIFFS. Gzip supported.</p><div class='section'><div class='section-title'>File Upload</div><div class='action-list'><div class='action-item'><div>- Select a file to upload to the EFS.</div><div class='button-group'><form method='post' enctype='multipart/form-data' style='display:flex;gap:8px;align-items:center;flex-wrap:wrap;'><input type='file' name='Choose file' accept='.gz,.html,.ico,.js,.json,.css,.png,.gif,.bmp,.jpg,.xml,.pdf,.htm' style='min-width:200px;'><button type='submit' name='submit'>Upload</button></form></div></div></div></div><div style='text-align:center;'><button onclick=\"window.location='/';\">Back</button></div></div></body></html>";
//---- len : 806 bytes
const char HTTP_BODYID[] PROGMEM = "<body><div class='container'><header><h1>ESP32 Dudu Tools</h1></header><p class='message'>Index.html file not found on the device.</p><div class='section'><div class='section-title'>Configuration Features</div><div class='action-list'><div class='action-item'><div>- Browse and manage files in the EFS.</div><div class='button-group'><button onclick=\"window.location='/explorer';\">Explorer</button><button onclick=\"window.location='/ls';\">List</button></div></div><div class='action-item'><div>- View the configuration file used at startup</div><div class='button-group'><button onclick=\"window.location='/config.json';\">Config.json</button></div></div><div class='action-item'><div>- Upload files to the Embedded File System</div><div class='button-group'><button onclick=\"window.location='/upload';\">Uploader</button></div></div><div class='action-item'><div>- Update ESP32 firmware over the air (OTA)</div><div class='button-group'><button onclick=\"window.location='/update';\">Update</button></div></div></div></div>";
//---- len : 1067 bytes
const char HTTP_BODYI0[] PROGMEM = "<div class='section'><div class='section-title'>System Actions</div><div class='system-actions'><ul><li>- <b>Restart</b>: Reboots the ESP32.</li><li>- <b>Save Config</b>: Saves settings to EFS. *</li><li>- <b>Restore</b>: Restores defaults and deletes files. **</li><li>- <b>Reset WiFi</b>: Resets WiFi settings. ***</li></ul></div><div class='form-group'><label for='cmd'>- Select a command from the list:</label><form action='/' method='post' style='display: flex; gap: 8px; align-items: center;'><select id='cmd' name='cmd' style='flex: 1; min-width: 150px;'><option value='none'></option><option value='restart'>Restart</option><option value='save-config'>Save Config *</option><option value='reset-wifi'>Reset WiFi ***</option><option value='restore'>Restore **</option></select><button type='submit'>Apply</button></form></div></div>";
//---- len : 877 bytes
const char HTTP_BODYI1[] PROGMEM = "<div class='footnotes'><p>* All parameters in config.json are affected.</p><p>** Login, password, and flags reset to default. EFS reformatted.</p><p>*** Device reboots in AP mode to select a new WiFi network.</p></div></div></body></html>";
//---- len : 276 bytes
const char HTTP_FIRM0[] PROGMEM = "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script><body><div class='container'><header><h1>ESP32 Firmware Update</h1></header><p class='message'>Use this tool to update the ESP32 firmware over the air. Default format: .bin</p><div class='section'><div class='section-title'>Firmware Update</div><div class='action-item'><div>- Upload a .bin firmware file to update the ESP32.</div><div class='button-group'><form method='POST' action='#' enctype='multipart/form-data' id='upload_form' style='display: flex; gap: 8px; align-items: center; flex: 1;'><input type='file' accept='.bin' name='update' style='flex: 1; min-width: 150px;'><button type='submit'>Update</button></form></div></div></div><div class='section'><div class='section-title'>Progress</div><div class='action-item' style='justify-content: center;'><div id='prg' style='text-align: center; font-weight: bold; color: #4CAF50;'>progress: 0%</div></div></div><div class='footnotes'><p>Warning: ESP32 will restart automatically after the update.</p></div><div style='text-align: center; margin-top: 20px;'><button onclick=\"window.location='/';\">Back</button></div></div><script>$('form').submit(function(e) {e.preventDefault();var form = $('#upload_form')[0];var data = new FormData(form);$.ajax({url: '/update',type: 'POST',data: data,contentType: false,processData: false,xhr: function() {var xhr = new window.XMLHttpRequest();xhr.upload.addEventListener('progress', function(evt) {if (evt.lengthComputable) {var per = evt.loaded / evt.total;$('#prg').html('progress: ' + Math.round(per * 100) + '%');}}, false);return xhr;},success: function(d, s) {console.log('success!')},error: function(a, b, c) {}});});</script></body></html>";
//---- len : 1769 bytes
const char HTTP_EXPL0[] PROGMEM = "<script>function clic(pa, el) {var r = confirm('Are you sure you want to ' + pa + ' ' + el + ' ?');if (r == true) {window.location = '/explorer?cmd=' + pa + '&file=' + el;}}</script><body><div class='container'><header><h1>File Explorer</h1></header><p class='message'>Browse and manage files in the EFS.</p><div class='section'><div class='section-title'>Files</div><div class='action-list'>";
//---- len : 429 bytes
//---- End Generated

// old size = 4830 new size = 7024 bytes incresing by 2194 bytes due to HTML content addition, but saving RAM by storing HTML in Flash memory (PROGMEM) 

// Global framework instance (used for callbacks)
FrameWeb* myFrameWeb;

/**
 * @brief Constructor of the FrameWeb class
 */
FrameWeb::FrameWeb(){
  myFrameWeb = this;
}

/**
 * @brief Destructor of the FrameWeb class
 */
FrameWeb::~FrameWeb(){
  webSocket.close();
  server.close();
  wifiManager.stopWebPortal();
}

/**
 * @brief Convert current configuration to JSON format
 * @return String JSON string containing all configuration parameters
 * 
 * Creates a JSON document from the current Config struct.
 * Useful for saving or displaying configuration.
 */
String FrameWeb::JsonConfig() {
  String configjson;
  // ArduinoJson 7
  JsonDocument rootcfg;
  // Set the values
  rootcfg["HostName"]      = config.HostName;
  JsonArray mac = rootcfg["MacAddress"].to<JsonArray>();
  for (int i=0; i<6; i++)
    mac.add(config.MacAddress[i]);
  rootcfg["ResetWifi"]     = config.ResetWifi;
  rootcfg["LoginName"]     = config.LoginName;
  rootcfg["LoginPassword"] = config.LoginPassword;
  rootcfg["UseToolsLocal"] = config.UseToolsLocal;
  // Transform to string
  serializeJson(rootcfg, configjson);
  return configjson;
}

/**
 * @brief Format size in bytes to human-readable units (B, KB, MB)
 * @param bytes Size in bytes
 * @return String Formatted size (ex: "1.5MB", "256KB", "512B")
 */
String FrameWeb::formatBytes(size_t bytes) { // convert sizes in bytes to KB and MB
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
  return String(bytes) + "B";
}

/**
 * @brief Recursively list files in a directory
 * @param ret Reference String that will be filled with formatted list
 * @param fs File system (SPIFFS)
 * @param dirname Directory path to list
 * @param levels Number of recursion levels (0 = no recursion)
 * 
 * Displays all files and subdirectories with formatted sizes.
 */
//  Directory list
void FrameWeb::listDir(String& ret, fs::FS &fs, const char * dirname, uint8_t levels) {
  ret += F("Listing directory: ");
  ret += dirname; ret += "\n";
  File root = fs.open(dirname);
  if (!root) { ret += F("Failed to open directory\n");  return;  }
  if (!root.isDirectory()) { ret += F("Not a directory\n\r"); return; }
  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      ret += F("  DIR : "); ret += file.name(); ret += "\n";
      if (levels)  listDir(ret, fs, file.name(), levels - 1);
    } else {
      ret += F("  FILE: "); ret += (file.name());  // SPIFFS_OBJ_NAME_LEN=32
      for (uint8_t l=strlen(file.name()); l<32; l+=8)
        ret += "\t";
      ret += F("  SIZE: "); ret += (formatBytes(file.size())); ret += "\n";
    }
    file = root.openNextFile();
  }
  return;
}

/**
 * @brief Determine MIME type of a file by its extension
 * @param filename File name
 * @return String MIME type (ex: "text/html", "image/png", "application/json")
 * 
 * If "download" parameter is present in HTTP request,
 * returns "application/octet-stream" to force download.
 */
String FrameWeb::getContentType(String filename) {
  if (server.hasArg("download"))        { return "application/octet-stream";
  } else if (filename.endsWith(".htm")) { return "text/html";
  } else if (filename.endsWith(".html")){ return "text/html";
  } else if (filename.endsWith(".css")) { return "text/css";
  } else if (filename.endsWith(".js"))  { return "application/javascript";
  } else if (filename.endsWith(".png")) { return "image/png";
  } else if (filename.endsWith(".gif")) { return "image/gif";
  } else if (filename.endsWith(".jpg")) { return "image/jpeg";
  } else if (filename.endsWith(".bmp")) { return "image/bmp";
  } else if (filename.endsWith(".ico")) { return "image/x-icon";
  } else if (filename.endsWith(".xml")) { return "text/xml";
  } else if (filename.endsWith(".pdf")) { return "application/x-pdf";
  } else if (filename.endsWith(".zip")) { return "application/x-zip";
  } else if (filename.endsWith(".gz"))  { return "application/x-gzip"; }
  return "text/plain";
}

/**
 * @brief Save configuration to config.json file on SPIFFS
 * @param filename Configuration file path
 * @param config Config structure to save
 * @return String Confirmation or error message
 */
// Save config file
String FrameWeb::saveConfiguration(const char *filename, const Config &config) {
  File file = SPIFFS.open(filename, "w");
  if (!file)
    return F("Can't write in Config file.");
  file.print(JsonConfig());
  file.close();
  return F("Config file has been saved.");
}

/**
 * @brief Start SPIFFS (SPI Flash File System)
 * 
 * Automatically formats SPIFFS if not formatted.
 * Sets bit 0 of initSetupState on success.
 */
// Start SPIFFS & Read config file
void FrameWeb::startSPIFFS() {
  if (SPIFFS.begin()==false){
    FDBXLN(F("SPIFFS was not formatted."));
    SPIFFS.format();
    SPIFFS.begin();
  } 
  initSetupState = initSetupState | (1<<0);
  //String ls;
  //listDir(ls, SPIFFS, "/", 0);
  //FDBXLN(ls);
}

/**
 * @brief Load configuration from config.json
 * @param filename Configuration file path
 * @param config Reference to Config structure to fill
 * @param hname Optional hostname (replaces file value if provided)
 * 
 * If file is missing or invalid, uses default values:
 *  - HostName: "esp32dudu" or value passed as parameter
 *  - MAC: hardware WiFi MAC address
 *  - LoginName/Password: "admin"/"admin"
 *  - UseToolsLocal: true
 * 
 * On JSON error, automatically creates file with defaults.
 */
void FrameWeb::loadConfiguration(const char *filename, Config &config, const char* hname) {
  // Open file for reading configuration
  File file = SPIFFS.open(filename, "r");
  if (!file)
    FDBXLN(F(" Config file is absent."));
  size_t size = file.size();
  if (size > 1024)
    FDBXLN(F(" Config file too large."));
  // allocate buffer for loading config
  std::unique_ptr<char[]> buf(new char[size]);
  file.readBytes(buf.get(), size);
  // ArduinoJson 7
  JsonDocument rootcfg;
  auto error = deserializeJson(rootcfg, buf.get());
  // Set config or defaults
  uint8_t hardwareMacAddess[6];
  esp_read_mac(hardwareMacAddess, ESP_MAC_WIFI_STA);
  strlcpy(config.HostName, rootcfg["HostName"] | ((hname!=NULL)?(hname):(HOSTDEFAULT)),sizeof(config.HostName));
  JsonArray mac = rootcfg["MacAddress"];
  for (int i=0; i<6; i++)
     config.MacAddress[i] = mac[i] | hardwareMacAddess[i];
  config.ResetWifi = rootcfg["ResetWifi"] | false;
  strlcpy(config.LoginName, rootcfg["LoginName"] | "admin",sizeof(config.LoginName));
  strlcpy(config.LoginPassword, rootcfg["LoginPassword"] | "admin",sizeof(config.LoginPassword));
  config.UseToolsLocal = rootcfg["UseToolsLocal"] | true;
  if ( error ) {
    FDBXLN(F("Error config file reading."));
    String ret = saveConfiguration(filename, config);
    FDBXLN(ret);
  } else {
    initSetupState = initSetupState | (1<<2);
  }
}

/**
 * @brief Start WiFiManager
 * 
 * Manages WiFi connection with following features:
 *  - Automatically connect to saved network
 *  - Enable Access Point (AP) if connection fails
 *  - Allow user to select WiFi via web interface
 *  - Configure custom MAC address
 *  - Restart if connection fails after timeout
 * 
 * Note: ConfigModeCallback is called when entering AP mode
 */
// Start WiFiManager
void FrameWeb::startWifiManager( /*void (*func)(WiFiManager* myWiFiManager )*/ ) {
#ifndef DEBUG_FRAMEWEB
  wifiManager.setDebugOutput(false);
#endif
  //! Warning MacAddress must be UNICAST frame that is bit 0 of first byte must be equals zero
  if ((config.MacAddress[0]&0x01)==0x01) {
    Serial.println("[F] WARNING: Mac address is not UNICAST!");
  }
  esp_base_mac_addr_set(config.MacAddress); // Wifi_STA=mac  wifi_AP=mac+1  BT=mac+2

  //Forcer à effacer les donnees WIFI dans l'eprom , permet de changer d'AP à chaque demmarrage ou effacer les infos d'une AP dans la memoire ( a valider , lors du premier lancement  )
  if (config.ResetWifi)  {
    //!\ Logging and Passwd stay in Flash
    wifiManager.resetSettings();
  }
  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode

  wifiManager.setAPCallback(configModeCallback);

  // Force host name
  WiFi.setHostname(config.HostName);
  
  if ( ! wifiManager.autoConnect( config.HostName) ) {
    FDBX("failed to connect and hit timeout.");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }
  // Wait for connection
  FDBX(F("Waitting Wifi connected..."));
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); FDBX(".");
  }
  if (WiFi.status() == WL_CONNECTED)
    initSetupState = initSetupState | (1<<2);
}

/**
 * @brief Start OTA (Over-The-Air update) service
 * 
 * Enables firmware update without USB cable:
 *  - Hostname: device name (for Arduino IDE)
 *  - Password: authentication for OTA
 * 
 * Sets bit 3 of initSetupState on success.
 */
// Start OverTheAir firmware uppload
void FrameWeb::startOTA() {
  // ArduinoOTA.setPort(8266); default is 8266
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(config.HostName);
  ArduinoOTA.setPassword(config.LoginPassword);
  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  ArduinoOTA.begin();
  initSetupState = initSetupState | (1<<3);
}

/**
 * @brief Display HTTP arguments and headers received (DEBUG)
 * 
 * Useful for analyzing HTTP requests during development.
 * Displays all POST/GET arguments and HTTP headers.
 */
// Show HTML Arguments and Header (use for debugging)
void FrameWeb::showAH() {
	String m = "[F]Nbr of args:";	m+=server.args();	m+="\n\r";
	for(int i = 0; i < server.args(); i++) {
		m+="Arg["+(String)i+"]=";		m+=server.argName(i)+":";	m+=server.arg(i)+"\n\r";
	}
	Serial.print(m);
	String mm = "[F]Nbr of heders:";	mm+=server.headers();	mm+="\n\r";
	for(int i = 0; i < server.headers(); i++) {
		mm+="[F]Header["+(String)i+"]=";		mm+=server.headerName(i)+":";	mm+=server.header(i)+"\n\r";
	}
	Serial.print(mm);
	Serial.print("[F]hostHeader:");Serial.println(server.hostHeader());
}

/**
 * @brief Convert WiFi error code to readable string
 * @param err WiFi error code
 * @return const char* Error description
 */
// Return Wifi Status code as String
const char* FrameWeb::wifiStatus(int err) {
  switch (err) {
    case 0: return "WL_IDLE_STATUS"; break;
    case 1: return "WL_NO_SSID_AVAIL"; break;
    case 2: return "WL_SCAN_COMPLETED"; break;
    case 3: return "WL_CONNECTED"; break;
    case 4: return "WL_CONNECT_FAILED"; break;
    case 5: return "WL_CONNECTION_LOST"; break;
    case 6: return "WL_DISCONNECTED"; break;
  }
  String ret = "wifi_Err="+String(err);
  return ret.c_str();
}

/**
 * @brief Convert HTTP error code to readable string
 * @param err HTTP or client error code
 * @return const char* Error description
 */
// Return Http some Status code as String
const char* FrameWeb::httpStatus(int err) {
  switch (err) {
    case -1  : return "CONNECTION_REFUSED"; break;
    case -2  : return "SEND_HEADER_FAILED"; break;
    case -3  : return "SEND_PAYLOAD_FAILED"; break;
    case -4  : return "NOT_CONNECTED"; break;
    case -5  : return "CONNECTION_LOST"; break;
    case -11 : return "READ_TIMEOUT"; break;
    case 200 : return "OK"; break;
  }
  String ret = "http_Code="+String(err);
  return ret.c_str();
}

/**
 * @brief Convert reset reason code to readable string
 * @param reason ESP32 reset reason code
 * @return const char* Reset reason description
 * 
 * Examples: POWERON_RESET, SW_RESET, DEEPSLEEP_RESET, etc.
 */
const char*  FrameWeb::resetReason(int reason){
  switch ( reason) {
    case 1 : return "POWERON_RESET"; break;          /**<1, Vbat power on reset*/
    case 3 : return "SW_RESET"; break;               /**<3, Software reset digital core*/
    case 4 : return "OWDT_RESET"; break;             /**<4, Legacy watch dog reset digital core*/
    case 5 : return "DEEPSLEEP_RESET"; break;        /**<5, Deep Sleep reset digital core*/
    case 6 : return "SDIO_RESET"; break;             /**<6, Reset by SLC module, reset digital core*/
    case 7 : return "TG0WDT_SYS_RESET"; break;       /**<7, Timer Group0 Watch dog reset digital core*/
    case 8 : return "TG1WDT_SYS_RESET"; break;       /**<8, Timer Group1 Watch dog reset digital core*/
    case 9 : return "RTCWDT_SYS_RESET"; break;       /**<9, RTC Watch dog Reset digital core*/
    case 10 : return "INTRUSION_RESET"; break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : return "TGWDT_CPU_RESET"; break;       /**<11, Time Group reset CPU*/
    case 12 : return "SW_CPU_RESET"; break;          /**<12, Software reset CPU*/
    case 13 : return "RTCWDT_CPU_RESET"; break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : return "EXT_CPU_RESET"; break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : return "RTCWDT_BROWN_OUT_RESET"; break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : return "RTCWDT_RTC_RESET"; break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : return "NO_MEAN"; break;
  }
  return "No_Mean";
}

/**
 * @brief Start WebSocket server on port 81
 * 
 * Enables bidirectional real-time communication with clients.
 * Events are processed via the webSocketEvent() callback function.
 * 
 * Sets bit 4 of initSetupState on success.
 */
// Start webSocket
void FrameWeb::startWebSocket() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  initSetupState = initSetupState | (1<<4);
}

String FrameWeb::simpleUpload(){
  String message = FPSTR(HTTP_HEADAL);
  message += FPSTR(HTTP_BODYUP);
  return message;
}

String FrameWeb::simpleIndex(){
  String message = FPSTR(HTTP_HEADAL);
  message += FPSTR(HTTP_BODYID);
  message += FPSTR(HTTP_BODYI0);
  message += externalHtmlTools;
  message += FPSTR(HTTP_BODYI1);
  return message;
}

String FrameWeb::simpleFirmware(){
  String message = FPSTR(HTTP_HEADAL);
  message += FPSTR(HTTP_FIRM0);
  return message;
}

// Handle Web server ----- used in startWebServer -------------------------------------------------------------------
bool FrameWeb::handleFileRead(String path) {               // send the right file to the client (if it exists)
  FDBXLN("handleFileRead path:" + path);
  if (path.endsWith("/")) path += "index.html";            // If a folder is requested, send the index file
  String contentType = getContentType(path);               // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {  // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                       // Use the compressed verion
    File file = SPIFFS.open(path, "r");                    // Open the file
    server.streamFile(file, contentType);                  // Send it to the client
    file.close();                                          // Close the file again
    FDBXLN("\tSent file: " + path);
    return true;
  } else {                                                 // Otherwise build answer locally
    if (config.UseToolsLocal) {
      if (path.endsWith("/index.html")){  // Tools                 // For default index.html page after authentification
        if (!server.authenticate(config.LoginName, config.LoginPassword)) {
           server.requestAuthentication();
           return true;
        }
        server.send(200, "text/html", simpleIndex() );     // If not upload.html in FS send light
        return true;
      }
      if (path.endsWith("/update")){                       // return index page which is stored in serverIndex
        if (!server.authenticate(config.LoginName, config.LoginPassword)) {
          server.requestAuthentication();
          return true;
        }
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", simpleFirmware());
        return true;
      }
    }
  }
  FDBXLN("\tFile Not Found: " + path);                  // If the file doesn't exist, return false
  return false;
}

void FrameWeb::handleFileUpload(){                      // upload a new file to the SPIFFS
  HTTPUpload& upload = server.upload();
  String path;
  if(upload.status == UPLOAD_FILE_START){
    path = upload.filename;
    if(!path.startsWith("/")) path = "/"+path;
    if(!path.endsWith(".gz")) {                          // The file server always prefers a compressed version of a file
      String pathWithGz = path+".gz";                    // So if an uploaded file is not compressed, the existing compressed
      if(SPIFFS.exists(pathWithGz))                      // version of that file must be deleted (if it exists)
         SPIFFS.remove(pathWithGz);
    }
    FDBX(F("handleFileUpload Name: "));
    FDBXLN(path);
    fsUploadFile = SPIFFS.open(path, "w");               // Open the file for writing in SPIFFS (create if it doesn't exist)
    path = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile) {                                    // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      FDBX(F("handleFileUpload Size: "));
      FDBXLN(upload.totalSize);
      server.sendHeader("Location","/success.html");      // Redirect the client to the success page
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}

String FrameWeb::textNotFound(){
  String message = "404: File Not Found\n\r";
  message += "URI: ";
  message += server.uri();
  message += " Method: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += " Arguments: ";
  message += server.args();
  message += "\n\r";
  for (uint8_t i=0; i<server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + ",";
  message += "\n\r";
  return message;
}

void FrameWeb::handlePost() {
  if (server.arg("cmd")!="") {
      if (!server.authenticate(config.LoginName, config.LoginPassword))
        return server.requestAuthentication();
      if (server.arg("cmd") == "save-config" ) saveConfiguration(filename, config);
      if (server.arg("cmd") == "reset-wifi" ) ResetWifi = true;
      if (server.arg("cmd") == "restart" ) RebootAsap = true;
      if (server.arg("cmd") == "restore" ) RestoreAsap = true;
  }
  server.sendHeader("Location","/");      // Redirect the client to the index page
  server.send(303);
}

void FrameWeb::handleNotFound(){ // if the requested file or page doesn't exist, return a 404 not found error
  if(!handleFileRead(server.uri())){          // check if the file exists in the flash memory (SPIFFS), if so, send it
    server.send(404, "text/plain", textNotFound());
  }
}

//  Directory list
void FrameWeb::explorer(String& ret, fs::FS &fs, const char * dirname, uint8_t levels) {
  File root = fs.open(dirname);
  if (!root) {  return;  }
  if (!root.isDirectory()) { return; }
  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      if (levels)  explorer(ret, fs, file.name(), levels - 1);
    } else {
      String strf = (file.name());
      ret += "<div class='action-item'><div><a href='";
      ret += strf;
      ret += "' >";
      ret += strf; 
      ret += "</a><span style='color: #999;'> ("+(formatBytes(file.size()))+")</span></div>";
      ret += "<div class='button-group'><button onclick=\"clic('download', '";
      ret += strf;
      ret += "')\">Download</button><button onclick=\"clic('remove', '";
      ret += strf;
      ret +="')\">Remove</button></div></div>";
    }
    file = root.openNextFile();
  }
  return;
}

void FrameWeb::download(String filename){
  File download = SPIFFS.open(filename);
  if (download) {
    server.sendHeader("Content-Type", "text/text");
    server.sendHeader("Content-Disposition", "attachment; filename="+filename);
    server.sendHeader("Connection", "close");
    server.streamFile(download, "application/octet-stream");
    download.close();
    return;
  }
  server.send(500, "text/plain", "500: couldn't download file");
}

void FrameWeb::sendLs(){
  String ls;
  listDir(ls, SPIFFS, "/", 0);
  server.send(200, "text/plain", ls);
}

void FrameWeb::upload_get(){
  if (!server.authenticate(config.LoginName, config.LoginPassword)) {
    return server.requestAuthentication();
  }
  if (!handleFileRead("/upload.html")) {    // upload.html exist on FS
    if (config.UseToolsLocal) server.send(200, "text/html", simpleUpload() ); // If not upload.html in FS send lightweight
    else server.send(404, "text/plain", "FileNotFound");
  }
}
void FrameWeb::upload_post(){
  server.send(200, "text/plain", "");
}

void FrameWeb::exploreWeb(){
#ifndef DEBUG_FRAMEWEB
  showAH();
#endif
  if (!server.authenticate(config.LoginName, config.LoginPassword)) 
    return server.requestAuthentication();
  if (server.arg("cmd")=="remove") { 
    if (server.arg("file") != "" ) SPIFFS.remove("/"+server.arg("file"));
  }
  if (server.arg("cmd")=="download") {
    if (server.arg("file") != "" ) download("/"+server.arg("file"));
  }
  // Return list of files in table
  String msg = FPSTR(HTTP_HEADAL);
  msg += FPSTR(HTTP_EXPL0);
  explorer(msg, SPIFFS, "/", 0);
  msg += F("</div></div><div style='text-align: center; margin-top: 20px;'><button onclick=\"window.location='/';\">Back</button></div></div></body></html>");
  server.send(200, "text/html", msg);
}

void FrameWeb::update(){
  server.sendHeader("Connection", "close");
  server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
  ESP.restart();
}

void FrameWeb::update2(){
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    Serial.printf("Update: %s\n\r", upload.filename.c_str());
    if(!Update.begin(UPDATE_SIZE_UNKNOWN)){ //start with max available size
      Update.printError(Serial);
    }
  } else if(upload.status == UPLOAD_FILE_WRITE){
    /* flashing firmware to ESP*/
    if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
      Update.printError(Serial);
    }
  } else if(upload.status == UPLOAD_FILE_END){
    if(Update.end(true)){ //true to set the size to the current progress
      Serial.printf("Update Success: %u\n\rRebooting...\n\r", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
  }
}

// Start web server -------------------------------------------------------------------
void FrameWeb::startWebServer(){
  // POST
  server.on("/post",  HTTP_POST, []() {        // If a POST request is sent to the /edit.html address,
     myFrameWeb->handlePost();
  });

  // Simple command wihout pwd
  server.on("/ls", [](){                      // Get list of file in FS
    myFrameWeb->sendLs();
  });

  /* handling uploading firmware file */
  server.on("/update", HTTP_POST, [](){
    myFrameWeb->update();
  },[](){
    myFrameWeb->update2();
  });

  // handling upload file
  server.on("/upload", HTTP_GET, []() {        // Upload
    myFrameWeb->upload_get();
  });

  server.on("/upload", HTTP_POST, []() {       // Back after selection
    myFrameWeb->upload_post();
    },[](){
     myFrameWeb->handleFileUpload();
     });

  server.onNotFound([]() {
    myFrameWeb->handleNotFound();
  });

  server.on("/explorer", [](){                  // Get list of file in FS
    myFrameWeb->exploreWeb();
  });

  server.begin();
  initSetupState = initSetupState | (1<<5);
}

// Start MDNS
void FrameWeb::startMDNS() {
  // - first argument is the domain name, in this example   the fully-qualified domain name is "esp32.local"
  // - second argument is the IP address to advertise   we send our IP address on the WiFi network
  if (!MDNS.begin(config.HostName))
    FDBXLN(F("Error setting up MDNS responder!"));
  FDBX( F("mDNS responder started: ")); FDBXLN(config.HostName);
  MDNS.addService("http",  "tcp", 80);
  MDNS.addService("ws",    "tcp", 81);
  MDNS.addService("esp32", "tcp", 8888); // Announce esp32 service port 8888 TCP
  initSetupState = initSetupState | (1<<6);
}

// Arduino core -------------------------------------------------------------
void FrameWeb::setup( /*void (*func)(WiFiManager* myWiFiManager),*/ const char* hostname) {
  FDBXMF("FrameWeb::setup started version: %s\n\r", FrameVersion);
  startSPIFFS();                   // Start FS (list all contents)
  loadConfiguration(filename, config, hostname); // Read config file
  startWifiManager();              // Start a Wi-Fi access point, and try to connect
  startOTA();                      // Start the OTA service
  startWebSocket();                // Start a WebSocket server
  startWebServer();                // Start a HTTP server with a file read handler and an upload handler
  startMDNS();                     // Start the mDNS responder  //DBXLN(FPSTR(FrameVersion));
  FDBXMF("FrameWeb::setup ready  IP: %s\n\r", WiFi.localIP().toString().c_str());
  FDBXMF("FrameWeb::setup ready MAC: %s\n\r", WiFi.macAddress().c_str() );
}

void FrameWeb::loop() {
  server.handleClient();         // constantly check for events
  webSocket.loop();              // constantly check for websocket events
  ArduinoOTA.handle();           // listen for OTA events
  if (ResetWifi) {
    // wifiManager.resetSettings(); // BUG the stored ssid no cleared !!
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); //load the flash-saved configs
    esp_wifi_init(&cfg); //initiate and allocate wifi resources (does not matter if connection fails)
    delay(2000); //wait a little bit
    if(esp_wifi_restore()!=ESP_OK){
      Serial.println("WiFi is not initialized by esp_wifi_init ");
    }else{
      Serial.println("WiFi Configurations Cleared!");
    }
    wifiManager.startConfigPortal(config.HostName, NULL);
    ESP.restart();
  }
  if (RebootAsap) ESP.restart(); // Restart in case of error
  if (RestoreAsap) {             // Reset to factory settings
    FDBXLN("Format SPIFS...");
    SPIFFS.format();
    wifiManager.resetSettings(); // BUG the stored ssid no cleared !!
    ESP.restart();
  }
}

