#ifndef Frame_h
#define Frame_h

// File FS (file system), added to pltformio.ini lib_deps
#include <FS.h>
#include <SPIFFS.h>
// JSon install ArduinoJson by Benoit Blanchon, added to pltformio.ini lib_deps
#include <ArduinoJson.h>
// OTA need WiFiUdp
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Update.h>
// WiFimanager need WiFi WebServer WiFimanager
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
 //https://github.com/tzapu/WiFiManager
#include <WiFiManager.h>
// WebSocket
#include <WebSocketsServer.h>
// mDNS
#include "ESPmDNS.h"

// Debug macro
#ifdef DEBUG_FRAME
  #define DBX(...) Serial.print(__VA_ARGS__)
  #define DBXLN(...) Serial.println(__VA_ARGS__)
#else
  #define DBX(...)
  #define DBXLN(...)
#endif

// constant HTML Uploader if not defined in FS
const char HTTP_HEADAL[] PROGMEM = "<!DOCTYPE html><html><head><title>HTML ESP32Dudu</title><meta content='width=device-width' name='viewport'></head>";
const char HTTP_BODYUP[] PROGMEM = "<body><center><header><h1 style=\"background-color:lightblue\">HTML Uploader</h1></header><div><p style=\"text-align: center\">Use this page to upload new files to the ESP32.<br/>You can use compressed (.gz) files.</p><form method=\"post\" enctype=\"multipart/form-data\" style=\"margin: 0px auto 8px auto\" ><input type=\"file\" name=\"Choose file\" accept=\".gz,.html,.ico,.js,.css,.png,.gif,.jpg,.xml,.pdf,.htm\"><input class=\"button\" type=\"submit\" value=\"Upload\" name=\"submit\"></form></div></center></body></html>";
// constant HTML Tools if not defined in FS
const char HTTP_BODYID[] PROGMEM = "<script>function valid(param) { var r = confirm(\"Are you sure you want to execute this action?\");if (r == true) { window.location=param; } }</script><body><center><header><h1 style=\"background-color: lightblue;\">HTML Esp32 Tools</h1></header><div><p style=\"text-align: center;\">Use this page to access the ESP32 embedded tools.<br />You are here because there is no index.html uploaded.</p><div style=\"text-align: left; position: absolute; left: 50%; transform: translate(-50%, 0%);\"><p style=\"line-height: .1;\"><em><strong>Configuration facilities</strong></em><br /><table width=\"400\" cellpadding=\"0\"><tr><td>- Show List of files in Embedded File System</td><td align=\"right\"><button  style=\"width: 90px;\" onClick=\"window.location='/ls';\">Ls</button></td></tr> <tr><td>- Show configuration file used at startup</td><td align=\"right\"><button style=\"width: 90px;\" onClick=\"window.location='/config.json';\">Config.json</button></td></tr><tr><td>- Upload files facility to the E.F.S.</td><td align=\"right\"><button style=\"width: 90px;\" onClick=\"window.location='/Upload';\">Uploader</button></td></tr><tr><td>- Update firmware O.T.A. to the EPS32</td><td align=\"right\"><button style=\"width: 90px;\" onClick=\"window.location='/update';\">Update</button></td></tr></table>";
const char HTTP_BODYI0[] PROGMEM = "<p style=\"line-height: .1;\"><em><strong>System facilities</strong></em></p><table width=\"400\" cellpadding=\"0\"><tbody><tr><tdstyle=\"line-height: 1.1; font-size: 10px;\">Several system commands are available:<br />- <b>Restart</b> launch an immediate reboot on the Esp32.<br />- <b>Save Config.</b> record the current configuration to E.F.S.*<br />- <b>Restore</b> default parameters and remove files to E.F.S**</td></tr></tbody></table><table width=\"400\" cellpadding=\"0\"><tbody><tr><td>- Select one command in the list :</td><td><form action=\"post\" method=\"post\"><select name=\"cmd\"><option value=\"none\"></option><option value=\"restart\">Restart</option><option value=\"save-config\">Save Config.*</option><option value=\"restore\">Restore**</option></select><button type=\"submit\">Valid</button></form></td></tr></tbody></table>";
const char HTTP_BODYI1[] PROGMEM = "</p><p style=\"line-height: 1.0; font-size: 10px;\">* All parameters in config.json file will be affected. <br>**The login/password and all flag will be set to default. Embedded File System will be reformatted &amp; cleared.</p></div><div>&nbsp;</div></div></center></body></html>";
// constant HTML update if not defined in FS
const char HTTP_FIRM0[] PROGMEM = "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script><center><header><h1 style='background-color: lightblue;'>HTML Update OTA</h1></header><div><p style='text-align: center;'>Use this page to update the firmware over the air to ESP32.<br/>You can use the binary format (firmware.bin) files.</p><form method='POST' action='#' enctype='multipart/form-data' id='upload_form'><input type='file' accept='.bin' name='update'><input type='submit' value='Update'></form><div id='prg'>progress: 0%</div></div><p style='line-height: 1.0; font-size: 10px;'>Warning: After firmware update the ESP32 will be restarted.</p> <script>$('form').submit(function(e){e.preventDefault();var form = $('#upload_form')[0];var data = new FormData(form); $.ajax({url: '/update',type: 'POST',data: data,contentType: false,processData:false,xhr: function() {var xhr=new window.XMLHttpRequest(); xhr.upload.addEventListener('progress', function(evt) {if (evt.lengthComputable) {var per = evt.loaded/evt.total; $('#prg').html('progress: '+Math.round(per*100)+'%');}}, false);return  xhr;},success:function(d, s){console.log('success!')},error: function (a, b, c) {}});});</script></center></body></ntml>";

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght);

//Init JSON
DynamicJsonBuffer jsonBuffer(500);
JsonObject& JSONRoot   = jsonBuffer.createObject();
JsonObject& JSONSystem = JSONRoot.createNestedObject("System");
JsonObject& JSONJeedom = JSONRoot.createNestedObject("Jeedom");
// Default value in loadConfiguration function
struct Config {            // First connexion LAN:esp32dudu IPAddress(192,168,0,1)
  char HostName[20];       // Default esp32dudu
  byte MacAddress[6];      // Default 0x30,0xAE,0xA4,0x90,0xFD,0xC8
  bool ResetWifi;          // Default false WiFimanager reconnect with last data
  char OTAHostName[20];    // Default esp32dudu
  char OTAPassword[20];    // Default empty
  char UploadName[20];     // Default login admin
  char UploadPassword[20]; // Default password admin
  bool UseToolsLocal;      // True if simpleUpload must be called in case of not Upload.html
};

// variables Global
const char *filename = "/config.json"; // file configuration
bool RebootAsap      = false;   // Error OTA
bool RestoreAsap     = false;   // Reset to factory settings
Config config;                  // Struct Config
File fsUploadFile;              // File variable to temporarily store the received file

// services WEB
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

String JsonConfig() {
  String configjson;
  StaticJsonBuffer<1600> jsonBuffercfg; // Use https://arduinojson.org/assistant/ to compute the capacity.
  JsonObject &rootcfg = jsonBuffercfg.createObject(); // Parse the root object
  // Set the values
  rootcfg["HostName"] = config.HostName;
  JsonArray& mac = rootcfg.createNestedArray("MacAddress");
  for (int i=0; i<6; i++)
    mac.add(config.MacAddress[i]);
  rootcfg["ResetWifi"]      = config.ResetWifi;
  rootcfg["OTAHostName"]    = config.OTAHostName;
  rootcfg["OTAPassword"]    = config.OTAPassword;
  rootcfg["UploadName"]     = config.UploadName;
  rootcfg["UploadPassword"] = config.UploadPassword;
  rootcfg["UseToolsLocal"]  = config.UseToolsLocal;
  // Transform to string
  rootcfg.printTo(configjson);
  return configjson;
}
String formatBytes(size_t bytes) { // convert sizes in bytes to KB and MB
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
  return String(bytes) + "B";
}
//  Directory list
void listDir(String& ret, fs::FS &fs, const char * dirname, uint8_t levels) {
  ret += F("Listing directory: ");
  ret += dirname; ret += "\n";
  File root = fs.open(dirname);
  if (!root) { ret += F("Failed to open directory\n");  return;  }
  if (!root.isDirectory()) { ret += F("Not a directory\n"); return; }
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
String getContentType(String filename) {
  if (server.hasArg("download"))        { return "application/octet-stream";
  } else if (filename.endsWith(".htm")) { return "text/html";
  } else if (filename.endsWith(".html")){ return "text/html";
  } else if (filename.endsWith(".css")) { return "text/css";
  } else if (filename.endsWith(".js"))  { return "application/javascript";
  } else if (filename.endsWith(".png")) { return "image/png";
  } else if (filename.endsWith(".gif")) { return "image/gif";
  } else if (filename.endsWith(".jpg")) { return "image/jpeg";
  } else if (filename.endsWith(".ico")) { return "image/x-icon";
  } else if (filename.endsWith(".xml")) { return "text/xml";
  } else if (filename.endsWith(".pdf")) { return "application/x-pdf";
  } else if (filename.endsWith(".zip")) { return "application/x-zip";
  } else if (filename.endsWith(".gz"))  { return "application/x-gzip"; }
  return "text/plain";
}

// Save config file
String saveConfiguration(const char *filename, const Config &config) {
  File file = SPIFFS.open(filename, "w");
  if (!file)
    return F("Can't write in Config file.");
  file.print(JsonConfig());
  file.close();
  return F("Config file has been saved.");
}


// Start SPIFFS & Read config file
void startSPIFFS() {
  if (SPIFFS.begin()==false){
    DBXLN(F("SPIFFS was not formatted."));
    SPIFFS.format();
    SPIFFS.begin();
  }
  String ls;
  listDir(ls, SPIFFS, "/", 0);
  DBX(ls);
}
void loadConfiguration(const char *filename, Config &config) {
  // Open file for reading configuration
  File file = SPIFFS.open(filename, "r");
  if (!file)
    DBXLN(F(" Config file is absent."));
  size_t size = file.size();
  if (size > 1024)
    DBXLN(F(" Config file too large."));
  // allocate buffer for loading config
  std::unique_ptr<char[]> buf(new char[size]);
  file.readBytes(buf.get(), size);
  StaticJsonBuffer<1600> jsonBufferConfig;
  JsonObject& rootcfg = jsonBufferConfig.parseObject(buf.get());
  // Set config or defaults
  strlcpy(config.HostName, rootcfg["HostName"] | "esp32dudu",sizeof(config.HostName));
  byte new_mac[8] = {0x30,0xAE,0xA4,0x90,0xFD,0xC8};
  JsonArray& mac = rootcfg["MacAddress"];
  for (int i=0; i<6; i++)
    config.MacAddress[i] = mac[i] | new_mac[i];
  config.ResetWifi = rootcfg["ResetWifi"] | false;
  strlcpy(config.OTAHostName, rootcfg["OTAHostName"] | "esp32dudu",sizeof(config.OTAHostName));
  strlcpy(config.OTAPassword, rootcfg["OTAPassword"] | "empty",sizeof(config.OTAPassword));
  strlcpy(config.UploadName, rootcfg["UploadName"] | "admin",sizeof(config.UploadName));
  strlcpy(config.UploadPassword, rootcfg["UploadPassword"] | "admin",sizeof(config.UploadPassword));
  config.UseToolsLocal = rootcfg["UseToolsLocal"] | true;
  if (!rootcfg.success()) {
    DBXLN(F("Error config file reading."));
    String ret = saveConfiguration(filename, config);
    DBXLN(ret);
  }
}

//  configModeCallback callback when entering into AP mode
void configModeCallback (WiFiManager *myWiFiManager) {
  DBXLN(F("Select AP.."));
  DBXLN(WiFi.softAPIP().toString());
  DBXLN(myWiFiManager->getConfigPortalSSID());
  DBXLN(WiFi.softAPIP());
  DBXLN(myWiFiManager->getConfigPortalSSID());
}

// Start WiFiManager
void startWifiManager() {
  WiFiManager wifiManager;
#ifndef DEBUG_FRAME
  wifiManager.setDebugOutput(false);
#endif
  esp_base_mac_addr_set(config.MacAddress); // Wifi_STA=mac  wifi_AP=mac+1  BT=mac+2
  // wifiManager.setAPStaticIPConfig(IPAddress(192,168,0,1), IPAddress(192,168,1,1), IPAddress(255,255,255,0));
  //Forcer à effacer les donnees WIFI dans l'eprom , permet de changer d'AP à chaque demmarrage ou effacer les infos d'une AP dans la memoire ( a valider , lors du premier lancement  )
  if (config.ResetWifi) wifiManager.resetSettings();
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);
  //Recupere les identifiants   ssid et Mot de passe dans l'eprom  et essayes de se connecter
  //Si pas de connexion possible , il demarre un nouveau point d'accés avec comme nom , celui definit dans la commande autoconnect ( ici : AutoconnectAP )
  wifiManager.autoConnect(config.HostName);
  // Wait for connection
  DBXLN(F("Wait wifi."));
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); DBX(".");
  }
}

// Start OTA
void startOTA(){
  // ArduinoOTA.setPort(8266); default is 8266
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(config.OTAHostName);
  ArduinoOTA.setPassword(config.OTAPassword);
  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  ArduinoOTA.begin();
}

// Start webSocket
void startWebSocket() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

String simpleUpload(){
  String message = FPSTR(HTTP_HEADAL);
  message += FPSTR(HTTP_BODYUP);
  return message;
}

String simpleIndex(){
  String message = FPSTR(HTTP_HEADAL);
  message += FPSTR(HTTP_BODYID);
  message += FPSTR(HTTP_BODYI0);
  message += FPSTR(HTTP_BODYI1);
  return message;
}

String simpleFirmware(){
  String message = FPSTR(HTTP_HEADAL);
  message += FPSTR(HTTP_FIRM0);
  return message;
}

// Handle Web server
bool handleFileRead(String path) {                         // send the right file to the client (if it exists)
  DBXLN("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";            // If a folder is requested, send the index file
  String contentType = getContentType(path);               // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {  // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                       // Use the compressed verion
    File file = SPIFFS.open(path, "r");                    // Open the file
    server.streamFile(file, contentType);                  // Send it to the client
    file.close();                                          // Close the file again
    DBXLN("\tSent file: " + path);
    return true;
  } else {
    if (config.UseToolsLocal) {
      if (path.endsWith("/index.html")){                     // Default index.html page
        if (!server.authenticate(config.UploadName, config.UploadPassword)) {
           server.requestAuthentication();
           return true;
        }
        server.send(200, "text/html", simpleIndex() );     // If not upload.html in FS send light
        return true;
      }
      if (path.endsWith("/update")){  // return index page which is stored in serverIndex
        if (!server.authenticate(config.UploadName, config.UploadPassword)) {
          server.requestAuthentication();
          return true;
        }
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", simpleFirmware());
        return true;
      }
    }
  }
  DBXLN("\tFile Not Found: " + path);             // If the file doesn't exist, return false
  return false;
}

void handleFileUpload(){                                // upload a new file to the SPIFFS
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
    DBX(F("handleFileUpload Name: "));
    DBXLN(path);
    fsUploadFile = SPIFFS.open(path, "w");                // Open the file for writing in SPIFFS (create if it doesn't exist)
    path = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile) {                                    // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      DBX(F("handleFileUpload Size: "));
      DBXLN(upload.totalSize);
      server.sendHeader("Location","/success.html");      // Redirect the client to the success page
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}
String textNotFound(){
  String message = "404: File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  return message;
}
void handlePost() {
  for (uint8_t i=0; i<server.args(); i++) {
    if (server.argName(i).equals("cmd")) {
      if (!server.authenticate(config.UploadName, config.UploadPassword))
        return server.requestAuthentication();
      if (server.arg(i).equals("save-config") ) saveConfiguration(filename, config);
      if (server.arg(i).equals("restart") ) RebootAsap = true;
      if (server.arg(i).equals("restore") ) RestoreAsap = true;
    }
  }
  server.sendHeader("Location","/");      // Redirect the client to the index page
  server.send(303);
}
void handleNotFound(){ // if the requested file or page doesn't exist, return a 404 not found error
  if(!handleFileRead(server.uri())){          // check if the file exists in the flash memory (SPIFFS), if so, send it
    server.send(404, "text/plain", textNotFound());
  }
}
// Start web server
void startWebServer(){
  // POST
  server.on("/post",  HTTP_POST, []() {        // If a POST request is sent to the /edit.html address,
    handlePost();
  });
 // Test
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well"); // Just for some test
  });
  // Simple command wihout pwd
  server.on("/ls", [](){                      // Get list of file in FS
    String ls;
    listDir(ls, SPIFFS, "/", 0);
    server.send(200, "text/plain", ls);
  });
  /* handling uploading firmware file */
  server.on("/update", HTTP_POST, [](){
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
    ESP.restart();
  },[](){
    HTTPUpload& upload = server.upload();
    if(upload.status == UPLOAD_FILE_START){
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if(!Update.begin(UPDATE_SIZE_UNKNOWN)){//start with max available size
        Update.printError(Serial);
      }
    } else if(upload.status == UPLOAD_FILE_WRITE){
      /* flashing firmware to ESP*/
      if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
        Update.printError(Serial);
      }
    } else if(upload.status == UPLOAD_FILE_END){
      if(Update.end(true)){ //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  // handling upload file
  server.on("/Upload", HTTP_GET, []() {        // Upload
    if (!server.authenticate(config.UploadName, config.UploadPassword)) {
      return server.requestAuthentication();
    }
    if (!handleFileRead("/upload.html")) {    // upload.html exist on FS
      if (config.UseToolsLocal) server.send(200, "text/html", simpleUpload() ); // If not upload.html in FS send light
      else server.send(404, "text/plain", "FileNotFound");
    }
  });
  server.on("/Upload", HTTP_POST, []() {       // Back after selection
    server.send(200, "text/plain", "");
  }, handleFileUpload);
  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "FileNotFound");
    }
  });
  server.onNotFound(handleNotFound);         // Not found page
  server.begin();
}
// Start MDNS
void startMDNS() {
  // - first argument is the domain name, in this example   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise   we send our IP address on the WiFi network
  if (!MDNS.begin(config.HostName))
    DBXLN(F("Error setting up MDNS responder!"));
  DBX( F("mDNS responder started: ")); DBXLN(config.HostName);
  MDNS.addService("http",  "tcp", 80);
  MDNS.addService("ws",    "tcp", 81);
  MDNS.addService("esp32", "tcp", 8888); // Announce esp32 service port 8888 TCP
}

// Arduino core -------------------------------------------------------------
void frame_setup() {
  DBXLN(F("Setup_Frame started."));
  startSPIFFS();                   // Start FS (list all contents)
  loadConfiguration(filename, config); // Read config file
  startWifiManager();              // Start a Wi-Fi access point, and try to connect
  startOTA();                      // Start the OTA service
  startWebSocket();                // Start a WebSocket server
  startWebServer();                // Start a HTTP server with a file read handler and an upload handler
  startMDNS();                     // Start the mDNS responder
  DBX(F("Setup_Frame finished IP:"));
  Serial.println(WiFi.localIP());;
}
// Main loop -----------------------------------------------------------------
void frame_loop() {
  server.handleClient();         // constantly check for events
  webSocket.loop();              // constantly check for websocket events
  ArduinoOTA.handle();           // listen for OTA events
  if (RebootAsap) ESP.restart(); // Restart in case of error
  if (RestoreAsap) {             // Reset to factory settings
    SPIFFS.format();
    WiFiManager wifiManager;
    wifiManager.resetSettings(); // BUG the stored ssid saty !!
    ESP.restart();
  }
}

#endif
