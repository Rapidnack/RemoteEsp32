#include <WiFi.h>
#include "RemoteEsp32.h"
#include "SSD1306Wire.h"
#include <AD9833.h>

AD9833 ad9833(5);

/* Set these to your desired credentials. */
//const char *ssid = "your ssid";
//const char *password = "your password";
const char *ssid     = "106F3F28087C_G";
const char *password = "xr84vyms434wt";

WiFiServer server(8888);
WiFiClient client1;
WiFiClient client2;

SSD1306Wire display(0x3c, 21, 22);

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

#define AD9833_BASE 20000
#define AD9833_BEGIN (AD9833_BASE+0)
#define AD9833_APPLY_SIGNAL (AD9833_BASE+1)
#define AD9833_RESET (AD9833_BASE+2)
#define AD9833_SET_FREQUENCY (AD9833_BASE+3)
#define AD9833_INCREMENT_FREQUENCY (AD9833_BASE+4)
#define AD9833_SET_PHASE (AD9833_BASE+5)
#define AD9833_INCREMENT_PHASE (AD9833_BASE+6)
#define AD9833_SET_WAVEFORM (AD9833_BASE+7)
#define AD9833_SET_OUTPUT_SOURCE (AD9833_BASE+8)
#define AD9833_ENABLE_OUTPUT (AD9833_BASE+9)
#define AD9833_SLEEP_MODE (AD9833_BASE+10)
#define AD9833_DISABLE_DAC (AD9833_BASE+11)
#define AD9833_DISABLE_INTERNAL_CLOCK (AD9833_BASE+12)
#define AD9833_GET_ACTUAL_PROGRAMMED_FREQUENCY (AD9833_BASE+13)
#define AD9833_GET_ACTUAL_PROGRAMMED_PHASE (AD9833_BASE+14)
#define AD9833_GET_RESOLUTION (AD9833_BASE+15)

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
    //#define AD9833_RESET (AD9833_BASE+2)
    case AD9833_RESET: ad9833.Reset(); break;
    //#define AD9833_SET_FREQUENCY (AD9833_BASE+3)
    case AD9833_SET_FREQUENCY: ad9833.SetFrequency((Registers)p1, (float)p2 / 10); break;
    //#define AD9833_INCREMENT_FREQUENCY (AD9833_BASE+4)
    case AD9833_INCREMENT_FREQUENCY: ad9833.IncrementFrequency((Registers)p1, (float)p2 / 10); break;
    //#define AD9833_SET_PHASE (AD9833_BASE+5)
    case AD9833_SET_PHASE: ad9833.SetPhase((Registers)p1, (float)p2 / 1000); break;
    //#define AD9833_INCREMENT_PHASE (AD9833_BASE+6)
    case AD9833_INCREMENT_PHASE: ad9833.IncrementPhase((Registers)p1, (float)p2 / 1000); break;
    //#define AD9833_SET_WAVEFORM (AD9833_BASE+7)
    case AD9833_SET_WAVEFORM: ad9833.SetWaveform((Registers)p1, (WaveformType)p2); break;
    //#define AD9833_SET_OUTPUT_SOURCE (AD9833_BASE+8)
    case AD9833_SET_OUTPUT_SOURCE: ad9833.SetOutputSource((Registers)p1, (Registers)p2); break;
    //#define AD9833_ENABLE_OUTPUT (AD9833_BASE+9)
    case AD9833_ENABLE_OUTPUT: ad9833.EnableOutput((bool)p1); break;
    //#define AD9833_SLEEP_MODE (AD9833_BASE+10)
    case AD9833_SLEEP_MODE: ad9833.SleepMode((bool)p1); break;
    //#define AD9833_DISABLE_DAC (AD9833_BASE+11)
    case AD9833_DISABLE_DAC: ad9833.DisableDAC((bool)p1); break;
    //#define AD9833_DISABLE_INTERNAL_CLOCK (AD9833_BASE+12)
    case AD9833_DISABLE_INTERNAL_CLOCK: ad9833.DisableInternalClock((bool)p1); break;
    //#define AD9833_GET_ACTUAL_PROGRAMMED_FREQUENCY (AD9833_BASE+13)
    case AD9833_GET_ACTUAL_PROGRAMMED_FREQUENCY:
      RemoteEsp32.intToBytes((int)(ad9833.GetActualProgrammedFrequency((Registers)p1) * 10), buffer + 8);
      break;
    //#define AD9833_GET_ACTUAL_PROGRAMMED_PHASE (AD9833_BASE+14)
    case AD9833_GET_ACTUAL_PROGRAMMED_PHASE:
      RemoteEsp32.intToBytes((int)(ad9833.GetActualProgrammedPhase((Registers)p1) * 1000), buffer + 8);
      break;
    //#define AD9833_GET_RESOLUTION (AD9833_BASE+15)
    case AD9833_GET_RESOLUTION:
      RemoteEsp32.intToBytes((int)(ad9833.GetResolution() * 1000), buffer + 8);
      break;
      
  }
  //RemoteEsp32.printBytes(buffer, 16 + extsBytes);
}


