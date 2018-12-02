#include <WiFi.h>
#include "RemoteEsp32.h"
#include "SSD1306Wire.h"

SSD1306Wire display(0x3c, 21, 22);

/* Set these to your desired credentials. */
const char *ssid = "ESPap";
const char *password = "thereisnospoon";

WiFiServer server(8888);
WiFiClient client1;
WiFiClient client2;

void callback(byte* buffer, int command, int p1, int p2, int extsBytes);

void setup() {
  Serial.begin(57600);
  
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.begin();
  Serial.println("ESP32 (AP) started");

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, myIP.toString());
  display.drawString(0, 16, "ESP32 (AP)");
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
