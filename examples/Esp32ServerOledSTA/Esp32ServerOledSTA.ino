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

#define OLEDDISPLAY_BASE 20200
// bool init();
#define OLEDDISPLAY_INIT (OLEDDISPLAY_BASE+0)
// void end();
#define OLEDDISPLAY_END (OLEDDISPLAY_BASE+1)
// void resetDisplay(void);
#define OLEDDISPLAY_RESET_DISPLAY (OLEDDISPLAY_BASE+2)
// void setColor(OLEDDISPLAY_COLOR color);
#define OLEDDISPLAY_SET_COLOR (OLEDDISPLAY_BASE+3)
// OLEDDISPLAY_COLOR getColor();
#define OLEDDISPLAY_COLOR (OLEDDISPLAY_BASE+4)
// void setPixel(int16_t x, int16_t y);
#define OLEDDISPLAY_SET_PIXEL (OLEDDISPLAY_BASE+5)
// void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
#define OLEDDISPLAY_DRAW_LINE (OLEDDISPLAY_BASE+6)
// void drawRect(int16_t x, int16_t y, int16_t width, int16_t height);
#define OLEDDISPLAY_DRAW_RECT (OLEDDISPLAY_BASE+7)
// void fillRect(int16_t x, int16_t y, int16_t width, int16_t height);
#define OLEDDISPLAY_FILL_RECT (OLEDDISPLAY_BASE+8)
// void drawCircle(int16_t x, int16_t y, int16_t radius);
#define OLEDDISPLAY_DRAW_CIRCLE (OLEDDISPLAY_BASE+9)
// void drawCircleQuads(int16_t x0, int16_t y0, int16_t radius, uint8_t quads);
#define OLEDDISPLAY_DRAW_CIRCLE_QUADS (OLEDDISPLAY_BASE+10)
// void fillCircle(int16_t x, int16_t y, int16_t radius);
#define OLEDDISPLAY_FILL_CIRCLE (OLEDDISPLAY_BASE+11)
// void drawHorizontalLine(int16_t x, int16_t y, int16_t length);
#define OLEDDISPLAY_DRAW_HORIZONTAL_LINE (OLEDDISPLAY_BASE+12)
// void drawVerticalLine(int16_t x, int16_t y, int16_t length);
#define OLEDDISPLAY_DRAW_VERTICAL_LINE (OLEDDISPLAY_BASE+13)
// void drawProgressBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress);
#define OLEDDISPLAY_DRAW_PROGRESS_BAR (OLEDDISPLAY_BASE+14)
// void drawFastImage(int16_t x, int16_t y, int16_t width, int16_t height, const uint8_t *image);
#define OLEDDISPLAY_DRAW_FAST_IMAGE (OLEDDISPLAY_BASE+15)
// void drawXbm(int16_t x, int16_t y, int16_t width, int16_t height, const uint8_t *xbm);
#define OLEDDISPLAY_DRAW_XBM (OLEDDISPLAY_BASE+16)
// void drawString(int16_t x, int16_t y, String text);
#define OLEDDISPLAY_DRAW_STRING (OLEDDISPLAY_BASE+17)
// void drawStringMaxWidth(int16_t x, int16_t y, uint16_t maxLineWidth, String text);
#define OLEDDISPLAY_DRAW_STRING_MAX_WIDTH (OLEDDISPLAY_BASE+18)
// uint16_t getStringWidth(const char* text, uint16_t length);
// uint16_t getStringWidth(String text);
#define OLEDDISPLAY_GET_STRING_WIDTH (OLEDDISPLAY_BASE+19)
// void setTextAlignment(OLEDDISPLAY_TEXT_ALIGNMENT textAlignment);
#define OLEDDISPLAY_SET_TEXT_ALIGNMENT (OLEDDISPLAY_BASE+20)
// void setFont(const uint8_t *fontData);
#define OLEDDISPLAY_SET_FONT (OLEDDISPLAY_BASE+21)
// void setFontTableLookupFunction(FontTableLookupFunction function);
// void displayOn(void);
#define OLEDDISPLAY_DISPLAY_ON (OLEDDISPLAY_BASE+22)
// void displayOff(void);
#define OLEDDISPLAY_DISPLAY_OFF (OLEDDISPLAY_BASE+23)
// void invertDisplay(void);
#define OLEDDISPLAY_INVERT_DISPLAY (OLEDDISPLAY_BASE+24)
// void normalDisplay(void);
#define OLEDDISPLAY_NORMAL_DISPLAY (OLEDDISPLAY_BASE+25)
// void setContrast(uint8_t contrast, uint8_t precharge = 241, uint8_t comdetect = 64);
#define OLEDDISPLAY_SET_CONTRAST (OLEDDISPLAY_BASE+26)
// void setBrightness(uint8_t);
#define OLEDDISPLAY_SET_BRIGHTNESS (OLEDDISPLAY_BASE+27)
// void resetOrientation();
#define OLEDDISPLAY_RESET_ORIENTATION (OLEDDISPLAY_BASE+28)
// void flipScreenVertically();
#define OLEDDISPLAY_FLIP_SCREEN_VERTICALLY (OLEDDISPLAY_BASE+29)
// void mirrorScreen();
#define OLEDDISPLAY_MIRROR_SCREEN (OLEDDISPLAY_BASE+30)
// virtual void display(void) = 0;
#define OLEDDISPLAY_DISPLAY (OLEDDISPLAY_BASE+31)
// void clear(void);
#define OLEDDISPLAY_CLEAR (OLEDDISPLAY_BASE+32)
// bool setLogBuffer(uint16_t lines, uint16_t chars);
#define OLEDDISPLAY_SET_LOG_BUFFER (OLEDDISPLAY_BASE+33)
// void drawLogBuffer(uint16_t x, uint16_t y);
#define OLEDDISPLAY_DRAW_LOG_BUFFER (OLEDDISPLAY_BASE+34)
// uint16_t getWidth(void);
#define OLEDDISPLAY_GET_WIDTH (OLEDDISPLAY_BASE+35)
// uint16_t getHeight(void);
#define OLEDDISPLAY_GET_HEIGHT (OLEDDISPLAY_BASE+36)
// size_t write(uint8_t c);
#define OLEDDISPLAY_WRITE_CHAR (OLEDDISPLAY_BASE+37)
// size_t write(const char* s);
#define OLEDDISPLAY_WRITE_STRING (OLEDDISPLAY_BASE+38)
#define OLEDDISPLAY_PRINT (OLEDDISPLAY_BASE+39)
#define OLEDDISPLAY_PRINTLN (OLEDDISPLAY_BASE+40)

void callback(byte* buffer, int command, int p1, int p2, int extsBytes) {
  //RemoteEsp32.printBytes(buffer, 16 + extsBytes);
  switch (command) {
    
    case OLEDDISPLAY_INIT: display.init(); break;
    case OLEDDISPLAY_END: display.end(); break;
    case OLEDDISPLAY_RESET_DISPLAY: display.resetDisplay(); break;
    //case DISPLAY_SET_COLOR: display.setColor(p1); break;
    //case DISPLAY_COLOR: display.getColor(); break;
    case OLEDDISPLAY_SET_PIXEL: display.setPixel(p1, p2); break;
    case OLEDDISPLAY_DRAW_LINE: {
        int p3 = RemoteEsp32.bytesToInt(buffer + 16);
        int p4 = RemoteEsp32.bytesToInt(buffer + 20);
        display.drawLine(p1, p2, p3, p4);
      }
      break;
    case OLEDDISPLAY_DRAW_RECT: {
        int p3 = RemoteEsp32.bytesToInt(buffer + 16);
        int p4 = RemoteEsp32.bytesToInt(buffer + 20);
        display.drawRect(p1, p2, p3, p4);
      }
      break;
    case OLEDDISPLAY_FILL_RECT: {
        int p3 = RemoteEsp32.bytesToInt(buffer + 16);
        int p4 = RemoteEsp32.bytesToInt(buffer + 20);
        display.fillRect(p1, p2, p3, p4);
      }
      break;
    case OLEDDISPLAY_DRAW_CIRCLE: {
        int p3 = RemoteEsp32.bytesToInt(buffer + 16);
        display.drawCircle(p1, p2, p3);
      }
      break;
    case OLEDDISPLAY_DRAW_CIRCLE_QUADS: {
        int p3 = RemoteEsp32.bytesToInt(buffer + 16);
        int p4 = RemoteEsp32.bytesToInt(buffer + 20);
        display.drawCircleQuads(p1, p2, p3, p4);
      }
      break;
    case OLEDDISPLAY_FILL_CIRCLE: {
        int p3 = RemoteEsp32.bytesToInt(buffer + 16);
        display.fillCircle(p1, p2, p3);
      }
      break;
    case OLEDDISPLAY_DRAW_HORIZONTAL_LINE: {
        int p3 = RemoteEsp32.bytesToInt(buffer + 16);
        display.drawHorizontalLine(p1, p2, p3);
      }
      break;
    case OLEDDISPLAY_DRAW_VERTICAL_LINE: {
        int p3 = RemoteEsp32.bytesToInt(buffer + 16);
        display.drawVerticalLine(p1, p2, p3);
      }
      break;
    case OLEDDISPLAY_DRAW_PROGRESS_BAR: {
        int p3 = RemoteEsp32.bytesToInt(buffer + 16);
        int p4 = RemoteEsp32.bytesToInt(buffer + 20);
        int p5 = RemoteEsp32.bytesToInt(buffer + 24);
        display.drawProgressBar(p1, p2, p3, p4, p5);
      }
      break;
    case OLEDDISPLAY_DRAW_STRING: {
        String p3 = String((char*)(buffer + 16));
        display.drawString(p1, p2, p3);
      }
      break;
    case OLEDDISPLAY_DRAW_STRING_MAX_WIDTH: {
        int p3 = RemoteEsp32.bytesToInt(buffer + 16);
        String p4 = String((char*)(buffer + 20));
        display.drawStringMaxWidth(p1, p2, p3, p4);
      }
      break;
    case OLEDDISPLAY_GET_STRING_WIDTH: {
        String p3 = String((char*)(buffer + 16));
        RemoteEsp32.intToBytes((int)display.getStringWidth(p3), buffer + 8);
      }
      break;
    //case OLEDDISPLAY_SET_TEXT_ALIGNMENT: setTextAlignment(p1); break;
    case OLEDDISPLAY_SET_FONT:
      switch (p1) {
        case 10: display.setFont(ArialMT_Plain_10); break;
        case 16: display.setFont(ArialMT_Plain_16); break;
        case 24: display.setFont(ArialMT_Plain_24); break;
      }
      break;
    case OLEDDISPLAY_DISPLAY_ON: display.displayOn(); break;
    case OLEDDISPLAY_DISPLAY_OFF: display.displayOff(); break;
    case OLEDDISPLAY_INVERT_DISPLAY: display.invertDisplay(); break;
    case OLEDDISPLAY_NORMAL_DISPLAY: display.normalDisplay(); break;
    case OLEDDISPLAY_SET_CONTRAST: {
        int p3 = RemoteEsp32.bytesToInt(buffer + 16);
        display.setContrast(p1, p2, p3);
      }
      break;
    case OLEDDISPLAY_SET_BRIGHTNESS: display.setBrightness(p1); break;
    case OLEDDISPLAY_RESET_ORIENTATION: display.resetOrientation(); break;
    case OLEDDISPLAY_FLIP_SCREEN_VERTICALLY: display.flipScreenVertically(); break;
    case OLEDDISPLAY_MIRROR_SCREEN: display.mirrorScreen(); break;
    case OLEDDISPLAY_DISPLAY: display.display(); break;
    case OLEDDISPLAY_CLEAR: display.clear(); break;
    case OLEDDISPLAY_SET_LOG_BUFFER: display.setLogBuffer(p1, p2); break;
    case OLEDDISPLAY_DRAW_LOG_BUFFER: display.drawLogBuffer(p1, p2); break;
    case OLEDDISPLAY_GET_WIDTH:
      RemoteEsp32.intToBytes((int)display.getWidth(), buffer + 8);
      break;
    case OLEDDISPLAY_GET_HEIGHT:
      RemoteEsp32.intToBytes((int)display.getHeight(), buffer + 8);
      break;
    case OLEDDISPLAY_WRITE_CHAR: display.write(p1); break;
    case OLEDDISPLAY_WRITE_STRING: {
        char *p3 = (char*)(buffer + 16);
        display.drawString(p1, p2, p3);
      }
      break;
    case OLEDDISPLAY_PRINT: {
        String p3 = String((char*)(buffer + 16));
        display.print(p3);
      }
      break;
    case OLEDDISPLAY_PRINTLN: {
        String p3 = String((char*)(buffer + 16));
        display.println(p3);
      }
      break;
    
    default:
      RemoteEsp32.intToBytes(ERR_UNKNOWN_COMMAND, buffer + 4);
      break;

  }
  //RemoteEsp32.printBytes(buffer, 16 + extsBytes);
}
