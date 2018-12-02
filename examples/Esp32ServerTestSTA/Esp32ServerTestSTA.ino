#include <WiFi.h>
#include "RemoteEsp32.h"
#include "SSD1306Wire.h"
#include <AD9833.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

SSD1306Wire display(0x3c, 21, 22);
AD9833 ad9833(5);
/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

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

#define ANALOG_READ_4 10000
#define ANALOG_FAST_READ 10001

#define ADXL345_BASE 20000
#define ADXL345_BEGIN (ADXL345_BASE+0)
#define ADXL345_SET_RANGE (ADXL345_BASE+1)
#define ADXL345_GET_EVENT (ADXL345_BASE+5)

#define AD9833_BASE 20100
#define AD9833_BEGIN (AD9833_BASE+0)
#define AD9833_APPLY_SIGNAL (AD9833_BASE+1)
#define AD9833_SET_FREQUENCY (AD9833_BASE+3)
#define AD9833_ENABLE_OUTPUT (AD9833_BASE+9)

#define OLEDDISPLAY_BASE 20200
#define OLEDDISPLAY_DRAW_STRING (OLEDDISPLAY_BASE+17)
#define OLEDDISPLAY_SET_FONT (OLEDDISPLAY_BASE+21)
#define OLEDDISPLAY_DISPLAY (OLEDDISPLAY_BASE+31)
#define OLEDDISPLAY_CLEAR (OLEDDISPLAY_BASE+32)

void callback(byte* buffer, int command, int p1, int p2, int extsBytes) {
  //RemoteEsp32.printBytes(buffer, 16 + extsBytes);
  switch (command) {
    
    //#define ANALOG_READ_4 10000
    case ANALOG_READ_4: {
      int NUM_CH = 4;
      int adcPins[NUM_CH] = { 32, 33, 34, 35 };
      int adcBuf[NUM_CH * 2];
      for (int ch = 0; ch < NUM_CH; ch++) {
        adcBuf[ch * 2] = analogRead(adcPins[ch]); 
        adcBuf[ch * 2 + 1] = millis(); 
      }
      extsBytes = (NUM_CH * 2) * 4;
      RemoteEsp32.intToBytes(extsBytes, buffer + 12);
      RemoteEsp32.intsToBytes(adcBuf, NUM_CH * 2, buffer + 16);
    }
    break;
    
    //#define ANALOG_FAST_READ 10001
    case ANALOG_FAST_READ: {
      int adcBuf[p2 * 2];
      int pos = 0;
      int n = 0;
      for (int i = 0; i < (p2 * 10) / 100; i++) { // 10% pre trigger
        adcBuf[pos * 2] = analogRead(p1); 
        adcBuf[pos * 2 + 1] = micros();
        pos = (pos < p2 - 1) ? pos + 1 : 0;
      }
      while (n < p2 * 50) {
        int val = analogRead(p1);
        adcBuf[pos * 2] = val; 
        adcBuf[pos * 2 + 1] = micros();
        pos = (pos < p2 - 1) ? pos + 1 : 0;
        n++;
        if (val < 0x800) break;        
      }
      while (n < p2 * 50) {
        int val = analogRead(p1);
        adcBuf[pos * 2] = val; 
        adcBuf[pos * 2 + 1] = micros();
        pos = (pos < p2 - 1) ? pos + 1 : 0;
        n++;
        if (val >= 0x800) break;        
      }
      for (int i = 0; i < (p2 * 90) / 100; i++) { // 90% post trigger
        adcBuf[pos * 2] = analogRead(p1); 
        adcBuf[pos * 2 + 1] = micros();
        pos = (pos < p2 - 1) ? pos + 1 : 0;
      }
      extsBytes = (p2 * 2) * 4;
      RemoteEsp32.intToBytes(extsBytes, buffer + 12);
      RemoteEsp32.intsToBytes(adcBuf, p2 * 2, buffer + 16);
    }
    break;
    
    //#define ADXL345_BEGIN (ADXL345_BASE+0)
    case ADXL345_BEGIN:
      RemoteEsp32.intToBytes((int)accel.begin(), buffer + 8);
      break;
    //#define ADXL345_SET_RANGE (ADXL345_BASE+1)
    case ADXL345_SET_RANGE: accel.setRange((range_t)p1); break;
    //#define ADXL345_GET_EVENT (ADXL345_BASE+5)
    case ADXL345_GET_EVENT: {
      /* Get a new sensor event */ 
      sensors_event_t event; 
      accel.getEvent(&event);
      extsBytes = 3 * 4;
      RemoteEsp32.intToBytes(extsBytes, buffer + 12);
      RemoteEsp32.intToBytes((int)(event.acceleration.x * 100), buffer + 16);
      RemoteEsp32.intToBytes((int)(event.acceleration.y * 100), buffer + 20);
      RemoteEsp32.intToBytes((int)(event.acceleration.z * 100), buffer + 24);
    }
    break;
    
    //#define AD9833_BEGIN (AD9833_BASE+0)
    case AD9833_BEGIN: ad9833.Begin(); break;
    //#define AD9833_APPLY_SIGNAL (AD9833_BASE+1)
    case AD9833_APPLY_SIGNAL: {
        int p3 = RemoteEsp32.bytesToInt(buffer + 16);
        int p4 = RemoteEsp32.bytesToInt(buffer + 20);
        int p5 = RemoteEsp32.bytesToInt(buffer + 24);
        ad9833.ApplySignal((WaveformType)p1, (Registers)p2, (float)p3 / 10, (Registers)p4, (float)p5 / 1000);
      }
      break;
    //#define AD9833_SET_FREQUENCY (AD9833_BASE+3)
    case AD9833_SET_FREQUENCY: ad9833.SetFrequency((Registers)p1, (float)p2 / 10); break;
    //#define AD9833_ENABLE_OUTPUT (AD9833_BASE+9)
    case AD9833_ENABLE_OUTPUT: ad9833.EnableOutput((bool)p1); break;

    //#define OLEDDISPLAY_DRAW_STRING (OLEDDISPLAY_BASE+17)
    case OLEDDISPLAY_DRAW_STRING: {
      String p3 = String((char*)(buffer + 16));
      display.drawString(p1, p2, p3);
    }
    break; 
    //#define OLEDDISPLAY_SET_FONT (OLEDDISPLAY_BASE+21)
    case OLEDDISPLAY_SET_FONT:
      switch (p1) {
        case 10: display.setFont(ArialMT_Plain_10); break;
        case 16: display.setFont(ArialMT_Plain_16); break;
        case 24: display.setFont(ArialMT_Plain_24); break;
      }
      break;
    //#define OLEDDISPLAY_DISPLAY (OLEDDISPLAY_BASE+31)
    case OLEDDISPLAY_DISPLAY: display.display(); break;
    //#define OLEDDISPLAY_CLEAR (OLEDDISPLAY_BASE+32)
    case OLEDDISPLAY_CLEAR: display.clear(); break;

    default:
      RemoteEsp32.intToBytes(ERR_UNKNOWN_COMMAND, buffer + 4);
      break;

  }
  //RemoteEsp32.printBytes(buffer, 16 + extsBytes);
}
