#include <Arduino.h>
//#define DEBUG_FRAME   // Debug frame before header
#include "Frame.h"

#define EspLedBlue 2
long previousMillis  = 0;       // Use in loop

// Web socket Use for external command
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  Serial.printf("[%u] get Message: %s\r\n", num, payload);
  switch(type) {
    case WStype_DISCONNECTED:
    break;
    case WStype_CONNECTED:
    {
      IPAddress ip = webSocket.remoteIP(num);
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

// Arduino core -------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Set pin mode
  pinMode(EspLedBlue, OUTPUT);     // Led is BLUE at statup
  digitalWrite(EspLedBlue, HIGH);  // After 5 seconds blinking indicate WiFI is OK
  delay(5000);                     // If stay BLUE after 5 sec mode AccessPoint
  // Start framework
  frame_setup();
}
// Main loop -----------------------------------------------------------------
void loop() {
  // Call frame loop
  frame_loop();
  // Is alive
  if ( millis() - previousMillis > 1000L) {
    previousMillis = millis();
    digitalWrite(EspLedBlue, !digitalRead(EspLedBlue));
  }
}
