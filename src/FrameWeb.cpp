#include "FrameWeb.h"

//---- Start Generated from src/FrameWeb.html file --- 2022-10-22 19:33:20.240677
const char HTTP_HEADAL[] PROGMEM = "<!DOCTYPE html><html><head><title>HTML ESP32 Dudu</title><meta content='width=device-width' name='viewport'></head>";
//---- len : 153 bytes
const char HTTP_BODYUP[] PROGMEM = "<body><center><header><h1 style='background-color:lightblue'>HTML Uploader</h1></header><div><p style='text-align: center'>Use this page to upload new files to the ESP32.<br />You can use compressed (.gz) files.</p><form method='post' enctype='multipart/form-data' style='margin: 0px auto 8px auto'><input type='file' name='Choose file' accept='.gz,.html,.ico,.js,.json,.css,.png,.gif,.bmp,.jpg,.xml,.pdf,.htm'><input class='button' type='submit' value='Upload' name='submit'></form></div><a class='button' href='/''>Back</a></center></body></html>";
//---- len : 586 bytes
const char HTTP_BODYID[] PROGMEM = "<body><center><header><h1 style='background-color: lightblue;'>= HTML Esp32 Dudu Tools =</h1></header><div><p style='text-align: center;'>Use this page to access the ESP32 embedded tools.<br/>You are here because there is no index.html uploaded.</p><div style='text-align: left; position: absolute; left: 50%; transform: translate(-50%, 0%);'><p style='line-height: .1;'><em><strong>Configuration facilities</strong></em><br /><table width='500' cellpadding='0'><tr><td>- Files explorer of SPI Flash File System</td><td align='right'><button style='width: 60%;' onClick=\"window.location=' /explorer';\">Explorer</button><button style='width: 30%;' onClick=\"window.location='/ls';\">Ls</button></td></tr><tr><td>- Show configuration file used at startup</td><td align='right'><button style='width: 90%;'onClick=\"window.location='/config.json';\">Config.json</button></td></tr><tr><td>- Upload files to SPI Flash File System</td><td align='right'><button style='width: 90%;' onClick=\"window.location='/upload';\">Uploader</button></td></tr><tr><td>- Update firmware O.T.A. to the EPS32</td><td align='right'><button style='width: 90%;' onClick=\"window.location='/update';\">Update</button></td></tr></table>";
//---- len : 1248 bytes
const char HTTP_BODYI0[] PROGMEM = "<p style='line-height: .1;'><em><strong>System facilities</strong></em></p><table width='400' cellpadding='0'><tbody><tr><tdstyle='line-height: 1.1; font-size: 10px;'>Several system commands are available:<br />- <b>Restart</b> launch an immediate reboot on the Esp32.<br />- <b>Save Config.</b> record the current configuration to E.F.S.*<br />- <b>Restore</b> default parameters and remove files to E.F.S**</td></tr></tbody></table><table width='500' cellpadding='0'><tbody><tr><td>- Select one command in the list :</td><td><form action='post' method='post'><select name='cmd'><option value='none'></option><option value='restart'>Restart</option><option value='save-config'>Save Config.*</option><option value='reset-wifi'>Reset WiFi***</option><option value='restore'>Restore**</option></select><button type='submit'>Valid</button></form></td></tr></tbody></table></p>";
//---- len : 911 bytes
const char HTTP_BODYI1[] PROGMEM = "<p style='line-height: 1.0; font-size: 10px;'>* All parameters in config.json file will be affected.<br>**The login/password and all flag will be set to default. Embedded File System will be reformatted &amp; cleared.<br>***Reboot in Access point mode to select a new WiFi.</p></div><div>&nbsp;</div></div></center></body></html>";
//---- len : 367 bytes
const char HTTP_FIRM0[] PROGMEM = "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script><center><header><h1 style='background-color: lightblue;'>Esp32 Update OTA</h1></header><div><p style='text-align: center;'>Use this page to update the firmware over the air to ESP32.<br />You can use the binary format (firmware.bin) files.</p><form method='POST' action='#' enctype='multipart/form-data' id='upload_form'><input type='file' accept='.bin'name='update'><input type='submit' value='Update'></form><div id='prg'>progress: 0%</div></div><p style='line-height: 1.0; font-size: 10px;'>Warning: After firmware update the ESP32 will be restarted.</p><script>$('form').submit(function (e) {e.preventDefault();var form = $('#upload_form')[0];var data = new FormData(form);$.ajax({url: '/update', type: 'POST', data: data, contentType: false, processData: false, xhr: function () {var xhr = new window.XMLHttpRequest();xhr.upload.addEventListener('progress', function (evt) {if (evt.lengthComputable) {var per = evt.loaded / evt.total;$('#prg').html('progress: ' + Math.round(per * 100) + '%');}}, false);return xhr;}, success: function (d, s) {console.log('success!')}, error: function (a, b, c) {}});});</script><a class='button' href='/'>Back</a></center></body></ntml>";
//---- len : 1301 bytes
const char HTTP_EXPL0[] PROGMEM = "<script>function clic(pa, el) {var r = confirm('Are you sure you want to ' + pa + ' ' + el + ' ?');if (r == true) {window.location = '/explorer?cmd=' + pa + '&file=' + el;}}</script><center><header><h1 style='background-color: lightblue'>File explorer</h1></header><div><table width='500' cellpadding='0'><tr><th>File Name</th><th>Size</th><th>Action</th></tr>";
//---- len : 397 bytes
//---- End Generated 

// Instance 
FrameWeb* myFrameWeb;

FrameWeb::FrameWeb(){
  myFrameWeb = this;
}
FrameWeb::~FrameWeb(){
}

String FrameWeb::JsonConfig() {
  String configjson;
  // ArduinoJson 6
  DynamicJsonDocument rootcfg(1024);
  // Set the values
  rootcfg["HostName"]      = config.HostName;
  JsonArray mac = rootcfg.createNestedArray("MacAddress");
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

String FrameWeb::getContentType(String filename) {
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
String FrameWeb::saveConfiguration(const char *filename, const Config &config) {
  File file = SPIFFS.open(filename, "w");
  if (!file)
    return F("Can't write in Config file.");
  file.print(JsonConfig());
  file.close();
  return F("Config file has been saved.");
}

// Start SPIFFS & Read config file
void FrameWeb::startSPIFFS() {
  if (SPIFFS.begin()==false){
    FDBXLN(F("SPIFFS was not formatted."));
    SPIFFS.format();
    SPIFFS.begin();
  } 
  //String ls;
  //listDir(ls, SPIFFS, "/", 0);
  //FDBXLN(ls);
}

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
  // ArduinoJson 6
  DynamicJsonDocument rootcfg(1024);
  auto error = deserializeJson(rootcfg, buf.get());
  // Set config or defaults
  uint8_t hardwareMacAddess[6];
  esp_read_mac(hardwareMacAddess, ESP_MAC_WIFI_STA);
  strlcpy(config.HostName, rootcfg["HostName"] | ((hname!=NULL)?(hname):("esp32dudu")),sizeof(config.HostName));
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
  }
}

// Start WiFiManager
void FrameWeb::startWifiManager( /*void (*func)(WiFiManager* myWiFiManager )*/ ) {
#ifndef DEBUG_FRAME
  wifiManager.setDebugOutput(false);
#endif
  //! Warning MacAddress must be UNICAST frame that is bit 0 of first byte must be equals zero
  if ((config.MacAddress[0]&0x01)==0x01) {
    Serial.println("[F] WARNING: Mac address is not UNICAST!");
  }
  esp_base_mac_addr_set(config.MacAddress); // Wifi_STA=mac  wifi_AP=mac+1  BT=mac+2

  //Forcer à effacer les donnees WIFI dans l'eprom , permet de changer d'AP à chaque demmarrage ou effacer les infos d'une AP dans la memoire ( a valider , lors du premier lancement  )
  if (config.ResetWifi)  {
    //!\ Deprecated
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
}

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
}

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

// Start webSocket
void FrameWeb::startWebSocket() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
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
      if (path.endsWith("/index.html")){                   // For default index.html page after authentification
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
			ret += "<tr><td>";
      ret += "<a href='";ret += strf;ret += "' >";ret += strf; ret += "</a>";
			ret += "</td><td>"+(formatBytes(file.size()))+"</td>\n";
			ret += "<td style='text-align: center;''><button onClick=\"clic('remove', '"; ret += (strf);
			ret += "' )\">Remove</button>\n <button onClick=\"clic('download', '"; ret += (strf);
		  ret += "' )\">Download</button></td></tr>\n";
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
#ifndef DEBUG_FRAME
  showAH();
#endif
  if (!server.authenticate(config.LoginName, config.LoginPassword)) 
    return server.requestAuthentication();
  if (server.arg("cmd")=="remove") {
    if (server.arg("file") != "" ) SPIFFS.remove(server.arg("file"));
  }
  if (server.arg("cmd")=="download") {
    if (server.arg("file") != "" ) download(server.arg("file"));
  }
  // Return list of files in table
  String msg = FPSTR(HTTP_HEADAL);
  msg += FPSTR(HTTP_EXPL0);
  explorer(msg, SPIFFS, "/", 0);
  msg += F("</table><a class='button' href='/''>Back</a></center></div></html>");
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
    delay(2000); //wait a bit
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
    FDBXLN("Fromat SPIFS...");
    SPIFFS.format();
    wifiManager.resetSettings(); // BUG the stored ssid no cleared !!
    ESP.restart();
  }
}