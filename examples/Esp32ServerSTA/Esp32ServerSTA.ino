#include <WiFi.h>
#include "RemoteEsp32.h"
#include "SSD1306Wire.h"

SSD1306Wire display(0x3c, 21, 22);

/* Set these to your desired credentials. */
const char *ssid = "your ssid";
const char *password = "your password";

WiFiServer server(8888);
WiFiClient client1;
WiFiClient client2;

void callback(byte* buffer, int command, int p1, int p2, int extsBytes);

void setup() {
  Serial.begin(57600);
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  //WiFi.mode(WIFI_STA);  // Disable Access Point
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
  server.begin();
  Serial.println("ESP32 (STA) started");

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, WiFi.localIP().toString());
  display.drawString(0, 16, "ESP32 (STA)");
  display.display();

  RemoteEsp32.attach(callback);
}

void loop() {
  if (!client1.connected()) {
    client1 = server.available();
  } else {
    RemoteEsp32.processInput(&client1);
  }  
  if (!client2.connected()) {
    client2 = server.available();
  } else {
    RemoteEsp32.processInput(&client2);
  }  
  RemoteEsp32.processInterrupt();
}

#define ANALOG_READ_N 10000
#define ANALOG_FAST_READ 10001

void callback(byte* buffer, int command, int p1, int p2, int extsBytes) {
  //RemoteEsp32.printBytes(buffer, 16 + extsBytes);
  switch (command) {
    
    case ANALOG_READ_N: {
      int adcPins[ADC_MAX_WIDTH] = { 36, 39, 32, 33, 34, 35 };
      int adcBuf[p1 * 2];
      for (int ch = 0; ch < p1; ch++) {
        adcBuf[ch * 2] = analogRead(adcPins[ch]); 
        adcBuf[ch * 2 + 1] = millis(); 
      }
      extsBytes = (p1 * 2) * 4;
      RemoteEsp32.intToBytes(extsBytes, buffer + 12);
      RemoteEsp32.intsToBytes(adcBuf, p1 * 2, buffer + 16);
    }
    break;
    
    case ANALOG_FAST_READ: {
      //int p3 = RemoteEsp32.bytesToInt(buffer + 16);
      int adcBuf[p2 * 2];
      //noInterrupts();
      for (int n = 0; n < p2; n++) {
        adcBuf[n * 2] = analogRead(p1); 
        adcBuf[n * 2 + 1] = micros();
        //delayMicroseconds(10);
      }
      //interrupts();
      extsBytes = (p2 * 2) * 4;
      RemoteEsp32.intToBytes(extsBytes, buffer + 12);
      RemoteEsp32.intsToBytes(adcBuf, p2 * 2, buffer + 16);
    }
    break;

  }
  //RemoteEsp32.printBytes(buffer, 16 + extsBytes);
}

