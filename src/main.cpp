/**
 * @file main.cpp
 * @brief Demo application for ESP32 FrameWeb Framework
 * @author Bruno Dupuy
 * 
 * This file demonstrates how to use the FrameWeb framework for an ESP32.
 * It includes:
 *  - WiFi management with web interface
 *  - Web server with file serving (HTTP port 80)
 *  - WebSocket communication (port 81)
 *  - OTA firmware updates
 *  - Network time protocol (NTP) synchronization
 *  - Serial debugging commands
 *  - LED status indicator
 */

#include <Arduino.h>
#define DEBUG_FRAMEWEB   // Enable debug output from FrameWeb
#include "FrameWeb.h"

// ============ Framework Instance ============
/// Global FrameWeb instance - manages all web services
FrameWeb frame;

// ============ Hardware Configuration ============
/// LED indicator pin - shows WiFi status (HIGH = on, LOW = off)
#define EspLedBlue 2

// ============ Timing Variables ============
/// Timer for 1-second heartbeat (LED blink, time update)
long previousMillis  = 0;
/// Timer for 1-minute display interval (date/time logging)
long previousMillisMinute = 0;

// ============ MAC Address Configuration ============
/// Custom MAC address for WiFi interface (for testing)
/// Format: 6 bytes representing the MAC address
/// Note: First byte must be even (UNICAST) - bit 0 must be 0
byte new_mac[6] = {0x00,0xAA,0xAA,0x99,0xFF,0xCC}; // TEST

/// Serial command buffer (for debugging via UART)
char cmd;

// ============ Time Configuration ============
/// GMT offset in seconds (3600 = UTC+1 in winter)
const long gmtOffset_sec     = 3600;
/// Daylight saving offset in seconds (3600 = UTC+1 extra hour)
const int daylightOffset_sec = 3600;
/// Time structure for storing current date/time
struct tm timeinfo;
/// NTP server for time synchronization
const char* ntpServer        = "fr.pool.ntp.org";

/**
 * @brief Format current time as HH:MM:SS string
 * @return String Time in format "HH:MM:SS" (24-hour format)
 * 
 * Uses the global timeinfo structure to get hour, minute, and second.
 * Formatted with leading zeros (e.g., "09:05:03").
 */
// Time HH:MM.ss
String getTime() {
    char temp[10];
    snprintf(temp, 10, "%02d:%02d:%02d", timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec );
    return String(temp);
}

/**
 * @brief Format current date (and optionally time) as string
 * @param sh If true: date only (DD/MM/YYYY), if false: date and time (DD/MM/YYYY HH:MM:SS)
 * @return String Formatted date/datetime
 * 
 * Uses European date format (day/month/year).
 * Examples:
 *  - getDate(true) = "10/03/2026"
 *  - getDate(false) = "10/03/2026 14:30:45"
 */
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

/**
 * @brief WebSocket event handler (callback for FrameWeb)
 * 
 * This function is called by FrameWeb when WebSocket events occur:
 *  - Client connection
 *  - Client disconnection
 *  - Text message received
 *  - Binary data received
 * 
 * Currently empty as demo - implement as needed for your application.
 * 
 * @param num WebSocket client number
 * @param type Event type (WStype_DISCONNECTED, WStype_CONNECTED, WStype_TEXT, etc.)
 * @param payload Pointer to payload data
 * @param lenght Length of payload in bytes
 */
// Web socket Use in FrameWeb for external command
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {}

/**
 * @brief Configuration save callback (required by FrameWeb)
 * 
 * Called by WiFiManager when configuration needs to be saved.
 * Can be used to perform additional actions when config changes.
 * Currently empty as demo.
 */
//callback notifying us of the need to save config
void saveConfigCallback () {}

/**
 * @brief Force custom configuration (for testing)
 * 
 * Sets all configuration parameters to fixed values:
 *  - HostName: "esp32"
 *  - MAC address: custom address defined in new_mac[]
 *  - Login: admin / admin
 *  - Enable local tools interface
 * 
 * Used during development to reset configuration quickly.
 * Triggered by 'f' command in serial console.
 */
// Force other host name and mac Addresses
// Set config or defaults
void forceNewMac() {
    strlcpy(frame.config.HostName, "esp32",sizeof(frame.config.HostName));
    for (int i=0; i<6; i++)
        frame.config.MacAddress[i] = new_mac[i];
    frame.config.ResetWifi = false;
    strlcpy(frame.config.LoginName, "admin",sizeof(frame.config.LoginName));
    strlcpy(frame.config.LoginPassword, "admin",sizeof(frame.config.LoginPassword));
    frame.config.UseToolsLocal = true;
    String ret = frame.saveConfiguration(frame.filename, frame.config);
}

/**
 * @brief Access Point mode callback (required by FrameWeb)
 * 
 * Called by WiFiManager when entering AP (Access Point) mode.
 * Can be used to signal user about AP mode (e.g., change LED color).
 * Currently empty as demo.
 */
//  configModeCallback callback when entering into AP mode
void configModeCallback (WiFiManager *myWiFiManager) {}

/**
 * @brief Arduino setup() - initialization
 * 
 * Initializes:
 *  1. Serial communication (115200 baud)
 *  2. GPIO pin for LED indicator
 *  3. FrameWeb framework (WiFi, web server, OTA, mDNS, WebSocket)
 *  4. Network Time Protocol (NTP) for date/time sync
 *  5. Timezone configuration (CET with daylight saving)
 * 
 * Reference: https://github.com/zhouhan0126/WIFIMANAGER-ESP32
 */
// https://github.com/zhouhan0126/WIFIMANAGER-ESP32
void setup() {
    Serial.begin(115200);
    
#ifdef DEBUG_FRAMEWEB
    Serial.println("Start Frame.Setup()");
#endif
    // ===== GPIO Configuration =====
    /// Initialize LED pin as output
    pinMode(EspLedBlue, OUTPUT);
    /// Set LED HIGH (on) during startup - blink indicates WiFi ready
    digitalWrite(EspLedBlue, HIGH);
    
    // ===== FrameWeb Initialization =====
    /// Start all framework services (WiFi, web server, OTA, etc.)
    frame.setup();

    // ===== Network Time Configuration =====
    /// Configure NTP server and timezone
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    /// Set system timezone with DST rules (CET: Central European Time)
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
    /// Fetch current time from system
    getLocalTime(&timeinfo);
    /// Toggle LED to show startup progress
    digitalWrite(EspLedBlue, !digitalRead(EspLedBlue));

#ifdef DEBUG_FRAMEWEB
    Serial.println("Server WEB started.");
#endif
}

/**
 * @brief Arduino loop() - main execution loop
 * 
 * Performs these tasks:
 *  1. Process serial commands for debugging (if DEBUG_FRAMEWEB enabled)
 *  2. Call FrameWeb loop to handle HTTP, WebSocket, OTA
 *  3. Update heartbeat LED every 1 second
 *  4. Display date/time every 1 minute
 * 
 * Serial Commands:
 *  - 'h' = Display help information
 *  - 'r' = Reboot ESP32
 *  - 'i' = Show system info (heap, MAC, IP, version)
 *  - 'f' = Force configuration (set custom MAC, hostname, credentials)
 */
// Main loop -----------------------------------------------------------------
void loop() {
#ifdef DEBUG_FRAMEWEB
    // ===== Serial Debug Commands =====
    /// Process incoming serial characters for debugging
    while (Serial.available() > 0) {
        uint8_t c = (uint8_t)Serial.read();
        // Ignore carriage return (13) and line feed (10)
        if (c != 13 && c != 10 ) {
            cmd = c;
            // Display help
            if (cmd=='h') { Serial.println("\n\t- Help info:\n\r r=reboot i=show info. f=force MAC(00-AA-AA-99-FF-CC)");}
            // Reboot device
            else if (cmd=='r') { ESP.restart(); }
            // Show system information
            else if (cmd=='i') { Serial.printf("Heap:%u Mac:%s IP:%s Version:%s\n\r",ESP.getFreeHeap(), WiFi.macAddress().c_str(), WiFi.localIP().toString().c_str(),FrameVersion );}
            // Force new configuration (MAC, hostname, credentials)
            else if (cmd=='f') { Serial.println("Mode config feilds (Mac, Host,...)."); forceNewMac();}
            cmd = ' ';
        }
    }
#endif
    // ===== Main Framework Loop =====
    /// Process all web requests, WebSocket events, OTA updates
    frame.loop();
    
    // ===== Heartbeat - 1 Second Interval =====
    /// Toggle LED and update time every 1000ms
    /// Used to show device is alive and syncing NTP time
    if ( millis() - previousMillis > 1000L) {
        previousMillis = millis();
        /// Blink LED (toggle output)
        digitalWrite(EspLedBlue, !digitalRead(EspLedBlue));
        /// Update time structure from system time
        getLocalTime(&timeinfo);
    }
    
    // ===== Periodic Logging - 1 Minute Interval =====
    /// Display formatted date and time to serial console
    /// Useful for timestamp logging of events
    if ( millis() - previousMillisMinute > 60000L) {
        previousMillisMinute = millis();
        /// Print date/time in format: DD/MM/YYYY HH:MM:SS
        Serial.println(getDate());
        Serial.printf("Heap:%u Mac:%s IP:%s Version:%s\n\r",ESP.getFreeHeap(), WiFi.macAddress().c_str(), WiFi.localIP().toString().c_str(),FrameVersion );
    }
}
