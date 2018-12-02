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
  WiFi.mode(WIFI_STA);  // Disable Access Point
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

void callback(byte* buffer, int command, int p1, int p2, int extsBytes) {
  //RemoteEsp32.printBytes(buffer, 16 + extsBytes);
  switch (command) {

    default:
      RemoteEsp32.intToBytes(ERR_UNKNOWN_COMMAND, buffer + 4);
      break;

  }
  //RemoteEsp32.printBytes(buffer, 16 + extsBytes);
}
